#include "LightClient.h"

LightClient::LightClient(QObject *parent) : QObject(parent)
{

}

bool LightClient::OpenCom(LightStruct lightStruct)
{
    if(lightStruct.portName.isEmpty())
        return  false;
    m_serComName = lightStruct.portName;

    m_serial = new QSerialPort; //申请内存,并设置父对象
    //波特率
    if(-1 == lightStruct.serBaudRate)
    {
        return false;
    }
    m_serial->setBaudRate(lightStruct.serBaudRate);
    // 校验，校验默认选择无
    if(-1 == lightStruct.serParity)
    {
        return false;
    }
    m_serial->setParity((QSerialPort::Parity)lightStruct.serParity);
    // 数据位，数据位默认选择8位
    if(-1 == lightStruct.serDataBit)
    {
        return false;
    }
    m_serial->setDataBits((QSerialPort::DataBits)lightStruct.serDataBit);
    // 停止位，停止位默认选择1位
    if(-1 == lightStruct.serStopBit)
    {
        return false;
    }
    m_serial->setStopBits((QSerialPort::StopBits)lightStruct.serStopBit);
    // 控制流，默认选择无
    if(-1 == lightStruct.serFlowCtrl)
    {
        return false;
    }
    m_serial->setFlowControl((QSerialPort::FlowControl)lightStruct.serFlowCtrl);

    if(!OpenSerial())
    {
       return false;
    }

    // 信号
    connect(m_serial,SIGNAL(readyRead()),this,SLOT(DataReceived()));// 接收数据

    return true;
}

bool LightClient::CloseCom()
{
    if ( nullptr == m_serial)
    {
        return false;
    }
    if(m_serial->isOpen()) // 如果串口打开了，先给他关闭
    {
        m_serial->clear();
        m_serial->close();
    }
    delete m_serial;
    m_serial = nullptr;

    m_isConnect = false;

    return  true;
}

bool LightClient::DataSend( QByteArray data)
{
    if (nullptr == m_serial)
    {
        return false;
    }
    if (!m_serial->isOpen())
    {
        return false;
    }
    m_serial->write(data);
    m_serial->flush();
    return true;
}

bool LightClient::IsConnected()
{
    return m_isConnect;
}

bool LightClient::OpenSerial()
{
    if (nullptr == m_serial)
    {
        return false;
    }
    if(m_serial->isOpen()) // 如果串口打开了，先给他关闭
    {
        m_serial->clear();
        m_serial->close();
    }
    //当前选择的串口名字
    m_serial->setPortName(m_serComName);
    //用ReadWrite 的模式尝试打开串口，无法收发数据时，发出警告
    if(!m_serial->open(QIODevice::ReadWrite))
    {
        return false;
    }
    m_isConnect = true;
    return true;
}

void LightClient::DataReceived()
{
    if ( nullptr == m_serial)
    {
        return;
    }
    QByteArray data = m_serial->readAll(); // 读取数据
    if(!data.isEmpty()) // 接收到数据
    {
        emit sigRecvData(m_serComName,data);
    }
    m_serial->flush();
}
