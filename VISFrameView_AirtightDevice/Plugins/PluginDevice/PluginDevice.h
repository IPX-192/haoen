#ifndef PLUGINDEVICE_H
#define PLUGINDEVICE_H

#include "VISFramePluginModel.h"

class PluginDevice:public VISFramePluginModel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_Interface_iid FILE "QPlugin.json")
    Q_INTERFACES(Plugin_Interface)
public:
    PluginDevice();

public:
    //按照顺序依次调用
    virtual bool ConnectCore(QObject* core);
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
};

#endif // PLUGINDEVICE_H
