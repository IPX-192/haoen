#include "AutoMeasureProcess.h"
#include "VisAppBus.h"
#include "PlcControl.h"
#include "VisMotorTool.h"
#include "VisMotorManager.h"

SINGLETON_IMPL(AutoMeasureProcess)
AutoMeasureProcess::AutoMeasureProcess(QObject *parent) : QObject(parent)
{
    VisAppBus::subscibeEvent(this, "AutoReset");
    VisAppBus::subscibeEvent(this, "AutoStart");
    VisAppBus::subscibeEvent(this, "AutoStop");
    VisAppBus::subscibeEvent(this, "AutoPause");
    VisAppBus::subscibeEvent(this, "AutoResume");
    VisAppBus::subscibeEvent(this, "ClearWarning");
    VisAppBus::subscibeEvent(this, "AutoEmg");
    VisAppBus::subscibeEvent(this, "NotifyTrayRequest");

    m_threadMonitor = QSharedPointer<ThreadMonitor>(new ThreadMonitor);
    m_threadWorkOrder = std::make_shared<ThreadWorkOrder>();
}

AutoMeasureProcess::~AutoMeasureProcess()
{

}

bool AutoMeasureProcess::InitRunParam()
{
    m_threadWorkOrder->InitParam();
    return true;
}

int AutoMeasureProcess::event_AutoReset()
{
    ShowSystemLog(Log_Info, QString(u8"设备开始复位"));
    if (!GlobalParam->flagOffline)
    {
        GlobalParam->SetMachineStatus(MachineRunStatus::Homed);
        if(0!=event_InitPlcData())
        {
            GlobalParam->SetMachineStatus(MachineRunStatus::NoHomed);
            return 0;
        }

        if (false == PlcControlIns.WritePLCReset())
        {
            ShowLog(SystemLog, 0, Log_Error, QString(u8"PLC复位信号设置失败"));
        }
    }
    else
    {
        GlobalParam->SetMachineStatus(MachineRunStatus::Idle);
    }

    return 0;
}

int AutoMeasureProcess::event_AutoStart()
{
    if (!PlcControlIns.GetIsHomed())
    {
        //        CMsgBox::showWarning(nullptr,QString::fromUtf8(u8"警告"), QString(u8"设备未复位，请先复位设备"));
        ShowSystemLog(Log_Info, QString(u8"设备未复位"));
    }
    ShowSystemLog(Log_Info, QString(u8"设备启动自动运行"));
    m_threadWorkOrder->start();
    PlcControlIns.WritePLCStart();
    ShowSystemLog(Log_Info, QString(u8"设备自动运行启动完成"));
    GlobalParam->SetMachineStatus(MachineRunStatus::AutoRunning);
    GlobalParam->AutoRunning=true;

    return 0;
}

int AutoMeasureProcess::event_AutoPause()
{
    ShowSystemLog(Log_Info, QString(u8"设备暂停自动运行"));
    m_threadWorkOrder->pause();
    GlobalParam->SetMachineStatus(MachineRunStatus::Pause);
    ShowSystemLog(Log_Info, QString(u8"设备自动运行暂停完成"));
    return 0;
}

int AutoMeasureProcess::event_AutoResume()
{
    ShowSystemLog(Log_Info, QString(u8"设备恢复自动运行"));
    GlobalParam->SetMachineStatus(MachineRunStatus::AutoRunning);
    m_threadWorkOrder->resume();
    ShowSystemLog(Log_Info, QString(u8"设备自动运行恢复完成"));
    return 0;
}

int AutoMeasureProcess::event_AutoStop()
{
    ShowSystemLog(Log_Info, QString(u8"设备停止自动运行"));
    m_threadWorkOrder->stop();
    PlcControlIns.WritePLCStop();

    ShowSystemLog(Log_Info, QString(u8"设备自动运行停止完成"));
    GlobalParam->SetMachineStatus(MachineRunStatus::Pause);
    GlobalParam->AutoRunning=false;

    return 0;
}

int AutoMeasureProcess::event_AutoEmg()
{
    ShowSystemLog(Log_Info, QString(u8"设备急停"));
    if (false == PlcControlIns.WritePLCEmgStop())
        ShowLog(SystemLog, 0, Log_Error, QString(u8"PLC急停信号设置失败"));

    ShowSystemLog(Log_Info, QString(u8"设备急停完成"));
    GlobalParam->SetMachineStatus(MachineRunStatus::NoHomed);
    GlobalParam->AutoRunning=false;
    return 0;
}

int AutoMeasureProcess::event_ClearWarning()
{
    if (false == PlcControlIns.ClearPLCWarning())
        ShowLog(SystemLog, 0, Log_Error, QString(u8"PLC报警清除失败"));

    ShowSystemLog(Log_Info, QString(u8"设备报警清除完成"));
    return 0;
}

void AutoMeasureProcess::StartMonitor()
{
    m_threadMonitor->start();
    return;
}

void AutoMeasureProcess::StopMonitor()
{
    m_threadMonitor->stop();
    return;
}

int AutoMeasureProcess::event_InitPlcData()
{
    ShowSystemLog(Log_Info, QString(u8"初始化电机参数"));
    VisMotorToolSpace::VisMotorInstance->InitUserParam();
    //初始化点位参数
    if (false == VisMotorToolSpace::VisMotorToolIns->SetPosToPLC())
    {
        ShowSystemLog(Log_Error, QString(u8"设置PLC点位失败"));
        return -1;
    }

    //初始化夹爪参数
    for(int i = 0;i<GripperCount;++i)
    {
        if (false == PlcControlIns.WriteD(GlobalParam->systemParam.gripperAddrParam[i].sGripperClampingForceAddr,GlobalParam->recipeElectricSaw.nGripperClampingForce))
        {
            ShowSystemLog(Log_Error, QString(u8"设置夹爪%1夹持力失败").arg(i+1));
            return -1;
        }
        if (false == PlcControlIns.WriteD(GlobalParam->systemParam.gripperAddrParam[i].sRotateForceAddr,GlobalParam->recipeElectricSaw.nRotateForce))
        {
            ShowSystemLog(Log_Error, QString(u8"设置夹爪%1旋转力失败").arg(i+1));
            return -1;
        }
    }

    //初始化托盘参数示例
    QStringList startAddr;
    startAddr<<"D1014"<<"D1054";
    for(int i=0;i<2;i++)
    {
        QVector<QVector4D>&feedTrayPos=GlobalParam->recipeTray.feedTrayPos[i];
        QVector<qint32> posData;
        for(int j=0;j<feedTrayPos.size();j++)
        {  //数据*对应轴脉冲当量
            posData.push_back(PlcControlIns.ConvertTiUnit(feedTrayPos.at(j).x(),Axis1_MotorGantryX));
            posData.push_back(PlcControlIns.ConvertTiUnit(feedTrayPos.at(j).y(),Axis2_MotorGantryY));
        }
        if (false == PlcControlIns.WritePageD(startAddr[i],posData.size(),posData)){
            ShowSystemLog(Log_Error, QString(u8"设置物料盘点位失败"));
            return -1;
        }
    }

    //NG盘
    //x"D1200",y"D1100"
    QVector<QVector4D>&ngTrayPos=GlobalParam->recipeTray.ngTrayPos;
    QVector<qint32> ngPosData;
    QString ngStartAddr = "D4600";
    for(int j=0;j<ngTrayPos.size();j++)
    {  //数据*对应轴脉冲当量
        ngPosData.push_back(PlcControlIns.ConvertTiUnit(ngTrayPos.at(j).x(),Axis1_MotorGantryX));
        ngPosData.push_back(PlcControlIns.ConvertTiUnit(ngTrayPos.at(j).y(),Axis2_MotorGantryY));
    }
    if (false == PlcControlIns.WritePageD(ngStartAddr,ngPosData.size(),ngPosData)){
        ShowSystemLog(Log_Error, QString(u8"设置NG盘点位失败"));
        return -1;
    }

    //切换单双目
    QString regName = "D9500";
    qint32  data = 1;//1:单，2：双
    PlcControlIns.WriteD(regName,data);

    return 0;
}

int AutoMeasureProcess::event_NotifyTrayRequest(bool isOpen)
{
    QString regName = "D9507";
    bool bRet = PlcControl::Instance().WriteML(regName, isOpen?1:0);
    if (!bRet) {
        QString errInfo = QString(u8"%1请求料盘:%2").arg(isOpen?u8"开启":u8"关闭").arg(regName);
        ShowSystemLog(Log_Error, errInfo);
        return -1;
    }
    return 0;
}
