#include "WidgetAutoPage.h"
#include "ui_WidgetAutoPage.h"
#include "../PluginParam/ParamManager.h"
#include "../interface/coreinterface.h"
#include <QDateTime>
#include "VisAppBus.h"
#include "CMsgBox.h"
#include "SwitchButton.h"
#include "VisMotorManager.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include "WidgetProductChange.h"
#include "NonBlockingMsgBox.h"
#include "Product/WidgetProductTotal.h"
#include "WidgetTray.h"
#include "WidgetLogAll.h"
#include "WidgetAritightPlotForm.h"

#pragma execution_character_set("utf-8")

WidgetAutoPage::WidgetAutoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAutoPage)
{
    ui->setupUi(this);
    ui->widgetLeftBar->setObjectName("leftbar");
    ui->stackedWidget->setObjectName("stackWidget");
	InitWidget();
}

WidgetAutoPage::~WidgetAutoPage()
{
    delete ui;
}

void WidgetAutoPage::InitWidget()
{
    QString Configpath = GlobalParam->recipeProduct.productPath;
    WidgetProductTotal * pProductTotalForm = new WidgetProductTotal(2,Configpath, ui->stackedWidget);
    WidgetTray* pTrayShowForm = new WidgetTray(this);
    WidgetLogAll*widgetLog = new WidgetLogAll(this);
    WidgetProductChange *pWidgetProductChange = new WidgetProductChange(this);

    WidgetAritightPlotForm *pAritightPlot = new WidgetAritightPlotForm(ui->stackedWidget);

    ui->stackedWidget->addWidget(pProductTotalForm);
    ui->stackedWidget->addWidget(pAritightPlot);
    ui->stackedWidget->addWidget(pTrayShowForm);
    ui->stackedWidget->addWidget(widgetLog);
    ui->stackedWidget->addWidget(pWidgetProductChange);

    //UpdateButtonStatus(MachineStatusIns.GetMahineRunStatus());

    return;
}

void WidgetAutoPage::InitLog()
{
    PluginLogInfo pluginLog;
    pluginLog.type = SystemLog;
    pluginLog.index = 0;
    pluginLog._pLog = std::bind(&WidgetLog::addLog, ui->widget, std::placeholders::_1, std::placeholders::_2);
    GlobalParam->frameCore->listPluginLog.append(pluginLog);
}

int WidgetAutoPage::event_ProductTotal(AritightTask item)
{
    ProductTask task;

    task.result  = item.result;   //测试结果
    task.station = item.station;      //工位
    task.errItem = item.errorMsg;     //错误项
    task.ctTime  = item.endDateTime.secsTo(item.startDateTime);       //CT时间

    VisAppBus::sendEventDirect("UpdateProduct",task);
    return 0;
}

void WidgetAutoPage::showEvent(QShowEvent* event)
{
    qDebug() << __FUNCTION__;
    QWidget::showEvent(event);
}
void WidgetAutoPage::hideEvent(QHideEvent* event)
{
    qDebug() << __FUNCTION__;
    QWidget::hideEvent(event);
}

