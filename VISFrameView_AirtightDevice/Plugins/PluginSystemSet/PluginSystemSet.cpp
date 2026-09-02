#include "PluginSystemSet.h"
#include "WidgetSystemSet.h"

PluginSystemSet::PluginSystemSet()
{
    pluginID = "PluginSystemSet";
	pluginVersion = "1.0.1";
	pluginAuther = "wangwei";
	pluginAuthority = OPERATOR;
    showOrder = 4;
}

void PluginSystemSet::InitWidgetList(Plugin_Interface *plugin)
{
    WidgetSystemSet* widget=new WidgetSystemSet();
    plugin->page=widget;
    plugin->icons << 0xf031 << 0xf036 << 0xf036<< 0xf133 << 0xf056 << 0xf026;
	plugin->iconArea = QSize(40, 40);
	plugin->iconSize = 25;
	plugin->btnHeight = 45;

    PluginLogInfo pluginLog;
    pluginLog.type = SystemLog;
    pluginLog.index = 0;
    pluginLog._pLog = std::bind(&WidgetSystemSet::AddLog, widget, std::placeholders::_1, std::placeholders::_2);
    frameCore->listPluginLog.append(pluginLog);
}

void PluginSystemSet::InitActionList(Plugin_Interface *plugin)
{
//    PluginActionInfo* action2 = new PluginActionInfo();
//    action2->_actionName = "ShowAdvancedSettingsForm";
//    action2->_actionDetail = tr("高级设置");
//    action2->_pAction = (FPTR_ACTION)(&PluginSystemSet::ShowAdvancedSettingsForm);
//    plugin->listAction.append(action2);
}

int PluginSystemSet::OnViewClosing()
{
    return 0;
}

int PluginSystemSet::OnInitialized()
{
    WidgetSystemSet* widget = (WidgetSystemSet*)this->page;
    widget->InitWidget();
	widget->LoadUIParam();
	return 0;
}
