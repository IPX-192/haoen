#include "CKDClampDebugForm.h"
#include "ui_CKDClampDebugForm.h"
#include "VisMotorManager.h"
#include "ParamManager.h"
#include <QMessageBox>
#pragma execution_character_set("utf-8")
CKDClampDebugForm::CKDClampDebugForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CKDClampDebugForm)
{
    ui->setupUi(this);
    InitWidget();
}

CKDClampDebugForm::~CKDClampDebugForm()
{
    delete ui;
}

void CKDClampDebugForm::InitWidget()
{
}

void CKDClampDebugForm::SetPointer(CKDClampingAddrParam* pCKDClampingAddrParam)
{
    mpCKDClampingAddrParam = pCKDClampingAddrParam;
    return;
}
void CKDClampDebugForm::on_PushButtonRead_clicked()
{
//    ReadParam(mpCKDClampingAddrParam->sPressPrecentageAddr,ui->lineEdit_nPressPrecentage);
//    ReadParam(mpCKDClampingAddrParam->sPressSpeedAddr, ui->lineEdit_nPressSpeed);
//    ReadParam(mpCKDClampingAddrParam->sPressDistanceAddr, ui->lineEdit_nPressDistance);
//    ReadParam(mpCKDClampingAddrParam->sHomePosAddr, ui->lineEdit_dHomePos);
//    ReadParam(mpCKDClampingAddrParam->sHomeSpeedAddr, ui->lineEdit_dHomeSpeed);
//    ReadParam(mpCKDClampingAddrParam->sCurrentPosAddr, ui->lineEdit_nCurrentPos);
}

void CKDClampDebugForm::on_PushButtonWrite_clicked()
{
//    WriteParam(mpCKDClampingAddrParam->sPressPrecentageAddr, ui->lineEdit_nPressPrecentage);
//    WriteParam(mpCKDClampingAddrParam->sPressSpeedAddr, ui->lineEdit_nPressSpeed);
//    WriteParam(mpCKDClampingAddrParam->sPressDistanceAddr, ui->lineEdit_nPressDistance);
//    WriteParam(mpCKDClampingAddrParam->sHomePosAddr, ui->lineEdit_dHomePos);
//    WriteParam(mpCKDClampingAddrParam->sHomeSpeedAddr, ui->lineEdit_dHomeSpeed);
}

void CKDClampDebugForm::on_PushButtonClampGroupRelease_clicked()
{
//    QString sButtonText = ui->PushButtonClampGroupRelease->text();
//    if (sButtonText == QString::fromUtf8("关闭"))
//    {
//        VisMotorInstance->WriteML(mpCKDClampingAddrParam->sClampReleaseAddr, false);
//        QThread::msleep(1000);
//        VisMotorInstance->WriteML(mpCKDClampingAddrParam->sClampAddr, true);
//        ui->PushButtonClampGroupRelease->setText(QString::fromUtf8("打开"));
//    }
//    else
//    {
//        VisMotorInstance->WriteML(mpCKDClampingAddrParam->sClampAddr, false);
//        QThread::msleep(1000);
//        VisMotorInstance->WriteML(mpCKDClampingAddrParam->sClampReleaseAddr, true);
//        ui->PushButtonClampGroupRelease->setText(QString::fromUtf8("关闭"));
//    }
//    return;
}

void CKDClampDebugForm::ReadParam(QString sAddr, QLineEdit* pLineEdit)
{
    qint32 nValue = -1;
    if (false == VisMotorInstance->ReadD(sAddr, nValue, ePlcDOne))
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), sAddr + QString::fromUtf8("读失败"));
    }
    pLineEdit->setText(QString::number(nValue));
    return;
}

void CKDClampDebugForm::WriteParam(QString sAddr, QLineEdit* pLineEdit)
{
    qint32 nValue = pLineEdit->text().toInt();
    if (false == VisMotorInstance->WriteD(sAddr, nValue, ePlcDOne))
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), sAddr + QString::fromUtf8("写失败"));
    }
    return;
}
