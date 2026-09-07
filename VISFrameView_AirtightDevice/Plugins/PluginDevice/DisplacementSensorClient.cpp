#include "DisplacementSensorClient.h"
#include <QTime>
#include <QApplication>
#include <QSerialPortInfo>
#include "ParamManager.h"
#include <QThread>
#include <thread>

DisplacementSensorClient::DisplacementSensorClient(QObject *parent)
    : QObject(parent)
{
    m_initThreadID = std::this_thread::get_id();
    m_serial = new QSerialPort(this);
    m_serial->disconnect();
    connect(m_serial, &QSerialPort::readyRead, this, &DisplacementSensorClient::slotReadSerialData);
    connect(m_serial, &QSerialPort::errorOccurred, this, &DisplacementSensorClient::slotSerialPortError);
}

DisplacementSensorClient::~DisplacementSensorClient()
{
    if (m_serial->isOpen())
    {
        m_serial->close();
    }
    m_serial->deleteLater();
    m_serial = nullptr;
}

void DisplacementSensorClient::SetName(QString name)
{
    m_name = name;
}

QStringList DisplacementSensorClient::ScanAllSerialPort()
{
    QStringList ret;
    for (const auto& info : QSerialPortInfo::availablePorts())
    {
        ret << info.portName();
    }
    return ret;
}

int DisplacementSensorClient::Connect(const DisplacementPortSettings& settings)
{
    QMutexLocker locker(&m_opMutex);
    if (m_serial->isOpen())
    {
        m_serial->close();
    }
    m_serial->setPortName(settings.PortName);
    m_serial->setBaudRate(settings.BaudRate);
    m_serial->setDataBits(static_cast<QSerialPort::DataBits>(settings.DataBits));
    m_serial->setStopBits(static_cast<QSerialPort::StopBits>(settings.StopBits));
    switch (settings.Parity)
    {
    case 1: m_serial->setParity(QSerialPort::OddParity); break;
    case 2: m_serial->setParity(QSerialPort::EvenParity); break;
    default: m_serial->setParity(QSerialPort::NoParity); break;
    }
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    bool openOk = m_serial->open(QIODevice::ReadWrite);
    ShowSystemLog(openOk ? Log_Info : Log_Error,
        QString(u8"%1 位移传感器串口连接%2！").arg(m_name).arg(openOk ? u8"成功" : u8"失败:" + m_serial->errorString()));
    if (openOk)
        m_recvBuffer.clear();
    return openOk ? 0 : -1;
}

int DisplacementSensorClient::DisconnectPort()
{
    QMutexLocker locker(&m_opMutex);
    if (m_serial->isOpen())
    {
        m_serial->close();
        ShowSystemLog(Log_Info, QString(u8"%1 位移传感器串口已断开").arg(m_name));
    }
    return 0;
}

bool DisplacementSensorClient::IsPortOpened()
{
    QMutexLocker locker(&m_opMutex);
    return m_serial->isOpen();
}

int DisplacementSensorClient::ReadSensorHeight(const QString& sensorId, double& outHeight)
{
    outHeight = 0.0;
    {
        QMutexLocker locker(&m_opMutex);
        if (!m_serial->isOpen())
        {
            m_lastErrMsg = u8"串口未打开";
            return -1;
        }
    }

    // 记录期待应答的传感器放大器ID(如"00"),用于M0响应里定位对应通道的数据
    {
        QMutexLocker locker(&m_opMutex);
        m_expectSensorId = sensorId;
    }

    bool bRet = false;
    QString cmd = "M0";   // DL-RS1A: 读取所有传感器放大器数据
    if (std::this_thread::get_id() == m_initThreadID)
    {
        bRet = slotSendAsciiCmd(cmd);
    }
    else
    {
        QMetaObject::invokeMethod(this, "slotSendAsciiCmd", Qt::BlockingQueuedConnection,
            Q_RETURN_ARG(bool, bRet), Q_ARG(QString, cmd));
    }
    if (!bRet || !WaitReply(3000))
    {
        return -1;
    }

    QMutexLocker locker(&m_opMutex);
    outHeight = m_currentHeight;
    return 0;
}

int DisplacementSensorClient::ResetSensorZero()
{
    {
        QMutexLocker locker(&m_opMutex);
        if (!m_serial->isOpen())
            return -1;
    }
    bool bRet;
    QString cmd = "AW,050,1";
    if (std::this_thread::get_id() == m_initThreadID)
    {
        bRet = slotSendAsciiCmd(cmd);
    }
    else
    {
        QMetaObject::invokeMethod(this, "slotSendAsciiCmd", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, bRet), Q_ARG(QString, cmd));
    }
    if (!bRet || !WaitReply(3000))
    {
        return -1;
    }
    return 0;
}
int DisplacementSensorClient::GetLatestRawHeight(double& outVal)
{
    QMutexLocker locker(&m_opMutex);
    outVal = m_currentHeight;
    return m_serial->isOpen() ? 0 : -1;
}

bool DisplacementSensorClient::slotSendAsciiCmd(const QString& cmd)
{
    QMutexLocker locker(&m_opMutex);
    m_replyFlag = false;
    if (!m_serial->isOpen())
    {
        m_lastErrMsg = QString(u8"串口未打开，发送指令失败");
        return false;
    }
    QByteArray sendBuf = (cmd + "\r\n").toLocal8Bit();
    qint64 writeLen = m_serial->write(sendBuf);
    m_serial->flush();
    return writeLen == sendBuf.size();
}

void DisplacementSensorClient::slotReadSerialData()
{
    QByteArray buf = m_serial->readAll();
    QMutexLocker locker(&m_opMutex);
    m_recvBuffer.append(buf);

    while (m_recvBuffer.contains("\r\n"))
    {
        int splitPos = m_recvBuffer.indexOf("\r\n");
        QByteArray frameRaw = m_recvBuffer.left(splitPos);
        m_recvBuffer = m_recvBuffer.mid(splitPos + 2);
        QString frameStr = QString::fromLocal8Bit(frameRaw).trimmed();

        if (!frameStr.isEmpty())
        {
            QStringList parts = frameStr.split(",");

            // 错误帧: ER,<指令>,<错误号>
            if (!parts.isEmpty() && parts[0] == "ER")
            {
                m_lastErrMsg = QString(u8"传感器返回错误帧：%1").arg(frameStr);
                m_replyFlag = true;
                continue;
            }

            // M0 响应: M0,<ID00数据>,<ID01数据>,...,<最后ID数据>, (末尾带逗号,拆出空串)
            if (parts.size() >= 2 && parts[0] == "M0")
            {
                int idIndex = m_expectSensorId.toInt();  // "00"->0, "01"->1
                QString valStr = parts.value(1 + idIndex);
                bool ok = false;
                double rawVal = valStr.toDouble(&ok);
                if (ok)
                {
                    m_currentHeight = rawVal;
                    m_curveHistory.push_back(rawVal);
                    if (m_curveHistory.size() > MAX_CURVE_POINTS)
                    {
                        m_curveHistory.erase(m_curveHistory.begin());
                    }
                    m_replyFlag = true;
                }
                else
                {
                    m_lastErrMsg = QString(u8"传感器数据解析失败：%1").arg(frameStr);
                    m_replyFlag = true;
                }
            }
        }
    }
}


void DisplacementSensorClient::slotSerialPortError(QSerialPort::SerialPortError err)
{
    if (err == QSerialPort::NoError) return;
    QMutexLocker locker(&m_opMutex);
    m_lastErrMsg = QString(u8"位移传感器串口异常：%1").arg(err);
    ShowSystemLog(Log_Error, QString(u8"%1 %2").arg(m_name).arg(m_lastErrMsg));
}

bool DisplacementSensorClient::WaitReply(int timeoutMs)
{
    QTime timer;
    timer.start();
    while (timer.elapsed() < timeoutMs)
    {
        {
            QMutexLocker locker(&m_opMutex);
            if (m_replyFlag)
            {
                m_replyFlag = false;
                return true;
            }
        }
        // 关键:必须处理事件队列,让 readyRead 信号能派发到 slotReadSerialData
        // 若用 QThread::msleep 会阻塞事件循环,导致收不到串口响应而超时
        QApplication::processEvents();
        QThread::msleep(5);   // 少量休眠,避免空转占满CPU
    }

    QMutexLocker locker(&m_opMutex);
    m_lastErrMsg = u8"位移传感器读取响应超时";
    return false;
}
