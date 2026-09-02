#include "WidgetBtnPanel.h"
#include "ui_WidgetBtnPanel.h"
#include <QTime>
#include <QMessageBox>
#include "SwitchButton.h"
#include "VisAppBus.h"
#include "ParamManager.h"

WidgetBtnPanel::WidgetBtnPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetBtnPanel)
{
    ui->setupUi(this);
    //m_autoModel=new SwitchButton();
    //m_autoModel->SetTitle(QString::fromLocal8Bit("自动模式"));
    //ui->frameAutoModel->layout()->addWidget(m_autoModel);
    //connect(m_autoModel, &SwitchButton::CheckedChanged, this, &WidgetBtnPanel::slotAutoModel,Qt::QueuedConnection);
    VisAppBus::subscibeEvent(this, "UpdateDevStatus");
}

WidgetBtnPanel::~WidgetBtnPanel()
{
    delete ui;
}

void WidgetBtnPanel::SetState(MachineRunStatus state)
{
	QList<QPushButton*> tbtns = this->findChildren<QPushButton*>();
	for (auto btn : tbtns) {
        if (state == MachineRunStatus::Idle) {
            btn->setEnabled(true);
        }	
        else {
            btn->setEnabled(false);
        }	
	}
    if (state == MachineRunStatus::NoHomed) {
        ChangeExeStatus(QString(u8"未复位"), QColor(0, 128, 0),true);
        ui->btnReset->setEnabled(true);
        ui->btnEstop->setEnabled(true);
        ui->btnAlarmCleared->setEnabled(true);
	}
    else if (state == MachineRunStatus::Homed) {
		ui->btnEstop->setEnabled(true);
        ChangeExeStatus(QString(u8"复位中"), QColor(255, 0, 0), false);
        ui->btnReset->setEnabled(true);
        ui->btnAlarmCleared->setEnabled(true);
	}
    else if (state == MachineRunStatus::Idle) {
        ChangeExeStatus(QString(u8"空闲"), QColor(0, 128, 0), true);
        ui->btnReset->setEnabled(false);
        ui->btnEstop->setEnabled(true);
        ui->btnAutoStart->setEnabled(true);
        ui->btnSpotCheck->setEnabled(true);
        ui->btnAlarmCleared->setEnabled(true);
    }
    else if (state == MachineRunStatus::AutoRunning) {
        ChangeExeStatus(QString(u8"自动生产"), QColor(0, 128, 0), true);
        ui->btnReset->setEnabled(false);
        ui->btnEstop->setEnabled(true);
        ui->btnAutoStart->setEnabled(false);
        ui->btnSpotCheck->setEnabled(false);
        ui->btnAlarmCleared->setEnabled(true);
        ui->btnAutoPause->setEnabled(true);
    }
    else if (state == MachineRunStatus::Pause) {
        ChangeExeStatus(QString(u8"手动模式"), QColor(0, 128, 0), true);
    }
}

void WidgetBtnPanel::on_btnReset_clicked()
{
    ShowSystemLog(Log_Info, QString::fromUtf8(u8"系统复位按钮点击"));
    int nRes = VisAppBus::sendEvent("AutoReset");
}

void WidgetBtnPanel::on_btnAutoStart_clicked()
{
    ShowSystemLog(Log_Info, QString::fromUtf8(u8"启动按钮点击"));
    int nRes = VisAppBus::sendEvent("AutoStart");
    if (nRes != 0) {
        ShowSystemLog(Log_Error, QString(u8"进入生产模式失败"));
        return;
    }
}

void WidgetBtnPanel::on_btnAutoStop_clicked()
{
    ShowSystemLog(Log_Info, QString::fromUtf8(u8"停止按钮点击"));
}

void WidgetBtnPanel::on_btnAutoPause_clicked()
{
    ShowSystemLog(Log_Info, QString::fromUtf8(u8"暂停按钮点击"));
    VisAppBus::sendEvent("AutoPause");
    ui->btnAutoPause->setEnabled(false);
    ui->btnAutoRestore->setEnabled(true);
}

void WidgetBtnPanel::on_btnClearModule_clicked()
{
	if (QMessageBox::question(this, u8"提示", u8"是否开始清料") == QMessageBox::Yes) {
		VisAppBus::sendEvent("ClearModule");
	}
}

void WidgetBtnPanel::on_btnAutoRestore_clicked()
{
    ShowSystemLog(Log_Info, QString::fromUtf8(u8"恢复按钮点击"));
    VisAppBus::sendEvent("AutoResume");
	ui->btnAutoPause->setEnabled(true);
    ui->btnAutoRestore->setEnabled(false);
}

void WidgetBtnPanel::on_btnAlarmCleared_clicked()
{
    ShowSystemLog(Log_Info, QString::fromUtf8(u8"报警清除按钮点击"));
    VisAppBus::sendEvent("ClearWarning");
}

void WidgetBtnPanel::on_btnEstop_clicked()
{
    ShowSystemLog(Log_Info, QString::fromUtf8(u8"紧急停止按钮点击"));
    VisAppBus::sendEvent("AutoEmg");
}

void WidgetBtnPanel::on_btnSpotCheck_clicked()
{
    int nRes = VisAppBus::sendEvent("AutoSpot");
    if (nRes != 0) {
        ShowSystemLog(Log_Error, QString(u8"进入点检模式失败"));
        return;
    }
}

int WidgetBtnPanel::event_UpdateDevStatus(MachineRunStatus status)
{
    SetState(status);
    return 0;
}

