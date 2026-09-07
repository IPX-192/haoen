#ifndef CYLINDERCTRL_H
#define CYLINDERCTRL_H

#include <QObject>
#include "Singleton.h"
#include "ParamDef.h"
#include "AirtightDef.h"

// ============================================================
//  CylinderCtrl:气缸控制单例(参照撕膜上料 CylinderCtrl,精简)
//  每个方法 = 一个气缸动作 + 到位检测 + 超时报警
// ============================================================
class CylinderCtrl : public QObject
{
    Q_OBJECT
    SINGLETON_DECL(CylinderCtrl)
public:
    explicit CylinderCtrl(QObject *parent = nullptr);

    //基础原语
    int SetDoubleIOOut(const QString& outOn, int levelOn,
                       const QString& outOff, int levelOff);   //双气缸互斥动作
    int CheckIoState(const QString& ioName, int level, const QString& errInfo, int timeoutMs = 5000);
    int WaitIoLevel(const QString& ioName, int level, int timeoutMs); //简单同步等待,不弹错

public slots:
    //气密测试工位 4 种气缸动作
    int event_SetTestUpperMold(int station, bool up);     //上模压紧(true=伸出压紧 / false=缩回)
    int event_SetTestFrontBack(int station, bool forward);//前后进出气密仪(true=向前送进 / false=向后拉回)
    //抓取龙门夹爪(气动)
    int event_SetGrabGrip(int grab, bool close);          //抓取龙门夹爪闭合/张开

signals:
    //后续联调加错误信号
};

#endif // CYLINDERCTRL_H