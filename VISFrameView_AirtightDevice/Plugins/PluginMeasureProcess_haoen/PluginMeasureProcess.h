#ifndef PLUGINMEASUREPROCESS_H
#define PLUGINMEASUREPROCESS_H

#include "VISFramePluginModel.h"
#include "Airtight/AirtightDef.h"

// ============================================================
//  PluginMeasureProcess:气密流程总入口
//  维护 6 个并行线程:
//    左工站:AirtightGrab(0) + 2 个 AirtightTestStation(Test1, Test2)
//    右工站:AirtightGrab(1) + 2 个 AirtightTestStation(Test3, Test4)
//  负责:事件总线入口 + 协调抓取/测试工位 + 复位/急停/清料 + 启动链
// ============================================================
class AirtightGrab;
class AirtightTestStation;

class PluginMeasureProcess : public VISFramePluginModel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_Interface_iid FILE "QPlugin.json")
    Q_INTERFACES(Plugin_Interface)
public:
    PluginMeasureProcess();

public:
    virtual void InitSubscibeEvent(Plugin_Interface *plugin);
    virtual void InitActionList(Plugin_Interface *plugin);
    virtual void InitWidgetList(Plugin_Interface *plugin);
    virtual int  OnCoreInitialized();
    virtual int  OnInitialized();
    virtual int  OnViewCreated();
    virtual int  OnViewClosing();

protected slots:

    int  event_ShowMessageBox(QString sMessage);
    int  event_PopupUserMsgBox(QStringList listBtn, QString info);

    //气密流程启动/暂停/复位/急停
    int  event_AutoStart();
    int  event_AutoPause();
    int  event_AutoResume();
    int  event_AutoReset();
    int  event_AutoEmg();
    int  event_ClearModule();
    int  event_ClearWarning();

    //气密仪结果回调
    int  event_AirtightResultBack(int station, bool ok, QString ngReason);

protected:
    int  InitIO();            //初始化 IO(三色灯/三色灯蜂鸣 等)
    int  InitDevicePos();     //电机到初始点位
    int  CheckDevicePos();    //复位前位置检查
    void InitThread();        //建 6 个线程(2 grab + 4 station)

protected:
    AirtightGrab*          m_grab[AirGrabCount]   = {nullptr, nullptr};
    AirtightTestStation*   m_station[AirStationCount] = {nullptr, nullptr, nullptr, nullptr};

    bool  m_running  = false;
    bool  m_paused   = false;
    bool  m_resetting= false;
    bool  m_emergency= false;
};

#endif // PLUGINMEASUREPROCESS_H
