#include "UdpClient.h"
#include <QApplication>
#include <QtCore/QMetaType>
#include <QDebug>
#include "VisAppBus.h"
#include "VisAppThreadPool.h"
#include "VisMotorManager.h"
#include "ParamManager.h"
#include "VisMotorToolData.h"
#include "CylinderCtrl.h"
#include "../PluginAutoPage/WidgetFlowState.h"

using namespace  VisMotorToolSpace;
Q_DECLARE_METATYPE(UpStreamClientStatus)

UdpClient::UdpClient(QObject *parent) : QObject(parent)
{
    m_initThreadID = std::this_thread::get_id();
    m_uSocket = new QUdpSocket(this);
    connect(m_uSocket, &QUdpSocket::readyRead, this, &UdpClient::slotUdpReceive);
    m_uSocket->bind(UdpClientPort, QUdpSocket::ShareAddress);

    m_timer = new QTimer(this);
    m_timer->setInterval(200);
	connect(m_timer, &QTimer::timeout, this, &UdpClient::slotTimeOut);
    m_statusNotifyTimer = new QTimer(this);
    m_statusNotifyTimer->setInterval(200);
    connect(m_statusNotifyTimer, &QTimer::timeout, this, &UdpClient::slotNotifyFullStatus);
    m_statusNotifyTimer->start();
    VisAppBus::subscibeEvent(this, "SetBackFlowReady");
    VisAppBus::subscibeEvent(this, "SetPipeLineInStatus");
}

bool UdpClient::Start()
{
	//获取回流线缓存托盘是否有无
     
    m_timer->start();
    return true;
}

void UdpClient::slotTimeOut()
{
     SendState();
}

bool UdpClient::SendState()
{
    if (std::this_thread::get_id() == m_initThreadID) {
            return slotPostData();
    }
	else {
		bool bRet = false;
		QMetaObject::invokeMethod(this, "slotPostData", Qt::BlockingQueuedConnection
			, Q_RETURN_ARG(bool, bRet));
		return bRet;
	}

	return true;
}

bool UdpClient::slotPostData()
{
	int state = m_trayRequest;    //要料信号
	int stateValue = (state << 3);
	state = m_trayInCurPos;       //输送线载具到位
    stateValue += (state << 4);
	state = m_existTrayBackFlow;  //回流线有料
    stateValue += (state << 7);
	if (!GlobalParam->autoRunning)
		stateValue = 0;

	QString sendData = QString("IO=%1;").arg(stateValue);
	QString ip = GlobalParam->systemParam.preDeviceIp;
    quint16 port = UdpServerPort;
	qint64 nRes = m_uSocket->writeDatagram(sendData.toLocal8Bit(), QHostAddress(ip), port);
	m_uSocket->flush();

    return true;
}

void UdpClient::slotNotifyFullStatus()
{
    UpStreamClientStatus status;
    status.m_trayRequest = m_trayRequest;
    status.m_trayInCurPos = m_trayInCurPos;
    status.m_existTrayBackFlow = m_existTrayBackFlow;
    status.m_preExistTray = m_preExistTray;
    status.m_preRequestBackFlow = m_preRequestBackFlow;
    status.m_trayInNextPosBackFlow = m_trayInNextPosBackFlow;
    status.m_transTray = m_transTray;
    status.m_transTrayBackFlow = m_transTrayBackFlow;
    VisAppBus::postEvent("PipeLineClientStatusNotify", status);
}

void UdpClient::slotUdpReceive()
{
    QByteArray data;
    QHostAddress address;
    quint16 port;
    while (m_uSocket->hasPendingDatagrams()) {
        data.resize(m_uSocket->pendingDatagramSize());
        m_uSocket->readDatagram(data.data(), data.size(), &address, &port);
    }
    QStringList infoList = QString::fromLocal8Bit(data).split(";");
    if (!infoList.size())return;
    QString stateInfo = infoList.at(0);
    int stateValue = stateInfo.right(stateInfo.length() - stateInfo.indexOf("=") - 1).toInt();
    m_preExistTray = stateValue & 1;              //上站有料
    m_preRequestBackFlow = (stateValue >> 1) & 1;      //回流线要料
    m_trayInNextPosBackFlow = (stateValue >> 2) & 1;   //回流线托盘到上站

    TrayInfo  trayInfo;
    if (infoList.size() > 1) {
        QByteArray trayData = infoList.at(1).toLocal8Bit();
        QDataStream in(&trayData, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_13);
        in >> trayInfo;
    }
    //本站输送线要料，上站输送线有料，输送线未传输状态
    if (m_trayRequest && m_preExistTray && !m_transTray) {
        m_transTray = true;
        m_trayInCurPos = false;
        ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"上站托盘准备到本站"));
        GlobalThreadPool->Commit(std::bind(&UdpClient::TransTrayToCur, this, trayInfo));
    }
    //上站回流线要料，本站回流线有料，回流线未传输状态
    if (m_preRequestBackFlow && m_existTrayBackFlow && (!m_transTrayBackFlow)) {
        m_trayInNextPosBackFlow = false;
        m_transTrayBackFlow = true;
        ShowLog(Log_PipeLineOnline, 0, Log_Info, QString(u8"回流线托盘准备到上站"));
        GlobalThreadPool->Commit(std::bind(&UdpClient::TransBackFlowTray, this));
    }
}

void UdpClient::TransTrayToCur(TrayInfo trayInfo)
{
	//根据机台适配动作

    m_trayRequest=false;
    m_trayInCurPos=true;
    m_transTray=false;
    sigPipeLineTrayReady(trayInfo);
}

void UdpClient::TransBackFlowTray()
{
	//根据机台适配动作

    m_existTrayBackFlow = true;
    m_transTrayBackFlow = false;
    sigSetBackFlowStatus(false);
}

void UdpClient::sigPipeLineTrayReady(TrayInfo trayInfo)
{
	VisAppBus::sendEventDirect("PipeLineTrayReady", trayInfo);
}

void UdpClient::sigSetBackFlowStatus(bool exist)
{
    VisAppBus::sendEventDirect("SetBackFlowStatus", exist);
}

int UdpClient::event_SetPipeLineInStatus(bool exist)
{
    m_trayRequest = !exist;
    m_trayInCurPos = exist;

    return 0;
}

int UdpClient::event_SetBackFlowReady(bool exist, QString trayCode)
{
    Q_UNUSED(trayCode);
    m_existTrayBackFlow = exist;
    m_transTrayBackFlow = false;
    return 0;
}

