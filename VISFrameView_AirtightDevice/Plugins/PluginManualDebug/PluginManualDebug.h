#ifndef PLUGINMANUALDEBUG_H
#define PLUGINMANUALDEBUG_H

#include "VISFramePluginModel.h"

class  PluginManualDebug:public VISFramePluginModel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_Interface_iid FILE "QPlugin.json")
    Q_INTERFACES(Plugin_Interface)
public:
    PluginManualDebug();

public:
    virtual void InitSubscibeEvent(Plugin_Interface* plugin);
    virtual void InitWidgetList(Plugin_Interface* plugin);
    virtual void InitActionList(Plugin_Interface* plugin);
    virtual int  OnInitialized();

protected:
    void  ShowMotorCtr(bool checkState);

public slots:
    //台风系统:启动/关闭(Out_TyphoonStart)
    int  event_TyphoonSysOpen();
    int  event_TyphoonSysClose();
    //台风系统:清灰/停止清灰(Out_TyphooClearDirty)
    int  event_TyphoonCleanOpen();
    int  event_TyphoonCleanClose();
};

#endif // PLUGINMANUALDEBUG_H
