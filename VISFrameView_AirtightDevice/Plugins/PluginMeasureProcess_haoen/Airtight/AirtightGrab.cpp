#include "AirtightGrab.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"
#include "VisAppBus.h"
#include "CylinderCtrl.h"
#include <QThread>
#include <thread>
#include <chrono>

using namespace VisMotorToolSpace;

AirtightGrab::AirtightGrab(int grab, QObject *parent) :
    m_grab(grab),
    QObject(parent)
{
    m_logType = QString("Log_AirtightGrab_%1").arg(grab + 1);
    VisAppBus::subscibeEvent(this, "StartAirtightGrab");
    VisAppBus::subscibeEvent(this, "StopAirtightGrab");
    VisAppBus::subscibeEvent(this, "AirConveyorReady");
    VisAppBus::subscibeEvent(this, "AirStationReady");
    VisAppBus::subscibeEvent(this, "AirtightResultBack");
}

AirtightGrab::~AirtightGrab() { stop(); }

void AirtightGrab::InitParam()
{
    m_pickNextHole   = 0;    //下一个要抓的待测格
    m_okTrayNextHole = 0;    //下一个要放 OK 的空格
    m_ngTrayNextHole = 0;    //下一个要放 NG 的空格
    m_conveyorReady  = false;
    m_mutexStation.lock();
    m_stationNeedFeed.clear();
    m_stationTested.clear();
    m_stationInfo.clear();
    for (int i = 0; i < 2; ++i)
        m_stationInfo.append(AirModuleInfo());
    m_mutexStation.unlock();

    start();
}


// ============================================================
//  doTask:线程主入口
// ============================================================
bool AirtightGrab::doTask()
{
    if (VisMotorInstance->IsEmgStop()) return false;
    if (!Process()) {
        if (!m_errInfo.isEmpty()) ShowSystemLog(Log_Error, m_errInfo);
        return false;
    }
    return true;
}


// ============================================================
//  Process:抓取主循环(领导流程)
//  1 料盘就位
//  2 等待空闲工位(空闲 = 没在测气密:没上料 或 测试完成)
//  3 该空闲工位是否需要下料
//  4 是 → 下料抓取 + 放回托盘;否 → 直接上料
//  5 上料抓取托盘产品 → 扫码 → 放置到该空闲工位,置为不空闲
//  关键:上下料同一夹爪,一次循环最多下料一次,下完紧接着同一工位上料
//
//  ★ 时序前提(由 AirtightTestStation::Process 的 [5]→[8] 严格顺序保证)★
//  稳态运行时,工位进 m_stationNeedFeed 之前,必然已记录在 m_stationTested 中
//  (AirtightResultBack 必先到);所以"needFeed 有 + tested 有 = 必下料再上料"。
//  唯一例外:启动初始空工位(event_StartAirtightStation 触发 AirStationReady,
//  m_stationTested 刚 InitParam clear 过),needUnload=false 直接上料是正确
//  行为 —— 此时工位本来就没料,不是"误判"。
//  若 AirtightTestStation 那边时序被改,需要重新审视这里的判断条件。
// ============================================================
bool AirtightGrab::Process()
{
    //[1] 料盘就位
    if (WaitConveyorReady() != 0) return true;   //未就位,本轮跳过

    //[2][3] 等待空闲工位,并判断它是否需要下料
    //       空闲工位 = m_stationNeedFeed(工位发 AirStationReady 入队)
    //       需要下料 = 该工位在 m_stationTested 里(测试完成,结果已回)
    int  station    = -1;
    bool needUnload = false;
    m_mutexStation.lock();
    if (!m_stationNeedFeed.isEmpty()) {
        station = m_stationNeedFeed.takeFirst();
        if (m_stationTested.contains(station)) {
            m_stationTested.removeAll(station);
            needUnload = true;
        }
    }
    m_mutexStation.unlock();

    if (station < 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return true;
    }

    int nRes = 0;

    //[4] 需要下料 → 下料抓取 + 放回托盘(OK→料盘空格 / NG→NG盘)
    if (needUnload) {
        nRes = GrabFromStation(station);
        if (nRes != 0) {
            m_errInfo = QString(u8"取回工位%1物料失败").arg(station + 1);
            return false;
        }
        nRes = BlankToTray(station);
        if (nRes != 0) {
            m_errInfo = QString(u8"工位%1物料放回料盘失败").arg(station + 1);
            return false;
        }
    }

    //[5] 上料抓取托盘产品(待测格)
    nRes = GrabFromTray(m_pickNextHole++);
    if (nRes != 0) return false;

    //[6] 扫码
    nRes = ScanCode(station);
    if (nRes != 0) return false;

    //[7] 放置到该空闲工位,置为不空闲(通知工位开始测试)
    nRes = PlaceToStation(station);
    if (nRes != 0) return false;
    VisAppBus::sendEvent("AirStationFeedDone", station);

    return true;
}


// ============================================================
//  WaitConveyorReady:等待流线/上料盘到位(占位:用 IO 触发)
// ============================================================
int AirtightGrab::WaitConveyorReady()
{
    if (m_conveyorReady) return 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 1;
}


// ============================================================
//  GrabFromTray:从料盘第 hole 格抓料(待测满料格)
//  轴运动:Z升到安全 → X+Y 到料盘抓取位 → Z下降到抓取位 → 夹爪闭合 → Z升
// ============================================================
int AirtightGrab::GrabFromTray(int hole)
{
    ShowLog(m_logType, m_grab, Log_Info,
            QString(u8"[抓取龙门%1] 开始抓料:料盘第%2格(待测)").arg(m_grab + 1).arg(hole + 1));

    //[1] 抓取龙门到安全位
    if (MoveGrabSafe() != 0) return -1;

    //[2] X+Y 同步移动到料盘抓取位(伪坐标,从配方取)
    //    QMap<QString,double> mapGroup;
    //    mapGroup[AirGrabAxisX(m_grab)] = 料盘X坐标(hole);
    //    mapGroup[AirGrabAxisY(m_grab)] = 料盘Y坐标(hole);
    //    VisMotorInstance->MoveAbsGroup(mapGroup);
    if (MoveGrabTo(/*xPos=*/0.0, /*yPos=*/0.0) != 0) return -1;

    //[3] Z 下降到料盘抓取位(单轴)
    if (MoveGrabZ(/*zPos=*/0.0) != 0) return -1;

    //[4] 抓取龙门夹爪闭合
    if (CylinderCtrl::instance()->event_SetGrabGrip(m_grab, true) != 0) return -1;

    //[5] 夹取二次检测(防漏抓):查夹爪闭合到位 IO
    //    VisMotorInstance->GetIoInput(AirGrabGripClose(m_grab)+"_chk");
    //    —— 实际 IO 名待 FeildDefine.h 补齐

    //[6] Z 升到安全位
    if (MoveGrabZ(/*zSafe=*/0.0) != 0) return -1;

    return 0;
}


// ============================================================
//  PlaceToStation:放到测试工位 station
//  轴运动:Z安全 → X+Y 到工位 → Z下降到放料位 → 夹爪张开 → Z升到安全
// ============================================================
int AirtightGrab::PlaceToStation(int station)
{
    ShowLog(m_logType, m_grab, Log_Info,
            QString(u8"[抓取龙门%1] 放料到测试工位%2").arg(m_grab + 1).arg(station + 1));

    if (MoveGrabSafe() != 0) return -1;
    if (MoveGrabTo(/*xPos=*/0.0, /*yPos=*/0.0) != 0) return -1;
    if (MoveGrabZ(/*zPos=*/0.0) != 0) return -1;
    if (CylinderCtrl::instance()->event_SetGrabGrip(m_grab, false) != 0) return -1;
    if (MoveGrabZ(/*zSafe=*/0.0) != 0) return -1;

    return 0;
}


// ============================================================
//  GrabFromStation:从测试工位取回(检测完成)
//  轴运动:同 PlaceToStation 的反向(无放料动作,只闭合夹爪)
// ============================================================
int AirtightGrab::GrabFromStation(int station)
{
    ShowLog(m_logType, m_grab, Log_Info,
            QString(u8"[抓取龙门%1] 取回测试工位%2物料").arg(m_grab + 1).arg(station + 1));

    if (MoveGrabSafe() != 0) return -1;
    if (MoveGrabTo(/*xPos=*/0.0, /*yPos=*/0.0) != 0) return -1;
    if (MoveGrabZ(/*zPos=*/0.0) != 0) return -1;
    if (CylinderCtrl::instance()->event_SetGrabGrip(m_grab, true) != 0) return -1;
    if (MoveGrabZ(/*zSafe=*/0.0) != 0) return -1;

    return 0;
}


// ============================================================
//  BlankToTray:放回料盘 —— OK → 料盘 OK 区第 N 个空格子 / NG → NG 料盘第 N 个格子
// ============================================================
int AirtightGrab::BlankToTray(int station)
{
    //查 m_stationInfo[AirSubOfStation(station)] 取得结果(OK/NG)
    m_mutexStation.lock();
    AirModuleInfo info = m_stationInfo[AirSubOfStation(station)];
    m_mutexStation.unlock();

    int hole = 0;
    QString trayName;
    if (info.result) {
        //OK 料放回同一个料盘的空格区:放第 m_okTrayNextHole 格,放完 +1
        hole = m_okTrayNextHole++;
        trayName = u8"OK 料盘";
    } else {
        //NG 料放独立 NG 料盘:放第 m_ngTrayNextHole 格,放完 +1
        hole = m_ngTrayNextHole++;
        trayName = u8"NG 料盘";
    }
    ShowLog(m_logType, m_grab, Log_Info,
            QString(u8"[抓取龙门%1] 放料到%2第%3格 (station=%4, NG原因=%5)")
                .arg(m_grab + 1).arg(trayName).arg(hole + 1)
                .arg(station + 1).arg(info.result ? u8"-" : info.ngReason));

    if (MoveGrabSafe() != 0) return -1;
    if (MoveGrabTo(/*xPos=*/0.0, /*yPos=*/0.0) != 0) return -1;
    if (MoveGrabZ(/*zPos=*/0.0) != 0) return -1;
    if (CylinderCtrl::instance()->event_SetGrabGrip(m_grab, false) != 0) return -1;
    if (MoveGrabZ(/*zSafe=*/0.0) != 0) return -1;

    return 0;
}


// ============================================================
//  ScanCode:扫码(产品条码,上料抓取后、放置前)
//  伪代码:带料移到扫码位 → 相机拍照 → 条码/DataMatrix 识别 → 校验
// ============================================================
int AirtightGrab::ScanCode(int station)
{
    ShowLog(m_logType, m_grab, Log_Info,
            QString(u8"[抓取龙门%1] 扫码(产品条码,目标工位%2)").arg(m_grab + 1).arg(station + 1));

    //TODO: ① MoveGrabTo(扫码位X/Y) ② VisCameraTool 抓图 ③ 条码/DataMatrix 识别
    //      ④ 条码存 m_stationInfo[AirSubOfStation(station)].barCode(需在 AirModuleInfo 加字段)
    //      ⑤ 需要 MES 校验时发 MesValidateNumber,失败置 result=false
    return 0;
}


// ============================================================
//  MoveGrabSafe:抓取龙门到安全位(Z 升 + X 中位 + Y 中位,3 轴同步)
// ============================================================
int AirtightGrab::MoveGrabSafe()
{
    // QMap<QString,double> mapGroup;
    // mapGroup[AirGrabAxisZ(m_grab)] = 安全Z;
    // mapGroup[AirGrabAxisX(m_grab)] = 安全X;
    // mapGroup[AirGrabAxisY(m_grab)] = 安全Y;
    // return VisMotorInstance->MoveAbsGroup(mapGroup);
    return VisMotorInstance->MoveAbsGroup(QMap<QString,double>()); //伪:空 map
}

int AirtightGrab::MoveGrabTo(double xPos, double yPos)
{
    QMap<QString,double> mapGroup;
    mapGroup[AirGrabAxisX(m_grab)] = xPos;
    mapGroup[AirGrabAxisY(m_grab)] = yPos;
    return VisMotorInstance->MoveAbsGroup(mapGroup);
}

int AirtightGrab::MoveGrabZ(double zPos)
{
    return VisMotorInstance->MotorMoveAbs(AirGrabAxisZ(m_grab), zPos);
}


// ============================================================
//  总线事件槽(槽名 = event_ + 事件名)
// ============================================================
int AirtightGrab::event_StartAirtightGrab(int grab)
{
    if (grab != m_grab) return 0;   //左右两个龙门各 1 实例订阅同一事件,按 grab 过滤
    /* TODO: 置启动标志 */
    return 0;
}

int AirtightGrab::event_StopAirtightGrab(int grab)
{
    if (grab != m_grab) return 0;
    /* TODO: 置停止标志 */
    return 0;
}

int AirtightGrab::event_AirConveyorReady(int grab)
{
    if (grab != m_grab) return 0;
    m_conveyorReady = true;
    return 0;
}

int AirtightGrab::event_AirStationReady(int station)
{
    //只处理本工站的工位(左=0/1,右=2/3)
    if (AirSideOfStation(station) != m_grab) return 0;
    m_mutexStation.lock();
    if (!m_stationNeedFeed.contains(station))
        m_stationNeedFeed.append(station);
    m_mutexStation.unlock();
    return 0;
}

int AirtightGrab::event_AirtightResultBack(int station, bool ok, QString ngReason)
{
    //只处理本工站的工位
    if (AirSideOfStation(station) != m_grab) return 0;
    m_mutexStation.lock();
    int sub = AirSubOfStation(station);
    if (sub >= 0 && sub < m_stationInfo.size()) {
        m_stationInfo[sub].station  = station;
        m_stationInfo[sub].result   = ok;
        m_stationInfo[sub].ngReason = ngReason;
    }
    if (!m_stationTested.contains(station))
        m_stationTested.append(station);
    m_mutexStation.unlock();
    return 0;
}

