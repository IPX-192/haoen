#include "PluginManualDebug.h"
#include "WidgetManualDebug.h"
#include <QMessageBox>
#include "VisAppBus.h"
#include "ParamManager.h"
#include "WidgetManualDebug.h"
#include "WidgetMotorCtrl.h"
#include "VisUIParam.h"
#include "VisMotorManager.h"
#include "../../interface/coreinterface.h"   // ShowSystemLog

using namespace VisMotorToolSpace;

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
    //台风控制系统(手动调机界面发事件)
    VisAppBus::subscibeEvent(this, "TyphoonSysOpen");
    VisAppBus::subscibeEvent(this, "TyphoonSysClose");
    VisAppBus::subscibeEvent(this, "TyphoonCleanOpen");
    VisAppBus::subscibeEvent(this, "TyphoonCleanClose");
}

void PluginManualDebug::InitWidgetList(Plugin_Interface *plugin)
{
    WidgetManualDebug* widget = new WidgetManualDebug();
    plugin->page=widget;
    plugin->icons << 0xf080 << 0xf03e << 0xf1fe << 0xf133<< 0xf03e<< 0xf133<< 0xf133;
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
	action1->_actionDetail = tr("电机控制");
	action1->_pAction = (FPTR_ACTION)(&PluginManualDebug::ShowMotorCtr);
	plugin->listAction.append(action1);
}

int PluginManualDebug::OnInitialized()
{
	WidgetMotorCtrl::instance()->InitMotor();
	WidgetManualDebug* widget =  (WidgetManualDebug*)page;
	widget->LoadUIParam();
	return 0;
}

void PluginManualDebug::ShowMotorCtr(bool checkState)
{
	if (GlobalParam->frameCore->curUserInfo.authority == OPERATOR) {
		QMessageBox::warning(nullptr, QString(u8"警告"), QString(u8"当前用户无权限"));
		return;
	}
	WidgetMotorCtrl::instance()->show();
}

//台风系统启动:Out_TyphoonStart = ON
int PluginManualDebug::event_TyphoonSysOpen()
{
    VisMotorInstance->SetIoOutput(Out_TyphoonStart, IO_ON);
    ShowSystemLog(Log_Info, QString(u8"台风系统启动(Out_TyphoonStart=ON)"));
    return 0;
}

//台风系统关闭:Out_TyphoonStart = OFF
int PluginManualDebug::event_TyphoonSysClose()
{
    VisMotorInstance->SetIoOutput(Out_TyphoonStart, IO_OFF);
    ShowSystemLog(Log_Info, QString(u8"台风系统关闭(Out_TyphoonStart=OFF)"));
    return 0;
}

//台风系统清灰:Out_TyphooClearDirty = ON
int PluginManualDebug::event_TyphoonCleanOpen()
{
    VisMotorInstance->SetIoOutput(Out_TyphooClearDirty, IO_ON);
    ShowSystemLog(Log_Info, QString(u8"台风系统清灰(Out_TyphooClearDirty=ON)"));
    return 0;
}

//台风系统停止清灰:Out_TyphooClearDirty = OFF
int PluginManualDebug::event_TyphoonCleanClose()
{
    VisMotorInstance->SetIoOutput(Out_TyphooClearDirty, IO_OFF);
    ShowSystemLog(Log_Info, QString(u8"台风系统停止清灰(Out_TyphooClearDirty=OFF)"));
    return 0;
}



