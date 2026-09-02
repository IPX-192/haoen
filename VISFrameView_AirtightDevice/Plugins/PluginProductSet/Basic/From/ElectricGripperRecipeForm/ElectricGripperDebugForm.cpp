#include "ElectricGripperDebugForm.h"
#include "ui_ElectricGripperDebugForm.h"
#include "VisMotorManager.h"
#include "ParamManager.h"
#include <QMessageBox>
#pragma execution_character_set("utf-8")
ElectricGripperDebugForm::ElectricGripperDebugForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ElectricGripperDebugForm)
{
    ui->setupUi(this);
}

ElectricGripperDebugForm::~ElectricGripperDebugForm()
{
    delete ui;
}

void ElectricGripperDebugForm::SetPointer(ElectricSawModuleParam* pElectricSawModuleParam)
{
    mpElectricSawModuleParam = pElectricSawModuleParam;
    return;
}

void ElectricGripperDebugForm::on_PushButtonRead_clicked()
{
//    ReadParam(mpElectricSawModuleParam->sGripperClampingForceAddr,ui->lineEdit_nGripperClampingForce);
//    ReadParam(mpElectricSawModuleParam->sClampingSpeedAddr, ui->lineEdit_nClampingSpeed);
//    ReadParam(mpElectricSawModuleParam->sClampingHomeSpeedAddr, ui->lineEdit_nClampingHomeSpeed);
//    ReadParam(mpElectricSawModuleParam->sRotateSpeedAddr, ui->lineEdit_nRotateSpeed);
//    ReadParam(mpElectricSawModuleParam->sRotateHomeSpeedAddr, ui->lineEdit_nRotateHomeSpeed);
//    ReadParam(mpElectricSawModuleParam->sRotateForceAddr, ui->lineEdit_nRotateForce);
//    ReadParam(mpElectricSawModuleParam->sClampPosAddr, ui->LineEdit_nClampPos);
//    ReadParam(mpElectricSawModuleParam->sRotateAngleAddr, ui->LineEdit_nRotateAngle);
//    ReadParam(mpElectricSawModuleParam->sClampHomePosAddr, ui->LineEdit_nClampHomePos);
//    ReadParam(mpElectricSawModuleParam->sRotateHomeAngleAddr, ui->LineEdit_nRotateHomeAngle);
//    ReadParam(mpElectricSawModuleParam->sClempCurrentPosAddr, ui->lineEdit_nGripperCurrentPos);
//    return;
}

void ElectricGripperDebugForm::on_PushButtonWrite_clicked()
{
//    WriteParam(mpElectricSawModuleParam->sGripperClampingForceAddr, ui->lineEdit_nGripperClampingForce);
//    WriteParam(mpElectricSawModuleParam->sClampingSpeedAddr, ui->lineEdit_nClampingSpeed);
//    WriteParam(mpElectricSawModuleParam->sClampingHomeSpeedAddr, ui->lineEdit_nClampingHomeSpeed);
//    WriteParam(mpElectricSawModuleParam->sRotateSpeedAddr, ui->lineEdit_nRotateSpeed);
//    WriteParam(mpElectricSawModuleParam->sRotateHomeSpeedAddr, ui->lineEdit_nRotateHomeSpeed);
//    WriteParam(mpElectricSawModuleParam->sRotateForceAddr, ui->lineEdit_nRotateForce);
//    WriteParam(mpElectricSawModuleParam->sClampPosAddr, ui->LineEdit_nClampPos);
//    WriteParam(mpElectricSawModuleParam->sRotateAngleAddr, ui->LineEdit_nRotateAngle);
//    WriteParam(mpElectricSawModuleParam->sClampHomePosAddr, ui->LineEdit_nClampHomePos);
//    WriteParam(mpElectricSawModuleParam->sRotateHomeAngleAddr, ui->LineEdit_nRotateHomeAngle);
}

void ElectricGripperDebugForm::on_PushButtonClampGroupRelease_clicked()
{
//    QString sButtonText = ui->PushButtonClampGroupRelease->text();
//    if (sButtonText == QString::fromUtf8("关闭"))
//    {
//        VisMotorInstance->WriteML(mpElectricSawModuleParam->sClampReleaseAddr,false);
//        QThread::msleep(1000);
//        VisMotorInstance->WriteML(mpElectricSawModuleParam->sClampAddr, true);
//        ui->PushButtonClampGroupRelease->setText(QString::fromUtf8("打开"));
//    }
//    else
//    {
//        VisMotorInstance->WriteML(mpElectricSawModuleParam->sClampAddr,false);
//        QThread::msleep(1000);
//        VisMotorInstance->WriteML(mpElectricSawModuleParam->sClampReleaseAddr,true);
//        ui->PushButtonClampGroupRelease->setText(QString::fromUtf8("关闭"));
//    }
//    return;
}

void ElectricGripperDebugForm::ReadParam(QString sAddr, QLineEdit* pLineEdit)
{
    qint32 nValue = -1;
    if (false == VisMotorInstance->ReadD(sAddr, nValue,ePlcDOne))
    {
        QMessageBox::information(this,QString::fromUtf8("提示"), sAddr + QString::fromUtf8("读失败"));
    }
    pLineEdit->setText(QString::number(nValue));
    return;
}

void ElectricGripperDebugForm::WriteParam(QString sAddr, QLineEdit* pLineEdit)
{
    qint32 nValue = pLineEdit->text().toInt();
    if (false == VisMotorInstance->WriteD(sAddr, nValue,ePlcDOne))
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), sAddr + QString::fromUtf8("写失败"));
    }
    return;
}
