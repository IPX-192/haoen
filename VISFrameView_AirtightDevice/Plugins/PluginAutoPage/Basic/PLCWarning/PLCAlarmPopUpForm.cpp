#include "PLCAlarmPopUpForm.h"
#include "ui_PLCAlarmPopUpForm.h"
#include <QDateTime>
#pragma execution_character_set("utf-8")
PLCAlarmPopUpForm::PLCAlarmPopUpForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PLCAlarmPopUpForm)
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint);
    return;
}

PLCAlarmPopUpForm::~PLCAlarmPopUpForm()
{
    delete ui;
}

void PLCAlarmPopUpForm::AddWarningInfo(QString sInfo)
{
    ui->WidgetWarning->AddWarningInfo(sInfo);
    return;
}

int PLCAlarmPopUpForm::GetWarningCount()
{
    return ui->WidgetWarning->GetWarningCount();
}

void PLCAlarmPopUpForm::ClearWarningInfo()
{
    ui->WidgetWarning->ClearWarningInfo();
    return;
}

void PLCAlarmPopUpForm::ShowPlcWarningForm()
{
    if (this->isActiveWindow())
    {
        return;
    }

    if (qAbs(QDateTime::currentDateTime().toSecsSinceEpoch() - mnLastPLCWarningTime) < 30)
    {
        return;
    }

    this->close();
    if (this->isMinimized())
    {
        this->showNormal();
        return;
    }
    this->show();
    return;
}

void PLCAlarmPopUpForm::on_PushButtonConfirm_clicked()
{
    //记录上次点击的时间的秒数
    mnLastPLCWarningTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    this->close();
    return;
}
