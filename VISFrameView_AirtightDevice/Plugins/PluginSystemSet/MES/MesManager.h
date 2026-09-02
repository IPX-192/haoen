#ifndef MESMANAGER_H
#define MESMANAGER_H

#include <QObject>
#include "ParamDef.h"
#include <QDateTime>
#include "messageStruct.h"

class MesManager:public QObject
{
    Q_OBJECT
public:
    explicit MesManager(QObject *parent = nullptr);
public slots:
    //获取工单
    int event_getWorkPwoList(QString trayCode,QString &workPwoNo,int &res,QString &msg);
    //条码校验
    int event_checkLensCode(int station,QString lensCode,int checkType,QString &sensorId,int &res,QString &msg);
    //数据上传
    int event_passStation(AritightTask item,int &res,QString &msg);
    //生产结束
    int event_productionEnded(int station,QString lensCode,bool result,int &res,QString &msg);
//    //参数管控请求
//    int event_getCtrIParameters(int station,QString &msg);
//    //参数管控请求输入
//    int event_uploadCtrIParameters(FinalItemTask item,QString &msg);
private:
    int   ModulePassStation(AritightTask item, QString &describe);
//    void  AssemblyData(FinalItemTask item,QVector<DC_TestData_In::testData> &allTestData);

//    //电流电压
//    void  AssemblyVppVccData(FinalItemTask item,int &index,QVector<DC_TestData_In::testData> &allTestData);


    DC_TestData_In::testData GetTestData(int &index,double low,double upper,double value,QString name,QString compareType,QString unit);

    QString getResult(QString type,double value,double low,double upper);
private:
    int m_scale = 0;
};

#endif // MESMANAGER_H
