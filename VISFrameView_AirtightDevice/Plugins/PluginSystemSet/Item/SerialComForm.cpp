#include "SerialComForm.h"
#include "ui_SerialComForm.h"
#include "VisAppBus.h"

SerialComForm::SerialComForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialComForm)
{
    ui->setupUi(this);
}

SerialComForm::~SerialComForm()
{
    delete ui;
}

void SerialComForm::SetDevice(int device)
{
    m_device = device;
}

TcpComStruct SerialComForm::GetParam()
{
    TcpComStruct param;
    param.ip = ui->lineEdit_ip->text().trimmed();
    param.port = ui->lineEdit_port->text().toInt();
    param.serverAddress = 1;   //Modbus Unit ID 默认1
    return param;
}

void SerialComForm::SetParam(TcpComStruct param)
{
    ui->lineEdit_ip->setText(param.ip);
    ui->lineEdit_port->setText(QString::number(param.port));
}

void SerialComForm::on_PushButtonOpenCom_clicked()
{
    VisAppBus::sendEvent("ConnectAritight", m_device);
}

void SerialComForm::on_PushButtonCloseCom_clicked()
{
    VisAppBus::sendEvent("DisConnectAritight", m_device);
}
