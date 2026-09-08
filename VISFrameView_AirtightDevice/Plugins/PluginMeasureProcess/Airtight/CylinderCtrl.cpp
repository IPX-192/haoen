#include "CylinderCtrl.h"
#include "VisMotorManager.h"
#include "VisAppBus.h"
#include "VisMotorToolData.h"

using namespace VisMotorToolSpace;

CylinderCtrl::CylinderCtrl(QObject *parent) : QObject(parent) {}


int CylinderCtrl::SetDoubleIOOut(const QString& outOn, int levelOn,
                                 const QString& outOff, int levelOff)
{
    int nRes = 0;
    if (!outOff.isEmpty()) {
        nRes = VisMotorInstance->SetIoOutput(outOff, (IOLevel)levelOff);
        if (nRes != 0) return nRes;
    }
    if (!outOn.isEmpty()) {
        nRes = VisMotorInstance->SetIoOutput(outOn, (IOLevel)levelOn);
        if (nRes != 0) return nRes;
    }
    return 0;
}

//同步等待 IO 到位,带超时
int CylinderCtrl::CheckIoState(const QString& ioName, int level, const QString& errInfo, int timeoutMs)
{
    int waitMs = 0;
    while (waitMs < timeoutMs) {
        if (VisMotorInstance->IsEmgStop()) return HardWareErr;
        if ((int)VisMotorInstance->GetIoInput(ioName) == level) return 0;
        QThread::msleep(20);
        waitMs += 20;
    }
    ShowSystemLog(Log_Error, errInfo.isEmpty() ? QString(u8"IO %1 等待 level=%2 超时").arg(ioName).arg(level) : errInfo);
    return -1;
}

int CylinderCtrl::WaitIoLevel(const QString& ioName, int level, int timeoutMs)
{
    int waitMs = 0;
    while (waitMs < timeoutMs) {
        if ((int)VisMotorInstance->GetIoInput(ioName) == level) return 0;
        QThread::msleep(20);
        waitMs += 20;
    }
    return -1;
}


// ============================================================
//  上模:伸出(IO_ON) ↔ 缩回(IO_OFF),到位检测 = 输入点同侧
// ============================================================
int CylinderCtrl::event_SetTestUpperMold(int station, bool up)
{
    QString outOn  = up ? AirTestOutUpperMoldExt(station) : AirTestOutUpperMoldRet(station);
    QString inChk  = up ? AirTestOutUpperMoldExt(station) : AirTestOutUpperMoldRet(station); //伪代码:到位检测点后续按 FeildDefine 补 In_Xxx
    int nRes = VisMotorInstance->SetDoubleIoOutput(outOn, IO_ON, QString(), IO_OFF);
    if (nRes != 0) return nRes;
    return CheckIoState(inChk, IO_ON,
                         QString(u8"测试工位%1上模%2 到位超时").arg(station + 1).arg(up ? u8"伸出" : u8"缩回"));
}


// ============================================================
//  前后气缸:向前(IO_ON,Out_...FrontBackExtend) ↔ 向后(IO_OFF,Out_...FrontBackRetract)
//  到位检测:前 = In_TestXFront / 后 = In_TestXBack
// ============================================================
int CylinderCtrl::event_SetTestFrontBack(int station, bool forward)
{
    int nRes = 0;
    if (forward) {
        nRes = VisMotorInstance->SetDoubleIoOutput(AirTestOutFrontBackExt(station), IO_ON,
                                                   AirTestOutFrontBackRet(station), IO_OFF);
        if (nRes != 0) return nRes;
        return CheckIoState(AirTestInFront(station), IO_ON,
                            QString(u8"测试工位%1向前(Y送入)到位超时").arg(station + 1));
    }
    else {
        nRes = VisMotorInstance->SetDoubleIoOutput(AirTestOutFrontBackRet(station), IO_ON,
                                                   AirTestOutFrontBackExt(station), IO_OFF);
        if (nRes != 0) return nRes;
        return CheckIoState(AirTestInBack(station), IO_ON,
                            QString(u8"测试工位%1向后(Y拉回)到位超时").arg(station + 1));
    }
}


// ============================================================
//  抓取龙门夹爪(气动)
//  暂用占位 IO 名，待完善
// ============================================================
int CylinderCtrl::event_SetGrabGrip(int grab, bool close)
{
    QString outOn  = close ? AirGrabGripClose(grab) : AirGrabGripOpen(grab);
    QString outOff = close ? AirGrabGripOpen(grab)  : AirGrabGripClose(grab);
    return VisMotorInstance->SetDoubleIoOutput(outOn, IO_ON, outOff, IO_OFF);
}
