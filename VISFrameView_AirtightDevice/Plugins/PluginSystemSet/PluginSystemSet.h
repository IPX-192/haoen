#ifndef PLUGINSYSTEMSET_H
#define PLUGINSYSTEMSET_H
#include <QSharedPointer>
#include "VISFramePluginModel.h"

//系统设置界面
class PluginSystemSet:public VISFramePluginModel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_Interface_iid FILE "QPlugin.json")
    Q_INTERFACES(Plugin_Interface)
public:
    PluginSystemSet();

public:
    virtual void InitWidgetList(Plugin_Interface* plugin);
    virtual void InitActionList(Plugin_Interface* plugin);
    virtual int  OnInitialized();
    virtual int  OnViewClosing();
};

#endif // PLUGINMANUALDEBUG_H
