#pragma once

#include <QtWidgets/QWidget>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThreadPool>
#include <iostream>
#include "messageStruct.h"
#include <QMutex>

//#ifdef EXPORTS
//#define MES_API __declspec(dllexport)
//#else
//#define MES_API __declspec(dllimport)
//#endif
#define MesInstance FreetechMes::getInstance();
class FreetechMes : public QObject
{
	Q_OBJECT
private:
	FreetechMes();
	// 禁止拷贝构造和赋值操作
	FreetechMes(const FreetechMes&) = delete;
	FreetechMes& operator=(const FreetechMes&) = delete;
	~FreetechMes();
public:
	static FreetechMes* getInstance();//需要在主线程调用一次初始化
	void setUrlIp(QString urlIp);//设置URL的ip如果有变动需要设置默认为10.182.2.13:6201
	/*通用接口(正常的流程,正常的调用顺序)*/
	QString GetToken(TOKEN_IN info, TOKEN_OUT& outData);//获取Token并设置到http请求头中（在调用其他请求中必须先调这个获取token）
    QString GetPWOList(int station,PwoInfo_IN inData, PwoInfo_Out& outData);//获取工单列表;
    QString PokaYoke(int station,ErrorList_IN inData, ErrorList_Out& outData);//请求防错验证
    QString ProductionEnd(int station,ProductionEnd_In inData, ProductionEnd_Out& outData);//生产结束
    QString DC_TestData(int station,DC_TestData_In inData, DC_TestData_Out& outData);//测试数据采集（部分不需要回传参考流程mes调用）
    QString IDBinding(int station,BindSensorID_In inData, BindSensorID_Out& outData);

	//额外的接口视情况用（参考流程的Mes调用）
    QString GetMFGCtrlParameters(int station,GetMFGCtrl_IN inData, GetMFGCtrl_OUT& outData);//下发管控参数
    QString UploadMFGCtrlParameterResult(int station,UploadMFGCtrlParameterResult_IN inData, UploadMFGCtrlParameterResult_Out& outData);//管控参数回传
    QString GetGlueCtrlInfo(int station,GetGlueCtrlInfo_IN inData, GetGlueCtrlInfo_OUT& outData);//获取胶水信息
    QString WIPBindToContainer(int station,WIPBindToContainer_In inData, WIPBindToContainer_Out& outData);//制品与容器绑定

public:
    void SetTimeout(int time);

private slots:
	bool postData(QJsonObject dataJson, QString messageType);
	void slotPostFinished(QNetworkReply* reply);

private:
	void processReply(const QJsonObject dataObj);//mes回复消息处理
	void initConnet();
	static FreetechMes* instance;//单例
	QString m_token; //存储使用的token
	//网络通信
	QNetworkAccessManager m_accessManger;
	QNetworkRequest m_request;

	bool m_waitingFlag;
	bool m_operationStatus;
	int m_timeout = 10000;//延时
	//每个接口返回的数据参数
	TOKEN_OUT m_tokenOutData;
	UploadMFGCtrlParameterResult_Out m_UploadMFGCtrlParameterResultData;
	EquipmentList_Out m_equipmentList;
	PwoInfo_Out m_PwoInfo;
	ProcessMatrix_Out m_matrix;
	ErrorList_Out m_error;
	DC_TestData_Out m_dcTestData;
	GetGlueCtrlInfo_OUT m_GetGlueCtrlInfoData;
	DC_Inspect_Out m_dcInspectData;
	ProductionEnd_Out m_ProductionEndData;
	BindSensorID_Out m_BindIdData;
	WIPBindToContainer_Out m_WipBingdData;
	STD_M01_Out m_STD_M01Data;
	CollectionDataRequest_Out m_CollectionDataRequest;
	GetMFGCtrl_OUT	m_GetMFGCtrl_OUTData;
	QString m_errorInfo;
	std::thread::id  m_currentId;
	static  QMutex  m_mutex;
	static  QMutex  m_mutex2;
	QString m_urlIp = "10.182.2.13:6201";
	QString m_messageType;
	bool ifConnect = false;
    int  m_station = 0;
};
