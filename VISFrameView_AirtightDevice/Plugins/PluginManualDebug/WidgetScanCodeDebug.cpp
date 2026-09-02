#include <QTime>
#include <QThread>
#include "WidgetScanCodeDebug.h"
#include "ui_WidgetScanCodeDebug.h"
#include "CMsgBox.h"
#include "coreinterface.h"
#include "VisAppBus.h"
#include "ParamManager.h"
#include "VisUIParam.h"
#include "ScanCodeForm.h"
#include <QTextCodec>

WidgetScanCodeDebug::WidgetScanCodeDebug(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetScanCodeDebug)
{
    ui->setupUi(this);
    InitWidget();
}

WidgetScanCodeDebug::~WidgetScanCodeDebug()
{
    delete ui;
}

void WidgetScanCodeDebug::InitWidget()
{
    ScanCodeForm *m_produces = new ScanCodeForm;
    m_produces->SetName(ModuleScanCode);
    ScanCodeForm *m_tray = new ScanCodeForm;
    m_tray->SetName(TrayScanCode);
    ui->gridLayout->addWidget(m_produces,0,0);
    ui->gridLayout->addWidget(m_tray,0,1);
    m_scanCodeFormList.append(m_produces);
    m_scanCodeFormList.append(m_tray);
}


void WidgetScanCodeDebug::LoadUIParam()
{
    QString filename = GlobalParam->systemParam.filepath+"system.ini";
    QSettings *config = new QSettings(filename, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));

    for(int i = 0;i<StationCount;++i)
    {
        config->beginGroup(QString("ScanCodeParam%1").arg(i+1));
        if(i == 0)
            VisUIParam::LoadIniToUI(config, m_scanCodeFormList.at(i), &GlobalParam->systemParam.produceCodeParam);
        else
            VisUIParam::LoadIniToUI(config, m_scanCodeFormList.at(i), &GlobalParam->systemParam.trayCodeParam);
        config->endGroup();
    }
}

void WidgetScanCodeDebug::UpdateParamToUI()
{
     LoadUIParam();
}

void WidgetScanCodeDebug::SaveUIParam()
{
    QString filename = GlobalParam->systemParam.filepath+"system.ini";
    QSettings *config = new QSettings(filename, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));

    for(int i = 0;i<StationCount;++i)
    {
        config->beginGroup(QString("ScanCodeParam%1").arg(i+1));
        if(i == 0)
            VisUIParam::SaveUIToIni(config, m_scanCodeFormList.at(i), &GlobalParam->systemParam.produceCodeParam);
        else
            VisUIParam::SaveUIToIni(config, m_scanCodeFormList.at(i), &GlobalParam->systemParam.trayCodeParam);
        config->endGroup();
    }
}
