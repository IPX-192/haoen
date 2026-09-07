#include "TrayRfidManager.h"
#include <QTime>
#include <QThread>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QModbusDevice>
#include "VisAppBus.h"
#include "ParamManager.h"
#include <QModbusRtuSerialMaster>

SINGLETON_IMPL(TrayRfidManager)
TrayRfidManager::TrayRfidManager(QObject *parent) : QObject(parent)
{
	m_modBusParams.parity = QSerialPort::NoParity;
	m_modBusParams.baud = QSerialPort::Baud115200;
	m_modBusParams.dataBits = QSerialPort::Data8;
	m_modBusParams.stopBits = QSerialPort::OneStop;
    m_modBusParams.responseTime = 700;
	m_modBusParams.port = QStringLiteral("COM10");
	m_device = new QModbusRtuSerialMaster(this);
}

bool TrayRfidManager::connect()
{
    disConnect();
	if (m_bConnectState)return true;
	m_device->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_modBusParams.port);
	m_device->setConnectionParameter(QModbusDevice::SerialParityParameter, m_modBusParams.parity);
	m_device->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, m_modBusParams.dataBits);
	m_device->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, m_modBusParams.stopBits);
	m_device->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, m_modBusParams.baud);
	m_device->setTimeout(m_modBusParams.responseTime);
	m_device->setNumberOfRetries(m_modBusParams.numberOfRetries);

//	QObject::connect(m_device, &QModbusDevice::errorOccurred, this, [this](QModbusDevice::Error) {
//		disConnect();
//	}, Qt::QueuedConnection);

	m_bConnectState = m_device->connectDevice();
	return m_bConnectState;
}

bool TrayRfidManager::disConnect()
{
	if (m_device == nullptr) return false;
	m_device->disconnect();
	m_device->disconnectDevice();
	m_bConnectState = false;

	return true;
}

bool TrayRfidManager::addRfid(int num)
{
    //添加序号上如果没有RFID则添加
    if(!m_allRfidCtrl.keys().contains(num)){
        CModbusRFIDClient *rfid = new CModbusRFIDClient(num,m_device,this);
        m_allRfidCtrl[num]= rfid;
    }
    else{
        m_errInfo=QStringLiteral("当前存在RFID读码器");
    }
    return true;
}

bool TrayRfidManager::removeRfid(int num)
{
    //判断序号上是否有RFID可以删除
    if(m_allRfidCtrl.keys().contains(num)){
        m_allRfidCtrl.remove(num);
    }
    else{
        m_errInfo=QStringLiteral("当前不存在RFID读码器");
    }
    return true;
}

bool TrayRfidManager::setRfidParm(QString comName,int baud)
{
    m_modBusParams.port = comName.toUpper();
    m_modBusParams.baud = baud;
    return true;
}


QString TrayRfidManager::readCurError()
{
    return m_errInfo;
}

bool TrayRfidManager::GetConnnectState()
{
    return m_bConnectState;
}

bool TrayRfidManager::SetModbusModel(int num)
{
    if(m_allRfidCtrl.keys().contains(num)){
        if(!m_allRfidCtrl[num]->SetModbusModel()){
            m_errInfo=m_allRfidCtrl[num]->readCurError();
            return false;
        }
    }
    else{
        m_errInfo=QStringLiteral("当前不存在RFID读码器");
        return false;

    }
    return true;
}

bool TrayRfidManager::GetModbusRFIDExist(int num,bool &exist)
{
    if(m_allRfidCtrl.keys().contains(num)){
        if(!m_allRfidCtrl[num]->GetModbusRFIDExist(exist)){
            m_errInfo=m_allRfidCtrl[num]->readCurError();
            return false;
        }
    }
    else{
        m_errInfo=QStringLiteral("当前不存在RFID读码器,reqNum:%1,existKeys:%2")
                      .arg(num).arg([this](){QStringList l;for(auto k:m_allRfidCtrl.keys())l<<QString::number(k);return l.join(',');}());
        return false;

    }
    return true;
}

bool TrayRfidManager::GetModbusTrayCode(int num,QString &barCode, int length)
{
    if(m_allRfidCtrl.keys().contains(num)){
        if(!m_allRfidCtrl[num]->GetModbusTrayCode(barCode,length)){
            m_errInfo=m_allRfidCtrl[num]->readCurError();
            return false;
        }
    }
    else{
        m_errInfo=QStringLiteral("当前不存在RFID读码器,reqNum:%1,existKeys:%2")
                      .arg(num).arg([this](){QStringList l;for(auto k:m_allRfidCtrl.keys())l<<QString::number(k);return l.join(',');}());
        return false;

    }
    return true;
}

bool TrayRfidManager::SetModbusTrayCode(int num,QString &barCode)
{
    if(m_allRfidCtrl.keys().contains(num)){
        if(!m_allRfidCtrl[num]->SetModbusTrayCode(barCode)){
            m_errInfo=m_allRfidCtrl[num]->readCurError();
            return false;
        }
    }
    else{
        m_errInfo=QStringLiteral("当前不存在RFID读码器");
        return false;

    }
    return true;
}

bool TrayRfidManager::TriggerReadCard(int num)
{
    if(m_allRfidCtrl.keys().contains(num))
    {
        if(!m_allRfidCtrl[num]->TriggerReadCard())
        {
            m_errInfo = m_allRfidCtrl[num]->readCurError();

            ShowSystemLog(Log_Error,QString::fromUtf8("TriggerReadCard失败，num=%1，err=%2").arg(num).arg(m_errInfo));
            return false;
        }
    }
    else
    {
        m_errInfo = QStringLiteral("当前不存在RFID读码器");
        ShowSystemLog(Log_Error,QString::fromUtf8("TriggerReadCard：%1").arg(m_errInfo));
        return false;
    }
    return true;
}


bool TrayRfidManager::ReadRfidTag(int num, QString &outBarCode, int timeoutMs, int codeLength)
{
    outBarCode.clear();
    QElapsedTimer timer;
    timer.start();
    ShowSystemLog(Log_Debug, QString(u8"RFID读卡开始:num=%1,timeout=%2ms,codeLen=%3").arg(num).arg(timeoutMs).arg(codeLength));
    //读卡失败重试1次(标签感应/干扰容错)
    for (int attempt = 0; attempt < 2; attempt++)
    {
        ShowSystemLog(Log_Debug, QString(u8"RFID第%1次尝试触发读卡").arg(attempt + 1));
        if(!TriggerReadCard(num))
        {
            m_errInfo = QStringLiteral("第%1次触发读卡失败:%2").arg(attempt + 1).arg(readCurError());
            ShowSystemLog(Log_Debug, m_errInfo);
            if (attempt == 0) continue;
            return false;
        }

        QThread::msleep(timeoutMs);

        bool exist = false;
        //检查返回值:通信失败时exist不可信,不能误判为"未检测到标签"
        if (!GetModbusRFIDExist(num, exist))
        {
            m_errInfo = QStringLiteral("第%1次读取标签状态失败:%2").arg(attempt + 1).arg(readCurError());
            ShowSystemLog(Log_Debug, m_errInfo);
            if (attempt == 0) continue;
            return false;
        }
        ShowSystemLog(Log_Debug, QString(u8"RFID第%1次标签存在检测:exist=%2").arg(attempt + 1).arg(exist));

        // 未检测到标签:跳过读条码(无标签时读条码必然返回Modbus异常响应,重试无意义还多耗时)
        if (!exist) {
            ShowSystemLog(Log_Debug, QString(u8"RFID第%1次未检测到标签,跳过读条码").arg(attempt + 1));
            if (attempt == 0) {
                continue;   // 重新触发读卡再试一次(标签可能仍在感应中)
            }
            m_errInfo = QStringLiteral("未检测到RFID标签");
            ShowSystemLog(Log_Error, QString(u8"RFID读卡最终失败:num=%1,原因=%2,耗时=%3ms").arg(num).arg(m_errInfo).arg(timer.elapsed()));
            return false;
        }

        // 直接读取条码
        bool codeOk = GetModbusTrayCode(num, outBarCode, codeLength);
        QString realCode = outBarCode.trimmed();
        ShowSystemLog(Log_Debug, QString(u8"RFID第%1次读条码:codeOk=%2,rawLen=%3,trimLen=%4,code=%5")
            .arg(attempt + 1).arg(codeOk).arg(outBarCode.length()).arg(realCode.length()).arg(realCode));

        // 必须：有标签 + 通信成功 + 条码非空，才算成功
        if (exist && codeOk && !realCode.isEmpty())
        {
            outBarCode = realCode;
            ShowSystemLog(Log_Info, QString(u8"RFID读卡成功:num=%1,code=%2,耗时=%3ms").arg(num).arg(realCode).arg(timer.elapsed()));
            return true;
        }
        if (attempt == 0) {
            ShowSystemLog(Log_Debug, QString(u8"RFID第1次读码失败(codeOk=%1,exist=%2,trimLen=%3,原因:%4),重试第2次")
                .arg(codeOk).arg(exist).arg(realCode.length()).arg(readCurError()));
            continue;
        }

        // 两次都失败，最终错误信息
        if(!exist)
        {
            m_errInfo = QStringLiteral("未检测到RFID标签");
        }
        else if(!codeOk)
        {
            m_errInfo = QStringLiteral("读取条码通信失败");
        }
        else
        {
            m_errInfo = QStringLiteral("读取条码数据为空");
        }
        ShowSystemLog(Log_Error, QString(u8"RFID读卡最终失败:num=%1,原因=%2,耗时=%3ms").arg(num).arg(m_errInfo).arg(timer.elapsed()));
        return false;
    }
    return false;
}

