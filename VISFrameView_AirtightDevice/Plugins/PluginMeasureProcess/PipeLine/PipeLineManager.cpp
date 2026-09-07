#include "PipeLineManager.h"
#include <QApplication>
#include <QTime>
#include "VisAppBus.h"
#include "VisMotorManager.h"
#include "VisAppThreadPool.h"
#include "ParamManager.h"
#include "UdpClient.h"
#include "UdpServer.h"
#include "CylinderCtrl.h"
#include <QtCore/QMetaType>
Q_DECLARE_METATYPE(TrayFunc)
Q_DECLARE_METATYPE(TrayInfo)
Q_DECLARE_METATYPE(QVector<TrayFunc>)
Q_DECLARE_METATYPE(QVector<bool>)
Q_DECLARE_METATYPE(QVector<TrayInfo>)

using namespace  VisMotorToolSpace;
PipeLineManager::PipeLineManager(QObject *parent) : QObject(parent)
{
    m_udpClient=new UdpClient(this);
    m_udpServer=new UdpServer(this);

	for (int i = 0; i < 2; i++) {
		m_pineLineUseCount[i] = 0;
    }
    m_mapTrayName[EmptyBuf]=u8"空盘缓存位";
    m_mapTrayName[FeedHolder]=u8"上料壳体";
    m_mapTrayName[FeedPCB]=u8"上料PCB";
    m_mapTrayName[PCBClean]=u8"PCB清洗";
    m_mapTrayName[FeedTurntable]=u8"转盘上料";
	VisAppBus::subscibeEvent(this, "SetPipeLineMove");
    VisAppBus::subscibeEvent(this,"BlankPipeLineTray");
    VisAppBus::subscibeEvent(this,"SetBackFlowReady");
	VisAppBus::subscibeEvent(this, "PipeLineOutTransEnd");
    VisAppBus::subscibeEvent(this, "RefreshAllTrayMap");
    qRegisterMetaType<TrayFunc>("TrayFunc");
    qRegisterMetaType<QVector<TrayFunc>>();
    qRegisterMetaType<QVector<bool>>();
    qRegisterMetaType<QVector<TrayInfo>>();

}

PipeLineManager::~PipeLineManager()
{
    setStatus(STOP);
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_condition.notify_one();
    }

    stop();
}

int PipeLineManager::Init()
{
    if (!m_udpServer->Start())return -1;
   // if (!m_udpClient->Start(GlobalParam->systemParam.preDeviceIp))return -1;
	for (int i = 0; i < 2; i++) {
		m_pineLineUseCount[i] = 0;
	}
    for(int i=EmptyBuf;i<=FeedTurntable;i++)
		m_mapExistTrayInfo[(TrayFunc)i].first=false;
    start();

    return 0;
}

bool PipeLineManager::doTask()
{
    // 阻塞等待：停机 或 有待移托盘任务
    std::unique_lock<std::mutex> locker(m_mutex);
    m_condition.wait(locker, [this] {
        return status == STOP || !m_listTrayTask.empty();
        });
    if (status == STOP) {
        return false;
    }
	if (VisMotorInstance->IsEmgStop())return false;
    if (m_listTrayTask.empty()) return true;
    QVector<TrayFunc> listTrayTask=m_listTrayTask;
    for(int i=0;i<listTrayTask.size();i++){
        TrayFunc src=listTrayTask[i];
        TrayFunc dst=(TrayFunc)(src+1);
        //下个位置空闲
        if(m_mapExistTrayInfo[dst].first==false){
           GlobalThreadPool->Commit(std::bind(&PipeLineManager::MoveTray,this,src));
           m_listTrayTask.removeOne(src);
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
}

int PipeLineManager::MoveTray(TrayFunc src)
{
	int nRes = 0;
    TrayFunc dst=(TrayFunc)(src+1);
    ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"%1处载具开始移动到%2处").arg(m_mapTrayName[src]).arg(m_mapTrayName[dst]));
	if (src == EmptyBuf) {
		nRes = MoveTrayToHolder();
		if (nRes != 0)return nRes;
	}
    else if(src==FeedHolder){
        nRes = MoveTrayToFeedPCB();
        if (nRes != 0)return nRes;
    }
    else if(src==FeedPCB){
		nRes = MoveTrayToCleanPCB();
		if (nRes != 0)return nRes;
    }
    else if(src==PCBClean){
		nRes = MoveTrayToTurntable();
		if (nRes != 0)return nRes;
    }

    ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"载具到达%1处").arg(m_mapTrayName[dst]));
	m_mapExistTrayInfo[dst].second = m_mapExistTrayInfo[src].second;
	m_mapExistTrayInfo[dst].first = true;
	m_mapExistTrayInfo[src].second = TrayInfo();
	m_mapExistTrayInfo[src].first = false;

    sigPipeLineTrayReady(dst, m_mapExistTrayInfo[dst].second);

    return 0;
}

int PipeLineManager::MoveTrayToHolder()
{
    //接驳台到回流线
    int nRes = VisMotorInstance->MovePositionAbs(BackFlowTransfer);
	if (nRes != 0)return nRes;
	//降阻挡
	nRes = CylinderCtrl::instance()->event_SetBlockUp(EmptyBuf, false);
	if (nRes != 0)return nRes;
	//流水线动作
	nRes = VisMotorInstance->MotorMoveAbs(MotorTransferX, -1, false, true);
	if (nRes != 0)return nRes;
	nRes = VisMotorInstance->SetIoOutput(Out_PipeLineBackFlowMotorL, IO_ON);
	if (nRes != 0)return nRes;
    //等待托盘流到接驳线左边
	if (!VisMotorInstance->SelectIoInput(IN_PipeLineTransferL, IO_ON, 20000)) {
        if (VisMotorInstance->IsEmgStop()) return HardWareErr;
        QStringList listCheckIO;
        QVector<int> vecNormalLevel;
        listCheckIO << IN_PipeLineTransferL;
        vecNormalLevel << IO_ON;
        QString errInfo = QString(u8"20秒内接驳线左边没有检测到托盘信号,请放盘后点确定,或点取消退出");
        ShowLog(Log_PipeLine, 0, Log_Error, errInfo);
        int nRet = VisAppBus::sendEvent("PopupErrNotify", errInfo, listCheckIO, vecNormalLevel);
        if (nRet != 0) return nRet;
	}
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	//停止流水线动作
    VisMotorInstance->StopContinuous(MotorTransferX);
	nRes = VisMotorInstance->SetIoOutput(Out_PipeLineBackFlowMotorL, IO_OFF);
	if (nRes != 0)return nRes;
	//上升阻挡
	nRes = CylinderCtrl::instance()->event_SetBlockUp(EmptyBuf, true);
	if (nRes != 0)return nRes;
    //接驳台到上料输送线
	nRes = VisMotorInstance->MovePositionAbs(FeedTransfer);
	if (nRes != 0)return nRes;
    //排查日志:接驳盘到上壳体处后,接驳料盘X的位置与状态(为下一步往右运动失败提供上下文)
    {
        double xAfterHolder = 0.0;
        VisMotorInstance->GetCurPos(MotorTransferX, xAfterHolder);
        int stateAfterHolder = VisMotorInstance->QueryMoveState(MotorTransferX);
        ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"接驳盘到上壳体处后:接驳料盘X位置=%1,运动状态=%2(0静止)").arg(xAfterHolder).arg(stateAfterHolder));
    }
	sigSetBackFlowStatus(false);
    return 0;
}

int PipeLineManager::MoveTrayToFeedPCB()
{
	//启动输送流水线
    int nRes = event_SetPipeLineMove(true);
    if (nRes != 0)return nRes;
    //排查日志:接驳料盘X往右运动前的位置与运动状态(定位"返回值4"根因)
    double curX = 0.0;
    VisMotorInstance->GetCurPos(MotorTransferX, curX);
    int moveState = VisMotorInstance->QueryMoveState(MotorTransferX);
    ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"接驳料盘X往右运动前:位置=%1,运动状态=%2(0静止)").arg(curX).arg(moveState));
	nRes = VisMotorInstance->MotorMoveAbs(MotorTransferX, 1, false, true);
	if (nRes != 0) {
        ShowLog(Log_PipeLine, 0, Log_Error, QString(u8"接驳料盘X往右运动失败:返回值=%1,运动前位置=%2,运动状态=%3").arg(nRes).arg(curX).arg(moveState));
        return nRes;
    }
	//等待托盘流到上料PCB
	if (!VisMotorInstance->SelectIoInput(IN_PipeLinePCB, IO_ON, 15000)) {
		QString errInfo = QString(u8"15秒内流线上料PCB处没有检测到托盘信号");
		ShowLog(Log_PipeLine, 0, Log_Error, errInfo);
        nRes = VisAppBus::sendEvent("PopupErrNotify", errInfo);
		if (nRes != 0)return nRes;
	}
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //停止流水线动作
    VisMotorInstance->StopContinuous(MotorTransferX);
    nRes = event_SetPipeLineMove(false);
    if (nRes != 0)return nRes;

    //托盘到达PCB上料工位:读取RFID托盘码(托盘信息还在FeedHolder位,写入源位后随MoveTray拷贝到FeedPCB)

    QString upTrayCode = m_mapExistTrayInfo[FeedHolder].second.trayCode;   //回流线带来的托盘码(目前不带,以后可能加)
    QString barCode;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    int ret = VisAppBus::sendEvent("ReadRfidTag", FeedPCB, barCode);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (ret == 0) {
        //RFID实测码与回流线带来的托盘码比对(回流码为空则跳过比对)
        if (!upTrayCode.isEmpty() && barCode != upTrayCode)
        {
            ShowLog(Log_PipeLine, 0, Log_Error,
                    QString(u8"托盘码比对不一致! RFID读取:%1 , 回流线带来:%2").arg(barCode).arg(upTrayCode));
        }
        m_mapExistTrayInfo[FeedHolder].second.trayCode = barCode;   //RFID实测码优先
        ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"PCB工位读取托盘码:%1").arg(barCode));
    }
    else {
        //读卡失败:保留回流线带来的托盘码,不清空(当前回流不带码,无影响)
        ShowLog(Log_PipeLine, 0, Log_Error,
                QString(u8"RFID读卡失败(上料PCB工位),保留回流托盘码:[%1],具体原因见系统日志").arg(upTrayCode));
    }

    //顶升托盘
    nRes = CylinderCtrl::instance()->event_SetPushUp(FeedPCB, true);
    if (nRes != 0)return nRes;
    return 0;
}

int PipeLineManager::MoveTrayToCleanPCB()
{
	//下降顶升
	int nRes = CylinderCtrl::instance()->event_SetPushUp(FeedPCB, false);
	if (nRes != 0)return nRes;
    //下降阻挡
	nRes = CylinderCtrl::instance()->event_SetBlockUp(FeedPCB, false);
	if (nRes != 0)return nRes;
	//启动输送流水线
	nRes = event_SetPipeLineMove(true);
	if (nRes != 0)return nRes;
	//等待托盘流到清洗PCB
	if (!VisMotorInstance->SelectIoInput(IN_PipeLineClean, IO_ON, 20000)) {
		QString errInfo = QString(u8"20秒内流线清洗PCB处没有检测到托盘信号");
		ShowLog(Log_PipeLine, 0, Log_Error, errInfo);
		nRes = VisAppBus::sendEvent("PopupErrNotify", errInfo);
		if (nRes != 0)return nRes;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	//停止流水线动作
    nRes = event_SetPipeLineMove(false);
	if (nRes != 0)return nRes;
	//顶升托盘
	nRes = CylinderCtrl::instance()->event_SetPushUp(PCBClean, true);
	if (nRes != 0)return nRes;
	//上升阻挡
	nRes = CylinderCtrl::instance()->event_SetBlockUp(FeedPCB, true);
	if (nRes != 0)return nRes;
    return 0;
}

int PipeLineManager::MoveTrayToTurntable()
{
	//下降顶升
	int nRes = CylinderCtrl::instance()->event_SetPushUp(PCBClean, false);
	if (nRes != 0)return nRes;
	//下降阻挡
	nRes = CylinderCtrl::instance()->event_SetBlockUp(PCBClean, false);
	if (nRes != 0)return nRes;
	//启动输送流水线
	nRes = event_SetPipeLineMove(true);
	if (nRes != 0)return nRes;
	//等待托盘流到清洗PCB
	if (!VisMotorInstance->SelectIoInput(IN_PipeLineTurntable, IO_ON, 20000)) {
		QString errInfo = QString(u8"20秒内流线转盘处没有检测到托盘信号");
		ShowLog(Log_PipeLine, 0, Log_Error, errInfo);
		nRes = VisAppBus::sendEvent("PopupErrNotify", errInfo);
		if (nRes != 0)return nRes;
	}
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	//停止流水线动作
    nRes = event_SetPipeLineMove(false);
	if (nRes != 0)return nRes;
	//顶升托盘
	nRes = CylinderCtrl::instance()->event_SetPushUp(FeedTurntable, true);
	if (nRes != 0)return nRes;
	//上升阻挡
	nRes = CylinderCtrl::instance()->event_SetBlockUp(PCBClean, true);
	if (nRes != 0)return nRes;
    //排查日志:到达转盘上料位时的载具条码
    ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"载具到达转盘上料位:holder=[%1],pcb=[%2]").arg(m_mapExistTrayInfo[PCBClean].second.holderBarCode).arg(m_mapExistTrayInfo[PCBClean].second.pcbBarCode));
	return 0;
}

void PipeLineManager::sigPipeLineTrayReady(TrayFunc type, TrayInfo info)
{
    VisAppBus::sendEventDirect("PipeLineTrayReady",type, info);
}


void PipeLineManager::sigSetPipeLineOutStatus(bool exist, TrayInfo info)
{
	VisAppBus::sendEvent("SetPipeLineOutStatus", exist, info);
}

void PipeLineManager::sigSetBackFlowStatus(bool exist)
{
	VisAppBus::sendEventDirect("SetBackFlowStatus", exist);
}

int PipeLineManager::event_SetPipeLineMove(bool enable)
{
    int index = 0;
    m_pipeLineMutex.lock();
	int nRes = 0;
    m_pineLineUseCount[index] += (enable ? 1 : -1);
	if (m_pineLineUseCount[index] < 0) {
		m_pineLineUseCount[index] = 0;
	}
	if (m_pineLineUseCount[index] == 0) {
        nRes = VisMotorInstance->SetIoOutput(Out_PipeLineMotorR, IO_OFF);
	}
	else if (m_pineLineUseCount[index] == 1) {
        nRes = VisMotorInstance->SetIoOutput(Out_PipeLineMotorR, IO_ON);
	}
    m_pipeLineMutex.unlock();
    return nRes;
}

int PipeLineManager::event_SetBackFlowReady(bool exist, QString trayCode)
{
    std::unique_lock<std::mutex> locker(m_mutex);
    //手动上盘/回流托盘就绪:空盘缓存位已有载具(位置占用或已在移动队列),忽略本次请求,避免重复入队
    if (m_mapExistTrayInfo[EmptyBuf].first || m_listTrayTask.contains(EmptyBuf)) {
        ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"%1已有载具,忽略重复上盘请求").arg(m_mapTrayName[EmptyBuf]));
        return 0;
    }
    ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"%1处有载具,托盘码:[%2]").arg(m_mapTrayName[EmptyBuf]).arg(trayCode));
    m_mapExistTrayInfo[EmptyBuf].first = true;
    if (!trayCode.isEmpty())
        m_mapExistTrayInfo[EmptyBuf].second.trayCode = trayCode;   //下游回流的托盘码写入载具信息,随载具流转
    m_listTrayTask.push_back(EmptyBuf);
    m_condition.notify_one();
    return 0;
}

int PipeLineManager::event_BlankPipeLineTray(TrayFunc type, TrayInfo info)
{
	m_mapExistTrayInfo[type].second = info;
    //排查日志:各工位上料完成写入流水线
    if (type == FeedPCB)
        ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"流水线接收上料PCB载具:holder=[%1],pcb=[%2]").arg(info.holderBarCode).arg(info.pcbBarCode));
    else if (type == FeedTurntable)
        ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"流水线接收转盘下料载具:holder=[%1],pcb=[%2]").arg(info.holderBarCode).arg(info.pcbBarCode));
    if (type == FeedTurntable) {
		ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"请求%1处载具流走").arg(m_mapTrayName[type]));
		sigSetPipeLineOutStatus(true, info);
    }
    else {
		std::unique_lock<std::mutex> locker(m_mutex);
		m_listTrayTask.push_back(type);
		m_condition.notify_one();

    }

    return 0;
}

int PipeLineManager::event_PipeLineOutTransEnd()
{
	m_mapExistTrayInfo[FeedTurntable].second = TrayInfo();
	m_mapExistTrayInfo[FeedTurntable].first = false;
    return 0;
}

int PipeLineManager::event_RefreshAllTrayMap(QVector<TrayFunc>& vecFunc, QVector<bool>& vecHasTray, QVector<TrayInfo>& vecTrayData)
{
    // 清空输出容器
    vecFunc.clear();
    vecHasTray.clear();
    vecTrayData.clear();
    for (auto iter = m_mapExistTrayInfo.begin(); iter != m_mapExistTrayInfo.end(); ++iter)
    {
        TrayFunc func = iter.key();
        auto& pair = iter.value();
        vecFunc.push_back(func);
        vecHasTray.push_back(pair.first);
        vecTrayData.push_back(pair.second);
    }
    return 0;
}
