#include "DeviceManage.h"
#include "VisAppBus.h"
#include "../../Plugins/PluginParam/ParamManager.h"
#include "VisCameraTool.h"
#include <QCoreApplication>
#include <QTime>


SINGLETON_IMPL(DeviceManage)

DeviceManage::DeviceManage(QObject *parent) : QObject(parent)
{
    QString path = QCoreApplication::applicationDirPath() + "/Config/Camera.xml";
    if (!VisCameraTool::instance()->LoadConfig(path))
        ShowSystemLog(Log_Error, QString::fromLocal8Bit("相机配置初始化失败!"));

    VisAppBus::subscibeEvent(this, "ScanCodeConnect");
    VisAppBus::subscibeEvent(this, "DisScanCodeConnect");

    VisAppBus::subscibeEvent(this, "ConnectAritight");
    VisAppBus::subscibeEvent(this, "DisConnectAritight");
    VisAppBus::subscibeEvent(this, "SelectAirtightTestProgram");
    VisAppBus::subscibeEvent(this, "StartAirtightTest");
    VisAppBus::subscibeEvent(this, "ResetAirtight");
    VisAppBus::subscibeEvent(this, "SelectAirtightParam");
    VisAppBus::subscibeEvent(this, "SetAirtightParam");
    VisAppBus::subscibeEvent(this, "ReadAirtightRealStatus");
    VisAppBus::subscibeEvent(this, "ReadAirtightRealResult");
    VisAppBus::subscibeEvent(this, "ReadAirtightLastResult");

    InitDevice();

    //启动气密结果轮询线程(轮询完成状态 → 读最终结果 → 发 "AirtightResultBack")
    StartAirtightPoll();
}

DeviceManage::~DeviceManage()
{
    StopAirtightPoll();
    InitOtherCam(false);
}

int DeviceManage::Reset()
{
    return 0;
}

int DeviceManage::InitDevice()
{
    int nRet = 0;

    m_produceScan.SetName(ModuleScanCode);
    m_trayScan.SetName(TrayScanCode);

//    // 工业相机初始化
//    nRet = InitOtherCam();

//    if (nRet == 0)
//        ShowSystemLog(Log_Info, QString::fromLocal8Bit("工业相机初始化成功"));
//    else
//        ShowSystemLog(Log_Error, QString::fromLocal8Bit("工业相机初始化失败"));
    //扫码枪初始化
    event_ScanCodeConnect(ModuleScanCode);

    //料盘扫码枪初始化
    event_ScanCodeConnect(TrayScanCode);

    //气密检测设备初始化
    if (InitAritight())
        ShowSystemLog(Log_Info, QString::fromLocal8Bit("气密检测设备初始化成功"));
    else
        ShowSystemLog(Log_Error, QString::fromLocal8Bit("气密检测设备初始化失败"));

    return 0;
}

int DeviceManage::InitOtherCam(bool open)
{
    bool initFlag = true;
    int nRes = 0;

    // 初始化工业相机
    QStringList listCam;
    QVector<int> listExpo;
    listCam << Cam ;
    listExpo << 1 ;
    for (int i = 0; i < listCam.size(); i++)
    {
        VisCameraTool::instance()->StopStream(0, listCam.at(i));
        VisCameraTool::instance()->CloseCamera(0, listCam.at(i));
        if (!open) continue;
        nRes = VisCameraTool::instance()->OpenCamera(0, listCam.at(i));
        if (nRes != 0) {
            ShowSystemLog(Log_Error, listCam.at(i) + QString(u8"打开失败"));
            initFlag = false;
            continue;
        }
        nRes = VisCameraTool::instance()->StartStream(0, listCam.at(i), false);
        if (nRes != 0) {
            ShowSystemLog(Log_Error, listCam.at(i) + QString(u8"开始出流失败"));
            initFlag = false;
            continue;
        }
        //设置相机曝光
        SetOtherCamEx(listCam.at(i), listExpo[i]);
        QCoreApplication::processEvents();
    }

    return initFlag ? 0 : -1;
}

int DeviceManage::SetOtherCamEx(QString camType, int ex)
{
    VisCameraTool::instance()->SetCamParam(0, camType, VisCameraTool::ShutterTime, ex);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}

bool DeviceManage::InitAritight()
{
    //2台双通道气密仪,各连接一次
    for (int i = 0; i < AirtightDeviceCount; ++i)
    {
        if (0 != event_ConnectAritight(i))
            return false;
    }
    return true;
}

int DeviceManage::event_ScanCodeConnect(QString name)
{
    QString ip = "";
    QString port = "";
    if(name == ModuleScanCode)
    {
        ip = GlobalParam->systemParam.produceCodeParam.sIP;
        m_produceScan.Connect(ip);
    }
    else
    {
        ip = GlobalParam->systemParam.trayCodeParam.sIP;
        m_trayScan.Connect(ip);
    }
    return 0;
}

int DeviceManage::event_DisScanCodeConnect(QString name)
{
    if(name == ModuleScanCode)
    {
        m_produceScan.Disconnected();
    }
    else
    {
        m_trayScan.Disconnected();
    }
    return 0;
}

int DeviceManage::event_ConnectAritight(int device)
{
    CModbusClient::Settings param;
    param.ip = GlobalParam->systemParam.tcpComStruct[device].ip;
    param.port = GlobalParam->systemParam.tcpComStruct[device].port;
    param.serverAddress = GlobalParam->systemParam.tcpComStruct[device].serverAddress;
    param.responseTime = 2000;
    param.numberOfRetries = 3;

    m_cModbusClient[device].SetParams(param);

    if (!m_cModbusClient[device].DeviceConnect())
        return -1;
    return 0;
}

int DeviceManage::event_DisConnectAritight(int device)
{
    m_cModbusClient[device].DeviceDisConnect();
    return 0;
}


// 选择测试程序(按通道独立程序号 寄存器26/27)
int DeviceManage::event_SelectAirtightTestProgram(int station,int programId)
{
    int device = StationToDevice(station);
    int channel = StationToChannel(station);
    return m_cModbusClient[device].SelectChannelProgram(channel, programId) ? 0 : -1;
}
// 启动
int DeviceManage::event_StartAirtightTest(int station)
{
    int device = StationToDevice(station);
    int channel = StationToChannel(station);
    return m_cModbusClient[device].StartTest(channel) ? 0 : -1;
}
// 停止
int DeviceManage::event_ResetAirtight(int station)
{
    int device = StationToDevice(station);
    int channel = StationToChannel(station);
    return m_cModbusClient[device].StopTest(channel) ? 0 : -1;
}
// 选择参数号(四通道协议无独立参数号,参数随程序下发,暂不处理)
int DeviceManage::event_SelectAirtightParam(int station,int paramId)
{
    Q_UNUSED(station);
    Q_UNUSED(paramId);
    return 0;
}
// 下发参数(选择通道后写参数寄存器)
int DeviceManage::event_SetAirtightParam(int station)
{
    int device = StationToDevice(station);
    int channel = StationToChannel(station);
    CModbusClient &client = m_cModbusClient[device];
    AirtightParam &p = GlobalParam->recipeAirtight.airtightParam[station];

    //选择指定通道(寄存器51: 1-4有效,选择后读写参数针对该通道)
    if (!client.WriteRegister(51, channel)) return -1;

    //时间参数(寄存器值=实际值×10)
    if (!client.WriteRegister(5, p.FillTime * 10)) return -2;   //充气时间
    if (!client.WriteRegister(6, p.StabTime * 10)) return -3;   //保压时间
    if (!client.WriteRegister(7, p.TestTime * 10)) return -4;   //检测时间
    if (!client.WriteRegister(9, p.DumpTime * 10)) return -5;   //排气时间

    //单位
    if (!client.WriteRegister(40, p.PressUnit)) return -6;      //压力单位
    if (!client.WriteRegister(41, p.LeakUnit)) return -7;       //泄漏单位

    //浮点参数(32位浮点)
    if (!client.WriteFloatRegisters(10, p.SetFill)) return -8;   //设定压力
    if (!client.WriteFloatRegisters(16, p.TestFail)) return -9;  //泄漏上限
    if (!client.WriteFloatRegisters(18, p.RefFail)) return -10;  //泄漏下限
    if (!client.WriteFloatRegisters(22, p.Volume)) return -11;   //产品容积

    return 0;
}
// 读取仪器测试状态(完成状态寄存器165/166: 0等待/1测试中/2完成/-1失败)
int  DeviceManage::event_ReadAirtightRealStatus(int station)
{
    int device = StationToDevice(station);
    int channel = StationToChannel(station);
    return m_cModbusClient[device].ReadFinishState(channel);
}
// 读取实时测试结果(返回值映射到流程期望: 0结束/1测试中/2终止/-1失败)
int  DeviceManage::event_ReadAirtightRealResult(int station,AritightTask &data)
{
    int device = StationToDevice(station);
    int channel = StationToChannel(station);
    CModbusClient::AirtightResult result;
    int res = m_cModbusClient[device].ReadResult(channel, result);
    if (res == -1) return -1;

    data.TestStage = result.stage;
    data.pressValue = result.pressValue;
    data.leakageValue = result.leakageValue;

    //完成状态映射: 2完成->0结束, 1测试中->1继续, 0等待/异常->2终止
    if (result.state == 2) return 0;
    if (result.state == 1) return 1;
    return 2;
}
// 读取最终测试结果
int  DeviceManage::event_ReadAirtightLastResult(int station,AritightTask &data)
{
    int device = StationToDevice(station);
    int channel = StationToChannel(station);
    CModbusClient::AirtightResult result;
    int res = m_cModbusClient[device].ReadResult(channel, result);
    if (res != 0) return -1;

    data.result = (result.res == 1);   //1=Pass, 2=Fail, 0=未出
    data.pressValue = result.pressValue;
    data.leakageValue = result.leakageValue;
    data.TestStage = result.stage;
    data.errorMsg = (result.res == 1) ? QString() : QStringLiteral("气密测试NG");
    data.errorCode = result.res;
    return 0;
}


// ============================================================
//  气密结果轮询(独立线程)
//  轮询每个工位的"完成状态"寄存器,检测到 测试中→完成 的跳变后,
//  读最终结果(Pass/Fail),再回调 "AirtightResultBack" 给流程层
//  (PluginMeasureProcess 主类/抓取龙门/测试工位 各自订阅)。
//  说明:这是"主动推结果"的骨架,后续可扩展——
//    * 结果合并扫码条码 / MES 编号
//    * NG 原因按泄漏值细分
//    * 与 MES 编号校验联动
// ============================================================
void DeviceManage::StartAirtightPoll()
{
    if (m_pollRun.load()) return;
    m_pollRun = true;
    m_pollThread = std::thread([this] { AirtightPollLoop(); });
}

void DeviceManage::StopAirtightPoll()
{
    m_pollRun = false;
    if (m_pollThread.joinable())
        m_pollThread.join();
}

void DeviceManage::AirtightPollLoop()
{
    //记录每个工位上一轮的完成状态,用于检测"测试中→完成"跳变
    int lastState[AirtightStationCount];
    for (int i = 0; i < AirtightStationCount; ++i) lastState[i] = -1;

    while (m_pollRun.load()) {
        for (int station = 0; station < AirtightStationCount; ++station) {
            int device = StationToDevice(station);
            if (!m_cModbusClient[device].GetConnectState())
                continue;   //设备未连接,本轮跳过

            int channel = StationToChannel(station);
            int state = m_cModbusClient[device].ReadFinishState(channel);   //0等待/1测试中/2完成/-1失败

            //刚完成(上一轮不是完成态,这一轮是完成态)才读结果并回调,避免重复发
            if (state == 2 && lastState[station] != 2) {
                CModbusClient::AirtightResult result;
                if (m_cModbusClient[device].ReadResult(channel, result) == 0) {
                    bool ok = (result.res == 1);   //1=Pass, 2=Fail
                    QString ngReason = ok ? QString() : QStringLiteral("气密测试NG");
                    VisAppBus::sendEvent("AirtightResultBack", station, ok, ngReason);
                }
            }
            lastState[station] = state;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
