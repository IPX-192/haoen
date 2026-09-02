#ifndef PARAMMANAGER_H
#define PARAMMANAGER_H
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QComboBox>
#include "../../interface/singleton.h"
#include "../../interface/coreinterface.h"
#include "../../interface/plugininterface.h"
#include "ParamDef.h"

#define  GlobalParam ParamManager::instance()

//全局参数管理类
class PARAMMANAGER_EXPORT ParamManager : public QObject
{
    Q_OBJECT
    SINGLETON_DECL(ParamManager)
public:
    explicit ParamManager(QObject *parent = nullptr);

public:
    bool             LoadPlatformMatrix();  //读取平台映射
    bool             SavePlatformMatrix();
    bool             LoadRecipeProduct(QString recipeName = "");   //读取产品配方
    bool             SaveRecipeProduct();   //保存产品配方

    bool             SaveRecipeList(QString filename, QStringList listValue);   //保存配方列表
    bool             LoadRecipeList(QString filename, QStringList& listValue);  //读取配方列表
    bool             LoadRecipeList(QString filename,QStringList& listValue, QComboBox* combox);  //读取配方列表

    void             SetMachineStatus(MachineRunStatus  status);

public:
    QVisF_Interface*  frameCore = nullptr;
    bool              flagOffline = false;
    bool              AutoRunning = false;
    MachineRunStatus  devStatus;         //设备状态
    RecipeProduct     recipeProduct;
    RecipeMotor       recipeMotor;
    RecipeTray        recipeTray;
    RecipeAirtight    recipeAirtight;
    RecipeElectricSaw recipeElectricSaw; //电动夹爪配方

    /***************系统参数********************/

    SystemParam       systemParam;       //系统参数
signals:

public slots:

};


#endif // PARAMMANAGER_H
