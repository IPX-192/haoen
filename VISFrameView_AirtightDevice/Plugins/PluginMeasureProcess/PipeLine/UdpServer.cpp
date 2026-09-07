#include "UdpServer.h"
#include <QApplication>
#include <QHostInfo>
#include <QDebug>
#include <QtCore/QMetaType>
#include "VisAppBus.h"
#include "VisAppThreadPool.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"
#include "ParamManager.h"
#include "CylinderCtrl.h"
#include "../Common/PipeLineDef.h"

using namespace  VisMotorToolSpace;
Q_DECLARE_METATYPE(DownStreamServerStatus)

UdpServer::UdpServer(QObject *parent) : QObject(parent)
{
    m_initThreadID = std::this_thread::get_id();
	m_uSocket = new QUdpSocket(this);
	connect(m_uSocket, &QUdpSocket::readyRead, this, &UdpServer::slotUdpReceive);

	m_timer = new QTimer(this);
    m_timer->setInterval(200);
	connect(m_timer, &QTimer::timeout, this, &UdpServer::slotTimeOut);
    m_statusNotifyTimer = new QTimer(this);
    m_statusNotifyTimer->setInterval(200);
    connect(m_statusNotifyTimer, &QTimer::timeout, this, &UdpServer::slotNotifyFullStatus);
    m_statusNotifyTimer->start();
	VisAppBus::subscibeEvent(this, "SetPipeLineOutStatus");
	VisAppBus::subscibeEvent(this, "SetBackFlowStatus");
    VisAppBus::subscibeEvent(this, "ResetPipeLineServerState");
    VisAppBus::subscibeEvent(this, "SimulateNextRequest");
    Start();
}

bool UdpServer::Start()
{
    IOLevel level = VisMotorInstance->GetIoInput(IN_PipeLineBackFlowBuf);
    m_requestTrayBackFlow = (level == IO_OFF);
    m_timer->start();
    m_uSocket->close();
    return  m_uSocket->bind(UdpServerPort, QUdpSocket::ShareAddress);
}

bool UdpServer::SendState()
{
    if (std::this_thread::get_id() == m_initThreadID) {
            return slotPostState();
        }
    else {
        bool bRet = false;
        QMetaObject::invokeMethod(this, "slotPostState", Qt::BlockingQueuedConnection
            , Q_RETURN_ARG(bool, bRet));
        return bRet;
    }

    return true;
}

bool UdpServer::slotPostState()
{
    //设置流线状态
    int statusValue = m_existTray;                      //本站有料
    int value = m_requestTrayBackFlow;                  //回流线要料
    statusValue += (value << 1);
    statusValue += (m_trayToBackFlow << 2);             //回流线到位
    //序列化托盘信息
    QByteArray sendTrayData;
    QDataStream out(&sendTrayData, QIODevice::WriteOnly);
    // 【关键】：设置版本号，确保发送端和接收端的解析方式一致
    out.setVersion(QDataStream::Qt_5_13);
    out << m_trayInfo;

    // 【关键改动】将二进制数据转为 Base64 字符串
    //QString trayBase64 = sendTrayData.toBase64();

    QByteArray sendData = QString("IO=%1;").arg(statusValue).toLocal8Bit();
    sendData += sendTrayData + ";";

    QString ip = GlobalParam->systemParam.nextDeviceIp;
    quint16 port = UdpClientPort;
    // 使用 toLatin1() 或 toUtf8()，因为 Base64 仅包含 ASCII 字符
    qint64 nRes = m_uSocket->writeDatagram(sendData, QHostAddress(ip), port);
    m_uSocket->flush();
    return true;
}

void UdpServer::slotNotifyFullStatus()
{
    DownStreamServerStatus status;
    status.m_existTray = m_existTray;
    status.m_requestTrayBackFlow = m_requestTrayBackFlow;
    status.m_trayToBackFlow = m_trayToBackFlow;
    status.m_nextRequest = m_nextRequest;
    status.m_nextBackFlowExist = m_nextBackFlowExist;
    status.m_trayInNextPos = m_trayInNextPos;
    status.m_transTray = m_transTray;
    status.m_transTrayBackFlow = m_transTrayBackFlow;
    VisAppBus::postEvent("PipeLineServerStatusNotify", status);
}

void UdpServer::sigPipeLineOutTransEnd()
{
    VisAppBus::sendEventDirect("PipeLineOutTransEnd");
}

void UdpServer::sigSetBackFlowReady(bool exist, QString trayCode)
{
    VisAppBus::sendEventDirect("SetBackFlowReady", exist, trayCode);
}

void UdpServer::slotTimeOut()
{
    SendState();
    if (m_simulateNextRequest) {
        TryTransTrayToNext();
    }
}


void UdpServer::slotUdpReceive()
{
    QByteArray data;
    QHostAddress address;
    quint16 port;
    while (m_uSocket->hasPendingDatagrams()) {
        data.resize(m_uSocket->pendingDatagramSize());
        m_uSocket->readDatagram(data.data(), data.size(), &address, &port);
    }

    QByteArrayList infoList = data.split(';');
    if (!infoList.size())return;
    QString stateInfo = infoList.at(0);
    int stateValue = stateInfo.right(stateInfo.length() - stateInfo.indexOf("=") - 1).toInt();
    m_nextRequest = (stateValue >> 3) & 1;             //下游设备要料
    m_trayInNextPos = (stateValue >> 4) & 1;           //载具托盘到达下一站
    m_nextBackFlowExist = (stateValue >> 7) & 1;       //下游回流线有盘

    if (infoList.size() < 2)return;
    QByteArray trayData= infoList.at(1);
    TrayInfo  trayInfo;
    QDataStream in(&trayData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    in >> trayInfo;

    if (m_nextRequest && m_existTray && !m_transTray) {
        TryTransTrayToNext();
    }
    if (m_nextBackFlowExist && m_requestTrayBackFlow && !m_transTrayBackFlow) {
        m_transTrayBackFlow = true;
        m_trayToBackFlow = false;
        ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"回流托盘准备到本站"));
        GlobalThreadPool->Commit(std::bind(&UdpServer::TransTrayToCur, this, trayInfo.trayCode));
    }
}


void UdpServer::TryTransTrayToNext()
{
    //真实下游要盘 或 调试模拟要盘
    if (!(m_nextRequest || m_simulateNextRequest)) {
        return;   //下游没要盘,静默
    }
    if (m_transTray) {
        //要盘了但传输状态卡死(上次传输失败未复位),这是"流不下去"的异常原因
        ShowLog(Log_PipeLineOnline, 0, Log_Error, QString(u8"下游要盘但传输状态卡死:transTray=true"));
        return;
    }
    if (!m_existTray) {
        return;   //要盘了但出口没盘(清料完成后正常),静默
    }
    m_transTray = true;
    m_trayInNextPos = false;
    ShowSystemLog(Log_Info, QString(u8"下发的pcb条码1:%1,托盘码:%2").arg(m_trayInfo.pcbBarCode).arg(m_trayInfo.trayCode));
    ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"托盘准备到下一站,托盘码:[%1],pcb码:[%2]").arg(m_trayInfo.trayCode).arg(m_trayInfo.pcbBarCode));
    GlobalThreadPool->Commit(std::bind(&UdpServer::TransTrayToNext, this));
}

void UdpServer::TransTrayToNext()
{
	m_transTray = true;
	m_trayInNextPos = false;
	//下降顶升
	int nRes = CylinderCtrl::instance()->event_SetPushUp(FeedTurntable, false);
    if (nRes != 0) {
        ShowLog(Log_PipeLineOnline, 0, Log_Error, QString(u8"传输失败:下降顶升返回%1,transTray卡死").arg(nRes));
        return;
    }
	//下降阻挡
	nRes = CylinderCtrl::instance()->event_SetBlockUp(FeedTurntable, false);
    if (nRes != 0) {
        ShowLog(Log_PipeLineOnline, 0, Log_Error, QString(u8"传输失败:下降阻挡返回%1,transTray卡死").arg(nRes));
        return;
    }
	//启动输送流水线
    nRes = VisAppBus::sendEventDirect("SetPipeLineMove", true);
    if (nRes != 0) {
        ShowLog(Log_PipeLineOnline, 0, Log_Error, QString(u8"传输失败:启动流水线返回%1,transTray卡死").arg(nRes));
        return;
    }
	
	ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"等待下游设备回复载具到位信号"));
	QTime time;
	time.start();
	//15秒内等待下游回复
	while (time.elapsed() < 30000) {
		if (VisMotorInstance->IsEmgStop())return;
        if (time.elapsed() > 5000 && (GlobalParam->flagOffline)) {
			m_trayInNextPos = true;
			break;
		}
		if (m_trayInNextPos)break;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	if (!m_trayInNextPos) {
		ShowLog(Log_PipeLineOnline, 0, Log_Error, QString(u8"等待下游设备回复载具到位超时"));
	}
	//上升阻挡
	nRes = CylinderCtrl::instance()->event_SetBlockUp(FeedTurntable, true);
	if (nRes != 0) {
        ShowLog(Log_PipeLineOnline, 0, Log_Error, QString(u8"传输失败:上升阻挡返回%1,transTray卡死").arg(nRes));
        return;
    }
	//停止流水线
    nRes = VisAppBus::sendEventDirect("SetPipeLineMove", false);
	if (nRes != 0) {
        ShowLog(Log_PipeLineOnline, 0, Log_Error, QString(u8"传输失败:停止流水线返回%1,transTray卡死").arg(nRes));
        return;
    }
    ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"载具到下游传输结束"));
	
	m_existTray = false;
	m_transTray = false;
	sigPipeLineOutTransEnd();
    m_trayInfo = TrayInfo();
}

void UdpServer::TransTrayToCur(QString trayCode)
{
    //启动回流线
    int nRes = VisMotorInstance->SetIoOutput(Out_PipeLineBackFlowMotorL, IO_ON);
    if (nRes != 0)return ;
    //等待出口检测到托盘
    bool exist = true;
    if (!VisMotorInstance->SelectIoInput(IN_PipeLineBackFlowBuf, IO_ON, 30000)) {
        QString errInfo = QString(u8"30秒内回流线托盘缓存没有检测到托盘信号");
        ShowLog(Log_PipeLineOnline, 0, Log_Error, errInfo);
        VisAppBus::sendEvent("PopupErrInfo", errInfo);
        exist = false;
    }
    else {
        ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"回流线托盘达到本站,托盘码:[%1]").arg(trayCode));
    }
    //停止流线
    nRes = VisMotorInstance->SetIoOutput(Out_PipeLineBackFlowMotorL, IO_OFF);
    if (nRes != 0)return;
    m_trayToBackFlow = true;
    m_transTrayBackFlow = false;
    m_requestTrayBackFlow = !exist;   //接到盘→停止要盘;超时没接到→继续要盘
    if(exist)
    {
        sigSetBackFlowReady(true, trayCode);
    }
}

int UdpServer::event_SetPipeLineOutStatus(bool exist, TrayInfo trayInfo)
{
    m_trayInfo = trayInfo;
    m_existTray = exist;
    //排查日志:UDP出口收到的载具条码
    ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"UDP出口载具:托盘码=[%1],holder=[%2],pcb=[%3]").arg(trayInfo.trayCode).arg(trayInfo.holderBarCode).arg(trayInfo.pcbBarCode));
    if (GlobalParam->flagOffline) {
		ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"托盘准备到下一站,托盘码:[%1],pcb码:[%2]").arg(m_trayInfo.trayCode).arg(m_trayInfo.pcbBarCode));
		GlobalThreadPool->Commit(std::bind(&UdpServer::TransTrayToNext, this));
	}

    return 0;
}

int UdpServer::event_SetBackFlowStatus(bool exist)
{
    m_requestTrayBackFlow = !exist;
    m_trayToBackFlow = exist;
    return 0;
}

int UdpServer::event_ResetPipeLineServerState()
{
    //本站信号
    m_existTray = false;              //本站流线出口有盘
    m_requestTrayBackFlow = false;    //本站回流线要料
    m_trayToBackFlow = false;         //回流线托盘到达本站
    //下游设备信号
    m_nextRequest = false;            //下游请求托盘
    m_nextBackFlowExist = false;      //下游回流线有盘
    m_trayInNextPos = false;          //输送线托盘到达下一站
    //传输状态
    m_transTray = false;              //输送线传输托盘状态
    m_transTrayBackFlow = false;      //回流线传输托盘状态
    //出口载具信息
    m_trayInfo = TrayInfo();
    //回流线要料状态按回流线缓存IO实际状态推导,与Start()保持一致
    IOLevel level = VisMotorInstance->GetIoInput(IN_PipeLineBackFlowBuf);
    m_requestTrayBackFlow = (level == IO_OFF);
    ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"下游联机8路状态已复位"));
    return 0;
}

int UdpServer::event_SimulateNextRequest(bool request)
{
    m_simulateNextRequest = request;
    ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"模拟下游要盘:%1").arg(request ? u8"开启" : u8"关闭"));
    return 0;
}

