#include "AirtightTestStation.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"
#include "VisAppBus.h"
#include "CylinderCtrl.h"
#include <QThread>
#include <thread>
#include <chrono>

using namespace VisMotorToolSpace;

AirtightTestStation::AirtightTestStation(int station, QObject *parent) :
    m_station(station),
    QObject(parent)
{
    m_logType = QString("Log_TestStation_%1").arg(station + 1);
    VisAppBus::subscibeEvent(this, "StartAirtightStation");
    VisAppBus::subscibeEvent(this, "StopAirtightStation");
    VisAppBus::subscibeEvent(this, "AirStationFeedDone");
    VisAppBus::subscibeEvent(this, "AirtightResultBack");
}

AirtightTestStation::~AirtightTestStation() { stop(); }

void AirtightTestStation::InitParam()
{
    m_feedDone = false;
    m_testDone = false;
    start();
}


// ============================================================
//  doTask / Process
// ============================================================
bool AirtightTestStation::doTask()
{
    if (VisMotorInstance->IsEmgStop()) return false;
    if (!Process()) {
        ShowSystemLog(Log_Error, QString(u8"测试工位%1 处理失败").arg(m_station + 1));
        return false;
    }
    return true;
}


// ============================================================
//  Process:测试工位主循环(顺序执行,参照 ThreadFeed 风格,不用状态机)
//  等放料 → 物料检测 → Y前 → 上模压紧 → 测试中(等结果) → 上模松 → Y后 → 通知取回
//
//  ★ 时序不变性 ★
//  [5] 退出靠 m_testDone 置位(由 event_AirtightResultBack 触发);
//  [8] 发 "AirStationReady" 在 [5] 之后,中间还要走 [6][7] 的 IO 动作。
//  因此:对抓取龙门而言,"AirtightResultBack" 必先于 "AirStationReady" 到达,
//  m_stationTested 必先于 m_stationNeedFeed 记录该工位。
//  重构本函数时请务必保持此顺序,否则 AirtightGrab::Process 会把
//  "工位有料未下"误判成 "工位空" → 直接上料 → 撞机/双料。
// ============================================================
bool AirtightTestStation::Process()
{
    //[1] 等待抓取龙门放料(m_feedDone 由 event_AirStationFeedDone 置位)
    while (1) {
        if (m_feedDone.load()) break;
        if (VisMotorInstance->IsEmgStop()) return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    m_feedDone = false;

    //[2] 物料检测(防呆)
    int nRes = CheckMaterial();
    if (nRes != 0) return false;

    //[3] Y向前(料送入气密仪)
    nRes = YForwardIntoTester();
    if (nRes != 0) return false;

    //[4] 上模压紧
    nRes = UpperMoldPress(true);
    if (nRes != 0) return false;

    //[5] 触发气密仪测试,等结果
    //    DeviceManage 回调 "AirtightResultBack"
    m_testDone = false;
    nRes = RunAirtightTest();
    if (nRes != 0) return false;
    while (1) {
        if (m_testDone.load()) break;
        if (VisMotorInstance->IsEmgStop()) return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    //[6] 上模松开
    nRes = UpperMoldPress(false);
    if (nRes != 0) return false;

    //[7] Y向后(料拉回测试位)
    nRes = YBackwardToStation();
    if (nRes != 0) return false;

    //[8] 通知抓取龙门:本工位已空,可送下一料
    VisAppBus::sendEvent("AirStationReady", m_station);

    return true;
}


// ============================================================
//  CheckMaterial:检测测试位物料(防呆)
//  In_TestXMaterial == IO_ON 表示有料
// ============================================================
int AirtightTestStation::CheckMaterial()
{
    ShowLog(m_logType, m_station, Log_Info,
            QString(u8"[测试工位%1] 检测物料").arg(m_station + 1));
    return CylinderCtrl::instance()->CheckIoState(
                AirTestInMaterial(m_station), IO_ON,
                QString(u8"测试工位%1 物料检测超时").arg(m_station + 1),
                3000);
}


// ============================================================
//  YForwardIntoTester:Y向前(气缸伸出,料送进气密仪)
// ============================================================
int AirtightTestStation::YForwardIntoTester()
{
    ShowLog(m_logType, m_station, Log_Info,
            QString(u8"[测试工位%1] Y向前(送入气密仪)").arg(m_station + 1));
    return CylinderCtrl::instance()->event_SetTestFrontBack(m_station, true);
}


// ============================================================
//  UpperMoldPress:上模压紧/松开
// ============================================================
int AirtightTestStation::UpperMoldPress(bool up)
{
    ShowLog(m_logType, m_station, Log_Info,
            QString(u8"[测试工位%1] 上模%2").arg(m_station + 1).arg(up ? u8"压紧" : u8"松开"));
    return CylinderCtrl::instance()->event_SetTestUpperMold(m_station, up);
}


// ============================================================
//  RunAirtightTest:触发气密仪通信(由 DeviceManage 接管)
//  实际协议调用由 CModbusClient 完成(TCP,左工站 通道1/2,右工站 通道1/2)
//  此处只发事件 "StartAirtightTest"(DeviceManage 订阅,按 station 映射设备/通道);
//  结果由 DeviceManage 轮询后回调 "AirtightResultBack" 返回
// ============================================================
int AirtightTestStation::RunAirtightTest()
{
    ShowLog(m_logType, m_station, Log_Info,
            QString(u8"[测试工位%1] 启动气密仪测试").arg(m_station + 1));

    VisAppBus::sendEvent("StartAirtightTest", m_station);
    return 0;
}


// ============================================================
//  YBackwardToStation:Y向后(气缸缩回,料拉回测试位)
// ============================================================
int AirtightTestStation::YBackwardToStation()
{
    ShowLog(m_logType, m_station, Log_Info,
            QString(u8"[测试工位%1] Y向后(拉回测试位)").arg(m_station + 1));
    return CylinderCtrl::instance()->event_SetTestFrontBack(m_station, false);
}


int AirtightTestStation::event_StartAirtightStation(int station)
{
     //工位判断
    if (station != m_station) return 0;
    //本工位空闲,通知抓取龙门可送料
    VisAppBus::sendEvent("AirStationReady", m_station);
    return 0;
}

int AirtightTestStation::event_StopAirtightStation(int station)
{
    if (station != m_station) return 0;
    /* TODO: 停止当前工位(等当前步结束后退出) */
    return 0;
}

int AirtightTestStation::event_AirStationFeedDone(int station)
{
    if (station != m_station) return 0;
    m_feedDone = true;
    return 0;
}

int AirtightTestStation::event_AirtightResultBack(int station, bool ok, QString ngReason)
{
    Q_UNUSED(ok); Q_UNUSED(ngReason);
    if (station != m_station) return 0;
    m_testDone = true;   //测试完成
    return 0;
}
