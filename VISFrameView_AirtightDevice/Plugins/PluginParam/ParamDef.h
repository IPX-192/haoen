#ifndef PARAMDEF_H
#define PARAMDEF_H

#include <QObject>
#include <QVariant>
#include <QImage>
#include <QPointF>
#include <QVector4D>
#include <QDateTime>
#include "../Common/FeildDefine.h"

#if defined(PLUGINPARAM_LIBRARY)
#  define PARAMMANAGER_EXPORT Q_DECL_EXPORT
#else
#  define PARAMMANAGER_EXPORT Q_DECL_IMPORT
#endif

#define  Log_PipeLine (QString(u8"流水线"))

#define Log_Process (QString(u8"流程日志"))
#define Log_Station (QString(u8"工位"))
#define Log_Mes (QString(u8"Mes日志"))

#define ModuleScanCode (QString(u8"产品"))
#define TrayScanCode (QString(u8"料盘"))

#define DateTimeStr     QStringLiteral("yyyy-MM-dd HH:mm:ss")

#define  VisonCam "视觉检测相机"
#define  StationCount 2
#define  GripperCount 1

#define Property_Var(type, name, value) Q_PROPERTY(type name MEMBER name) type name=value;

//Matrix项
struct  MatrixSetting {
    QString  platformName;     //平台名称(MES)
    QString  productName;      //产品名称
    QString  productShow;      //上位机显示(机种切换)
    QString  productNo;        //装配序列号
    QString  barCodeLimit1;    //主轨道产品固定条码
    QString  barCodeStartPos1; //固定条码起始位置
    QString  barCodeLimit2;    //次轨道固定条码
    QString  barCodeStartPos2; //固定条码起始位置
    QString  productFamily1;   //主轨道托盘类型
    QString  fixtureSn1;       //主轨道治具码
    QString  productFamily2;   //次轨道托盘类型
    QString  fixtureSn2;       //次轨道治具码
    QString  lensProductNo;    //镜头水洗序列号
    QString  lensCodeEnable;   //镜头码防错启用
};

struct ProductMatrix {
    QString productName;       //产品名称
    QString recipeMotor;       //电机点位配方名称
    QString recipeTray;        //托盘配方名称
    QString recipeElectricSaw; //电动夹爪配方名称
    QString recipeAirtight;    //气密配方
};


//型号参数
class PARAMMANAGER_EXPORT RecipeProduct : public QObject
{
    Q_OBJECT
public:
    QString          productPath = "";             //产品型号路径
    QStringList      listProduct;                  //产品型号列表
    QVector<MatrixSetting> listPlatformMatrix;     //平台列表映射
    QVector<ProductMatrix> listRecipe;             //产品配方
    MatrixSetting* curMatrix = nullptr;
    QString          curProduct = "123";                   //当前产品型号
};

//电机点位配方
class PARAMMANAGER_EXPORT RecipeMotor : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(QStringList listRecipe MEMBER listRecipe)
public:
    QString      filepath = "";      //配方路径
    QStringList  listRecipe;         //视觉配方列表
    QString      curRecipe;          //当前配方
};

//模板匹配
class PARAMMANAGER_EXPORT ShapeMatch : public QObject
{
    Q_OBJECT
public:
    Property_Var(int , exposure , 1000)     //曝光时间
    Property_Var(int , lightValue , 50)     //光源亮度
    Property_Var(int , contrastLow , 0)     //对比度(低)
    Property_Var(int , contrastHigh , 0)    //对比度(高)

    Property_Var(int , level , 0)           //金字塔级别
    Property_Var(QString , metricStr , "")  //度量
    Property_Var(double , angleStep , 0.2)  //角度步长
    Property_Var(QString , optimizStr , "") //最优化
    Property_Var(QString , modleName , "")  //模板名称
    QRectF rectModle;        //模板区域
};

//托盘配方
class PARAMMANAGER_EXPORT RecipeTray : public QObject
{
    Q_OBJECT
public:
    RecipeTray &operator=(const RecipeTray&);
    bool LoadPosList(QString sFilePath, QObject* uiObj);
    bool SavePosList(QString sFilePath, QObject* uiObj);
public:
    Property_Var(QString , filepath , "") //配方路径
    Property_Var(QStringList , listRecipe , QStringList())  //配方列表
    Property_Var(QString , curRecipe , "") //当前配方
    //具体托盘参数
    Property_Var(int , feedTrayW , 1)
    Property_Var(int , feedTrayH , 1)
    Property_Var(int , ngTrayW , 1)
    Property_Var(int , ngTrayH , 1)

    QVector<QVector4D> feedTrayPos[2]; //工位1上料托盘位置列表
    QVector<QVector4D> ngTrayPos;
};

//电动夹爪配方
class PARAMMANAGER_EXPORT RecipeElectricSaw : public QObject
{
    Q_OBJECT
public:
    Property_Var(QString , filepath , "") //配方路径
    Property_Var(QStringList , listRecipe , QStringList())  //配方列表
    Property_Var(QString , curRecipe , "") //当前配方

    Property_Var(int , nGripperClampingForce , 1)//夹爪夹持力
    Property_Var(int , nRotateForce , 1)//旋转力度
};

//夹爪参数寄存器配置
class PARAMMANAGER_EXPORT GripperAddrParam : public QObject
{
    Q_OBJECT
public:
    Property_Var(QString , sGripperClampingForceAddr , "")//夹爪力度设置寄存器地址
    Property_Var(QString , sClampingSpeedAddr , "")//夹爪速度设置寄存器地址

    Property_Var(QString , sClampingHomeSpeedAddr , "")
    Property_Var(QString , sRotateSpeedAddr , "")
    Property_Var(QString , sRotateHomeSpeedAddr , "")
    Property_Var(QString , sRotateForceAddr , "")
    Property_Var(QString , sClampPosAddr , "")

    Property_Var(QString , sRotateAngleAddr , "")
    Property_Var(QString , sClampHomePosAddr , "")
    Property_Var(QString , sRotateHomeAngleAddr , "")
    Property_Var(QString , sClempCurrentPosAddr , "")
    Property_Var(QString , sClampAddr , "")
    Property_Var(QString , sClampReleaseAddr , "")
};

//气密配方
class PARAMMANAGER_EXPORT AirtightParam : public QObject
{
    Q_OBJECT
public:
    Property_Var(int , processId , 0)   // 程序号
    Property_Var(int , paramId , 0)     // 参数号
    Property_Var(int , CouplADelayTime , 0)     // 0-650

    Property_Var(int , FillTime , 0)     // 充气时间 0~650s
    Property_Var(int , StabTime , 0)     // 稳定时间 0~650s
    Property_Var(int , TestTime , 0)     // 测试时间 0~650s
    Property_Var(int , DumpTime , 0)     // 排气时间 0~650s

    Property_Var(int , PressUnit , 0)    // 压力单位 见“单位列表”
    Property_Var(int , MaxFill , 0)      // 最大充气压力 -9999~9999
    Property_Var(int , MinFill , 0)      // 最小充气压力 -9999~9999
    Property_Var(int , SetFill , 0)      // 设定的充气压力 -9999~9999

    Property_Var(int , LeakUnit , 0)     // 泄漏单位 见“单位列表”
    Property_Var(int , VolumeUnit , 0)   // 容积单位 见“单位列表”
    Property_Var(int , Volume , 0)       // 容积大小 0~9999
    Property_Var(int , TestFail , 0)     // 泄漏上限 0~9999
    Property_Var(int , RefFail , 0)      // 泄漏下限 0~9999
};
//气密配方
class PARAMMANAGER_EXPORT RecipeAirtight : public QObject
{
    Q_OBJECT
public:
    Property_Var(QString , filepath , "") //配方路径
    Property_Var(QStringList , listRecipe , QStringList())  //配方列表
    Property_Var(QString , curRecipe , "") //当前配方

    AirtightParam airtightParam[2];  //气密参数
};

//数据存储
class PARAMMANAGER_EXPORT DataStorage : public QObject
{
    Q_OBJECT
public:
    Property_Var(QString , sProductSavePath , "")
    Property_Var(int , decimals , 4)//小数点位数
};

struct SerialComStruct
{
public:
    QString portName = "";       // 用于存储串口的名称，例如 "COM1"
    int serFlowCtrl = 0;    // 用于设置串口的流控制模式，默认值为 0
    int serParity = 0;       // 用于设置串口的校验模式，默认值为 0
    int serStopBit = 1;      // 用于设置串口的停止位数量，默认值为 1
    int serDataBit = 8;      // 用于设置串口的数据位数量，默认值为 8
    int serBaudRate = 9600;  // 用于设置串口的波特率，默认值为 9600
};

class PARAMMANAGER_EXPORT ScanCodeParam : public QObject{
    Q_OBJECT
public:
    Property_Var(QString , sIP , "127.0.0.1")
};

struct ShieldParam
{
    bool  safeDoor=false;  //安全门
    bool  passTray=false;  //托盘过站
    bool  station1=false;  //站1
    bool  station2=false;  //站2
    bool  station3=false;  //站3
    bool  station4=false;  //站4
    bool  lightAlwaysOpen = false; //光源长亮
    bool  plcWarnTop = false;    //报警弹窗

    bool  angleDetect = false;  //角度检测
    bool  materialDetect = false;   //物料检测
    bool  screwHoleDetect = true;  //螺丝孔检测
    bool  scanTrayCode = true;  //扫描托盘
    bool  scanChipCode = false;  //扫描产品码
};

class PARAMMANAGER_EXPORT MesParam : public QObject
{
    Q_OBJECT
public:
    Property_Var(bool , mesEnable , true)
    Property_Var(QString , mesIp , "127.0.0.1")

    Property_Var(QString , mesPort , "")
    Property_Var(QString , userName , "")
    Property_Var(QString , userPassword , "")
    Property_Var(int , mesTimeout ,10000)

    //新增
    Property_Var(QString , sEquipmentCode , "M-MSF-001")//设备编码
    Property_Var(QString , sOperationInstance , "A1-MSF-120") //托盘操作码
    Property_Var(QString , sWorkStationCodeOne , "A1-MSF-110-W")//PCBA 工作站编码1

    Property_Var(QString , sWorkStationCodeTwo , "A1-MSF-110-W")//PCBA 工作站编码2
    Property_Var(QString , sWIP_ID_Type_CodeTray , "25") //工单类型编码 25容器 31产品
    Property_Var(QString , sWIP_ID_Type_CodeChip , "31")//工单类型编码 25容器 31产品

    Property_Var(QString , sModel , "FVM21")//型号
    Property_Var(QString , sContainerTypeChip , "P") //PCBA 产品代码
    Property_Var(QString , sContainerTypeTray , "H")//托盘产品代码
};

// 系统设置
class PARAMMANAGER_EXPORT SystemParam : public QObject
{
    Q_OBJECT
public:
    Property_Var(QString , filepath , "")//配方路径
    QString      nextDeviceIp = "10.255.10.99";

    SerialComStruct serialComStruct[2];// 气密设备参数
    ScanCodeParam   produceCodeParam;  // 产品参数
    ScanCodeParam   trayCodeParam;     // 料盘参数
    DataStorage     dataStorage;       // 数据存储
    GripperAddrParam  gripperAddrParam[GripperCount];  //夹爪地址参数
    ShieldParam       paramShield;       //屏蔽参数

    MesParam          mesParam;          //MES参数

};


struct AritightTask
{
    int  station = 0;                    //测量工位 0
    int  errorCode = 0;                  //测试流程错误码 0：表示正常，没有错误
    bool result = true;                  //测量结果
    QString barCode = "";                //镜头码
    QString trayBarCode = "";                //镜头码
    QString errorMsg = "";               //NG原因
    int     indexTray= 0 ;               //上料托盘序号
    QDateTime startDateTime;             //开始时间  //2022-12-05 08:31:04
    QDateTime endDateTime ;              //结束时间
    QString workPwoNo = "";              //工单号

    int TestStage = 0;                   //阶段
    int pressValue = 0;                  //压力
    int leakageValue = 0;                //泄漏
};
Q_DECLARE_METATYPE(AritightTask)

enum MachineRunStatus
{
    NoHomed,      //未复位(急停)
    Homed,        //复位中
    Idle,         //空闲
    AutoRunning,  //自动模式
    Pause,        //暂停
};

#endif // PARAMDEF_H
