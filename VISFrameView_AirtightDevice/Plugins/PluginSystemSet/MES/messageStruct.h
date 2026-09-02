#pragma once
#include <QtWidgets/QWidget>
#include <QDateTime>

/*
通用的参数含义说明：
ErrCode为错误码0为成功其它为失败
ErrSeverity错误严重级别
ErrText为具体的错误信息


*/
/* 获取token输入信息 */
struct TOKEN_IN
{
	int CommunityId;			//社区标识
	QString UserCode;          // 登录名
	QString PlainPWD;           // 密码
};
//获取token返回信息（输出）
struct TOKEN_OUT
{
	int ErrSeverity;
	QString Access_Token;   //token数值   
    int ErrCode;
	QString ErrText;
};
/* 设备清单 (输出)*/
class EquipmentList_Out
{
public:
	struct EquipmentItem
	{
		QString EquipmentCode;	//设备代码
		QString lineCode;		//产线代码
	};
	QString excode = "GetEquipmentList";				//调用接口
	int Errcode;				//错误代码
	QString ErrText;			//错误文本
	int ErrSeverity;        //错误级别
	QVector <EquipmentItem> equipmentList; //设备

};
//下放管控参数(输入)
struct GetMFGCtrl_IN {
	QString EquipmentCode;
	QString ProductNo;
	QString OperationInstanceCode;
	QString WorkStationCode;
	QString Model;
	QString ParameterName;
	QString ParameterType;
};
//下放管控参数(输出)

class GetMFGCtrl_OUT {
public:
	struct GetMFGCtrl_OUTRowData {
		QString ParameterName;
		QString ParameterValue;
		QString Lowerlimit;
		QString Upperlimit;
		QString Remark1;
		QString Remark2;
		QString Remark3;
	};
	QVector<GetMFGCtrl_OUTRowData>rowData;
	int ErrCode;
	int ErrSeverity;
	QString ErrText;
};


//管控参数回传(输入)
class UploadMFGCtrlParameterResult_IN {
public:
	struct UploadMFGCtrlParameterResultRowData {
		QString ParameterName;
		QString ParameterValue;
		QString Lowerlimit;
		QString Upperlimit;
		QString ParameterType;
		QString ActualValue;
		QString Result;
		QString Remark;
	};
	QString EquipmentCode;
	QString PWONo;
	QString ProductNo;
	QString OperationInstanceCode;
	QString WorkStationCode;
	QString Model;
	QString Conclusion;
	QVector<UploadMFGCtrlParameterResultRowData>rowData;
};
//管控参数回传(输出)
struct UploadMFGCtrlParameterResult_Out {
	int ErrCode;
	QString ErrText;
	int ErrSeverity;
};

//获取胶水信息（输入）
struct GetGlueCtrlInfo_IN {
	QString SKUID;
	QString	EquipmentCode;
	QString OperationInstanceCode;
	QString WorkStationCode;
};
//获取胶水信息（输出）
class GetGlueCtrlInfo_OUT {
public:
	struct GetGlueCtrlInfoRowData {
		QString SKUID;
		QString MaterialCode;
		QString MoveOutTime;
		int		UpperExposeTimes;
		QString ScrapTime;
	};
	int ErrCode;
	QString ErrText;
	int ErrSeverity;
	QVector<GetGlueCtrlInfoRowData>m_rowData;
};

/* 工单信息（输入） */
struct PwoInfo_IN {
        bool isProduct = true;      // true:产品  false:托盘
	QString equipmentCode;
	QString operationInstanceCode;
	QString toolingSeque;
	QString ContainerType;
        QString WIP_ID_Type_Code;
        QString WIP_ID_Code;
};

/* 工单信息（输出） */
class PwoInfo_Out {
public:
	struct PwoProduct {
		QString PWOCode;		//生产工单代码
		QString ProductNo;		//产品编号
		int ProductID;
		int Qty;				//工单数量
		int	AvailableQty;
		int PWOPriority;
		QString ProductFamily;
		int ContainerBindingQty;
		QString LotNumber;
		QDateTime PlannedStartTime;		//工单计划开始时间
		QDateTime PlannedEndTime;		//工单计划结束时间
	};
        QString excode = "";				//调用接口
    int Errcode;				//错误代码
	QString ErrText;			//错误文本
	int ErrSeverity;        //错误级别
	QVector<PwoProduct> allProduct; //所有工单产品信息

};

/* 测试数据的输入参数和输出参数 */
class DC_TestData_In {
public:
	struct testData {
        int Ordinal;            // 测试顺序
        QString TestItemCode;   // 测试code
        QString MetricName;     // 测试名称
        int LowLimit;           // 下限
        /*
         * GELE：大于等于 X 小于等于
         * GTLE：大于 X 小于等于
         * GELT: 大于等于 X 小于
         * GTLT 大于 X 小于
         * EQ 等于
         * NE 不等于
         * GT 大于
         * GE 大于等于
         * LT 小于
         * LE 小于等于
         * BOOL 布尔是否
         * */
        QString Criterion;      // 测试通过标准
        int HighLimit;          // 上限
        int Scale;              // 放大数量级
        QString UnitOfMeasure;  // 单位
        QString Conclusion;     // P / F
        QString remark;         // 备注
		QString ImageBase64;
        int64_t Metric01;       // 测试值

        testData(){
            Ordinal = 0;
            TestItemCode = "";
            MetricName = "";
            LowLimit = 0;
            Criterion = "GE";
            HighLimit = 0;
            Scale = 0;
            UnitOfMeasure = "";
            Conclusion = "P";
            remark = "";
            ImageBase64 = "";
            Metric01 = 0;
        }
	};
	///*struct FailureModes
	//{
	//	int Ordinal;
	//	QString CompLocCode;
	//	QString MaterialCode;
	//	QString DefectCode;
	//	QString RootOperationInstanceCode;
	//	QString DefectTypeCode;
	//	QString DefectOwner;
	//	int CntDefect;
	//	QString DefectImageBase64;
	//};
	//struct Recipes
	//{
	//	int Ordinal;
	//	QString ParamCode;
	//	QString ParamDesc;
	//	QString DataType;
	//	int Scale;
	//	QString UnitOfMeasure;
	//	int ParamValue;
	//	QString ParamStrValue;
	//};*/
	QString ExCode;
	QString EquipmentCode;
	QString OperationInstanceCode;
	QString ProductNo;
	QString WIP_ID_Type_Code;
	QString WIP_ID_Code;
	QString SerialNumber;
    QString SensorID;
	QString PWONo;
	QString ContainerNo;
	QString WIPStationCode;
	QString RecipeID;
	QString NumSubWIPs;
	QString StartTime;
	QString EndTime;
	QString CntOfTests;
	QString FailureCode;
	QString FailureMessage;
	int TestConclusion;
	QString TestSteps;
	int NumOfMetrics;
	QVector <testData> allTestData;
};
struct DC_TestData_Out {
	int ErrCode;
	QString ErrText;
	int ErrSeverity;
};

/* 检查数据的输入参数和输出参数 */
class DC_Inspect_In {
public:
	struct  Detail
	{
		int Ordinal;
		QString CompLocCode;
		QString MaterialCode;
		QString DefectCode;
		QString ConfirmedDefectCode;
		bool StandardDefectCode;
		QString RootOperationInstanceCode;
		QString DefectTypeCode;
		QString DefectOwner;
		int CntDefect;
		QString DefectImageBase64;
	};
	QString ExCode;
	QString ProductNo;
	QString EquipmentCode;
	QString OperationInstanceCode;
	QString WorkStationCode;
	QString WIP_ID_Type_Code;
	QString WIP_ID_Code;
	QString PWONo;
	QString ContainerNo;
	QString StartTime;
	QString EndTime;
	int InspectionType;
	int InspectionConclusion;
	QVector<Detail>allDetail;
};

struct DC_Inspect_Out {
	QString ExCode;
	int  ErrCode;
	QString ErrText;
	int ErrSeverity;
};

/* 生产结束的输入参数和输出参数 */
class ProductionEnd_In {

public:
	QString ExCode;
	QString EquipmentCode;
	QString ProductNo;
	QString OperationInstanceCode;
	QString WorkStationCode;
	QString WIP_ID_Type_Code;
	QString WIP_ID_Code;
	int Qty;
	int Conclusion;
};
struct ProductionEnd_Out {
	QString ExCode;
	int ErrCode;
	QString ErrText;
	int ErrSeverity;

};

/* SensorID绑定 */
class BindSensorID_In {
public:
	struct IDdata
	{
		QString ID_Part_Type_Code;
		QString ID_Part_Name;
		QString ID_Part_SN_Scanner_Code;
		QString Sequence_Number;
	};
	QString ExCode;
	QString ProductNo;
	QString EquipmentCode;
	QString OperationInstanceCode;
	QString WorkStationCode;
	QString WIP_ID_Type_Code;
	QString WIP_ID_Code;
	QVector<IDdata>allID;
};
struct BindSensorID_Out {
	QString ExCode;
	int Part_Number_Feedback;
	int ErrCode;
	QString ErrText;
	int ErrSeverity;
};

/* 制品与容器绑定的输入参数和输出参数 */
class WIPBindToContainer_In {
public:
	struct WIPData
	{
		QString WIP_ID_Type_Code;
		QString WIP_ID_Code;
		QString SensorID;
		int Qty;
		int QtyScale;
		QString ProductNo;
	};
	QString ExCode;
	QString ProductNo;
	QString EquipmentCode;
	QString OperationInstanceCode;
	QString WorkStationCode;
	QString PWONo;
	int Qty;
	QString ContainerNo;
	QVector<WIPData>allWIP;
};
struct WIPBindToContainer_Out {
	int ErrCode;
	QString ErrText;
	int ErrSeverity;
};
/* 工艺矩阵 */
class ProcessMatrix_Out {
public:
	struct curState {
		QString OperationInstanceCode;//工序实例代码
		bool B01;
		bool B02;
		bool B03;
		bool B04;
		bool B05;
		bool B06;
		bool B07;
		bool B08;
		bool B09;
		bool B10;
		bool B11;
		bool B12;
		bool B13;
		bool B14;
		bool B15;
		bool B16;
		bool B17;
		bool B18;
		bool B19;
		bool B20;
		bool B21;
		bool B22;
		bool B23;
		bool B24;
		bool B25;
		bool B26;
		bool B27;
		bool B28;
		bool B29;
		bool B30;
		bool B31;
		bool B32;
		bool B56;

	};
	struct oneMatrix {
		QString LineCode;		//产线代码
		QVector<curState>state;		//防错状态
	};

	QString excode = "GetProcessControlMatrix";				//调用接口
	int Errcode;				//错误代码
	QString ErrText;			//错误文本
	int ErrSeverity;        //错误级别
	QVector<oneMatrix> allMatrix; //所有产线信息

};
/* 防错验证输入 */
struct ErrorList_IN
{
	QString EquipmentCode;
	QString ProductNo;
	QString OperationInstanceCode;
	QString WorkStationCode;
	QString WIP_ID_Type_Code;
	QString WIP_ID_Code;
	QString Product_Family;
    QString pText;
};

/* 防错验证输出 */
struct ErrorList_Out
{
	QString ExCode = "PokaYoke";
	int ObjectType;
	int Errcode;
	QString ErrText;
	int ErrSeverity;
	int ContainerBindingQty;
    QString pValue;
};
/* 管控工艺输出参数 */
class  STD_M01_Out
{
public:
	struct STD_M
	{
		int Ordinal;
		QString ParamCode;
		QString ParamDesc;
		QString dataType;
		int ParamValue;
		int Scale;
		QString ParamStrValue;
		QString UnitOfMeasure;
		int RecordingMode;
		int SamplingCycle;
	};
	QString ExCode;
	QString RecipeID;
	int  ErrCode;
	QString ErrText;
	int ErrSeverity;
	QVector <STD_M> allMartix;
};
/* 采集参数下发输入参数 */
class CollectionDataRequest_In {
public:
	struct collectParm
	{
		int ItemCode;
		QString ItemName;
		QString SrcOperationInstanceCode;
		QString ItemType;
	};
	QString ExCode;
	QString EquipmentCode;
	QString ProductNo;
	QString OperationInstanceCode;
	QString WorkStationCode;
	QString PWONo;
	QString WIP_ID_Type_Code;
	QString WIP_ID_Code;
	QVector <collectParm>allCollectParm;
};

class CollectionDataRequest_Out {
public:
	struct itemParm
	{
		int ItemCode;
		QString ItemName;
		QString SrcOperationInstanceCode;
		QString TtemType;
		QString ItemValue;
	};
	QString ExCode;
	int ErrCode;
	QString ErrText;
	QVector<itemParm>allItem;
};
