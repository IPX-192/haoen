#include <QDir>
#include "PluginParam.h"
#include "ParamManager.h"
#include "VisAppBus.h"
#include <QApplication>

PluginParam::PluginParam()
{
	pluginID = "PluginParam";
	pluginVersion = "1.0.1";
	pluginAuther = "wangwei";
    pluginAuthority = OPERATOR;
	loadOrder = 0;
	QFont font("SimSun", 15); // 设置全局默认字体为Arial，大小为15
	font.setBold(false);
	qApp->setFont(font); // 应用全局字体设置
}

bool PluginParam::ConnectCore(QObject *core)
{
	VISFramePluginModel::ConnectCore(core);
    GlobalParam->frameCore=frameCore;
    return 0;
}

int PluginParam::OnCoreInitialized()
{
    GlobalParam->systemParam.filepath = frameCore->configDirPath + "System/";
    GlobalParam->recipeProduct.productPath = frameCore->configDirPath + "Product/";
    GlobalParam->recipeMotor.filepath = GlobalParam->recipeProduct.productPath + "Motor/";
    GlobalParam->recipeTray.filepath = GlobalParam->recipeProduct.productPath + "Tray/";
    GlobalParam->recipeAirtight.filepath = GlobalParam->recipeProduct.productPath + "Aritight/";
    GlobalParam->recipeElectricSaw.filepath = GlobalParam->recipeProduct.productPath + "RecipeElectricSaw/";

    QDir dir;
    dir.mkpath(GlobalParam->systemParam.filepath);
    dir.mkpath(GlobalParam->recipeProduct.productPath);
    dir.mkpath(GlobalParam->recipeMotor.filepath);
    dir.mkpath(GlobalParam->recipeTray.filepath);
    dir.mkpath(GlobalParam->recipeElectricSaw.filepath);
	dir.mkpath(GlobalParam->recipeAirtight.filepath);
    return 0;
}

void PluginParam::InitActionList(Plugin_Interface *plugin)
{

}

int PluginParam::OnInitialized()
{
	return 0;
}
