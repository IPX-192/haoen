#include "CModbusClient.h"
#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QModbusDevice>
#include <QEventLoop>
#include <QTimer>
#include <QDataStream>

CModbusClient::CModbusClient(QObject *parent) : QObject(parent)
{
    m_modBusParams.ip = "192.168.88.88";
    m_modBusParams.port = 9998;
    m_modBusParams.serverAddress = 1;
    m_modBusParams.responseTime = 2000;
    m_modBusParams.numberOfRetries = 3;
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
        m_device = nullptr;
    }

    m_device = new QModbusTcpClient(this);
    m_device->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_modBusParams.ip);
    m_device->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_modBusParams.port);
    m_device->setTimeout(m_modBusParams.responseTime);
    m_device->setNumberOfRetries(m_modBusParams.numberOfRetries);

    m_connectFlag = m_device->connectDevice();
    return m_connectFlag;
}

bool CModbusClient::DeviceDisConnect()
{
    m_connectFlag = false;
    if (!m_device)
        return false;
    m_device->disconnectDevice();
    return true;
}

void CModbusClient::SetParams(Settings cfg)
{
    m_modBusParams.ip = cfg.ip;
    m_modBusParams.port = cfg.port;
    m_modBusParams.serverAddress = cfg.serverAddress;
    m_modBusParams.responseTime = cfg.responseTime;
    m_modBusParams.numberOfRetries = cfg.numberOfRetries;
}

//同步等待一次 Modbus 回复,超时返回 false
bool CModbusClient::WaitReply(QModbusReply *reply)
{
    if (!reply)
        return false;

    bool ok = false;
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    connect(reply, &QModbusReply::finished, &loop, [&]() {
        if (reply->error() == QModbusDevice::NoError)
            ok = true;
        loop.quit();
    });
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(m_modBusParams.responseTime);
    loop.exec();
    timer.stop();

    reply->deleteLater();
    return ok;
}

//读保持寄存器(功能码0x03)
bool CModbusClient::ReadRegisters(quint16 addr, quint16 count, QVector<quint16> &values)
{
    if (!m_device || m_device->state() != QModbusDevice::ConnectedState)
        return false;

    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, addr, count);
    QModbusReply *reply = m_device->sendReadRequest(unit, m_modBusParams.serverAddress);
    if (!reply)
        return false;

    bool ok = false;
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    connect(reply, &QModbusReply::finished, &loop, [&]() {
        if (reply->error() == QModbusDevice::NoError)
        {
            values = reply->result().values();
            ok = true;
        }
        loop.quit();
    });
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(m_modBusParams.responseTime);
    loop.exec();
    timer.stop();

    reply->deleteLater();
    return ok;
}

//写单保持寄存器(功能码0x06)
bool CModbusClient::WriteRegister(quint16 addr, quint16 value)
{
    if (!m_device || m_device->state() != QModbusDevice::ConnectedState)
        return false;

    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, addr, 1);
    unit.setValue(0, value);
    QModbusReply *reply = m_device->sendWriteRequest(unit, m_modBusParams.serverAddress);
    return WaitReply(reply);
}

//写32位浮点到2个连续寄存器(Float ABCD 大端,功能码0x10)
bool CModbusClient::WriteFloatRegisters(quint16 addr, double value)
{
    if (!m_device || m_device->state() != QModbusDevice::ConnectedState)
        return false;

    //把 double 转成 float,按大端拆成4字节
    float f = static_cast<float>(value);
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream << f;

    quint16 hi = (static_cast<quint8>(ba.at(0)) << 8) | static_cast<quint8>(ba.at(1));
    quint16 lo = (static_cast<quint8>(ba.at(2)) << 8) | static_cast<quint8>(ba.at(3));

    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, addr, 2);
    unit.setValue(0, hi);
    unit.setValue(1, lo);
    QModbusReply *reply = m_device->sendWriteRequest(unit, m_modBusParams.serverAddress);
    return WaitReply(reply);
}

//写单线圈(功能码0x05)
bool CModbusClient::WriteCoil(quint16 addr, bool on)
{
    if (!m_device || m_device->state() != QModbusDevice::ConnectedState)
        return false;

    QModbusDataUnit unit(QModbusDataUnit::Coils, addr, 1);
    unit.setValue(0, on ? 0xFF00 : 0x0000);
    QModbusReply *reply = m_device->sendWriteRequest(unit, m_modBusParams.serverAddress);
    return WaitReply(reply);
}

//心跳(线圈11 写1)
bool CModbusClient::Heartbeat()
{
    return WriteCoil(11, true);
}

bool CModbusClient::StartTest(int channel)
{
    return WriteCoil(coilStart(channel), true);
}

bool CModbusClient::StopTest(int channel)
{
    return WriteCoil(coilStop(channel), true);
}

bool CModbusClient::StartAll()
{
    return WriteCoil(1, true);
}

bool CModbusClient::StopAll()
{
    return WriteCoil(2, true);
}

int CModbusClient::ReadFinishState(int channel)
{
    QVector<quint16> values;
    if (!ReadRegisters(regFinishState(channel), 1, values))
        return -1;
    return static_cast<int>(values.value(0, 0));
}

int CModbusClient::ReadStage(int channel)
{
    QVector<quint16> values;
    if (!ReadRegisters(regStage(channel), 1, values))
        return -1;
    return static_cast<int>(values.value(0, 0));
}

int CModbusClient::ReadResult(int channel, AirtightResult &result)
{
    QVector<quint16> values;

    //结果 105/106
    if (!ReadRegisters(regResult(channel), 1, values))
        return -1;
    result.res = static_cast<int>(values.value(0, 0));

    //完成状态 165/166
    if (!ReadRegisters(regFinishState(channel), 1, values))
        return -1;
    result.state = static_cast<int>(values.value(0, 0));

    //阶段 101/102
    if (!ReadRegisters(regStage(channel), 1, values))
        return -1;
    result.stage = static_cast<int>(values.value(0, 0));

    //稳压压力 109/111(32位浮点占2寄存器)
    if (!ReadRegisters(regPress(channel), 2, values))
        return -1;
    result.pressValue = TwoRegToFloat(values.value(0, 0), values.value(1, 0));

    //泄漏值 117/119(32位浮点占2寄存器)
    if (!ReadRegisters(regLeak(channel), 2, values))
        return -1;
    result.leakageValue = TwoRegToFloat(values.value(0, 0), values.value(1, 0));

    return 0;
}

bool CModbusClient::SelectProgram(int programId)
{
    return WriteRegister(52, static_cast<quint16>(programId));
}

bool CModbusClient::SelectChannelProgram(int channel, int programId)
{
    quint16 addr = (channel == 1) ? 26 : 27;
    return WriteRegister(addr, static_cast<quint16>(programId));
}

//两个16位寄存器按 Float ABCD(大端) 解析为32位浮点
double CModbusClient::TwoRegToFloat(quint16 hi, quint16 lo)
{
    QByteArray ba;
    ba.append(static_cast<char>((hi >> 8) & 0xFF));   //寄存器1高字节
    ba.append(static_cast<char>(hi & 0xFF));          //寄存器1低字节
    ba.append(static_cast<char>((lo >> 8) & 0xFF));   //寄存器2高字节
    ba.append(static_cast<char>(lo & 0xFF));          //寄存器2低字节

    QDataStream stream(ba);
    stream.setByteOrder(QDataStream::BigEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    float f = 0.0f;
    stream >> f;
    return static_cast<double>(f);
}
