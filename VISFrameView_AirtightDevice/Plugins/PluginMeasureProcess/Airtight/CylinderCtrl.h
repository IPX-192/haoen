#ifndef CYLINDERCTRL_H
#define CYLINDERCTRL_H

#include <QObject>
#include "Singleton.h"
#include "ParamDef.h"
#include "AirtightDef.h"


class CylinderCtrl : public QObject
{
    Q_OBJECT
    SINGLETON_DECL(CylinderCtrl)
public:
    explicit CylinderCtrl(QObject *parent = nullptr);

    int SetDoubleIOOut(const QString& outOn, int levelOn,
                       const QString& outOff, int levelOff);   //双气缸互斥动作
    int CheckIoState(const QString& ioName, int level, const QString& errInfo, int timeoutMs = 5000);
    int WaitIoLevel(const QString& ioName, int level, int timeoutMs);

public slots:

    int event_SetTestUpperMold(int station, bool up);     //上模压紧(true=伸出压紧 / false=缩回)
    int event_SetTestFrontBack(int station, bool forward);//前后进出气密仪(true=向前送进 / false=向后拉回)
    int event_SetGrabGrip(int grab, bool close);          //抓取龙门夹爪闭合/张开


};

#endif // CYLINDERCTRL_H
