#ifndef PLUGINAUTOPAGE_H
#define PLUGINAUTOPAGE_H

#include "VISFramePluginModel.h"
#include "../../interface/singleton.h"

//自动运行界面
class PluginAutoPage: public VISFramePluginModel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_Interface_iid FILE "QPlugin.json")
    Q_INTERFACES(Plugin_Interface)
public:
    PluginAutoPage();

public:
    virtual void InitWidgetList(Plugin_Interface* plugin);
    virtual void InitActionList(Plugin_Interface* plugin);
    virtual int  OnViewCreated();
};

#endif // PLUGINAUTOPAGE_H
