#include "PluginMeasureProcess.h"
#include <QProcess>
#include "./EquipmentRelated/PlcControl.h"
#include "./EquipmentRelated/AutoMeasureProcess.h"
#include "NonBlockingMsgBox.h"

PluginMeasureProcess::PluginMeasureProcess()
{
    pluginID = "PluginMeasureProcess";
	pluginVersion = "1.0.1";
	pluginAuther = "wangwei";
	pluginAuthority = OPERATOR;

}

void PluginMeasureProcess::InitSubscibeEvent(Plugin_Interface *plugin)
{
    VisAppBus::subscibeEvent(this, "WritePagePLC");
    VisAppBus::subscibeEvent(this, "ReadPagePLC");
    VisAppBus::subscibeEvent(this, "WriteShieldToPlC");
    VisAppBus::subscibeEvent(this, "ShowMessageBox");
    VisAppBus::subscibeEvent(this, "PopupUserMsgBox");
}

void PluginMeasureProcess::InitActionList(Plugin_Interface *plugin)
{

}

void PluginMeasureProcess::InitWidgetList(Plugin_Interface *plugin)
{

}

int PluginMeasureProcess::OnCoreInitialized()
{
    return 0;
}

int PluginMeasureProcess::OnInitialized()
{
    GlobalParam->SetMachineStatus(MachineRunStatus::NoHomed);
    
    return 0;
}

int PluginMeasureProcess::OnViewCreated()
{
    AutoMeasureProcessIns->StartMonitor();
    return 0;
}

int PluginMeasureProcess::OnViewClosing()
{
    AutoMeasureProcessIns->StopMonitor();
    return 0;
}

int PluginMeasureProcess::event_WritePagePLC(QString regName, int length, QVector<int>& data)
{
    bool bRet = PlcControl::Instance().WritePageD(regName, length, data);
    return bRet ? 0 : -1;

}

int PluginMeasureProcess::event_ReadPagePLC(QString regName, int length, QVector<int>& data)
{
    bool bRet = PlcControl::Instance().ReadPageD(regName, length, data);
    return bRet ? 0 : -1;

}

int PluginMeasureProcess::event_ShowMessageBox(QString sMessage)
{
    ShowSystemLog(Log_Info, sMessage);
    PlcControlIns.SetAlarm(true);
    auto res = MsgBoxManager::instance().showInfo(nullptr, QString(u8"提示信息"), sMessage, NonBlockingMsgBox::Ok | NonBlockingMsgBox::Cancel);
    PlcControlIns.SetAlarm(false);
    return (NonBlockingMsgBox::Ok == res);
}

int PluginMeasureProcess::event_PopupUserMsgBox(QStringList listBtn, QString info)
{
    ShowSystemLog(Log_Info, info);
    PlcControlIns.SetAlarm(true);
    int btnClick = MsgBoxManager::instance().showWarning(nullptr, QString(u8"提示信息"), info, listBtn);
    PlcControlIns.SetAlarm(false);
    return btnClick;
}

int PluginMeasureProcess::event_WriteShieldToPlC(QMap<QString, QPair<bool*, QString>>shieldDataMap)
{
    for (auto it : shieldDataMap) {
        QString regName = it.second;
        if (!regName.isEmpty()) {
            bool bRet = PlcControl::Instance().WriteML(regName, !*it.first);
            if (!bRet) {
                QString errInfo = QString(u8"写入PLC屏蔽点失败:%1").arg(regName);
                ShowSystemLog(Log_Error, errInfo);
                MsgBoxManager::instance().showInfo(nullptr, QString(u8"提示信息"), errInfo, NonBlockingMsgBox::Ok);
                return -1;
            }
        }
    }
    return 0;
}
