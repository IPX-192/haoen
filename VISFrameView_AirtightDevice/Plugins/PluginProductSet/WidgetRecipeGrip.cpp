#include "WidgetRecipeGrip.h"
#include "ui_WidgetRecipeGrip.h"
#include "ParamManager.h"
#include "VisUIParam.h"
#include <QMessageBox>
#include <QSignalBlocker>
#include "../interface/coreinterface.h"

#pragma execution_character_set("utf-8")

WidgetRecipeGrip::WidgetRecipeGrip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetRecipeGrip)
{
    ui->setupUi(this);
}

WidgetRecipeGrip::~WidgetRecipeGrip()
{
    delete ui;
}

void WidgetRecipeGrip::LoadUIParam()
{
    QString filename = GlobalParam->recipeElectricSaw.filepath +"/RecipeElectricSaw.xml";
    GlobalParam->LoadRecipeList(filename, GlobalParam->recipeElectricSaw.listRecipe, ui->comboBox_sRecipeName);

    ui->comboBox_sRecipeName->blockSignals(true);
    int index = GlobalParam->recipeElectricSaw.listRecipe.indexOf(GlobalParam->recipeElectricSaw.curRecipe);
    ui->comboBox_sRecipeName->setCurrentIndex(index);
    ui->comboBox_sRecipeName->blockSignals(false);

    filename = GlobalParam->recipeElectricSaw.filepath + GlobalParam->recipeElectricSaw.curRecipe + ".ini";
    VisUIParam::LoadIniToUI(filename, this, &GlobalParam->recipeElectricSaw);

    ShowSystemLog(Log_Info, QString(u8"夹爪配方文件加载成功！"));
}

void WidgetRecipeGrip::UpdateParamToUI()
{
    LoadUIParam();
}

void WidgetRecipeGrip::SaveUIParam()
{
    QString filename = GlobalParam->recipeElectricSaw.filepath + ui->comboBox_sRecipeName->currentText() + ".ini";
    VisUIParam::SaveUIToIni(filename, this, &GlobalParam->recipeElectricSaw);
}

bool WidgetRecipeGrip::CopyFile(const QString &src, const QString &dst)
{
    if(QFile::exists(dst))
        QFile::remove(dst);
    if(!QFile::copy(src, dst))
        return false;
    return true;
}


void WidgetRecipeGrip::on_btnSave_clicked()
{
    QString recipeName = ui->lineEdit->text();
    if (recipeName.isEmpty()) {
        QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能为空");
        return;
    }
    if (GlobalParam->recipeElectricSaw.listRecipe.contains(recipeName)) {
        QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能重复");
        return;
    }
    QString srcRecipeName = ui->comboBox_sRecipeName->currentText();
    ui->lineEdit->setText("");
    ui->comboBox_sRecipeName->addItem(recipeName);
    if(!GlobalParam->recipeElectricSaw.listRecipe.contains(recipeName))
        GlobalParam->recipeElectricSaw.listRecipe.push_back(recipeName);

    QString filename = GlobalParam->recipeElectricSaw.filepath + "/RecipeElectricSaw.xml";
    GlobalParam->SaveRecipeList(filename, GlobalParam->recipeElectricSaw.listRecipe);

    QString dstRecipeName = recipeName;
    QString srcRecipeNamePath = GlobalParam->recipeElectricSaw.filepath+srcRecipeName+".ini";
    QString dstRecipeNamePath = GlobalParam->recipeElectricSaw.filepath+dstRecipeName+".ini";

    //复制当前配方到新创建的配方
    CopyFile(srcRecipeNamePath,dstRecipeNamePath);
}

void WidgetRecipeGrip::on_comboBox_sRecipeName_currentIndexChanged(const QString &arg1)
{
    QString filename = GlobalParam->recipeElectricSaw.filepath + arg1 + ".ini";
    VisUIParam::LoadIniToUI(filename, this, &GlobalParam->recipeElectricSaw);
}
