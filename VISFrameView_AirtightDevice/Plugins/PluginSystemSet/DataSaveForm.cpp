#include <QFileDialog>
#include <QString>
#include <QSettings>
#include <QApplication>
#include "DataSaveForm.h"
#include "ui_DataSaveForm.h"
#include "ParamManager.h"
#include "VisUIParam.h"
#include "../interface/coreinterface.h"

DataSaveForm::DataSaveForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataSaveForm)
{
    ui->setupUi(this);
}

DataSaveForm::~DataSaveForm()
{
    delete ui;
}

void DataSaveForm::LoadUIParam()
{
    QString sFilePath = GlobalParam->systemParam.filepath + "dataStorage.ini";
    VisUIParam::LoadIniToUI(sFilePath,this,&GlobalParam->systemParam.dataStorage);
}

void DataSaveForm::UpdateParamToUI()
{
    LoadUIParam();
}

void DataSaveForm::SaveUIParam()
{
    QString sFilePath = GlobalParam->systemParam.filepath + "dataStorage.ini";
    VisUIParam::SaveUIToIni(sFilePath,this,&GlobalParam->systemParam.dataStorage);
}

void DataSaveForm::on_PushButtonChosePath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(
                this,tr("选择目录"),qApp->applicationDirPath());
    if (!path.isEmpty()) {
        ui->LineEdit_sProductSavePath->setText(path);
    }
}
