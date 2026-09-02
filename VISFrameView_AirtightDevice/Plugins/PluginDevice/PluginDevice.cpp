#include "PluginDevice.h"
//#include "DeviceBaseOperator.h"
#include "DeviceManage.h"
#pragma execution_character_set("utf-8")
PluginDevice::PluginDevice()
{
    pluginID = "PluginDevice";
	pluginVersion = "1.0.1";
    pluginAuther = "wangwei";
}

bool PluginDevice::ConnectCore(QObject *core)
{
    return true;
}

void PluginDevice::InitSubscibeEvent(Plugin_Interface *plugin)
{

}

void PluginDevice::InitActionList(Plugin_Interface *plugin)
{
}

void PluginDevice::InitWidgetList(Plugin_Interface *plugin)
{

}

int PluginDevice::OnCoreInitialized()
{
    return 0;
}

int PluginDevice::OnInitialized()
{
    return 0;
}

int PluginDevice::OnViewCreated()
{
    DeviceManage::instance();
    return 0;
}

int PluginDevice::OnViewClosing()
{
    return 0;
}
