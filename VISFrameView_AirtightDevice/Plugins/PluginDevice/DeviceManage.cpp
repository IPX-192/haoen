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
}

DeviceManage::~DeviceManage()
{
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
    for(int i = 0;i<StationCount;++i)
    {
        if(0!=event_ConnectAritight(i))
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

int DeviceManage::event_ConnectAritight(int station)
{
    CModbusClient::Settings param;
    param.port = GlobalParam->systemParam.serialComStruct[station].portName;
    param.parity = GlobalParam->systemParam.serialComStruct[station].serParity;
    param.baud = GlobalParam->systemParam.serialComStruct[station].serBaudRate;
    param.dataBits = GlobalParam->systemParam.serialComStruct[station].serDataBit;
    param.stopBits = GlobalParam->systemParam.serialComStruct[station].serStopBit;
    param.responseTime = 2000;
    param.numberOfRetries = 3;

    m_cModbusClient[station].SetParams(param);

    if(!m_cModbusClient[station].DeviceConnect())
    {
        return -1;
    }
    return 0;
}

int DeviceManage::event_DisConnectAritight(int station)
{
    m_cModbusClient[station].DeviceDisConnect();
    return 0;
}


// 选择测试程序(1=IP67,2=IP68)
int DeviceManage::event_SelectAirtightTestProgram(int station,int programId)
{
    return m_cModbusClient[station].SelectTestProgram(programId)?0:-1;
}
// 启动
int DeviceManage::event_StartAirtightTest(int station)
{
    return m_cModbusClient[station].StartAirtightTest()?0:-1;
}
// 复位
int DeviceManage::event_ResetAirtight(int station)
{
    return m_cModbusClient[station].ResetAirtight()?0:-1;
}
// 设置参数
int DeviceManage::event_SelectAirtightParam(int station,int paramId)
{
    return m_cModbusClient[station].SelectParam(paramId)?0:-1;
}
// 设置参数
int DeviceManage::event_SetAirtightParam(int station)
{
    bool res = false;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_CouplADelayTime,GlobalParam->recipeAirtight.airtightParam[station].CouplADelayTime);
    if(!res)return -1;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_FillTime,GlobalParam->recipeAirtight.airtightParam[station].FillTime);
    if(!res)return -2;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_StabTime,GlobalParam->recipeAirtight.airtightParam[station].StabTime);
    if(!res)return -3;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_TestTime,GlobalParam->recipeAirtight.airtightParam[station].TestTime);
    if(!res)return -4;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_DumpTime,GlobalParam->recipeAirtight.airtightParam[station].DumpTime);
    if(!res)return -5;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_PressUnit,GlobalParam->recipeAirtight.airtightParam[station].PressUnit);
    if(!res)return -6;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_MaxFill,GlobalParam->recipeAirtight.airtightParam[station].MaxFill);
    if(!res)return -7;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_MinFill,GlobalParam->recipeAirtight.airtightParam[station].MinFill);
    if(!res)return -8;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_SetFill,GlobalParam->recipeAirtight.airtightParam[station].SetFill);
    if(!res)return -9;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_LeakUnit,GlobalParam->recipeAirtight.airtightParam[station].LeakUnit);
    if(!res)return -10;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_VolumeUnit,GlobalParam->recipeAirtight.airtightParam[station].VolumeUnit);
    if(!res)return -11;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_Volume,GlobalParam->recipeAirtight.airtightParam[station].Volume);
    if(!res)return -12;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_TestFail,GlobalParam->recipeAirtight.airtightParam[station].TestFail);
    if(!res)return -13;
    res = m_cModbusClient[station].SetParam(CModbusClient::MODE_RefFail,GlobalParam->recipeAirtight.airtightParam[station].RefFail);
    if(!res)return -14;
    return 0;
}
// 读取仪器实时测试状态，判断仪器是否测试结束
int  DeviceManage::event_ReadAirtightRealStatus(int station)
{
    return m_cModbusClient[station].ReadRealStatus()?0:-1;
}
// 读取实时测试结果
int  DeviceManage::event_ReadAirtightRealResult(int station,AritightTask &data)
{
    CModbusClient::AritghtResult result;
    int res = m_cModbusClient[station].ReadRealResult(result);
    if(res == -1) return res;
    if(res == 2 ) return res;
    data.result = result.res==0?true:false;
    data.pressValue = result.pressValue;
    data.leakageValue = result.leakageValue;
    data.TestStage = result.TestStage;
    data.errorMsg = result.errorMsg;
    data.errorCode = result.alarmCode;
    return 0;
}
// Last results最终的测试结果
int  DeviceManage::event_ReadAirtightLastResult(int station,AritightTask &data)
{
    CModbusClient::AritghtResult result;
    int res = m_cModbusClient[station].ReadLastResult(result);
    if(res!=0) return res;
    data.result = result.res==0?true:false;
    data.pressValue = result.pressValue;
    data.leakageValue = result.leakageValue;
    data.TestStage = result.TestStage;
    data.errorMsg = result.errorMsg;
    data.errorCode = result.alarmCode;
    return 0;
}
