#include "WidgetSystemSet.h"
#include "ui_WidgetSystemSet.h"
#include "DataSaveForm.h"
#include "MesParamForm.h"
#include "VisAppBus.h"
#include "WidgetGripper.h"
#include "WidgetShield.h"
#include "WidgetAritightForm.h"

#pragma execution_character_set("utf-8")

WidgetSystemSet::WidgetSystemSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSystemSet)
{
    ui->setupUi(this);
    ui->widgetLeftBar->setObjectName("leftbar");
    ui->stackedWidget->setObjectName("stackWidget");
}

WidgetSystemSet::~WidgetSystemSet()
{
    delete ui;
}

void WidgetSystemSet::AddLog(QString msg, LogLevel level)
{
    ui->widget_Log->addLog(msg,level);
}

void WidgetSystemSet::InitWidget()
{
    DataSaveForm* pDataSaveForm = new DataSaveForm(ui->stackedWidget);
    MesParamForm* pMesParamForm = new MesParamForm(ui->stackedWidget);
    WidgetGripper* pElectricGripperParamForm = new WidgetGripper(ui->stackedWidget);
    WidgetShield*widgetShield=new WidgetShield(ui->stackedWidget);
    WidgetAritightForm *widgetAritight = new WidgetAritightForm(ui->stackedWidget);
    
    ui->stackedWidget->addWidget(pDataSaveForm);
    ui->stackedWidget->addWidget(pMesParamForm);
    ui->stackedWidget->addWidget(pElectricGripperParamForm);
    ui->stackedWidget->addWidget(widgetShield);
    ui->stackedWidget->addWidget(widgetAritight);
}

void WidgetSystemSet::LoadUIParam()
{
	for (int i = 0; i < ui->stackedWidget->count(); i++) {
		QMetaObject::invokeMethod(ui->stackedWidget->widget(i), "LoadUIParam", Qt::DirectConnection);
	}
}

void WidgetSystemSet::on_btnUpdateUI_clicked()
{
    QMetaObject::invokeMethod(ui->stackedWidget->currentWidget(), "UpdateParamToUI", Qt::DirectConnection);
}

void WidgetSystemSet::on_btnSaveParam_clicked()
{
    QMetaObject::invokeMethod(ui->stackedWidget->currentWidget(), "SaveUIParam", Qt::DirectConnection);
}

