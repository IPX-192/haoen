#include "MesParamForm.h"
#include "ui_MesParamForm.h"
#include "ParamManager.h"
#include "VisAppBus.h"
#include "VisUIParam.h"
#include "CMsgBox.h"
#include "MES/FreetechMesManage.h"

MesParamForm::MesParamForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MesParamForm)
{
    ui->setupUi(this);
    ui->PushButtonMesTest->setVisible(false);
    ui->PushButtonMesConnect->setVisible(false);
    ui->PushButtonMesDisConnect->setVisible(false);
}

MesParamForm::~MesParamForm()
{
    delete ui;
}

void MesParamForm::LoadUIParam()
{
    QString sFilePath = GlobalParam->systemParam.filepath + "MesParam.ini";
    VisUIParam::LoadIniToUI(sFilePath, this, &GlobalParam->systemParam.mesParam);
    InitMes();
}

void MesParamForm::UpdateParamToUI()
{
    QString sFilePath  = GlobalParam->systemParam.filepath + "MesParam.ini";
    VisUIParam::LoadIniToUI(sFilePath,this, &GlobalParam->systemParam.mesParam);
}

void MesParamForm::SaveUIParam()
{
    QString sFilePath = GlobalParam->systemParam.filepath + "MesParam.ini";
    VisUIParam::SaveUIToIni(sFilePath, this, &GlobalParam->systemParam.mesParam);
    InitMes();
}

void MesParamForm::InitMes()
{
    if(GlobalParam->systemParam.mesParam.mesEnable)
    {
        QString urlIp = GlobalParam->systemParam.mesParam.mesIp+":"+GlobalParam->systemParam.mesParam.mesPort;
        FreetechMes::getInstance()->setUrlIp(urlIp);
        TOKEN_IN inData;
        TOKEN_OUT outData;
        inData.CommunityId = 0;//社区标识
        inData.UserCode = GlobalParam->systemParam.mesParam.userName;// 登录名
        inData.PlainPWD = GlobalParam->systemParam.mesParam.userPassword;// 密码
        QString res = FreetechMes::getInstance()->GetToken(inData, outData);
        if(!res.isEmpty())
        {
            ShowSystemLog(Log_Error, QStringLiteral("MES初始化失败"));
            CMsgBox::showFatal(this,QStringLiteral("提示"),QStringLiteral("MES初始化失败,原因%1！！！").arg(res),CMsgBox::Ok);
        }
        else
            ShowSystemLog(Log_Info, QStringLiteral("MES初始化成功"));
    }
}

void MesParamForm::on_PushButtonMesConnect_clicked()
{
    ui->PushButtonMesConnect->setEnabled(false);
    QString sMesIp = ui->lineEdit_mesIp->text();
    int nMesPort = ui->lineEdit_mesPort->text().toInt();
    int nRet = VisAppBus::sendEvent("ConnectMes",sMesIp, nMesPort);
    ui->PushButtonMesConnect->setEnabled(true);
    if (0 != nRet)
    {
        CMsgBox::showWarning(this, u8"错误", u8"连接服务器失败,请检查服务器IP和端口是否正确");
    }
    else
    {
        CMsgBox::showInfo(this, u8"提示", u8"连接服务器成功");
    }
}

void MesParamForm::on_PushButtonMesDisConnect_clicked()
{
    int nRet = VisAppBus::sendEvent("DisConnectMes");
    if (0 != nRet)
    {
        CMsgBox::showWarning(this, u8"错误", u8"断开服务器失败");
    }
    else
    {
        CMsgBox::showInfo(this, u8"提示", u8"断开服务器成功");
    }
}

void MesParamForm::on_PushButtonMesTest_clicked()
{
    int nRet = VisAppBus::sendEvent("Mes_TestMesStr");
    if (0 != nRet)
    {
        CMsgBox::showWarning(this, u8"错误", u8"测试服务器失败");
    }
    else
    {
        CMsgBox::showInfo(this, u8"提示", u8"测试服务器成功");
    }
}
