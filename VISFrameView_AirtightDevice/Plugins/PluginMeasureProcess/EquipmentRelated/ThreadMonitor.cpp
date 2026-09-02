#include "ThreadMonitor.h"
#include "PlcControl.h"
#include <QDebug>
ThreadMonitor::ThreadMonitor(QObject *parent) : QObject(parent)
{
   setSleepPolicy(SLEEP_FOR,200);
}

ThreadMonitor::~ThreadMonitor()
{

}

bool ThreadMonitor::doTask()
{
    ScanPlcIsHome();
    ScanPlcRun();
    return true;
}

void ThreadMonitor::ScanPlcIsHome()
{
    if(GlobalParam->flagOffline)return;

    if(!PlcControl::Instance().GetIsHomed())
    {
         if(PlcControl::Instance().ReadResetStatus())
         {
             PlcControl::Instance().SetHomeStatus(true);
         }
    }

    return;
}


void ThreadMonitor::ScanPlcRun()
{
    if(GlobalParam->flagOffline)return;
    if (GlobalParam->AutoRunning) return;
    int nStatus = -999;
    if (!PlcControlIns.ReadAutoMode(nStatus)) return;
    //自动模式
    if (nStatus == 1)
    {
        ShowSystemLog(Log_Info, QString::fromUtf8("检测到PLC设备启动运行"));
        VisAppBus::sendEvent("UpdateDevStatus",MachineRunStatus::AutoRunning);
        VisAppBus::sendEvent("AutoStart");
    }

    return;
}
