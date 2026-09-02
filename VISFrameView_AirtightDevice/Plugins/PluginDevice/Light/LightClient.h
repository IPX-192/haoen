#ifndef LIGHTCLIENT_H
#define LIGHTCLIENT_H

#include <QObject>
#include <QtSerialPort/QSerialPort> // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo> // 提供系统中存在的串口信息
#include "../../Plugins/PluginParam/ParamDef.h"

class LightClient : public QObject
{
    Q_OBJECT
public:
    explicit LightClient(QObject *parent = nullptr);

    bool OpenCom(LightStruct lightStruct);
    bool CloseCom() ;

    bool DataSend(QByteArray data);
    // 串口是否连接
    bool IsConnected();

private:
    // 串口开关
    bool OpenSerial();

signals:
    void sigRecvData(QString name,QByteArray data);
private slots:
    void DataReceived();

private:
    QSerialPort *m_serial = nullptr; // 定义全局的串口对象
    // 参数配置
    QString m_serComName="COM1";//串口号
    int m_serFlowCtrl=0;//控制流
    int m_serParity=0;//校验位
    int m_serStopBit=1;//停止位
    int m_serDataBit=8;//数据位
    int m_serBaudRate=9600;//波特率

    bool  m_isConnect = false;
};

#endif // LIGHTCLIENT_H
