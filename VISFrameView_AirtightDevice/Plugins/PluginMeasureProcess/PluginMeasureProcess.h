#ifndef PLUGINMEASUREPROCESS_H
#define PLUGINMEASUREPROCESS_H

#include "VISFramePluginModel.h"

//流程管理类
class PluginMeasureProcess:public VISFramePluginModel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_Interface_iid FILE "QPlugin.json")
    Q_INTERFACES(Plugin_Interface)
public:
    PluginMeasureProcess();

public:
    //按照顺序依次调用
    virtual void InitSubscibeEvent(Plugin_Interface* plugin);
    virtual void InitActionList(Plugin_Interface* plugin);
    virtual void InitWidgetList(Plugin_Interface* plugin);

    //PluginConnected之后，登录界面显示前，此时系统只收集了插件
    virtual int  OnCoreInitialized();
    //OnCoreInitialized之后，登录界面显示结束，
    virtual int  OnInitialized();
    //当QMainWindow主视图构造完成后，显示前
    virtual int  OnViewCreated();
    //当QMainWindow主视图Closeing时要执行的过程
    virtual int  OnViewClosing();

protected slots:
    int  event_WritePagePLC(QString regName, int length, QVector<int>& data);
    int  event_ReadPagePLC(QString regName, int length, QVector<int>& data);
    int  event_ShowMessageBox(QString sMessage);
    int  event_PopupUserMsgBox(QStringList listBtn, QString info);
    int  event_WriteShieldToPlC(QMap<QString, QPair<bool*, QString>>shieldDataMap);
};

#endif // PLUGINMEASUREPROCESS_H
