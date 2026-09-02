#include "WidgetProductChange.h"
#include "ui_WidgetProductChange.h"
#include "ParamManager.h"
#include <QDir>
#include <QPushButton>
#include <QMessageBox>
#include <QTextCodec>
#include "VisMotorToolData.h"

WidgetProductChange::WidgetProductChange(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetProductChange)
{
    ui->setupUi(this);
    connect(GlobalParam->frameCore, &QVisF_Interface::sig_OutputInfo, this, &WidgetProductChange::slot_InputInfo);
}

WidgetProductChange::~WidgetProductChange()
{
    delete ui;
}

void WidgetProductChange::slot_InputInfo(tagOutputInfo &inputInfo)
{
    if (inputInfo._type == INFT_ProductChange) {
        ui->comboBox_SelectProduct->blockSignals(true);
        //产品型号发生变化，更新UI
        ui->comboBox_SelectProduct->clear();
        QVector<MatrixSetting>& listPlatformMatrix=GlobalParam->recipeProduct.listPlatformMatrix;  //平台列表映射
        for (int i=0;i<listPlatformMatrix.size();i++) {
            ui->comboBox_SelectProduct->addItem(listPlatformMatrix[i].productShow,listPlatformMatrix[i].productName);
            if(listPlatformMatrix[i].productShow==GlobalParam->recipeProduct.curProduct)
                GlobalParam->recipeProduct.curMatrix=&listPlatformMatrix[i];
        }
        if (listPlatformMatrix.size() > 0) {
            ui->comboBox_SelectProduct->setCurrentText(GlobalParam->recipeProduct.curProduct);
        }
        ui->comboBox_SelectProduct->blockSignals(false);

        GlobalParam->frameCore->systemName = QString(u8"机种型号：%1  成品料号：%2")
            .arg(GlobalParam->recipeProduct.curProduct).arg(GlobalParam->recipeProduct.curMatrix->productNo);
    }
}

void WidgetProductChange::on_pushButton_clicked()
{
    QString curProductName=ui->comboBox_SelectProduct->currentData().toString();
    QVector<ProductMatrix>& listRecipe = GlobalParam->recipeProduct.listRecipe;
    for(int i=0;i<listRecipe.size();i++){
        if(listRecipe[i].productName == curProductName){
            GlobalParam->recipeMotor.curRecipe=listRecipe[i].recipeMotor;
            GlobalParam->recipeTray.curRecipe = listRecipe[i].recipeTray;
            GlobalParam->recipeElectricSaw.curRecipe = listRecipe[i].recipeElectricSaw;
            GlobalParam->recipeAirtight.curRecipe = listRecipe[i].recipeAirtight;
            break;
        }
    }
    QVector<MatrixSetting>& listPlatformMatrix = GlobalParam->recipeProduct.listPlatformMatrix;
    for(int i=0;i<listPlatformMatrix.size();i++){
        if(listPlatformMatrix[i].productName == curProductName){
            GlobalParam->recipeProduct.curMatrix = &listPlatformMatrix[i];
            break;
        }
    }

    GlobalParam->recipeProduct.curProduct = ui->comboBox_SelectProduct->currentText();
   // GlobalParam->LoadRecipeProduct(GlobalParam->recipeProduct.curProduct);
    GlobalParam->SaveRecipeProduct();
    tagOutputInfo outInfo;
    outInfo._type = INFT_ProductChange;
    this->blockSignals(true);
    emit GlobalParam->frameCore->sig_OutputInfo(outInfo);
    this->blockSignals(false);
    VisAppBus::sendEvent("InitPlcData");
    return;
}

void WidgetProductChange::SaveChangeInfo()
{
    GlobalParam->frameCore->applicationDirPath;
    //QSettings *settings = new QSettings("");
}
