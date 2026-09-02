#include "WidgetAritight.h"
#include "ui_WidgetAritight.h"
#include "ParamManager.h"
#include "VisUIParam.h"
#include <QMessageBox>
#include <QSignalBlocker>
#include "../interface/coreinterface.h"
#include "Item/AritightForm.h"
#include <QTextCodec>

WidgetAritight::WidgetAritight(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAritight)
{
    ui->setupUi(this);

    InitWidget();
}

WidgetAritight::~WidgetAritight()
{
    delete ui;
}

void WidgetAritight::InitWidget()
{
    for (int i = 0;i<StationCount;++i)
    {
        AritightForm *aritightForm = new AritightForm;
        ui->tabWidget->addTab(aritightForm,QString(u8"工位%1").arg(i+1));
        aritightForm->SetStation(i);
        m_aritightFormList.append(aritightForm);
    }
}

void WidgetAritight::LoadUIParam()
{
    QString filename = GlobalParam->recipeAirtight.filepath +"/Aritight.xml";
    GlobalParam->LoadRecipeList(filename, GlobalParam->recipeAirtight.listRecipe, ui->comboBox_sRecipeName);
    ui->comboBox_sRecipeName->blockSignals(true);
    int index = GlobalParam->recipeAirtight.listRecipe.indexOf(GlobalParam->recipeAirtight.curRecipe);
    ui->comboBox_sRecipeName->setCurrentIndex(index);
    ui->comboBox_sRecipeName->blockSignals(false);

    filename = GlobalParam->recipeAirtight.filepath + GlobalParam->recipeAirtight.curRecipe + ".ini";
    QSettings *config = new QSettings(filename, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));
    for(int i = 0;i<StationCount;++i)
    {
        config->beginGroup(QString("Station%1").arg(i+1));
        VisUIParam::LoadIniToUI(config, m_aritightFormList.at(i), &GlobalParam->recipeAirtight.airtightParam[i]);
        config->endGroup();

        VisAppBus::sendEvent("SelectAirtightTestProgram",i,GlobalParam->recipeAirtight.airtightParam[i].processId);
        VisAppBus::sendEvent("SelectAirtightParam",i,GlobalParam->recipeAirtight.airtightParam[i].paramId);
        VisAppBus::sendEvent("SetAirtightParam",i);
    }

    ShowSystemLog(Log_Info, QString(u8"气密配方文件加载成功！"));
}

void WidgetAritight::UpdateParamToUI()
{
    LoadUIParam();
}

void WidgetAritight::SaveUIParam()
{
    if(GlobalParam->recipeAirtight.curRecipe.isEmpty())
        return;
    QString filename = GlobalParam->recipeAirtight.filepath + ui->comboBox_sRecipeName->currentText() + ".ini";
    QSettings *config = new QSettings(filename, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));

    for(int i = 0;i<StationCount;++i)
    {
        config->beginGroup(QString("Station%1").arg(i+1));
        VisUIParam::SaveUIToIni(config, m_aritightFormList.at(i), &GlobalParam->recipeAirtight.airtightParam[i]);
        config->endGroup();

        VisAppBus::sendEvent("SelectAirtightTestProgram",i,GlobalParam->recipeAirtight.airtightParam[i].processId);
        VisAppBus::sendEvent("SelectAirtightParam",i,GlobalParam->recipeAirtight.airtightParam[i].paramId);
        VisAppBus::sendEvent("SetAirtightParam",i);
    }
}

bool WidgetAritight::CopyFile(const QString &src, const QString &dst)
{
    if(QFile::exists(dst))
        QFile::remove(dst);
    if(!QFile::copy(src, dst))
        return false;
    return true;
}


void WidgetAritight::on_btnSave_clicked()
{
    QString recipeName = ui->lineEdit->text();
    if (recipeName.isEmpty()) {
        QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能为空");
        return;
    }
    if (GlobalParam->recipeAirtight.listRecipe.contains(recipeName)) {
        QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能重复");
        return;
    }
    QString srcRecipeName = ui->comboBox_sRecipeName->currentText();
    ui->lineEdit->setText("");
    ui->comboBox_sRecipeName->addItem(recipeName);
    if(!GlobalParam->recipeAirtight.listRecipe.contains(recipeName))
        GlobalParam->recipeAirtight.listRecipe.push_back(recipeName);

    QString filename = GlobalParam->recipeAirtight.filepath + "/Aritight.xml";
    GlobalParam->SaveRecipeList(filename, GlobalParam->recipeAirtight.listRecipe);

    QString dstRecipeName = recipeName;
    QString srcRecipeNamePath = GlobalParam->recipeAirtight.filepath+srcRecipeName+".ini";
    QString dstRecipeNamePath = GlobalParam->recipeAirtight.filepath+dstRecipeName+".ini";

    //复制当前配方到新创建的配方
    CopyFile(srcRecipeNamePath,dstRecipeNamePath);
}

void WidgetAritight::on_comboBox_Recipe_currentIndexChanged(const QString &arg1)
{
    QString filename = GlobalParam->recipeAirtight.filepath + arg1 + ".ini";
    VisUIParam::LoadIniToUI(filename, this, &GlobalParam->recipeAirtight);
}
