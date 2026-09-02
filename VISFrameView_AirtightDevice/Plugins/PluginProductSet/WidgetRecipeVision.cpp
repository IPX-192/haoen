#include "WidgetRecipeVision.h"
#include "ui_WidgetRecipeVision.h"
#include <QMessageBox>
#include <QTextCodec>
#include "ParamManager.h"
#include "VisUIParam.h"
//#include "CameraDebugForm.h"
#include "./Basic/From/CameraDebugForm/WidgetDetectCam.h"
#pragma execution_character_set("utf-8")

WidgetRecipeVision::WidgetRecipeVision(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetRecipeVision)
{
    ui->setupUi(this);
    InitWidget();
}

WidgetRecipeVision::~WidgetRecipeVision()
{
    delete ui;
}

void WidgetRecipeVision::InitWidget()
{
    //mpCameraDebugForm = new CameraDebugForm(this);
    WidgetDetectCam*widget= new WidgetDetectCam(0,this);
    ui->tabWidget->addTab(widget,u8"1工位相机");
    widget= new WidgetDetectCam(1,this);
    ui->tabWidget->addTab(widget,u8"2工位相机");
}

void WidgetRecipeVision::LoadUIParam()
{
    RecipeVision& recipeVison = GlobalParam->recipeVison;
    QString filename = recipeVison.filepath + "Vision.xml";
    GlobalParam->LoadRecipeList(filename, recipeVison.listRecipe, ui->comboBox_Recipe);
    LoadRecipeFile();
   //m_widgetVisionUp->InitUIParam();
   //m_widgetVisionDown->InitUIParam();
}

void WidgetRecipeVision::UpdateParamToUI()
{
	for (int i = 0; i < ui->tabWidget->count(); i++) {
        QMetaObject::invokeMethod(ui->tabWidget->widget(i), "UpdateParamToUI");
	}
}

void WidgetRecipeVision::SaveUIParam()
{
	QString filename = GlobalParam->recipeVison.filepath + GlobalParam->recipeVison.curRecipe + ".ini";
	QSettings setting(filename, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("UTF-8"));
	for (int i = 0; i < ui->tabWidget->count(); i++) {
		QMetaObject::invokeMethod(ui->tabWidget->widget(i), "SaveUIParam", Q_ARG(QSettings*, &setting));
	}
}

void WidgetRecipeVision::LoadRecipeFile()
{
    QString filename = GlobalParam->recipeVison.filepath + GlobalParam->recipeVison.curRecipe + ".ini";
	QSettings setting(filename, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("UTF-8"));
    for(int i=0;i<ui->tabWidget->count();i++){
        WidgetDetectCam*widget=(WidgetDetectCam*)ui->tabWidget->widget(i);
        widget->LoadUIParam(&setting);
    }

    ui->comboBox_Recipe->blockSignals(true);
    int index = GlobalParam->recipeVison.listRecipe.indexOf(GlobalParam->recipeVison.curRecipe);
    ui->comboBox_Recipe->setCurrentIndex(index);
    ui->comboBox_Recipe->blockSignals(false);
    UpdateParamToUI();
}

void WidgetRecipeVision::on_btnSaveAs_clicked()
{
    QString recipeName = ui->LineEditSaveAsName->text();
    if (recipeName.isEmpty()) {
        QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能为空");
        return;
    }
    if (GlobalParam->recipeVison.listRecipe.contains(recipeName)) {
        QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能重复");
        return;
    }
    ui->LineEditSaveAsName->setText("");
    ui->comboBox_Recipe->addItem(recipeName);
    GlobalParam->recipeVison.listRecipe.push_back(recipeName);
    QString filename = GlobalParam->recipeVison.filepath + "Vision.xml";
    GlobalParam->SaveRecipeList(filename, GlobalParam->recipeVison.listRecipe);

    filename = GlobalParam->recipeVison.filepath + recipeName + ".ini";
   
    QSettings setting(filename, QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("UTF-8"));
	for (int i = 0; i < ui->tabWidget->count(); i++) {
		QMetaObject::invokeMethod(ui->tabWidget->widget(i), "SaveUIParam", Q_ARG(QSettings*, &setting));
	}
    
	//VisUIParam::SaveUIToIni(filename, this, &recipeVison);
}
