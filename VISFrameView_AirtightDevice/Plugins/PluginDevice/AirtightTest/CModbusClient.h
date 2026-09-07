#ifndef CMODBUSCLIENT_H
#define CMODBUSCLIENT_H

#include <QObject>
#include <QVector>

class QModbusClient;
class QModbusReply;

//双通道气密仪 Modbus TCP 客户端
//希立仪器 SEQSRS-M105002R-2FD,基于四通道标准协议(只用通道1/2)
//线圈: 1统一启动 2统一停止 3/4通道1启停 5/6通道2启停 11心跳
//寄存器(通道1/通道2): 完成状态165/166 结果105/106 阶段101/102
//                     稳压压力109/111 泄漏值117/119(均为32位浮点占2寄存器)
class CModbusClient : public QObject
{
    Q_OBJECT
public:
    struct Settings
    {
        QString ip = "192.168.88.88";   //TCP IP
        int port = 9998;                //TCP 端口
        int serverAddress = 1;          //Modbus 从站地址(Unit ID)
        int responseTime = 2000;        //响应超时 ms
        int numberOfRetries = 3;        //重试次数
    };

    //通道编号(仪器侧)
    enum Channel
    {
        Ch1 = 1,
        Ch2 = 2
    };

    //测试结果
    struct AirtightResult
    {
        int res = 0;              //测试结果: 0未出 1Pass 2Fail
        int state = 0;            //完成状态: 0等待 1测试中 2完成
        int stage = 0;            //进度条阶段: 0未测 1检验 2充气 3保压 4检测 5排气 6完成
        double pressValue = 0.0;  //稳压压力(kPa)
        double leakageValue = 0.0;//泄漏值
    };

public:
    explicit CModbusClient(QObject *parent = nullptr);
    ~CModbusClient();

    bool GetConnectState();
    bool DeviceConnect();          //建立 TCP 连接
    bool DeviceDisConnect();       //断开连接
    void SetParams(Settings cfg);
    Settings GetParams() { return m_modBusParams; }

    //心跳(TCP 连接校验,线圈11)
    bool Heartbeat();

    //启动/停止(通道独立)
    bool StartTest(int channel);   //写线圈3/5
    bool StopTest(int channel);    //写线圈4/6
    bool StartAll();               //写线圈1
    bool StopAll();                //写线圈2

    //读状态
    int  ReadFinishState(int channel);   //完成状态: 0等待/1测试中/2完成,-1通信失败
    int  ReadStage(int channel);         //进度条阶段,-1通信失败

    //读结果(结果+完成状态+阶段+稳压压力+泄漏值)
    int  ReadResult(int channel, AirtightResult &result);

    //切换程序
    bool SelectProgram(int programId);                     //寄存器52
    bool SelectChannelProgram(int channel, int programId); //寄存器26/27

    //基础读写原语(供参数下发等扩展使用)
    bool ReadRegisters(quint16 addr, quint16 count, QVector<quint16> &values);
    bool WriteRegister(quint16 addr, quint16 value);        //功能码0x06
    bool WriteFloatRegisters(quint16 addr, double value);   //写32位浮点到2个连续寄存器(功能码0x10)
    bool WriteCoil(quint16 addr, bool on);                  //功能码0x05

private:
    bool WaitReply(QModbusReply *reply);                    //同步等待回复
    double TwoRegToFloat(quint16 hi, quint16 lo);           //两个16位寄存器按Float ABCD转32位浮点

    //双通道协议地址映射
    quint16 regFinishState(int channel) const { return channel == 1 ? 165 : 166; }
    quint16 regResult(int channel) const { return channel == 1 ? 105 : 106; }
    quint16 regStage(int channel) const { return channel == 1 ? 101 : 102; }
    quint16 regPress(int channel) const { return channel == 1 ? 109 : 111; }
    quint16 regLeak(int channel) const { return channel == 1 ? 117 : 119; }
    quint16 coilStart(int channel) const { return channel == 1 ? 3 : 5; }
    quint16 coilStop(int channel) const { return channel == 1 ? 4 : 6; }

private:
    QModbusClient *m_device = nullptr;
    Settings m_modBusParams;
    bool m_connectFlag = false;
};

#endif
