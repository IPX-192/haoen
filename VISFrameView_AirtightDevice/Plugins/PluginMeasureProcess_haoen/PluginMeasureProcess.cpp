#include "PluginMeasureProcess.h"
#include <QProcess>
#include "NonBlockingMsgBox.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"
#include "VisAppBus.h"
#include "Airtight/AirtightGrab.h"
#include "Airtight/AirtightTestStation.h"
#include "Airtight/CylinderCtrl.h"

using namespace VisMotorToolSpace;

PluginMeasureProcess::PluginMeasureProcess()
{
    pluginID = "PluginMeasureProcess";
    pluginVersion = "1.0.1";
    pluginAuther = "wangwei";
    pluginAuthority = OPERATOR;

    //初始化气缸控制单例
    CylinderCtrl::instance();
}


// ============================================================
//  框架事件初始化
// ============================================================
void PluginMeasureProcess::InitSubscibeEvent(Plugin_Interface *plugin)
{
    VisAppBus::subscibeEvent(this, "ShowMessageBox");
    VisAppBus::subscibeEvent(this, "PopupUserMsgBox");

    //流程启停
    VisAppBus::subscibeEvent(this, "AutoStart");
    VisAppBus::subscibeEvent(this, "AutoPause");
    VisAppBus::subscibeEvent(this, "AutoResume");
    VisAppBus::subscibeEvent(this, "AutoReset");
    VisAppBus::subscibeEvent(this, "AutoEmg");
    VisAppBus::subscibeEvent(this, "ClearModule");
    VisAppBus::subscibeEvent(this, "ClearWarning");

    //气密测试结果回调(由 DeviceManage/CModbusClient 触发;主类仅记日志,抓取/工位自行订阅)
    VisAppBus::subscibeEvent(this, "AirtightResultBack");
}

void PluginMeasureProcess::InitActionList(Plugin_Interface *plugin)  {}
void PluginMeasureProcess::InitWidgetList(Plugin_Interface *plugin) {}

int PluginMeasureProcess::OnCoreInitialized() { return 0; }


// ============================================================
//  OnInitialized:启动时建 6 个线程(2 grab + 4 station)
// ============================================================
int PluginMeasureProcess::OnInitialized()
{
    GlobalParam->SetMachineStatus(MachineRunStatus::NoHomed);
    InitThread();
    return 0;
}

int PluginMeasureProcess::OnViewCreated()  { return 0; }
int PluginMeasureProcess::OnViewClosing()  { return 0; }


// ============================================================
//  InitThread:建 2 个抓取 + 4 个测试工位线程
// ============================================================
void PluginMeasureProcess::InitThread()
{
    //[1] 2 个抓取龙门(左/右)
    for (int g = 0; g < AirGrabCount; ++g) {
        if (!m_grab[g])
            m_grab[g] = new AirtightGrab(g, this);
        m_grab[g]->InitParam();
    }

    //[2] 4 个测试工位(Test1-4)
    for (int s = 0; s < AirStationCount; ++s) {
        if (!m_station[s])
            m_station[s] = new AirtightTestStation(s, this);
        m_station[s]->InitParam();
    }
}


// ============================================================
//  IO / 位置 初始化(伪:具体坐标后续填)
// ============================================================
int PluginMeasureProcess::InitIO()
{
    //三色灯:绿亮表示就绪
    VisMotorInstance->SetIoOutput(Out_TowerGreen,  IO_ON);
    VisMotorInstance->SetIoOutput(Out_TowerYellow, IO_OFF);
    VisMotorInstance->SetIoOutput(Out_TowerRed,    IO_OFF);
    VisMotorInstance->SetIoOutput(Out_TowerBuzzer, IO_OFF);

    //[TODO] 后续:料盘夹紧初始化 / 输送线上下电 / 顶升缩回 等
    //  VisMotorInstance->SetDoubleIoOutput(Out_Tray1Clamp, IO_ON, ...);
    return 0;
}

int PluginMeasureProcess::InitDevicePos()
{
    //[TODO] 联调:把 6 轴(Axis0-5) + 6 个气缸(各工位上模/前后)全移到初始位
    //  当前仅伪代码:MoveAbsGroup(空 QMap)
    QMap<QString, double> mapGroup;
    return VisMotorInstance->MoveAbsGroup(mapGroup);
}

int PluginMeasureProcess::CheckDevicePos()
{
    //[TODO] 联调:检查各轴当前位置是否安全(不会撞机)
    return 0;
}


// ============================================================
//  event_AutoStart:启动流程
//  链:CheckDevicePos → InitIO → InitDevicePos → 通知各线程开始
// ============================================================
int PluginMeasureProcess::event_AutoStart()
{
    if (VisMotorInstance->GetIoInput(In_Emg) == IO_OFF && !GlobalParam->flagOffline) {
        QMessageBox::warning(nullptr, u8"警告", u8"急停按钮未复位,无法启动");
        return -1;
    }
    if (!VisMotorInstance->IsResetted() && !VisMotorDataInstance->m_flagOffline) {
        ShowSystemLog(Log_Error, u8"设备未复位,不能开始生产");
        return -1;
    }
    if (QMessageBox::question(nullptr, u8"询问", u8"是否启动自动模式?") == QMessageBox::No)
        return -1;

    if (CheckDevicePos() != 0) return -1;
    if (InitIO() != 0)         return -1;
    if (InitDevicePos() != 0)  return -1;

    m_running = true;
    VisMotorDataInstance->m_running = true;

    //通知 6 个线程开始
    for (int g = 0; g < AirGrabCount; ++g)
        if (m_grab[g]) VisAppBus::sendEvent("StartAirtightGrab", g);
    for (int s = 0; s < AirStationCount; ++s)
        if (m_station[s]) VisAppBus::sendEvent("StartAirtightStation", s);

    ShowSystemLog(Log_Info, u8"气密流程已进入生产模式");
    return 0;
}


// ============================================================
//  event_AutoPause / Resume / Reset / Emg / ClearModule / ClearWarning
//  伪:具体走 VisMotorInstance->SetPause/MotorStop/SetIoOutput 等
// ============================================================
int PluginMeasureProcess::event_AutoPause()
{
    m_paused = true;
    VisMotorInstance->SetPause();
    VisMotorInstance->SetIoOutput(Out_TowerYellow, IO_ON);
    VisMotorInstance->SetIoOutput(Out_TowerGreen,  IO_OFF);
    return 0;
}

int PluginMeasureProcess::event_AutoResume()
{
    m_paused = false;
    VisMotorInstance->ResumeMove();
    VisMotorInstance->SetIoOutput(Out_TowerYellow, IO_OFF);
    VisMotorInstance->SetIoOutput(Out_TowerGreen,  IO_ON);
    return 0;
}

int PluginMeasureProcess::event_AutoReset()
{
    if (m_resetting) return 0;
    m_resetting = true;

    //[1] 电机使能
    if (!VisMotorInstance->OpenMotorEnable())  { m_resetting = false; return -1; }
    if (!VisMotorInstance->WaitMotorEnable())  { m_resetting = false; return -1; }

    //[2] 检查安全位置
    if (CheckDevicePos() != 0) { m_resetting = false; return -1; }

    //[3] 所有轴回零
    if (!VisMotorInstance->MakeHomeAllAxis())  { m_resetting = false; return -1; }

    //[4] 初始化 IO + 电机到初始位
    InitIO();
    InitDevicePos();

    //[5] 通知 6 个线程 Init(复位后状态清零)
    InitThread();

    m_resetting = false;
    ShowSystemLog(Log_Info, u8"气密设备复位完成");
    return 0;
}

int PluginMeasureProcess::event_AutoEmg()
{
    if (m_emergency) return 0;
    m_emergency = true;
    m_running = false;
    VisMotorDataInstance->m_running = false;

    VisMotorInstance->SetIoOutput(Out_TowerRed,    IO_ON);
    VisMotorInstance->SetIoOutput(Out_TowerYellow, IO_OFF);
    VisMotorInstance->SetIoOutput(Out_TowerGreen,  IO_OFF);
    VisMotorInstance->SetIoOutput(Out_TowerBuzzer, IO_ON);
    VisMotorInstance->SetEmgStop(true);
    VisMotorInstance->MotorStop();

    //[TODO] 6 个线程进入急停态(HThread 收到 IsEmgStop=true 后 doTask 返回 false,自然退出)
    ShowSystemLog(Log_Error, u8"气密设备急停");
    return 0;
}

int PluginMeasureProcess::event_ClearModule()
{
    ShowSystemLog(Log_Info, u8"开始清料");
    //[TODO] 清料流程:把所有线程状态重置,清空 m_stationNeedFeed/ m_stationTested 队列
    for (int g = 0; g < AirGrabCount; ++g)
        if (m_grab[g]) { /* m_grab[g]->event_SetClearFlag(); */ }
    ShowSystemLog(Log_Info, u8"清料完成");
    return 0;
}

int PluginMeasureProcess::event_ClearWarning()
{
    m_emergency = false;
    VisMotorInstance->SetEmgStop(false);
    VisMotorInstance->SetIoOutput(Out_TowerBuzzer, IO_OFF);
    if (m_running) {
        VisMotorInstance->SetIoOutput(Out_TowerYellow, IO_OFF);
        VisMotorInstance->SetIoOutput(Out_TowerGreen,  IO_ON);
    } else {
        VisMotorInstance->SetIoOutput(Out_TowerYellow, IO_ON);
        VisMotorInstance->SetIoOutput(Out_TowerGreen,  IO_OFF);
    }
    VisMotorInstance->SetIoOutput(Out_TowerRed, IO_OFF);
    return 0;
}


int PluginMeasureProcess::event_AirtightResultBack(int station, bool ok, QString ngReason)
{
    ShowSystemLog(Log_Info,
                  QString(u8"[主类] 气密结果回调:工位%1 %2 NG原因=%3")
                      .arg(station + 1)
                      .arg(ok ? u8"OK" : u8"NG")
                      .arg(ngReason));

    //抓取龙门(AirtightGrab)与测试工位(AirtightTestStation)自行订阅 "AirtightResultBack":
    //  抓取龙门:记录结果 + 入队待取回,取回后 BlankToTray 按 OK/NG 分流
    //  测试工位:置 m_testDone,推进 Testing → PReturn
    return 0;
}


int PluginMeasureProcess::event_ShowMessageBox(QString sMessage)
{
    ShowSystemLog(Log_Info, sMessage);
    auto res = MsgBoxManager::instance().showInfo(nullptr, QString(u8"提示信息"), sMessage, NonBlockingMsgBox::Ok | NonBlockingMsgBox::Cancel);
    return (NonBlockingMsgBox::Ok == res);
}

int PluginMeasureProcess::event_PopupUserMsgBox(QStringList listBtn, QString info)
{
    ShowSystemLog(Log_Info, info);
    int btnClick = MsgBoxManager::instance().showWarning(nullptr, QString(u8"提示信息"), info, listBtn);
    return btnClick;
}
