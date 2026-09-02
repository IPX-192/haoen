#include "PluginAutoPage.h"
#include <functional>
#include "WidgetAutoPage.h"
#include "VisAppBus.h"
#include "ParamManager.h"
#include "WidgetStatusBar.h"
PluginAutoPage::PluginAutoPage()
{
    pluginID = "PluginAutoPage";
    pluginVersion = "1.0.1";
	pluginAuther = "wangwei";
    pluginAuthority = OPERATOR;
    showOrder = 1;
}

void PluginAutoPage::InitWidgetList(Plugin_Interface *plugin)
{
    WidgetAutoPage* widget=new WidgetAutoPage();
    widget->InitLog();
    plugin->page=widget;
    plugin->icons << 0xf080 << 0xf03e << 0xf1fe << 0xf138 << 0xf133 << 0xf143 ;
	plugin->iconArea = QSize(40, 40);
	plugin->iconSize = 25;
	plugin->btnHeight = 45;

    WidgetStatusBar* widgetStatus = new WidgetStatusBar();
    PluginWidgetInfo* pluginWidgetStatus = new PluginWidgetInfo();
    pluginWidgetStatus->_widget = widgetStatus;
    pluginWidgetStatus->_widgetDetail = u8"状态栏";
    plugin->listWidget.push_back(pluginWidgetStatus);
}

void PluginAutoPage::InitActionList(Plugin_Interface *plugin)
{
//    PluginActionInfo* action1 = new PluginActionInfo();
//    action1->_actionName = "StepUp";
//    action1->_actionDetail = tr("Step Up form 0 ~ 10.");
//    action1->_pAction = (FPTR_ACTION)(&PluginAutoPage::TestFun);
//    plugin->listAction.append(action1);
}

int PluginAutoPage::OnViewCreated()
{
    return 0;
}


