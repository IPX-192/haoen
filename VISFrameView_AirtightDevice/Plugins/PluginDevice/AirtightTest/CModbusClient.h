#ifndef CMODBUSCLIENT_H
#define CMODBUSCLIENT_H

#include <QObject>
#include <QModbusDataUnit>
#include <QSerialPort>

class QModbusClient;
class QModbusReply;

//测试灯控制类
class CModbusClient : public QObject
{
    Q_OBJECT
public:
    struct Settings
    {
        QString port = "";
        int parity = QSerialPort::NoParity;
        int baud = QSerialPort::Baud115200;
        int dataBits = QSerialPort::Data8;
        int stopBits = QSerialPort::OneStop;
        int responseTime = 10000;
        int numberOfRetries = 3;
    };

    enum ParamMode {
        MODE_CouplADelayTime = 0,    // 0-650
        MODE_FillTime ,       // 充气时间 0~650s
        MODE_StabTime ,       // 稳定时间 0~650s
        MODE_TestTime ,       // 测试时间 0~650s
        MODE_DumpTime ,       // 排气时间 0~650s
        MODE_PressUnit,       // 压力单位 见“单位列表”
        MODE_MaxFill  ,       // 最大充气压力 -9999~9999
        MODE_MinFill  ,       // 最小充气压力 -9999~9999
        MODE_SetFill  ,       // 设定的充气压力 -9999~9999
        MODE_LeakUnit ,       // 泄漏单位 见“单位列表”
        MODE_VolumeUnit ,     // 容积单位 见“单位列表”
        MODE_Volume ,         // 容积大小 0~9999
        MODE_TestFail ,       // 泄漏上限 0~9999
        MODE_RefFail ,        // 泄漏下限 0~9999
        MODE_UNKOWN
     };

    struct AritghtResult{
        int res = 0;//0:OK 1:NG 2:AL
        QString errorMsg = "";
        int alarmCode = 0;
        int TestStage = 0;
        int pressValue = 0;
        int leakageValue = 0;
    };
public:
    explicit CModbusClient(QObject *parent = nullptr);
    ~CModbusClient();
public:
    bool GetConnectState();
    /**
      *@brief 连接
      *@return false 连接失败
      */
    bool DeviceConnect();

    /**
      *@brief 断开
      *@return false 断开连接失败
      */
    bool DeviceDisConnect();

    /**
      *@brief 发送数据
      *@param sendData 发送的16进制PDU（功能码加数据域）
      */
    void SendData(QByteArray sendData);

    /**
      *@brief 设置设备串口参数
      *@param serial 参数结构体
      */
    void SetParams(Settings serial);            //设置modbus参数

    /**
      *@brief 返回设备串口参数
      *@return 参数结构体
      */
    Settings GetPatams()                        //返回串口参数
    {
        return m_modBusParams;
    }
    /**
       *@brief 睡眠执行其它响应
       *@param msec 毫秒
       */
    void SleepEvents(int msec);

    // 选择测试程序（1=IP67，2=IP68）
    bool SelectTestProgram(int programId);
    // 启动
    bool StartAirtightTest();
    // 复位
    bool ResetAirtight();
    // 设置参数
    bool SelectParam(int paramId);
    // 设置参数
    bool SetParam(ParamMode mode,int param);
    // 读取仪器实时测试状态，判断仪器是否测试结束
    int  ReadRealStatus();
    // 读取实时测试结果
    int  ReadRealResult(AritghtResult &result);
    // Last results最终的测试结果
    int  ReadLastResult(AritghtResult &result);
private:
    //内部辅助函数
    void ResetReplyCache();                  // 重置响应缓存
    void  GetPressUnit(int param,uint8_t &byte1,uint8_t &byte2,uint8_t &byte3,uint8_t &byte4);
    void  GetLeakUnit(int param,uint8_t &byte1,uint8_t &byte2,uint8_t &byte3,uint8_t &byte4);
    void  GetVolumeUnit(int param,uint8_t &byte1,uint8_t &byte2,uint8_t &byte3,uint8_t &byte4);
    bool  WaitResult(QByteArray sendData);
    QByteArray SwapOrder(QByteArray in);
    QString GetResultMsg(QByteArray in);
    quint16 ByteArrayToUInt16(const QByteArray &ba, bool littleEndian = false);
private:
    QModbusClient *m_device = nullptr;
    quint8 m_serverAddress = 0x01;                 //从机的地址
    Settings m_modBusParams;                    //modbus参数
    bool m_connectFlag = false;
    bool m_replyFlag = false;                   //是否接收到回复信息
    bool m_sendFlag = false;
    QString m_localAddress;
    QString m_functionCode;
    QString m_replyData;
signals:
    void ReplyMessage(bool sendFlag, QString localAddress, QString functionCode, QString data);
};

#endif
