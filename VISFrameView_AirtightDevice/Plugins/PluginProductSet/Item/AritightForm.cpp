#include "AritightForm.h"
#include "ui_AritightForm.h"
#include "VisAppBus.h"
#include "ParamDef.h"

AritightForm::AritightForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AritightForm)
{
    ui->setupUi(this);
}

AritightForm::~AritightForm()
{
    delete ui;
}

void AritightForm::SetStation(int station)
{
    m_station = station;
}

void AritightForm::on_pushButton_start_clicked()
{
    AritightTask item;
    item.station = m_station;
    VisAppBus::sendEvent("ClearPlot",m_station);
    VisAppBus::sendEvent("AritightFunTest",item);
}

void AritightForm::on_pushButton_reset_clicked()
{
    VisAppBus::sendEvent("ResetAirtight",m_station);
}
