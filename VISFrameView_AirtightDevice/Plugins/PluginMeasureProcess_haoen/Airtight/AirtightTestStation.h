#ifndef AIRTIGHTTESTSTATION_H
#define AIRTIGHTTESTSTATION_H

#include <QObject>
#include <atomic>
#include "hthread.h"
#include "AirtightDef.h"

// ============================================================
//  AirtightTestStation:气密测试工位(共 4 实例,独立 HThread)
//  每个工位负责:等待放料 → 物料检测 → Y 前后气缸进出气密仪
//            → 上模压紧 → 气密仪通信 → 上模松开 → 拉回
//            → 通知抓取龙门取回
//  气密仪通信由 DeviceManage 承担,本类只触发事件:
//    "StartAirtightTest"(触发) / "AirtightResultBack"(结果,DeviceManage 轮询后回调)
// ============================================================
class AirtightTestStation : public QObject, public HThread
{
    Q_OBJECT
public:
    explicit AirtightTestStation(int station, QObject *parent = nullptr);
    ~AirtightTestStation();

    void  InitParam();

protected:
    virtual bool doTask();
    bool  Process();

    int  CheckMaterial();                //物料检测(In_TestXMaterial)
    int  YForwardIntoTester();           //前后气缸伸出(Y 向,料送入气密仪)
    int  UpperMoldPress(bool up);        //上模压紧(true=伸出)
    int  RunAirtightTest();              //气密仪通信(发事件)
    int  YBackwardToStation();           //前后气缸缩回(料拉回测试位)

protected:
    const int m_station;
    QString   m_logType;
    std::atomic<bool> m_feedDone{false};   //放料完成标志(由 event_AirStationFeedDone 置位)
    std::atomic<bool> m_testDone{false};   //测试完成标志(由 event_AirtightResultBack 置位)

public slots:
    int  event_StartAirtightStation(int station);   //开始测试循环(按 station 过滤)
    int  event_StopAirtightStation(int station);    //停止(按 station 过滤)
    int  event_AirStationFeedDone(int station);     //抓取龙门放料完成,本工位可开始(按 station 过滤)
    int  event_AirtightResultBack(int station, bool ok, QString ngReason); //气密测试结果回调
};

#endif // AIRTIGHTTESTSTATION_H
