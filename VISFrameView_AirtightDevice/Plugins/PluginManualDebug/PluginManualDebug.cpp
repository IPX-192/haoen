#include "PluginManualDebug.h"
#include "WidgetManualDebug.h"
#include "VisAppBus.h"
#include "ParamManager.h"
#include "WidgetManualDebug.h"
#include "WidgetMotorCtrl.h"
#include "VisUIParam.h"
PluginManualDebug::PluginManualDebug()
{
	pluginID = "PluginManualDebug";
	pluginVersion = "1.0.1";
	pluginAuther = "wangwei";
	pluginAuthority = OPERATOR;
	showOrder = 2;
}

void PluginManualDebug::InitSubscibeEvent(Plugin_Interface* plugin)
{

}

void PluginManualDebug::InitWidgetList(Plugin_Interface *plugin)
{
    WidgetManualDebug* widget = new WidgetManualDebug();
    plugin->page=widget;
    plugin->icons << 0xf080 << 0xf03e << 0xf1fe << 0xf036;
	plugin->iconArea = QSize(40, 40);
	plugin->iconSize = 25;
	plugin->btnHeight = 45;


	PluginLogInfo pluginLog;
	pluginLog.type = DebugLog;
	pluginLog.index = 0;
	pluginLog._pLog = std::bind(&WidgetManualDebug::AddLog, widget, std::placeholders::_1, std::placeholders::_2);
	frameCore->listPluginLog.append(pluginLog);
}

void PluginManualDebug::InitActionList(Plugin_Interface *plugin)
{
	PluginActionInfo* action1 = new PluginActionInfo();
	action1->_actionName = "ShowMotorCtr";
	action1->_actionDetail = tr("电机调试");
	action1->_pAction = (FPTR_ACTION)(&PluginManualDebug::ShowMotorCtr);
	plugin->listAction.append(action1);
}
int PluginManualDebug::OnInitialized()
{
    static_cast<WidgetManualDebug*>(page)->LoadUIParam();
    WidgetMotorCtrl::instance()->InitMotor();

    return 0;
}
void PluginManualDebug::ShowMotorCtr(bool checkState)
{
	WidgetMotorCtrl::instance()->show();
}


