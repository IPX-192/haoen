#include "WidgetManualDebug.h"
#include "ui_WidgetManualDebug.h"
#include <QStyle>
#include "WidgetScanCodeDebug.h"
#include "coreinterface.h"
#include "VisAppBus.h"
#include "WidgetConveyorControl.h"
#include "CylinderControl/WidgetCylinderTab.h"

WidgetManualDebug::WidgetManualDebug(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetManualDebug)
{
    ui->setupUi(this);
    ui->widgetLeftBar->setObjectName("leftbar");
    ui->stackedWidget->setObjectName("stackWidget");

    //初始化点位窗口
    QWidget* widgetIO = VisMotorToolSpace::VisMotorToolIns->GetWidget(VisMotorToolSpace::VisMotorTool::F_IoMonitor);
    WidgetScanCodeDebug *pScanCodeForm = new WidgetScanCodeDebug(this);
    WidgetCylinderTab* pCylinderControlForm = new WidgetCylinderTab(this);
    ConveyorControlWidget *pConveyorControl = new ConveyorControlWidget(this);
    ui->stackedWidget->addWidget(widgetIO);
    ui->stackedWidget->addWidget(pScanCodeForm);
    ui->stackedWidget->addWidget(pCylinderControlForm);
    ui->stackedWidget->addWidget(pConveyorControl);
}

WidgetManualDebug::~WidgetManualDebug()
{
    delete ui;
}

void WidgetManualDebug::AddLog(QString msg, LogLevel level)
{
    ui->widget_Log->addLog(msg, level);
}

void WidgetManualDebug::LoadUIParam()
{
    for (int i = 0; i < ui->stackedWidget->count(); i++) {
        QMetaObject::invokeMethod(ui->stackedWidget->widget(i), "LoadUIParam", Qt::DirectConnection);
    }
}

void WidgetManualDebug::UpdateUIParam()
{
    for (int i = 0; i < ui->stackedWidget->count(); i++) {
        QMetaObject::invokeMethod(ui->stackedWidget->widget(i), "UpdateParamToUI", Qt::DirectConnection);
    }
}


void WidgetManualDebug::on_btnUpdateUI_clicked()
{
    QMetaObject::invokeMethod(ui->stackedWidget->currentWidget(), "UpdateParamToUI", Qt::DirectConnection);
}

void WidgetManualDebug::on_btnSaveParam_clicked()
{
    QMetaObject::invokeMethod(ui->stackedWidget->currentWidget(), "SaveUIParam", Qt::DirectConnection);
}
