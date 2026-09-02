#include "WidgetProductSet.h"
#include "ui_WidgetProductSet.h"
#include "WidgetMatrixCfg.h"
#include "WidgetRecipePlatform.h"
#include "VisAppBus.h"
#include "WidgetRecipeTray.h"
#include "WidgetRecipeMotor.h"
#include "WidgetRecipeGrip.h"
#include "WidgetAritight.h"

#pragma execution_character_set("utf-8")
WidgetProductSet::WidgetProductSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetProductSet)
{
    ui->setupUi(this);
    ui->widgetLeftBar->setObjectName("leftbar");
    ui->stackedWidget->setObjectName("stackWidget");
    WidgetMatrixCfg* widgetMatrix = new WidgetMatrixCfg(ui->stackedWidget);
    WidgetRecipePlatform* widgetPlatform = new WidgetRecipePlatform(ui->stackedWidget);
    WidgetRecipeTray* widgetTray = new WidgetRecipeTray(ui->stackedWidget);
    WidgetRecipeMotor* pWidgetRecipeMotor = new WidgetRecipeMotor(ui->stackedWidget);
    WidgetRecipeGrip* widgetGrip = new WidgetRecipeGrip(ui->stackedWidget);
	WidgetAritight *widgetAritight = new WidgetAritight(ui->stackedWidget);

    ui->stackedWidget->addWidget(widgetMatrix);
    ui->stackedWidget->addWidget(widgetPlatform);
    ui->stackedWidget->addWidget(pWidgetRecipeMotor);
    ui->stackedWidget->addWidget(widgetTray);
    ui->stackedWidget->addWidget(widgetGrip);
	ui->stackedWidget->addWidget(widgetAritight);
    connect(GlobalParam->frameCore, &QVisF_Interface::sig_OutputInfo, this, &WidgetProductSet::slot_InputInfo);
}

WidgetProductSet::~WidgetProductSet()
{
    delete ui;
}

void WidgetProductSet::LoadUIParam()
{
    for (int i = 0; i < ui->stackedWidget->count(); i++) {
        QMetaObject::invokeMethod(ui->stackedWidget->widget(i), "LoadUIParam", Qt::DirectConnection);
    }
}

void WidgetProductSet::UpdateUIParam()
{
    for (int i = 0; i < ui->stackedWidget->count(); i++) {
        QMetaObject::invokeMethod(ui->stackedWidget->widget(i), "UpdateParamToUI", Qt::DirectConnection);
    }
}

void WidgetProductSet::AddLog(QString msg, LogLevel level)
{
    ui->widget_Log->addLog(msg, level);
}

void WidgetProductSet::Init()
{
    for (int i = 0; i < 1; i++) {
        QMetaObject::invokeMethod(ui->stackedWidget->widget(i), "LoadMatrixFile", Qt::DirectConnection);
    }
}

int WidgetProductSet::slot_InputInfo(tagOutputInfo &info)
{
    if (info._type == INFT_ProductChange) {
        int nRes = 0;
        for (int i = 0; i < ui->stackedWidget->count(); i++) {
            QMetaObject::invokeMethod(ui->stackedWidget->widget(i), "LoadUIParam", Qt::DirectConnection);
            if (nRes != 0)return nRes;
        }
    }
    return 0;
}

void WidgetProductSet::on_btnUpdateUI_clicked()
{
    UpdateUIParam();
}

void WidgetProductSet::on_btnSaveParam_clicked()
{
    ui->btnSaveParam->setEnabled(false);
    QMetaObject::invokeMethod(ui->stackedWidget->currentWidget(), "SaveUIParam", Qt::DirectConnection);
    ui->btnSaveParam->setEnabled(true);
}
