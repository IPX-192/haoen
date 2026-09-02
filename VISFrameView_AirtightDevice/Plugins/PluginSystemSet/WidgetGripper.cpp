#include "WidgetGripper.h"
#include "ui_WidgetGripper.h"
#include "ParamManager.h"
#include "VisUIParam.h"
#include "Item/ElectricGripperParamForm.h"
#include <QTextCodec>

WidgetGripper::WidgetGripper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetGripper)
{
    ui->setupUi(this);
    InitWidget();
}

WidgetGripper::~WidgetGripper()
{
    delete ui;
}

void WidgetGripper::InitWidget()
{
    for (int i = 0;i<GripperCount;++i)
    {
        ElectricGripperParamForm *gripperForm = new ElectricGripperParamForm;
        gripperForm->SetTittle(QString(u8"夹爪%1").arg(i+1));
        ui->gridLayout_gripper->addWidget(gripperForm,0,i);
        m_gripperList.append(gripperForm);
    }
}

void WidgetGripper::LoadUIParam()
{
    QString sConfigPath = GlobalParam->systemParam.filepath + "ElectricGripperAddrConfig.ini";
    QSettings *config = new QSettings(sConfigPath, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));

    for(int i = 0;i<GripperCount;++i)
    {
        config->beginGroup(QString("Gripper%1").arg(i+1));
        VisUIParam::LoadIniToUI(config, m_gripperList.at(i), &GlobalParam->systemParam.gripperAddrParam[i]);
        config->endGroup();
    }
    delete  config;
    config = nullptr;
}

void WidgetGripper::UpdateParamToUI()
{
    LoadUIParam();
}

void WidgetGripper::SaveUIParam()
{
    QString sConfigPath = GlobalParam->systemParam.filepath + "ElectricGripperAddrConfig.ini";
    QSettings *config = new QSettings(sConfigPath, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));

    for(int i = 0;i<GripperCount;++i)
    {
        config->beginGroup(QString("Gripper%1").arg(i+1));
        VisUIParam::SaveUIToIni(config, m_gripperList.at(i), &GlobalParam->systemParam.gripperAddrParam[i]);
        config->endGroup();
    }
    delete  config;
    config = nullptr;
}
