#include "WidgetRecipeMotor.h"
#include "ui_WidgetRecipeMotor.h"
#include <QMessageBox>
#include <QSignalBlocker>
#include "VisMotorTool.h"
#include "VisMotorToolData.h"
#include "VisMotorManager.h"
#include "ParamManager.h"
#pragma execution_character_set("utf-8")

WidgetRecipeMotor::WidgetRecipeMotor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetRecipeMotor)
{
    ui->setupUi(this);

}

WidgetRecipeMotor::~WidgetRecipeMotor()
{
    delete ui;
}

void WidgetRecipeMotor::showEvent(QShowEvent *event)
{
    //初始化点位窗口
    QWidget* widgetPoint = VisMotorToolSpace::VisMotorToolIns->GetWidget(VisMotorToolSpace::VisMotorTool::F_PointDebugForm);
    widgetPoint->setStyleSheet(QString("font:%1pt;").arg(qApp->font().pointSize()));
    ui->groupBox->layout()->addWidget(widgetPoint);
}

void WidgetRecipeMotor::LoadUIParam()
{
    QString filename = GlobalParam->recipeMotor.filepath +"/MotorList.xml";
    GlobalParam->LoadRecipeList(filename, GlobalParam->recipeMotor.listRecipe, ui->comboBox_Recipe);

    ui->comboBox_Recipe->blockSignals(true);
    int index = GlobalParam->recipeMotor.listRecipe.indexOf(GlobalParam->recipeMotor.curRecipe);
    ui->comboBox_Recipe->setCurrentIndex(index);
    ui->comboBox_Recipe->blockSignals(false);

    RecipeMotor& recipeMotor = GlobalParam->recipeMotor;
    filename = recipeMotor.filepath + recipeMotor.curRecipe + ".xml";
    bool bRet = VisMotorToolSpace::VisMotorToolIns->SetPointFile(filename);
    if (false == bRet)
    {
        ShowSystemLog(Log_Error, QString(u8"伺服配方文件加载%1失败！").arg(filename));
    }
    ShowSystemLog(Log_Info, QString(u8"夹爪配方文件加载成功！"));
}

void WidgetRecipeMotor::SaveUIParam()
{

}

void WidgetRecipeMotor::UpdateParamToUI()
{

}
void WidgetRecipeMotor::on_btnSave_clicked()
{
	QString recipeName = ui->lineEdit->text();
	if (recipeName.isEmpty()) {
		QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能为空");
		return;
	}
	RecipeMotor& recipeMotor = GlobalParam->recipeMotor;
	if (recipeMotor.listRecipe.contains(recipeName)) {
		QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能重复");
		return;
	}
	ui->lineEdit->setText("");
	ui->comboBox_Recipe->blockSignals(true);
	ui->comboBox_Recipe->addItem(recipeName);
	ui->comboBox_Recipe->blockSignals(false);
	recipeMotor.listRecipe.push_back(recipeName);
	QString curFileName;
	if (!recipeMotor.curRecipe.isEmpty())
		curFileName = recipeMotor.filepath + recipeMotor.curRecipe + ".xml";
	else
		curFileName = GlobalParam->recipeProduct.productPath + "defaultMotor.xml";

	//判断当前文件是否存在
	QString appPath = QCoreApplication::applicationDirPath();
	QString sDefalutPosFilePath = appPath + "/Config/pos.xml";

	QFile file(curFileName);
	if (!file.exists()) {
		ShowSystemLog(Log_Error, QString(u8"当前配方文件不存在,自动拷贝默认文件！"));

		if (!QFile::exists(sDefalutPosFilePath)) {
			QMessageBox::information(this, u8"提示信息", u8"缺少默认位置配置文件pos.xml");
			return;
		}
		QFile::copy(sDefalutPosFilePath,curFileName); //自动生成一次当前文件
		curFileName = sDefalutPosFilePath;
	}

	QString newFileName = recipeMotor.filepath + recipeName + ".xml";
	QFile::copy(curFileName, newFileName);

	QString  filename = recipeMotor.filepath + "MotorList.xml";
	GlobalParam->SaveRecipeList(filename, recipeMotor.listRecipe);
}

void WidgetRecipeMotor::on_comboBox_Recipe_currentIndexChanged(const QString &arg1)
{
	QString filename = GlobalParam->recipeMotor.filepath + arg1 + ".xml";
    bool bRet = VisMotorToolSpace::VisMotorToolIns->SetPointFile(filename);
	if (false == bRet)
	{
		ShowSystemLog(Log_Error, QString(u8"伺服配方文件加载%1失败！").arg(filename));
	}
	return;
}
