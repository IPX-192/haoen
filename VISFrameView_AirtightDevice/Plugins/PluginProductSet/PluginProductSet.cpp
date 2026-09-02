#include "PluginProductSet.h"
#include "WidgetProductSet.h"
#include "VisAppBus.h"
#include "ParamManager.h"

PluginProductSet::PluginProductSet()
{
    pluginID = "PluginProductSet";
	pluginVersion = "1.0.1";
	pluginAuther = "wangwei";
	pluginAuthority = OPERATOR;
    showOrder = 3;
}

void PluginProductSet::InitSubscibeEvent(Plugin_Interface* plugin)
{
}

void PluginProductSet::InitWidgetList(Plugin_Interface *plugin)
{
    WidgetProductSet* widget=new WidgetProductSet();
    plugin->page=widget;
    plugin->icons << 0xf036 << 0xf249 << 0xf055 << 0xf036 << 0xf249<< 0xf139;
	plugin->iconArea = QSize(40, 40);
	plugin->iconSize = 25;
	plugin->btnHeight = 45;

    PluginLogInfo pluginLog;
    pluginLog.type = SystemLog;
    pluginLog.index = 0;
    pluginLog._pLog = std::bind(&WidgetProductSet::AddLog, widget, std::placeholders::_1, std::placeholders::_2);
    frameCore->listPluginLog.append(pluginLog);
}

void PluginProductSet::InitActionList(Plugin_Interface *plugin)
{

}

int PluginProductSet::OnInitialized()
{
    ParamManager::instance()->LoadRecipeProduct();
    WidgetProductSet* widget = (WidgetProductSet*)this->page;
    widget->Init();
	tagOutputInfo outInfo;
	outInfo._type = INFT_ProductChange;
	emit GlobalParam->frameCore->sig_OutputInfo(outInfo);
    return 0;
}
