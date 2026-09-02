#include "CModbusClient.h"
#include <QModbusRtuSerialMaster>
#include <QLoggingCategory>
#include <QTime>
#include <QApplication>
#include <QModbusPdu>
#include <QSerialPortInfo>
#include <QSettings>
#include <QTextCodec>
#include <QTimer>
#include <QMetaMethod>

CModbusClient::CModbusClient(QObject *parent) : QObject(parent)
{
    m_modBusParams.parity = QSerialPort::NoParity;
    m_modBusParams.baud = QSerialPort::Baud9600;
    m_modBusParams.dataBits = QSerialPort::Data8;
    m_modBusParams.stopBits = QSerialPort::OneStop;
    m_modBusParams.responseTime = 2000;
    m_modBusParams.numberOfRetries = 3;
//    connect(this, &CModbusClient::ReplyMessage, [this](bool sendFlag,
//            QString localAddress, QString functionCode, QString data){
//        m_sendFlag = sendFlag;
//        m_localAddress = localAddress;
//        m_functionCode = functionCode;
//        m_replyData = data;
//    });
}

CModbusClient::~CModbusClient()
{
    if (m_device)
    {
        m_device->disconnectDevice();
        delete m_device;
    }
}

bool CModbusClient::GetConnectState()
{
    return m_connectFlag;
}

bool CModbusClient::DeviceConnect()
{
    if (m_device)
    {
        DeviceDisConnect();
        delete m_device;
    }
    m_device = new QModbusRtuSerialMaster(this);
    m_device->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_modBusParams.port);
    m_device->setConnectionParameter(QModbusDevice::SerialParityParameter, m_modBusParams.parity);
    m_device->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, m_modBusParams.dataBits);
    m_device->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, m_modBusParams.stopBits);
    m_device->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, m_modBusParams.baud);
    m_device->setTimeout(m_modBusParams.responseTime);
    m_device->setNumberOfRetries(m_modBusParams.numberOfRetries);

    connect(m_device, &QModbusDevice::errorOccurred, this, [this](QModbusDevice::Error) {
        DeviceDisConnect();
        return false;
    }, Qt::QueuedConnection);

    connect(m_device, &QModbusDevice::stateChanged, [](QModbusDevice::State state) {
        switch (state) {
        case QModbusDevice::UnconnectedState:
            //    qDebug().noquote() << QStringLiteral("State: Entered unconnected state.");
            break;
        case QModbusDevice::ConnectingState:
            //  qDebug().noquote() << QStringLiteral("State: Entered connecting state.");
            break;
        case QModbusDevice::ConnectedState:
            //  qDebug().noquote() << QStringLiteral("State: Entered connected state.");
            break;
        case QModbusDevice::ClosingState:
            //   qDebug().noquote() << QStringLiteral("State: Entered closing state.");
            break;
        }
    });
    m_connectFlag = m_device->connectDevice();
    return m_connectFlag;
}

bool CModbusClient::DeviceDisConnect()
{
    m_connectFlag = false;
    if (!m_device)
        return false;
    m_device->disconnect();
    m_device->disconnectDevice();
    return true;
}

void CModbusClient::SendData(QByteArray sendData)
{
    const bool isCustom = true;             //自定义功能区代码
    const QByteArray pduData = sendData;
    QModbusReply *reply = nullptr;
    if (isCustom && pduData.isEmpty())
        return;
    const quint8 address = m_serverAddress;
    if (isCustom)
        reply = m_device->sendRawRequest(QModbusRequest(QModbusRequest::FunctionCode(
                                                            pduData[0]), pduData.mid(1)), address);
    if (reply)
    {
        if (!reply->isFinished())
        {
            connect(reply, &QModbusReply::finished, [reply, this](){
                m_replyFlag = true;
                emit ReplyMessage(true, QString::number(reply->serverAddress()),
                                  QString::number(reply->rawResult().functionCode()),
                                  reply->rawResult().data().toHex().toUpper());
                reply->deleteLater();
            });
        }
        else
        {
            m_replyFlag = true;
            emit ReplyMessage(false, "", "", "");  //发送失败
            reply->deleteLater();
        }
    }
}

void CModbusClient::SetParams(Settings serial)
{
    m_modBusParams.port = serial.port;
    m_modBusParams.parity = serial.parity;
    m_modBusParams.baud = serial.baud;
    m_modBusParams.dataBits = serial.dataBits;
    m_modBusParams.stopBits = serial.stopBits;
    m_modBusParams.responseTime = serial.responseTime;
    m_modBusParams.numberOfRetries = serial.numberOfRetries;
}

void CModbusClient::SleepEvents(int msec)
{
    QTime time;
    time.start();
    while (time.elapsed() < msec)
        QApplication::processEvents();
}

// 内部辅助函数实现（支撑多通道功能）
void CModbusClient::ResetReplyCache()
{
    m_replyFlag = false;
    m_sendFlag = false;
    m_localAddress.clear();
    m_functionCode.clear();
    m_replyData.clear();
}

// 选择测试程序（1=IP67，2=IP68）
bool CModbusClient::SelectTestProgram(int programId)
{
    if (!m_connectFlag)
        return false;

    if (m_device->state() != QModbusDevice::ConnectedState) {
        qWarning() << "Modbus未连接，无法选择程序";
        return false;
    }

    // 将int转换为16位无符号整数（Modbus寄存器是16位），避免4字节截断错误
    uint16_t regValue = static_cast<uint16_t>(programId); // 确保programId在0~65535范围内

    // 将16位值转换为大端字节序（Modbus标准）
    union {
        uint16_t u16;
        uint8_t bytes[2];
    } regUnion;
    regUnion.u16 = regValue;

    // 手动转大端（兼容不同CPU字节序）
    uint8_t highByte = (regUnion.u16 >> 8) & 0xFF;  // 高字节
    uint8_t lowByte = regUnion.u16 & 0xFF;          // 低字节

    QByteArray data;
    data.append(static_cast<uint8_t>(0x10));      // 功能码0x10
    data.append(static_cast<uint8_t>(0x20));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));        // 寄存器个数高字节
    data.append(static_cast<uint8_t>(0x01));      // 寄存器个数低字节
    data.append(static_cast<uint8_t>(0x02));      // 数据长度4字节
    data.append(highByte);  // 最高字节
    data.append(lowByte);

    bool result = WaitResult(data);
    ResetReplyCache();
    return result;
}
// 启动
bool CModbusClient::StartAirtightTest()
{
    //启动
    //<S>: ff 05 00 01 ff 00 c8 24 <R>: ff 05 00 01 ff 00 c8 24
    if (!m_connectFlag)
        return false;

    if (m_device->state() != QModbusDevice::ConnectedState) {
        qWarning() << "Modbus未连接，无法选择程序";
        return false;
    }

    QByteArray data;
    data.append(static_cast<uint8_t>(0x05));      // 功能码0x10
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x01));
    data.append(static_cast<uint8_t>(0xff));        // 寄存器个数高字节
    data.append(static_cast<char>(0x00));

    bool result = WaitResult(data);
    ResetReplyCache();
    return result;
}
// 复位
bool CModbusClient::ResetAirtight()
{
    //复位（停止）
    //<S>: ff 05 00 00 ff 00 99 e4<R>: ff 05 00 00 ff 00 99 e4
    if (!m_connectFlag)
        return false;

    if (m_device->state() != QModbusDevice::ConnectedState) {
        return false;
    }

    QByteArray data;
    data.append(static_cast<uint8_t>(0x05));      // 功能码0x10
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0xff));
    data.append(static_cast<char>(0x00));

    bool result = WaitResult(data);
    ResetReplyCache();
    return result;
}

bool CModbusClient::SelectParam(int paramId)
{
    if (!m_connectFlag)
        return false;

    if (m_device->state() != QModbusDevice::ConnectedState) {
        qWarning() << "Modbus未连接，无法选择程序";
        return false;
    }

    // 将int转换为16位无符号整数（Modbus寄存器是16位），避免4字节截断错误
    uint16_t regValue = static_cast<uint16_t>(paramId-1); // 确保programId在0~65535范围内

    // 将16位值转换为小端字节序（Modbus标准）
    union {
        uint16_t u16;
        uint8_t bytes[2];
    } regUnion;
    regUnion.u16 = regValue;

    // 手动转小端（兼容不同CPU字节序）
    uint8_t highByte = (regUnion.u16 >> 8) & 0xFF;  // 高字节
    uint8_t lowByte = regUnion.u16 & 0xFF;          // 低字节

    QByteArray data;
    data.append(static_cast<uint8_t>(0x10));      // 功能码0x10
    data.append(static_cast<uint8_t>(0x20));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));        // 寄存器个数高字节
    data.append(static_cast<uint8_t>(0x01));      // 寄存器个数低字节
    data.append(static_cast<uint8_t>(0x02));      // 数据长度4字节
    data.append(lowByte);
    data.append(highByte);

    bool result = WaitResult(data);
    ResetReplyCache();
    return result;
}

bool CModbusClient::SetParam(CModbusClient::ParamMode mode, int param)
{
    if (!m_connectFlag)
        return false;

    if (m_device->state() != QModbusDevice::ConnectedState) {
        qWarning() << "Modbus未连接，无法选择程序";
        return false;
    }

    // 将int转换为32位无符号整数（Modbus寄存器是16位），避免4字节截断错误
    uint32_t regValue = static_cast<uint32_t>(param); // 确保programId在0~65535范围内

    // 将16位值转换为小端字节序（Modbus标准）
    union {
        uint32_t u32;
        uint8_t bytes[4];
    } regUnion;
    regUnion.u32 = regValue;

    uint8_t byte1 = (regUnion.u32 >> 24) & 0xFF;
    uint8_t byte2 = (regUnion.u32 >> 16) & 0xFF;
    uint8_t byte3 = (regUnion.u32 >> 8) & 0xFF;
    uint8_t byte4 = regUnion.u32 & 0xFF;

    QByteArray data;
    data.append(static_cast<uint8_t>(0x10));      // 功能码0x10
    data.append(static_cast<uint8_t>(0x60));
    switch (mode) {
    case MODE_CouplADelayTime://延时
        data.append(static_cast<char>(0x0A));
        break;
    case MODE_FillTime://充气时间
        data.append(static_cast<char>(0x01));
        break;
    case MODE_StabTime://稳定时间
        data.append(static_cast<char>(0x02));
        break;
    case MODE_TestTime://测试时间
        data.append(static_cast<char>(0x03));
        break;
    case MODE_DumpTime://排气时间
        data.append(static_cast<char>(0x09));
        break;
    case MODE_PressUnit://压力单位
        data.append(static_cast<char>(0x35));
        GetPressUnit(param,byte1,byte2,byte3,byte4);
        break;
    case MODE_MaxFill://最大充气压力
        data.append(static_cast<char>(0x33));
        break;
    case MODE_MinFill://最小充气压力
        data.append(static_cast<char>(0x32));
        break;
    case MODE_SetFill://设定的充气压力
        data.append(static_cast<char>(0x42));
        break;
    case MODE_LeakUnit://泄漏单位
        data.append(static_cast<char>(0x7F));
        GetLeakUnit(param,byte1,byte2,byte3,byte4);
        break;
    case MODE_VolumeUnit://容积单位
        data.append(static_cast<char>(0xA1));
        GetVolumeUnit(param,byte1,byte2,byte3,byte4);
        break;
    case MODE_Volume://容积大小
        data.append(static_cast<char>(0x14));
        break;
    case MODE_TestFail://泄漏上限
        data.append(static_cast<char>(0x3C));
        break;
    case MODE_RefFail://泄漏下限
        data.append(static_cast<char>(0x3E));
        break;
    }
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x02));
    data.append(static_cast<char>(0x04));        // 寄存器个数高字节
    data.append(byte4);
    data.append(byte3);
    data.append(byte2);
    data.append(byte1);

    bool result = WaitResult(data);
    ResetReplyCache();
    return result;
}

int CModbusClient::ReadRealStatus()
{
    if (!m_connectFlag)
        return -1;

    if (m_device->state() != QModbusDevice::ConnectedState) {
        return -1;
    }

    QByteArray data;
    data.append(static_cast<uint8_t>(0x03));      // 功能码0x03
    data.append(static_cast<uint8_t>(0x22));
    data.append(static_cast<uint8_t>(0x04));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x01));        // 寄存器个数高字节

    bool result = WaitResult(data);
    int res = 0;
    if(result)
    {
        QByteArray currBytes = QByteArray::fromHex(m_replyData.toUtf8());
        int count = currBytes.size();//需确认回复的长度
        if(currBytes.at(1) == 0x20 && currBytes.at(2) == 0x80)
        {
            res = 0;
        }
        else if(currBytes.at(1) == 0x80 && currBytes.at(2) == 0x20)
        {
            res = 3;
        }
    }
    ResetReplyCache();
    return res;
}
// 工具函数：字节数组转 16 位无符号整数（大端/小端可选）
quint16 CModbusClient:: ByteArrayToUInt16(const QByteArray &ba, bool littleEndian)
{
    if (ba.size() < 2) return 0;
    quint16 value;
    QDataStream stream(ba);
    stream.setByteOrder(littleEndian ? QDataStream::LittleEndian : QDataStream::BigEndian);
    stream >> value;
    return value;
}
int CModbusClient::ReadRealResult(AritghtResult &result)
{
    if (!m_connectFlag)
        return -1;

    if (m_device->state() != QModbusDevice::ConnectedState) {
        return -1;
    }

    QByteArray data;
    data.append(static_cast<uint8_t>(0x03));      // 功能码0x03
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x30));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x0D));

    bool waitRes = WaitResult(data);
    int res = 0;
    if(waitRes)
    {
        QByteArray currBytes = QByteArray::fromHex(m_replyData.toUtf8());
        QByteArray arrayNum = currBytes.mid(4,2);//结果数量
        QByteArray arrayState = currBytes.mid(8,2);//测试状态
        QByteArray arrayStage = currBytes.mid(10,2);//阶段

        QByteArray arrayNumTmp;
        arrayNumTmp.append(arrayNum.at(1));
        arrayNumTmp.append(arrayNum.at(0));
        int num = ByteArrayToUInt16(arrayNumTmp,true) ;
        QVector<QByteArray> dataVec;
        for(int i = 0;i<num;++i)
        {
            dataVec.append(currBytes.mid(12+i*8,4));
        }
        if(arrayState.at(0) == 0x20 && arrayState.at(1) == 0x80)
        {
            res = 0;
        }
        else if(arrayState.at(0) == 0x80 && arrayState.at(1) == 0x20)
        {
            res = 2;
        }
        result.TestStage = SwapOrder(arrayStage).toInt();
        result.pressValue = SwapOrder(dataVec[0]).toInt();
        if(dataVec.size()>=2)
            result.leakageValue = SwapOrder(dataVec[1]).toInt();
    }
    ResetReplyCache();
    return res;
}

int CModbusClient::ReadLastResult(AritghtResult &result)
{
    int res = ReadRealStatus();
    if(res != 0){
        return -1;
    }

    if (!m_connectFlag)
        return -1;

    if (m_device->state() != QModbusDevice::ConnectedState) {
        return -1;
    }
    //FF 03 00 11 00 0C 00 14
    QByteArray data;
    data.append(static_cast<uint8_t>(0x03));      // 功能码0x03
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x11));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x0C));

    bool waitRes = WaitResult(data);
    if(waitRes)
    {
        QByteArray currBytes = QByteArray::fromHex(m_replyData.toUtf8());
        QByteArray arrayResult= currBytes.mid(6,2);//结果
        QByteArray arrayError = currBytes.mid(8,2);//报警信息
        QByteArray arrayPress = currBytes.mid(10,4);//压力值
        QByteArray arrayLeaks = currBytes.mid(18,4);//泄漏值

        result.res = SwapOrder(arrayResult).toInt();
        result.errorMsg = GetResultMsg(SwapOrder(arrayResult));
        result.alarmCode = SwapOrder(arrayError).toInt();
        if(result.alarmCode == 0){
            result.pressValue = SwapOrder(arrayPress).toInt();
            result.leakageValue = SwapOrder(arrayLeaks).toInt();
        }
    }
    ResetReplyCache();
    res = 0;
    return res;
}

void CModbusClient::GetPressUnit(int param,uint8_t &byte1, uint8_t &byte2, uint8_t &byte3, uint8_t &byte4)
{
    if(param == 0){//bar
        byte4 = 0x2A;
        byte3 = 0xF8;
        byte2 = 0x00;
        byte1 = 0x00;
    }
    else if(param == 1){//mbar
        byte4 = 0x36;
        byte3 = 0xB0;
        byte2 = 0x00;
        byte1 = 0x00;
    }
    else if(param == 2){//psi
        byte4 = 0x32;
        byte3 = 0xC8;
        byte2 = 0x00;
        byte1 = 0x00;
    }
    else if(param == 3){//pa
        byte4 = 0xA7;
        byte3 = 0xF8;
        byte2 = 0x00;
        byte1 = 0x00;
    }
    else if(param == 4){//kpa
        byte4 = 0x2E;
        byte3 = 0xE0;
        byte2 = 0x00;
        byte1 = 0x00;
    }
    else if(param == 5){//mpa
        byte4 = 0x3A;
        byte3 = 0x98;
        byte2 = 0x00;
        byte1 = 0x00;
    }
}

void CModbusClient::GetLeakUnit(int param, uint8_t &byte1, uint8_t &byte2, uint8_t &byte3, uint8_t &byte4)
{
    if(param == 0){//cm3/s
        byte4 = 0x00;
        byte3 = 0x00;
        byte2 = 0x00;
        byte1 = 0x00;
    }
    else if(param == 1){//cm3/min
        byte4 = 0x03;
        byte3 = 0xE8;
        byte2 = 0x00;
        byte1 = 0x00;
    }
    else if(param == 2){//cm3/h
        byte4 = 0x07;
        byte3 = 0xD0;
        byte2 = 0x00;
        byte1 = 0x00;
    }
    else if(param == 3){//mm3/s
        byte4 = 0x0B;
        byte3 = 0xB8;
        byte2 = 0x00;
        byte1 = 0x00;
    }
}

void CModbusClient::GetVolumeUnit(int param, uint8_t &byte1, uint8_t &byte2, uint8_t &byte3, uint8_t &byte4)
{
    if(param == 0){//ml
        byte4 = 0xEE;
        byte3 = 0x48;
        byte2 = 0x00;
        byte1 = 0x00;
    }
    else if(param == 1){//l
        byte4 = 0xF2;
        byte3 = 0x30;
        byte2 = 0x00;
        byte1 = 0x00;
    }
}

bool CModbusClient::WaitResult(QByteArray sendData)
{
    ResetReplyCache();

    bool result = true;
    QEventLoop look;
    QMetaObject::Connection c1;
    c1 = connect(this, &CModbusClient::ReplyMessage, [&](bool sendFlag,
            QString localAddress, QString functionCode, QString data){
        m_sendFlag = sendFlag;
        m_localAddress = localAddress;
        m_functionCode = functionCode;
        m_replyData = data;
        look.quit();
    });

    SendData(sendData);

    QTimer timer;
    connect(&timer, &QTimer::timeout, this, [&]()
    {
        m_sendFlag = false;
        look.quit(); // 超时退出
    });
    timer.start(m_modBusParams.responseTime);
    //进入事件循环
    look.exec();
    timer.stop();
    disconnect(c1);

    if (!m_sendFlag || (m_localAddress.toInt() != m_serverAddress) || (m_functionCode.toInt() != sendData[0])) {
        result = false;
    } else {
        result = true;
    }

    return result;
}

QByteArray CModbusClient::SwapOrder(QByteArray in)
{
    QByteArray data ;
    if(in.size()<2)
        return data;
    for(int i = (in.size()-1);i>=0;--i)
    {
        data.append(in.at(i));
    }
    return data;
}

QString CModbusClient::GetResultMsg(QByteArray data)
{
    QString msg = "";
    if (data.size() < 2) {
        msg = QString(u8"数据长度不足，无法解析");
        return msg;
    }
    quint16 statusValue = *reinterpret_cast<const quint16*>(data.constData());

    // 定义每一位的掩码
    const quint16 BIT0_PASS = 0x0001;
    const quint16 BIT1_MAX_FLOW_FAIL = 0x0002;
    const quint16 BIT2_MIN_FLOW_FAIL = 0x0004;
    const quint16 BIT3_ALARM = 0x0008;

    // 位运算判断每一位的状态
    bool isPass = (statusValue & BIT0_PASS) != 0;
    bool isMaxFlowFail = (statusValue & BIT1_MAX_FLOW_FAIL) != 0;
    bool isMinFlowFail = (statusValue & BIT2_MIN_FLOW_FAIL) != 0;
    bool isAlarm = (statusValue & BIT3_ALARM) != 0;

    if(isPass)
    {
        msg = "OK";
    }
    if(isMaxFlowFail)
    {
        msg = QString(u8"NG,泄露超上限");
    }
    if(isMinFlowFail)
    {
        msg = QString(u8"NG,泄露超下限");
    }
    if(isAlarm)
    {
        msg = QString(u8"报警");
    }
    return msg;
}
