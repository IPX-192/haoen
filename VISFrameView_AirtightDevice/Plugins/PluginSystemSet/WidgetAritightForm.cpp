#include "WidgetAritightForm.h"
#include "ui_WidgetAritightForm.h"
#include "Item/SerialComForm.h"
#include "ParamManager.h"
#include <QSettings>
#include <QTextCodec>

WidgetAritightForm::WidgetAritightForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAritightForm)
{
    ui->setupUi(this);
    InitWidget();
}

WidgetAritightForm::~WidgetAritightForm()
{
    delete ui;
}
void WidgetAritightForm::InitWidget()
{
    //2台双通道气密仪,左右工站各一台
    for (int i = 0;i<AirtightDeviceCount;++i)
    {
        SerialComForm *aritightForm = new SerialComForm;
        aritightForm->SetDevice(i);
        ui->tabWidget->addTab(aritightForm, i == 0 ? QString(u8"左工站气密仪") : QString(u8"右工站气密仪"));
        m_serialComFormList.append(aritightForm);
    }
}

void WidgetAritightForm::LoadUIParam()
{
    QString filename = GlobalParam->systemParam.filepath+"/system.ini";
    QSettings *config = new QSettings(filename, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));

    for (int i = 0;i<m_serialComFormList.count();++i)
    {
        config->beginGroup(QString("AirtightTcp%1").arg(i+1));
        GlobalParam->systemParam.tcpComStruct[i].ip = config->value("Ip", "192.168.88.88").toString();
        GlobalParam->systemParam.tcpComStruct[i].port = config->value("Port", 9998).toInt();
        GlobalParam->systemParam.tcpComStruct[i].serverAddress = config->value("ServerAddress", 1).toInt();
        config->endGroup();

        TcpComStruct param;
        param.ip = GlobalParam->systemParam.tcpComStruct[i].ip;
        param.port = GlobalParam->systemParam.tcpComStruct[i].port;
        param.serverAddress = GlobalParam->systemParam.tcpComStruct[i].serverAddress;

        m_serialComFormList.at(i)->SetParam(param);
    }

    delete config;
}

void WidgetAritightForm::UpdateParamToUI()
{
    LoadUIParam();
}

void WidgetAritightForm::SaveUIParam()
{
    if(GlobalParam->systemParam.filepath.isEmpty())
        return;
    QSettings *config = new QSettings(GlobalParam->systemParam.filepath+"/system.ini", QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));
    for (int i = 0;i<m_serialComFormList.count();++i)
    {
        config->beginGroup(QString("AirtightTcp%1").arg(i+1));
        TcpComStruct param = m_serialComFormList.at(i)->GetParam();
        GlobalParam->systemParam.tcpComStruct[i].ip = param.ip;
        GlobalParam->systemParam.tcpComStruct[i].port = param.port;
        GlobalParam->systemParam.tcpComStruct[i].serverAddress = param.serverAddress;
        config->setValue("Ip", GlobalParam->systemParam.tcpComStruct[i].ip);
        config->setValue("Port", GlobalParam->systemParam.tcpComStruct[i].port);
        config->setValue("ServerAddress", GlobalParam->systemParam.tcpComStruct[i].serverAddress);
        config->endGroup();
    }
    delete config;
}
