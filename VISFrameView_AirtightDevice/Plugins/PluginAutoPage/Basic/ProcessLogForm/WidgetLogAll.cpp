#include "WidgetLogAll.h"
#include "ui_WidgetLogAll.h"
#include "ParamManager.h"

WidgetLogAll::WidgetLogAll(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetLogAll)
{
    ui->setupUi(this);
}

WidgetLogAll::~WidgetLogAll()
{
    delete ui;
}

void WidgetLogAll::InitLog(QList<PluginLogInfo>& listPluginLog)
{
	QStringList listLogType;
    listLogType << Log_Process ;
	QVector<WidgetLog*>listLogWidget;
    listLogWidget << ui->widget_Process ;
	for (int i = 0; i < listLogType.size(); i++) {
		PluginLogInfo pluginLog;
		pluginLog.type = listLogType[i];
		pluginLog.index = 0;
        pluginLog._pLog = std::bind(&WidgetLog::addLog, listLogWidget[i], std::placeholders::_1, std::placeholders::_2);
		listPluginLog.append(pluginLog);
	}

    listLogWidget.clear();
    listLogWidget << ui->widget_work1 << ui->widget_work2;
    for (int i = 0; i < listLogWidget.size(); i++) {
        PluginLogInfo pluginLog;
        pluginLog.type = Log_Station;
        pluginLog.index = i;
        pluginLog._pLog = std::bind(&WidgetLog::addLog, listLogWidget[i], std::placeholders::_1, std::placeholders::_2);
        listPluginLog.append(pluginLog);
    }
}
