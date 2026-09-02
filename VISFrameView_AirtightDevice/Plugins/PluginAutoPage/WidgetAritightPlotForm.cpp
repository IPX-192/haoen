#include "WidgetAritightPlotForm.h"
#include "ui_WidgetAritightPlotForm.h"
#include "AritightPlotItem.h"
#include "ParamManager.h"
#include "VisAppBus.h"

WidgetAritightPlotForm::WidgetAritightPlotForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAritightPlotForm)
{
    ui->setupUi(this);
    qRegisterMetaType<AritightTask>("AritightTask");
    for (int i = 0; i <StationCount;++i) {
        AritightPlotItem *item = new AritightPlotItem;
        item->SetTittle(i,QString(u8"工位%1").arg(i+1));
        ui->gridLayout->addWidget(item,0,i);
        m_listPlot.append(item);
    }

    VisAppBus::subscibeEvent(this,"DrawAirtightPlot");
    VisAppBus::subscibeEvent(this,"SetAirtightParam");
    VisAppBus::subscibeEvent(this,"ClearPlot");
}

WidgetAritightPlotForm::~WidgetAritightPlotForm()
{
    delete ui;
}

int WidgetAritightPlotForm::event_DrawAirtightPlot(int station, AritightTask data)
{
    m_listPlot.at(station)->DrawAirtightPlot(station,data);
    return 0;
}

int WidgetAritightPlotForm::event_SetAirtightParam(int station)
{
    m_listPlot.at(station)->SetAirtightParam(station);
    return 0;
}

int WidgetAritightPlotForm::event_ClearPlot(int station)
{
    m_listPlot.at(station)->ClearPlot(station);
    return 0;
}
