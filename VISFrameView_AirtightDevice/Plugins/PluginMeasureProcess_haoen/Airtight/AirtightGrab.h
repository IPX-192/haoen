#ifndef AIRTIGHTGRAB_H
#define AIRTIGHTGRAB_H

#include <QObject>
#include <QMutex>
#include <QList>
#include "hthread.h"
#include "AirtightDef.h"

// ============================================================
//  AirtightGrab:抓取龙门(左/右各 1 实例,继承 HThread 独立线程)
//  负责(领导流程):料盘就位 → 等空闲工位(空闲=没在测气密:没上料或测试完成)
//     → 该空闲工位需下料则先下料 → 上料抓取 → 扫码 → 放置到该工位
//  上下料同一夹爪,一次循环最多下料一次,下完紧接着同一工位上料。

//    Axis0/3(400W) = X 横梁横移(覆盖料盘↔两个测试工位)
//    Axis1/4(200W) = Y 梁上滑块前后
//    Axis2/5(100W带刹车) = Z 垂直升降
// ============================================================
class AirtightGrab : public QObject, public HThread
{
    Q_OBJECT
public:
    explicit AirtightGrab(int grab, QObject *parent = nullptr);
    ~AirtightGrab();

    void  InitParam();          //初始化 + 启动线程

protected:
    virtual bool doTask();
    bool  Process();

    int   WaitConveyorReady();                  //等流线/上料盘到位
    int   GrabFromTray(int trayHole);           //从料盘第 trayHole 格抓料
    int   PlaceToStation(int station);          //放到测试工位 station
    int   GrabFromStation(int station);         //从测试工位 station 取回(已测完)
    int   BlankToTray(int station);             //放回料盘:OK→OK区空位 / NG→NG 料盘
    int   ScanCode(int station);                //扫码(产品条码,上料抓取后、放置前)

    int   MoveGrabSafe();                       //抓取龙门到安全位(Z 升 + X 中位 + Y 中位)
    int   MoveGrabTo(double xPos, double yPos); //X+Y 同步移动(伪坐标,平面定位)
    int   MoveGrabZ(double zPos);               //Z 单轴升降

protected:
    const int m_grab;       //AirGrabLeft / AirGrabRight
    QString   m_logType;    //日志分类
    QString   m_errInfo;

    int  m_pickNextHole   = 0;
    int  m_okTrayNextHole = 0;
    int  m_ngTrayNextHole = 0;

    //流程状态
    bool m_conveyorReady = false;        //流线/上料盘到位(占位)
    QMutex m_mutexStation;
    //待送料工位(从空→待料):放料后等测试工位开始
    QList<int> m_stationNeedFeed;
    //待取料工位(测试完成):抓取后等抓回
    QList<int> m_stationTested;
    //每个工位当前的物料结果
    QList<AirModuleInfo> m_stationInfo;

public slots:
    int  event_StartAirtightGrab(int grab);   //开始抓取循环(按 grab 过滤)
    int  event_StopAirtightGrab(int grab);    //停止(按 grab 过滤)
    int  event_AirConveyorReady(int grab);    //流线/上料盘到位通知(按 grab 过滤)
    int  event_AirStationReady(int station);  //测试工位空,允许抓取龙门送料(按工站过滤)
    int  event_AirtightResultBack(int station, bool ok, QString ngReason); //测试结果回调(按工站过滤)
};

#endif // AIRTIGHTGRAB_H
