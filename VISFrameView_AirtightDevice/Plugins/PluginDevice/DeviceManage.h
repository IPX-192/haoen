#ifndef DEVICEMANAGE_H
#define DEVICEMANAGE_H

#include <QObject>
#include <QMap>
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
signals:
    void ErrorMessageSignals(QString strError, int nCurrentLine);
public slots:
    //扫码枪
    int event_ScanCodeConnect(QString name);
    int event_DisScanCodeConnect(QString name);
    //气密设备连接
    int event_ConnectAritight(int station);
    int event_DisConnectAritight(int station);

    //气密设备程序号
    int event_SelectAirtightTestProgram(int station,int programId);
    //气密设备启动
    int event_StartAirtightTest(int station);
    //气密设备复位
    int event_ResetAirtight(int station);
    //气密设备参数
    int event_SelectAirtightParam(int station,int paramId);
    //气密设备参数设置
    int event_SetAirtightParam(int station);
    //读取仪器实时测试状态，判断仪器是否测试结束
    int event_ReadAirtightRealStatus(int station);
    // 读取实时测试结果
    int event_ReadAirtightRealResult(int station, AritightTask &data);
    // Last results最终的测试结果
    int event_ReadAirtightLastResult(int station, AritightTask &data);
private:
    ScanCodeClient m_produceScan;
    ScanCodeClient m_trayScan;
    static DeviceManage *m_pThis;
    CModbusClient m_cModbusClient[2];
};

#endif // DEVICEMANAGE_H
