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

    for (int i = 0;i<StationCount;++i)
    {
        SerialComForm *aritightForm = new SerialComForm;
        ui->tabWidget->addTab(aritightForm,QString(u8"工位%1").arg(i+1));
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
        config->beginGroup(QString("SerCom%1").arg(i+1));
        GlobalParam->systemParam.serialComStruct[i].portName    = config->value("PortName"   ).toString();
        GlobalParam->systemParam.serialComStruct[i].serParity   = config->value("SerParity"  ).toInt();
        GlobalParam->systemParam.serialComStruct[i].serDataBit  = config->value("SerDataBit" ).toInt();
        GlobalParam->systemParam.serialComStruct[i].serStopBit  = config->value("SerStopBit" ).toInt();
        GlobalParam->systemParam.serialComStruct[i].serBaudRate = config->value("SerBaudRate").toInt();
        GlobalParam->systemParam.serialComStruct[i].serFlowCtrl = config->value("SerFlowCtrl").toInt();
        config->endGroup();

        SerialComStruct param;
        param.portName    = GlobalParam->systemParam.serialComStruct[i].portName    ;
        param.serParity   = GlobalParam->systemParam.serialComStruct[i].serParity   ;
        param.serDataBit  = GlobalParam->systemParam.serialComStruct[i].serDataBit  ;
        param.serStopBit  = GlobalParam->systemParam.serialComStruct[i].serStopBit  ;
        param.serBaudRate = GlobalParam->systemParam.serialComStruct[i].serBaudRate ;
        param.serFlowCtrl = GlobalParam->systemParam.serialComStruct[i].serFlowCtrl ;

        m_serialComFormList.at(i)->SetParam(param);
    }
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
        config->beginGroup(QString("SerCom%1").arg(i+1));
        SerialComStruct param = m_serialComFormList.at(i)->GetParam();
        GlobalParam->systemParam.serialComStruct[i].portName = param.portName;
        GlobalParam->systemParam.serialComStruct[i].serParity = param.serParity;
        GlobalParam->systemParam.serialComStruct[i].serDataBit = param.serDataBit;
        GlobalParam->systemParam.serialComStruct[i].serStopBit = param.serStopBit;
        GlobalParam->systemParam.serialComStruct[i].serBaudRate = param.serBaudRate;
        GlobalParam->systemParam.serialComStruct[i].serFlowCtrl = param.serFlowCtrl;
        config->setValue("PortName",       GlobalParam->systemParam.serialComStruct[i].portName   );
        config->setValue("SerParity",      GlobalParam->systemParam.serialComStruct[i].serParity  );
        config->setValue("SerDataBit",     GlobalParam->systemParam.serialComStruct[i].serDataBit );
        config->setValue("SerStopBit",     GlobalParam->systemParam.serialComStruct[i].serStopBit );
        config->setValue("SerBaudRate",    GlobalParam->systemParam.serialComStruct[i].serBaudRate);
        config->setValue("SerFlowCtrl",    GlobalParam->systemParam.serialComStruct[i].serFlowCtrl);
        config->endGroup();
    }
}
