#include "SerialComForm.h"
#include "ui_SerialComForm.h"
#include <QSerialPortInfo>
#include "VisAppBus.h"

SerialComForm::SerialComForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialComForm)
{
    ui->setupUi(this);
    ui->comboBox_com->clear();
    ui->comboBox_com->addItems(GetPortNameList());
}

SerialComForm::~SerialComForm()
{
    delete ui;
}

void SerialComForm::SetStation(int station)
{
    m_station = station;
}

SerialComStruct SerialComForm::GetParam()
{
    SerialComStruct param;
    param.portName = ui->comboBox_com->currentText();
    param.serParity = ui->comboBox_parity->currentText().toInt();
    param.serDataBit = ui->comboBox_data->currentText().toInt();

    param.serStopBit = ui->comboBox_stop->currentText().toInt();
    param.serBaudRate = ui->comboBox_baudrate->currentText().toInt();
    param.serFlowCtrl = ui->comboBox_ctrl->currentIndex();
    return param;
}

void SerialComForm::SetParam(SerialComStruct param)
{
    ui->comboBox_com->setCurrentText(param.portName);
    ui->comboBox_ctrl->setCurrentIndex(param.serFlowCtrl);
    ui->comboBox_stop->setCurrentText(QString::number(param.serStopBit));
    ui->comboBox_data->setCurrentText(QString::number(param.serDataBit));
    ui->comboBox_parity->setCurrentText(QString::number(param.serParity));
    ui->comboBox_baudrate->setCurrentText(QString::number(param.serBaudRate));
}
//获取可用串口列表
QStringList SerialComForm::GetPortNameList()
{
    QList<QSerialPortInfo> list;
    QStringList listPortName;
    list = QSerialPortInfo::availablePorts();
    for (int i = 0; i < list.size(); i++)
    {
        listPortName << list.at(i).portName() ;//串口名字
    }
    return listPortName;
}

void SerialComForm::on_PushButtonOpenCom_clicked()
{
    VisAppBus::sendEvent("ConnectAritight",m_station);
}

void SerialComForm::on_PushButtonCloseCom_clicked()
{
    VisAppBus::sendEvent("DisConnectAritight",m_station);
}
