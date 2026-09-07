#ifndef DEVICEMANAGE_H
#define DEVICEMANAGE_H

#include <QObject>
#include <QMap>
#include <thread>
#include <atomic>
#include "../../interface/singleton.h"
#include "ScanCode/ScanCodeClient.h"
#include "AirtightTest/CModbusClient.h"
#include "ParamDef.h"

#if defined(PLUGINDEVICE_LIBRARY)
#  define DEVICEMANAGER_EXPORT Q_DECL_EXPORT
#else
#  define DEVICEMANAGER_EXPORT Q_DECL_IMPORT
#endif

#define Cam u8"相机"


class DEVICEMANAGER_EXPORT DeviceManage : public QObject
{
    SINGLETON_DECL(DeviceManage)
    Q_OBJECT

protected:
    explicit DeviceManage(QObject *parent = nullptr);
    ~DeviceManage();
public:
//    static DeviceManage *GetIns();
    int  Reset();
    int  InitDevice();

private:
    //初始化相机
    int  InitOtherCam(bool open = true);
    int  SetOtherCamEx(QString camType, int ex);

    //气密初始化
    bool InitAritight();

    //工位(0-3) -> 仪器台号(0=左工站仪,1=右工站仪)
    int  StationToDevice(int station) const { return station / 2; }
    //工位(0-3) -> 仪器通道(1/2)
    int  StationToChannel(int station) const { return station % 2 + 1; }
signals:
    void ErrorMessageSignals(QString strError, int nCurrentLine);
public slots:
    //扫码枪
    int event_ScanCodeConnect(QString name);
    int event_DisScanCodeConnect(QString name);
    //气密设备连接(device: 0=左工站仪, 1=右工站仪)
    int event_ConnectAritight(int device);
    int event_DisConnectAritight(int device);

    //气密设备程序号(station 0-3)
    int event_SelectAirtightTestProgram(int station,int programId);
    //气密设备启动(station 0-3)
    int event_StartAirtightTest(int station);
    //气密设备停止(station 0-3)
    int event_ResetAirtight(int station);
    //气密设备参数(station 0-3)
    int event_SelectAirtightParam(int station,int paramId);
    //气密设备参数设置(station 0-3)
    int event_SetAirtightParam(int station);
    //读取仪器测试状态,判断是否测试结束(station 0-3)
    int event_ReadAirtightRealStatus(int station);
    // 读取测试结果(station 0-3)
    int event_ReadAirtightRealResult(int station, AritightTask &data);
    // 最终的测试结果(station 0-3)
    int event_ReadAirtightLastResult(int station, AritightTask &data);
private:
    ScanCodeClient m_produceScan;
    ScanCodeClient m_trayScan;
    static DeviceManage *m_pThis;
    CModbusClient m_cModbusClient[AirtightDeviceCount];   //2台双通道气密仪(左右工站)

    //气密结果轮询(独立线程:轮询各工位完成状态 → 读最终结果 → 发 "AirtightResultBack")
    void  AirtightPollLoop();
    void  StartAirtightPoll();                            //启动轮询线程
    void  StopAirtightPoll();                             //停止轮询线程
    static const int AirtightStationCount = 4;            //4 测试工位(0-3)
    std::thread       m_pollThread;
    std::atomic<bool> m_pollRun{false};
};

#endif // DEVICEMANAGE_H
