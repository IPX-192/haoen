#include "WidgetStatusBar.h"
#include "ui_WidgetStatusBar.h"
#include "ParamManager.h"
#pragma execution_character_set("utf-8")

WidgetStatusBar::WidgetStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetStatusBar)
{
    ui->setupUi(this);
    m_timer=new QTimer(this);
    m_timer->setInterval(100);
    connect(m_timer,&QTimer::timeout,this,&WidgetStatusBar::slotTimeout);
    m_timer->start();
}

WidgetStatusBar::~WidgetStatusBar()
{
    delete ui;
}

void WidgetStatusBar::slotTimeout()
{
    if (GlobalParam->systemParam.mesParam.mesEnable)
    {
        ui->LabelMesOpen->setStyleSheet("background-color: rgb(0, 255, 0);");
        ui->LabelMesOpen->setText(QString::fromUtf8("MES已开启"));
    }
    else
    {
        ui->LabelMesOpen->setStyleSheet("background-color: rgb(255, 0, 0);");
        ui->LabelMesOpen->setText(QString::fromUtf8("MES已关闭"));
    }
}
