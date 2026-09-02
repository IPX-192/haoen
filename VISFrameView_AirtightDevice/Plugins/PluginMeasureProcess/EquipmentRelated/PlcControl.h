#ifndef PLCCONTROL_H
#define PLCCONTROL_H
#include <QList>
#include <QMutex>
#include "ParamManager.h"
#include <QVector>

#ifndef PlcControlIns
#define PlcControlIns PlcControl::Instance()
#endif
class PlcControl
{
public:
    static PlcControl &Instance();
    ~PlcControl();

    /********PLC设备相关********/
    //设置上位机PLC复位状态
    void SetHomeStatus(bool status);
    //获取上位机PLC复位状态
    bool GetIsHomed();
    //PLC复位
    bool WritePLCReset();
    //PLC启动
    bool WritePLCStart();
    //PLC停止
    bool WritePLCStop();
    //PLC急停
    bool WritePLCEmgStop();
    //PLC自动/手动模式切换
    bool WritePLCHandeMode(bool autoMode);
    //清除PLC报警
    bool ClearPLCWarning();
    //读取设备复位动作状态 1已完成 0未完成
    bool ReadResetStatus();
    //读取设备报警状态 1报警 0正常
    bool ReadAlarmStatus(int& status);
    //读取自动流程状态 1自动 0手动
    bool ReadAutoMode(int& status);
    //设置蜂鸣器报警
    bool SetAlarm(bool on);

    /********PLC直接操作寄存器相关********/
    //转换为PLC数据,数据*对应轴脉冲当量
    double ConvertTiUnit(double dValue,QString axisName);
    //写入单个ML寄存器
    bool WriteML(QString& regName, bool state);
    //写入单个ML寄存器(缓存)
    bool ReadML(QString& regName, bool& state);
    //写入单个D寄存器
    bool WriteD(QString& regName, qint32 data);
    //写入单个D寄存器(缓存)
    bool ReadD(QString& regName, qint32& data);
    //写入连续D寄存器
    bool WritePageD(QString& regName, int length, QVector<qint32>& data);
    //实时读连续D寄存器
    bool ReadPageD(QString& regName, int length, QVector<qint32>& data);
    //实时读连续M寄存器
    bool ReadPageML(QString& regName, int length, QVector<bool>& data);

private:
    PlcControl();
    bool WriteMlInterval(QString strRegName,int nInterval);

protected:
    bool  plcIsHomed=false;


signals:

public slots:
};

#endif // PLCCONTROL_H
