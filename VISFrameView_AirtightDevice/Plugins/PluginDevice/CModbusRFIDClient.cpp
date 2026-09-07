#include "CModbusRFIDClient.h"
#include <QModbusRtuSerialMaster>
#include <QTime>
#include <QApplication>
#include <QModbusPdu>
#include <QSerialPortInfo>
#include <QTimer>
#include "VisAppBus.h"
#include "VisMotorManager.h"
#include "ParamManager.h"



std::mutex CModbusRFIDClient::m_mutexSendData; // 静态互斥锁的定义
CModbusRFIDClient::CModbusRFIDClient(int serverAddress, QModbusClient *device,QObject *parent) : QObject(parent)
{
    m_initThreadID=std::this_thread::get_id();
    m_serverAddress = 2;          // 固定从站地址2，多工位共用同一设备
    m_device = device;
}

CModbusRFIDClient::~CModbusRFIDClient()
{

}

bool CModbusRFIDClient::SetModbusModel()
{
    QByteArray data;
    data.append(static_cast<uint8_t>(0x06));      // 功能码 0x06 = Write Single Register
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x03));       // 寄存器 0x0003 = 工作模式
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x05));       // 值 0x0005 = ModbusRtu II 模式
    QByteArray recvData;
    bool replyFlag = false;
    if(!SendData(data,recvData,replyFlag))return false;
    if (recvData.size() < 6 ||
        static_cast<quint8>(recvData.at(1)) != 0x06)
    {
        m_errInfo = QStringLiteral("设置Modbus模式失败");
        return false;
    }
    return true;
}

bool CModbusRFIDClient::GetModbusRFIDExist(bool &exist)
{
    QByteArray data;
    data.append(static_cast<uint8_t>(0x03));      // 功能码0x03
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x04));      // 寄存器 0x0004 = 标签存在状态
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x01));
    QByteArray recvData;
    bool replyFlag = false;
    if(!SendData(data,recvData,replyFlag)){
        ShowSystemLog(Log_Debug, QStringLiteral("RFID读标签状态通信失败:addr=0x%1 err=%2")
            .arg(m_serverAddress, 2, 16, QChar('0')).arg(m_errInfo));
        return false;
    }
    if (recvData.size() < 5) {                    // PDU无CRC，5字节: [Slave][FC][ByteCnt][Hi][Lo]
        m_errInfo = QStringLiteral("RFID标签状态响应长度不足:%1<5").arg(recvData.size());
        ShowSystemLog(Log_Debug, QStringLiteral("RFID标签状态响应异常:实际长度=%1,期望>=5").arg(recvData.size()));
        return false;
    }
    if (static_cast<quint8>(recvData.at(1)) != 0x03) {
        m_errInfo = QStringLiteral("RFID标签状态功能码错误:0x%1").arg(static_cast<quint8>(recvData.at(1)), 2, 16, QChar('0'));
        ShowSystemLog(Log_Debug, m_errInfo);
        return false;
    }
    quint8 highByte = static_cast<quint8>(recvData.at(3));
    quint8 lowByte  = static_cast<quint8>(recvData.at(4));
    quint16 statusValue = (highByte << 8) | lowByte;
    // 0x0004: 0x0001=有标签, 0x0000=无标签
    if (statusValue == 0x0001) {
        exist = true;
    } else if (statusValue == 0x0000) {
        exist = false;
    } else {
        m_errInfo = QStringLiteral("RFID标签状态值异常:0x%1").arg(statusValue, 4, 16, QChar('0'));
        ShowSystemLog(Log_Debug, m_errInfo);
        return false;
    }
    return true;
}

bool CModbusRFIDClient::GetModbusTrayCode(QString &barCode,int length)
{
    if (length <= 0) length = 100;                         // 防止 length=0 导致空请求
    quint16 regCount = (length % 2 == 0) ? (length / 2) : (length / 2 + 1);
    QByteArray data;
    data.append(static_cast<char>(0x03));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x0A));
    data.append(static_cast<char>(regCount >> 8));
    data.append(static_cast<char>(regCount & 0xFF));

    QByteArray recvData;
    bool replyFlag = false;
    if(!SendData(data,recvData,replyFlag))
    {
        ShowSystemLog(Log_Debug, QStringLiteral("RFID读条码通信失败:addr=0x%1 err=%2")
            .arg(m_serverAddress, 2, 16, QChar('0')).arg(m_errInfo));
        return false;
    }
    //解析数据
    if (recvData.size() < 3 + 2 * regCount) {     // PDU无CRC: [Slave][FC][ByteCnt] + 数据
        m_errInfo = QStringLiteral("条码数据响应长度不足:%1<期望:%2").arg(recvData.size()).arg(3 + 2 * regCount);
        ShowSystemLog(Log_Debug, m_errInfo);
        return false;
    }
    int dataStartIndex = 3;
    int dataByteCount = static_cast<quint8>(recvData.at(2)); // 强转避免>127变负数
    QByteArray rawBytes = recvData.mid(dataStartIndex, dataByteCount);
    barCode = QString::fromLatin1(rawBytes.data(), dataByteCount);
    //  清理末尾填充的空字符或无效字符
    barCode.remove(QChar('\0'));

    // 查 0x0005 确认读操作成功
    QByteArray ckData;
    ckData.append(static_cast<char>(0x03));
    ckData.append(static_cast<char>(0x00));
    ckData.append(static_cast<char>(0x05));
    ckData.append(static_cast<char>(0x00));
    ckData.append(static_cast<char>(0x01));
    bool ckFlag = false;
    if (!SendData(ckData, recvData, ckFlag)) {
        //读操作状态查询通信失败:读操作结果未知,不能当成功
        m_errInfo = QStringLiteral("读操作状态查询通信失败:%1").arg(readCurError());
        ShowSystemLog(Log_Debug, m_errInfo);
        return false;
    }
    if (recvData.size() >= 5 && static_cast<quint8>(recvData.at(1)) == 0x03)
    {
        quint16 opStatus = (static_cast<quint8>(recvData.at(3)) << 8) | static_cast<quint8>(recvData.at(4));
        if (opStatus != 0x0000) {
            m_errInfo = QStringLiteral("读操作失败,0x0005=0x%1").arg(opStatus, 4, 16, QChar('0'));
            ShowSystemLog(Log_Debug, QStringLiteral("RFID读条码0x0005状态非0:0x%1").arg(opStatus, 4, 16, QChar('0')));
            return false;
        }
    }
    else {
        m_errInfo = QStringLiteral("读操作状态确认报文异常");
        ShowSystemLog(Log_Debug, m_errInfo);
        return false;
    }
    return true;
}

bool CModbusRFIDClient::SetModbusTrayCode(QString &barCode)
{
    QByteArray writeBytes = barCode.toLatin1();
    // 奇数长度自动补0对齐寄存器，不再直接返回false
    if (writeBytes.size() % 2 != 0)
    {
        writeBytes.append('\0');
    }
    int length = writeBytes.size();
    quint16 regCount = length / 2;
    quint8  byteCount = static_cast<quint8>(length);

    QByteArray data;
    data.append(static_cast<char>(0x10));       // 功能码 0x10 = Write Multiple Registers
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x0A));        // 起始地址 0x000A
    data.append(static_cast<char>(regCount >> 8));
    data.append(static_cast<char>(regCount & 0xFF)); // 寄存器数量
    data.append(static_cast<char>(byteCount));        // 字节数

    // 按 Modbus 大端字节序逐字节写入
    for(int i=0; i<(length/2); i++) {
        data.append(static_cast<char>(writeBytes.at(i*2)));      // 高字节
        data.append(static_cast<char>(writeBytes.at(i*2 + 1)));  // 低字节
    }
    QByteArray recvData;
    bool replyFlag = false;
    if(!SendData(data,recvData,replyFlag))
    {

        return false;
    }
    // 写多个寄存器应答: [SlaveID][0x10][StartHi][StartLo][RegCntHi][RegCntLo]
    if (recvData.size() >= 6 && static_cast<quint8>(recvData.at(1)) == 0x10)
    {
        // 查 0x0005 确认写操作成功
        QByteArray ckData;
        ckData.append(static_cast<char>(0x03));
        ckData.append(static_cast<char>(0x00));
        ckData.append(static_cast<char>(0x05));
        ckData.append(static_cast<char>(0x00));
        ckData.append(static_cast<char>(0x01));
        bool ckFlag = false;
        if (SendData(ckData, recvData, ckFlag) &&
            recvData.size() >= 5 &&
            static_cast<quint8>(recvData.at(1)) == 0x03)
        {
            quint16 opStatus = (static_cast<quint8>(recvData.at(3)) << 8) | static_cast<quint8>(recvData.at(4));
            if (opStatus != 0x0000)
            {
                m_errInfo = QStringLiteral("写操作失败,0x0005=0x%1").arg(opStatus, 4, 16, QChar('0'));
                //ShowSystemLog(Log_Debug, QString(u8"RFID写入条码:%1").arg(m_errInfo));
                return false;
            }
        }
        else
        {
            //ShowSystemLog(Log_Debug, QString(u8"RFID写入条码：0x0005状态查询失败"));
        }
        //ShowSystemLog(Log_Debug, QString(u8"RFID写入条码成功:%1").arg(barCode));
        return true;
    }
    m_errInfo = QStringLiteral("写多寄存器应答异常");
    //ShowSystemLog(Log_Debug, m_errInfo);
    return false;
}


bool CModbusRFIDClient::TriggerReadCard()
{
    QByteArray data;
    data.append(static_cast<uint8_t>(0x06));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x05)); // 寄存器0x0005
    data.append(static_cast<char>(0x00));
    data.append(static_cast<uint8_t>(0x01));// 写入1触发射频读卡
    QByteArray recvData;
    bool replyFlag = false;

    if (!SendData(data, recvData, replyFlag))
    {
        ShowSystemLog(Log_Debug, QStringLiteral("RFID触发读卡通信失败:addr=0x%1 err=%2")
            .arg(m_serverAddress, 2, 16, QChar('0')).arg(m_errInfo));
        return false;
    }
    if (recvData.size() < 6 || static_cast<quint8>(recvData.at(1)) != 0x06)
    {
        m_errInfo = QStringLiteral("触发读卡应答报文异常:长度=%1,功能码=0x%2")
            .arg(recvData.size()).arg(recvData.size() > 1 ? QString::number(static_cast<quint8>(recvData.at(1)), 16) : QStringLiteral("无"));
        ShowSystemLog(Log_Debug, m_errInfo);
        return false;
    }

    return true;
}
QString CModbusRFIDClient::readCurError()
{
    return m_errInfo;
}

bool CModbusRFIDClient::slotSendData(QByteArray& sendData, QByteArray& recvData, bool& replyFlag)
{
    m_errInfo.clear();
    if (sendData.isEmpty()) return false;
    const quint8 fc = static_cast<quint8>(sendData[0]);
    const quint8 address = m_serverAddress;
    QModbusReply *reply = nullptr;

    // 标准功能码走标准 API，避免 rawResult().isValid() 为 false
    if (fc == 0x03 || fc == 0x04)  // 读寄存器
    {
        quint16 startAddr = (static_cast<quint8>(sendData[1]) << 8) | static_cast<quint8>(sendData[2]);
        quint16 regCount  = (static_cast<quint8>(sendData[3]) << 8) | static_cast<quint8>(sendData[4]);
        auto regType = (fc == 0x03) ? QModbusDataUnit::HoldingRegisters : QModbusDataUnit::InputRegisters;
        reply = m_device->sendReadRequest(QModbusDataUnit(regType, startAddr, regCount), address);
    }
    else if (fc == 0x06)  // 写单寄存器
    {
        quint16 regAddr = (static_cast<quint8>(sendData[1]) << 8) | static_cast<quint8>(sendData[2]);
        quint16 value   = (static_cast<quint8>(sendData[3]) << 8) | static_cast<quint8>(sendData[4]);
        QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, regAddr, 1);
        unit.setValue(0, value);
        reply = m_device->sendWriteRequest(unit, address);
    }
    else if (fc == 0x10)  // 写多寄存器
    {
        quint16 startAddr = (static_cast<quint8>(sendData[1]) << 8) | static_cast<quint8>(sendData[2]);
        quint16 regCount  = (static_cast<quint8>(sendData[3]) << 8) | static_cast<quint8>(sendData[4]);
        int byteCount = static_cast<quint8>(sendData[5]);
        QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, startAddr, regCount);
        for (int i = 0; i < byteCount / 2; i++)
            unit.setValue(i, (static_cast<quint8>(sendData[6 + i*2]) << 8) | static_cast<quint8>(sendData[6 + i*2 + 1]));
        reply = m_device->sendWriteRequest(unit, address);
    }
    else
    {
        reply = m_device->sendRawRequest(QModbusRequest(QModbusRequest::FunctionCode(fc), sendData.mid(1)), address);
    }

    if (!reply) {
        m_errInfo = QStringLiteral("sendRequest返回null:%1").arg(m_device->errorString());
        ShowSystemLog(Log_Debug, QStringLiteral("RFID sendRequest返回null:fc=0x%1,%2")
            .arg(fc, 2, 16, QChar('0')).arg(m_device->errorString()));
        return false;
    }

    if (!reply->isFinished()) {
        // 完成标志用堆上 shared_ptr,避免 lambda 捕获栈变量引用(超时后 finished 延迟触发会悬空)
        auto finishedFlag = std::make_shared<bool>(false);
        auto validFlag = std::make_shared<bool>(false);

        connect(reply, &QModbusReply::finished, this, [this, reply, finishedFlag, validFlag]() {
            m_mtxReplyMap.lock();
            bool isTimeout = m_replyTimeout.value(reply, false);
            m_replyTimeout.remove(reply);
            m_mtxReplyMap.unlock();

            // 超时遗留的 reply:slotSendData 已返回,这里只负责清理
            if (isTimeout) {
                reply->deleteLater();
                return;
            }

            if (reply->error() == QModbusDevice::NoError)
                *validFlag = true;
            else
                m_errInfo = QStringLiteral("Modbus响应错误:%1").arg(reply->errorString());
            *finishedFlag = true;
        });

        if (!WaitTime(finishedFlag)) {
            m_mtxReplyMap.lock();
            m_replyTimeout[reply] = true;
            m_mtxReplyMap.unlock();
            return false;
        }

        if (!*validFlag) {
            reply->deleteLater();
            return false;
        }
        recvData.clear();
        recvData.append(static_cast<char>(address));
        if (fc == 0x03 || fc == 0x04) {
            recvData.append(static_cast<char>(fc));
            const auto& values = reply->result().values();
            int byteCnt = values.size() * 2;
            recvData.append(static_cast<char>(byteCnt));
            for (auto v : values) {
                recvData.append(static_cast<char>(v >> 8));
                recvData.append(static_cast<char>(v & 0xFF));
            }
        } else if (fc == 0x06) {
            recvData.append(static_cast<char>(fc));
            recvData.append(sendData.mid(1, 4));
        } else if (fc == 0x10) {
            recvData.append(static_cast<char>(fc));
            recvData.append(sendData.mid(1, 4));  // startAddr + regCount
        }
        reply->deleteLater();
        return true;
    }
    else {
        if (reply->error() == QModbusDevice::NoError)
        {
            // 同步完成，构建 recvData
            recvData.clear();
            recvData.append(static_cast<char>(address));
            if (fc == 0x03 || fc == 0x04) {
                recvData.append(static_cast<char>(fc));
                const auto& values = reply->result().values();
                int byteCnt = values.size() * 2;
                recvData.append(static_cast<char>(byteCnt));
                for (auto v : values) {
                    recvData.append(static_cast<char>(v >> 8));
                    recvData.append(static_cast<char>(v & 0xFF));
                }
            } else if (fc == 0x06) {
                recvData.append(static_cast<char>(fc));
                recvData.append(sendData.mid(1, 4));
            } else if (fc == 0x10) {
                recvData.append(static_cast<char>(fc));
                recvData.append(sendData.mid(1, 4));
            }
            reply->deleteLater();
            return true;
        }
        m_errInfo = QStringLiteral("同步完成但失败,err:%1").arg(reply->error());
        reply->deleteLater();
        return false;
    }
}
bool CModbusRFIDClient::WaitTime(std::shared_ptr<bool> finishedFlag)
{
    QTime timewait;
    timewait.start();
    while(timewait.elapsed()<2500){
        if(VisMotorToolSpace::VisMotorInstance->IsEmgStop()){
            m_errInfo=QStringLiteral("急停中,RFID读取被终止");
            ShowSystemLog(Log_Error, m_errInfo);
            return false;
        }
        QApplication::processEvents();
        if (finishedFlag && *finishedFlag) return true;
    }
    m_errInfo=QStringLiteral("等待接收数据超时(2500ms)");
    ShowSystemLog(Log_Debug, m_errInfo);
    return false;
}

bool CModbusRFIDClient::SendData(QByteArray& sendData, QByteArray& recvData,bool& replyFlag)
{
    for(int i=0;i<5;i++)
    {
        replyFlag = false;
        bool bRet = false;
        if (std::this_thread::get_id() == m_initThreadID){
            bRet=slotSendData(sendData, recvData, replyFlag);
        }
        else {
            m_mutexSendData.lock();
            QMetaObject::invokeMethod(this, [&](){
                bRet = slotSendData(sendData, recvData, replyFlag);
            }, Qt::BlockingQueuedConnection);
            m_mutexSendData.unlock();
        }
        if(bRet)return true;
        if(VisMotorToolSpace::VisMotorInstance->IsEmgStop())
        {
            return false;
        }
        // Modbus异常响应(ProtocolError)是从站明确拒绝请求,重试同样的请求必然得到同样异常,直接返回不再重试
        if (m_errInfo.contains("Modbus Exception Response"))
        {
            ShowSystemLog(Log_Debug, QStringLiteral("RFID Modbus异常响应,不重试,err=%1").arg(m_errInfo));
            return false;
        }
        ShowSystemLog(Log_Debug, QStringLiteral("RFID Modbus发送第%1次失败,重试,err=%2").arg(i + 1).arg(m_errInfo));
        if (std::this_thread::get_id() == m_initThreadID)
        {
            QTime time;
            time.start();
            while(time.elapsed() < 600)
                QApplication::processEvents();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(600));   //600
        }
    }
    return false;
}


QByteArray CModbusRFIDClient::SwapOrder(QByteArray in)
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
void CModbusRFIDClient::appendCRC(QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (int i = 0; i < data.size(); ++i) {
        crc ^= (quint8)data[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    data.append((char)(crc & 0xFF));       // CRC Low
    data.append((char)((crc >> 8) & 0xFF)); // CRC High
}
// 工具函数：字节数组转 16 位无符号整数（大端/小端可选）
quint16 CModbusRFIDClient:: ByteArrayToUInt16(const QByteArray &ba, bool littleEndian)
{
    if (ba.size() < 2) return 0;
    quint16 value;
    QDataStream stream(ba);
    stream.setByteOrder(littleEndian ? QDataStream::LittleEndian : QDataStream::BigEndian);
    stream >> value;
    return value;
}
