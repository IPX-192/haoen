#include "ScrewLockRecipeForm.h"
#include "ui_ScrewLockRecipeForm.h"
#include <QMessageBox>
#include <QSignalBlocker>
#include <QCoreApplication>
#include "CMsgBox.h"
#include "VisUIParam.h"
#include "VisMotorManager.h"
#include "OpenProtocolMtf6000.h"
#include "MachineStatus.h"
#pragma execution_character_set("utf-8")

ScrewLockRecipeForm::ScrewLockRecipeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScrewLockRecipeForm)
{
    ui->setupUi(this);
    InitWidget();
}

ScrewLockRecipeForm::~ScrewLockRecipeForm()
{
    delete ui;
}

void ScrewLockRecipeForm::InitWidget()
{
	m_pItemModelScrewLockPos = new QStandardItemModel();
	m_pItemModelScrewLockPos->setColumnCount(3);
	m_pItemModelScrewLockPos->setHeaderData(0, Qt::Horizontal, "X");
	m_pItemModelScrewLockPos->setHeaderData(1, Qt::Horizontal, "Y");
	m_pItemModelScrewLockPos->setHeaderData(2, Qt::Horizontal, "Z");

	ui->tableView_ScrewLockPos->setModel(m_pItemModelScrewLockPos);
	ui->tableView_ScrewLockPos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableView_ScrewLockPos->show();
	ui->tableView_ScrewLockPos->setColumnHidden(2, true);
    connect(ui->RadioButtonDriverBitOne,&QRadioButton::clicked,this,&ScrewLockRecipeForm::DriverBitChangeSlot);
	connect(ui->RadioButtonDriverBitTwo, &QRadioButton::clicked, this, &ScrewLockRecipeForm::DriverBitChangeSlot);
	connect(ui->RadioButtonDriverBitThree, &QRadioButton::clicked, this, &ScrewLockRecipeForm::DriverBitChangeSlot);
	connect(ui->RadioButtonDriverBitFour, &QRadioButton::clicked, this, &ScrewLockRecipeForm::DriverBitChangeSlot);

	connect(OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(1).data(), &OpenProtocolMtf6000::recvTighteningResultSignals,
		this, &ScrewLockRecipeForm::recvTighteningResultOneSlot);
	connect(OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(2).data(), &OpenProtocolMtf6000::recvTighteningResultSignals,
		this, &ScrewLockRecipeForm::recvTighteningResultTwoSlot);
	connect(OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(3).data(), &OpenProtocolMtf6000::recvTighteningResultSignals,
		this, &ScrewLockRecipeForm::recvTighteningResultThreeSlot);
	connect(OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(4).data(), &OpenProtocolMtf6000::recvTighteningResultSignals,
		this, &ScrewLockRecipeForm::recvTighteningResultFourSlot);
}

void ScrewLockRecipeForm::LoadUIParam()
{
	//加载参数
	QSignalBlocker blocker(ui->comboBox_Recipe);
	QString filepath = GlobalParam->recipeScrewLock.filepath + "/"
		+ GlobalParam->recipeScrewLock.curRecipe;
	LoadRecipeParam(filepath, &GlobalParam->recipeScrewLock);
	CopyParamToTemp(&GlobalParam->recipeScrewLock, &m_RecipeScrewLockTemp);
	UpdateParamToUI();

	filepath = GlobalParam->recipeScrewLock.filepath + "RecipeScrewLock.xml";
	GlobalParam->LoadRecipeList(filepath, GlobalParam->recipeScrewLock.listRecipe,
		ui->comboBox_Recipe);

	//更新当前选项到界面
	for (int i = 0;i != ui->comboBox_Recipe->count();i++)
	{
		if (GlobalParam->recipeScrewLock.curRecipe == ui->comboBox_Recipe->itemText(i))
		{
			ui->comboBox_Recipe->setCurrentIndex(i);
		}
	}
	InitPestChose();
	return;
}

void ScrewLockRecipeForm::SaveUIParam()
{
	QString recipeName = ui->comboBox_Recipe->currentText();
	QString filename = GlobalParam->recipeScrewLock.filepath + recipeName;
	SaveRecipeParam(filename);
	if (GlobalParam->recipeScrewLock.curRecipe == recipeName)
	{
		CopyParamToTemp(&m_RecipeScrewLockTemp, &GlobalParam->recipeScrewLock);
	}
	//InitPestChose();
	return;
}

void ScrewLockRecipeForm::UpdateParamToUI()
{
	QList<ScrewLockHolePos_T> listPos;
    if (ui->RadioButtonDriverBitOne->isChecked())
	{
		listPos = m_RecipeScrewLockTemp.screwLockParamOne.listHolePos;
        VisUIParam::UpdateParamToUI(&m_RecipeScrewLockTemp.screwLockParamOne, this);
		ui->widget_MtfParam->SetOpenProtocolMtf(OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(1));
	}
	if (ui->RadioButtonDriverBitTwo->isChecked())
	{
		listPos = m_RecipeScrewLockTemp.screwLockParamTwo.listHolePos;
        VisUIParam::UpdateParamToUI(&m_RecipeScrewLockTemp.screwLockParamTwo, this);
		ui->widget_MtfParam->SetOpenProtocolMtf(OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(2));
	}
	if (ui->RadioButtonDriverBitThree->isChecked())
	{
		listPos = m_RecipeScrewLockTemp.screwLockParamThree.listHolePos;
        VisUIParam::UpdateParamToUI(&m_RecipeScrewLockTemp.screwLockParamThree, this);
		ui->widget_MtfParam->SetOpenProtocolMtf(OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(3));
	}
	if (ui->RadioButtonDriverBitFour->isChecked())
	{
		listPos = m_RecipeScrewLockTemp.screwLockParamFour.listHolePos;
        VisUIParam::UpdateParamToUI(&m_RecipeScrewLockTemp.screwLockParamFour, this);
		ui->widget_MtfParam->SetOpenProtocolMtf(OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(4));
	}
	UpdateDataToTable(listPos);
	InitPestChose();
	return;
}

void ScrewLockRecipeForm::recvTighteningResultOneSlot(QByteArray sTighteningId, QString sResultInfo)
{
	ui->widget_MtfParam->AddTigheningInfo(1, sTighteningId.toInt(), sResultInfo);
}

void ScrewLockRecipeForm::recvTighteningResultTwoSlot(QByteArray sTighteningId, QString sResultInfo)
{
	ui->widget_MtfParam->AddTigheningInfo(2, sTighteningId.toInt(), sResultInfo);
}

void ScrewLockRecipeForm::recvTighteningResultThreeSlot(QByteArray sTighteningId, QString sResultInfo)
{
	ui->widget_MtfParam->AddTigheningInfo(3, sTighteningId.toInt(), sResultInfo);
}

void ScrewLockRecipeForm::recvTighteningResultFourSlot(QByteArray sTighteningId, QString sResultInfo)
{
	ui->widget_MtfParam->AddTigheningInfo(4, sTighteningId.toInt(), sResultInfo);
}

void ScrewLockRecipeForm::on_btnSave_clicked()
{
	QString recipeName = ui->lineEdit->text();
	if (recipeName.isEmpty()) {
		QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能为空");
		return;
	}
	if (GlobalParam->recipeScrewLock.listRecipe.contains(recipeName)) {
		QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能重复");
		return;
	}
	ui->lineEdit->setText("");
	ui->comboBox_Recipe->addItem(recipeName);
	GlobalParam->recipeScrewLock.listRecipe.push_back(recipeName);
	//GlobalParam->recipeScrewLock.listRecipe = m_RecipeScrewLockTemp.listRecipe;
	QString filename = GlobalParam->recipeScrewLock.filepath + "RecipeScrewLock.xml";
	GlobalParam->SaveRecipeList(filename, GlobalParam->recipeScrewLock.listRecipe);

	filename = GlobalParam->recipeScrewLock.filepath + recipeName;
	SaveRecipeParam(filename);
	return;
}

void ScrewLockRecipeForm::on_comboBox_Recipe_currentIndexChanged(const QString &arg1)
{
	m_RecipeScrewLockTemp.curRecipe = arg1;
	QString filename = GlobalParam->recipeScrewLock.filepath + arg1;
	LoadRecipeParam(filename,&m_RecipeScrewLockTemp);
	UpdateParamToUI();
}

void ScrewLockRecipeForm::LoadRecipeParam(QString sPath, RecipeScrewLock* pRecipe)
{
	QSettings settings(sPath, QSettings::IniFormat);
	settings.beginGroup("screwLockParamOne");
    VisUIParam::LoadIniToUI(&settings, this,
		&pRecipe->screwLockParamOne);
	settings.endGroup();

	settings.beginGroup("screwLockParamTwo");
    VisUIParam::LoadIniToUI(&settings, this,
		&pRecipe->screwLockParamTwo);
	settings.endGroup();

	settings.beginGroup("screwLockParamThree");
    VisUIParam::LoadIniToUI(&settings, this,
		&pRecipe->screwLockParamThree);
	settings.endGroup();

	settings.beginGroup("screwLockParamFour");
    VisUIParam::LoadIniToUI(&settings, this,
		&pRecipe->screwLockParamFour);
	settings.endGroup();
	pRecipe->LoadPosList(sPath);
}

void ScrewLockRecipeForm::SaveRecipeParam(QString sPath)
{
	QSettings settings(sPath, QSettings::IniFormat);
	if (ui->RadioButtonDriverBitOne->isChecked())
	{
		settings.beginGroup("screwLockParamOne");
        VisUIParam::SaveUIToIni(&settings, this,
			&m_RecipeScrewLockTemp.screwLockParamOne);
		settings.endGroup();
		m_RecipeScrewLockTemp.screwLockParamOne.listHolePos = GetTablePos();
	}

	if (ui->RadioButtonDriverBitTwo->isChecked())
	{
		settings.beginGroup("screwLockParamTwo");
        VisUIParam::SaveUIToIni(&settings, this,
			&m_RecipeScrewLockTemp.screwLockParamTwo);
		settings.endGroup();
		m_RecipeScrewLockTemp.screwLockParamTwo.listHolePos = GetTablePos();
	}
	if (ui->RadioButtonDriverBitThree->isChecked())
	{
		settings.beginGroup("screwLockParamThree");
        VisUIParam::SaveUIToIni(&settings, this,
			&m_RecipeScrewLockTemp.screwLockParamThree);
		settings.endGroup();
		m_RecipeScrewLockTemp.screwLockParamThree.listHolePos = GetTablePos();
	}

	if (ui->RadioButtonDriverBitFour->isChecked())
	{
		settings.beginGroup("screwLockParamFour");
        VisUIParam::SaveUIToIni(&settings, this,
			&m_RecipeScrewLockTemp.screwLockParamFour);
		settings.endGroup();
		m_RecipeScrewLockTemp.screwLockParamFour.listHolePos = GetTablePos();
	}
	m_RecipeScrewLockTemp.SavePosList(sPath);
}

void ScrewLockRecipeForm::CopyParamToTemp(RecipeScrewLock* pSrc, RecipeScrewLock* pDst)
{
	VisUIParam::QObjectCopy(&pSrc->screwLockParamOne,&pDst->screwLockParamOne);
	VisUIParam::QObjectCopy(&pSrc->screwLockParamTwo, &pDst->screwLockParamTwo);
	VisUIParam::QObjectCopy(&pSrc->screwLockParamThree, &pDst->screwLockParamThree);
	VisUIParam::QObjectCopy(&pSrc->screwLockParamFour, &pDst->screwLockParamFour);
	//pDst->listRecipe = pSrc->listRecipe;
	//pDst->curRecipe = pSrc->curRecipe;
	pDst->screwLockParamOne.listHolePos = pSrc->screwLockParamOne.listHolePos;
	pDst->screwLockParamTwo.listHolePos = pSrc->screwLockParamTwo.listHolePos;
	pDst->screwLockParamThree.listHolePos = pSrc->screwLockParamThree.listHolePos;
	pDst->screwLockParamFour.listHolePos = pSrc->screwLockParamFour.listHolePos;
}

int ScrewLockRecipeForm::GetCurrentStation()
{
	if (ui->RadioButtonDriverBitOne->isChecked())
	{
		return 1;
	}
	if (ui->RadioButtonDriverBitTwo->isChecked())
	{
		return 2;
	}
	if (ui->RadioButtonDriverBitThree->isChecked())
	{
		return 3;
	}
	if (ui->RadioButtonDriverBitFour->isChecked())
	{
		return 4;
	}
	return 0;
}

void ScrewLockRecipeForm::GetPos(int nStation, double& dPosX, double& dPosY, double& dPosZ)
{
	if (1 == nStation)
	{
		GetPosByAxisName(Axis10_Station1DriverBitX,dPosX);
		GetPosByAxisName(Axis0_MaterialMove1, dPosY);
		GetPosByAxisName(Axis11_Station1DriverBitZ, dPosZ);
	}
	else if (2 == nStation)
	{
		GetPosByAxisName(Axis12_Station2DriverBitX, dPosX);
		GetPosByAxisName(Axis1_MaterialMove2, dPosY);
		GetPosByAxisName(Axis13_Station2DriverBitZ, dPosZ);
	}
	else if (3 == nStation)
	{
		GetPosByAxisName(Axis14_Station3DriverBitX, dPosX);
		GetPosByAxisName(Axis2_MaterialMove3, dPosY);
		GetPosByAxisName(Axis15_Station3DriverBitZ, dPosZ);
	}
	else if (4 == nStation)
	{
		GetPosByAxisName(Axis16_Station4DriverBitX, dPosX);
		GetPosByAxisName(Axis3_MaterialMove4, dPosY);
		GetPosByAxisName(Axis17_Station4DriverBitZ, dPosZ);
	}
}

void ScrewLockRecipeForm::GetPosByAxisName(QString sAxisName, double& dPos)
{
	int nRet = VisMotorInstance->GetCurPos(sAxisName, dPos);
	if (nRet != 0) {
		QMessageBox::information(this, QString::fromUtf8("提示"),
			sAxisName + QString::fromUtf8(" 获取位置失败 ") + QString::number(nRet));
	}
}

void ScrewLockRecipeForm::MoveToPos(int nStation, double dPosX, double dPosY, double dPosZ)
{
	int nRet = 0;
	if (1 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis10_Station1DriverBitX, dPosX,false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis10_Station1DriverBitX + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
		nRet = VisMotorInstance->MotorMoveAbs(Axis0_MaterialMove1, dPosY, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis0_MaterialMove1 + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	else if (2 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis12_Station2DriverBitX, dPosX, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis12_Station2DriverBitX + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
		nRet = VisMotorInstance->MotorMoveAbs(Axis1_MaterialMove2, dPosY, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis1_MaterialMove2 + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	else if (3 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis14_Station3DriverBitX, dPosX, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis14_Station3DriverBitX + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
		nRet = VisMotorInstance->MotorMoveAbs(Axis2_MaterialMove3, dPosY, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis2_MaterialMove3 + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	else if (4 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis16_Station4DriverBitX, dPosX, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis16_Station4DriverBitX + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
		nRet = VisMotorInstance->MotorMoveAbs(Axis3_MaterialMove4, dPosY, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis3_MaterialMove4 + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	return;
}

void ScrewLockRecipeForm::MoveToTighteningH(int nStation, double dPosZ)
{
	int nRet = 0;
	if (1 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis11_Station1DriverBitZ, dPosZ, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis11_Station1DriverBitZ + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	else if (2 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis13_Station2DriverBitZ, dPosZ, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis13_Station2DriverBitZ + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	else if (3 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis15_Station3DriverBitZ, dPosZ, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis15_Station3DriverBitZ + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	else if (4 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis17_Station4DriverBitZ, dPosZ, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis17_Station4DriverBitZ + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	return;
}

void ScrewLockRecipeForm::MoveToSafeH(int nStation, double dPosZ)
{
	int nRet = 0;
	if (1 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis11_Station1DriverBitZ, dPosZ, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis11_Station1DriverBitZ + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	else if (2 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis13_Station2DriverBitZ, dPosZ, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis13_Station2DriverBitZ + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	else if (3 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis15_Station3DriverBitZ, dPosZ, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis15_Station3DriverBitZ + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	else if (4 == nStation)
	{
		nRet = VisMotorInstance->MotorMoveAbs(Axis17_Station4DriverBitZ, dPosZ, false);
		if (nRet != 0) {
			QMessageBox::information(this, QString::fromUtf8("提示"),
				Axis17_Station4DriverBitZ + QString::fromUtf8(" 移动位置失败 ") + QString::number(nRet));
			return;
		}
	}
	return;
}

bool ScrewLockRecipeForm::GetCurrentChosePos(ScrewLockHolePos_T& tScrewLockHolePos)
{
	QItemSelectionModel* pSelectmodel = ui->tableView_ScrewLockPos->selectionModel();
	int nSelectedIndex = pSelectmodel->currentIndex().row();
	if (0 > nSelectedIndex)
	{
		QMessageBox::information(this, QString::fromUtf8("提示"),
			QString::fromUtf8("请先选中某一行数据。"));
		return false;
	}

	tScrewLockHolePos.dX = m_pItemModelScrewLockPos->item(nSelectedIndex, 0)->text().toDouble();
	tScrewLockHolePos.dY = m_pItemModelScrewLockPos->item(nSelectedIndex, 1)->text().toDouble();
	//pos.dZ = m_pItemModelScrewLockPos->item(i, 2)->text().toDouble();
	tScrewLockHolePos.dZ = ui->lineEdit_dDriverBitWorkHeight->text().toDouble();
	return true;
}

void ScrewLockRecipeForm::InitPestChose()
{
	if (GlobalParam->recipeScrewLock.curRecipe !=
		ui->comboBox_Recipe->currentText())
	{
		return;
	}

	if (!MachineStatusIns.GetAtlasMtf6000OnLine())
	{
		return;
	}

	OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(1)->
		selectPset(m_RecipeScrewLockTemp.screwLockParamOne.nPestIndex);
	OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(2)->
		selectPset(m_RecipeScrewLockTemp.screwLockParamTwo.nPestIndex);
	OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(3)->
		selectPset(m_RecipeScrewLockTemp.screwLockParamThree.nPestIndex);
	OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(4)->
		selectPset(m_RecipeScrewLockTemp.screwLockParamFour.nPestIndex);
	return;
}

void ScrewLockRecipeForm::on_pushButton_Add_clicked()
{
	double dPosX = 0, dPosY = 0, dPosZ = 0;
	GetPos(GetCurrentStation(), dPosX, dPosY, dPosZ);
	int nRowCnt = m_pItemModelScrewLockPos->rowCount();
	m_pItemModelScrewLockPos->setItem(nRowCnt, 0,
		new QStandardItem(QString::number(dPosX)));
	m_pItemModelScrewLockPos->setItem(nRowCnt, 1,
		new QStandardItem(QString::number(dPosY)));
	m_pItemModelScrewLockPos->setItem(nRowCnt, 2,
		new QStandardItem(QString::number(dPosZ)));
	return;
}

void ScrewLockRecipeForm::on_pushButton_Del_clicked()
{
	QItemSelectionModel* pSelectmodel = ui->tableView_ScrewLockPos->selectionModel();
	int nSelectedIndex = pSelectmodel->currentIndex().row();
	if (0 > nSelectedIndex)
	{
		QMessageBox::information(this, QString::fromUtf8("提示"),
			QString::fromUtf8("请先选中某一行数据。"));
		return;
	}
    m_pItemModelScrewLockPos->removeRow(nSelectedIndex);
	return;
}

void ScrewLockRecipeForm::on_pushButton_MoveTo_clicked()
{
	ScrewLockHolePos_T tScrewLockHolePos;
	if (false == GetCurrentChosePos(tScrewLockHolePos))
	{
		return;
	}

	if (CMsgBox::Cancel == CMsgBox::showQuestion(this, QString::fromUtf8("询问"),
		QString::fromUtf8("是否运动到位置？"), CMsgBox::Ok | CMsgBox::Cancel))
	{
		return;
	}

	MoveToPos(GetCurrentStation(), tScrewLockHolePos.dX, tScrewLockHolePos.dY, tScrewLockHolePos.dZ);
	return;
}

void ScrewLockRecipeForm::on_pushButton_MoveToTighteningH_clicked()
{
	ScrewLockHolePos_T tScrewLockHolePos;
	if (false == GetCurrentChosePos(tScrewLockHolePos))
	{
		return;
	}

	if (CMsgBox::Cancel == CMsgBox::showQuestion(this, QString::fromUtf8("询问"),
		QString::fromUtf8("是否运动到位置？"), CMsgBox::Ok | CMsgBox::Cancel))
	{
		return;
	}
	MoveToTighteningH(GetCurrentStation(),tScrewLockHolePos.dZ);
	return;
}

void ScrewLockRecipeForm::on_pushButton_MoveToSafeH_clicked()
{
	ScrewLockHolePos_T tScrewLockHolePos;
	if (false == GetCurrentChosePos(tScrewLockHolePos))
	{
		return;
	}
	MoveToSafeH(GetCurrentStation(), 1);
	return;
}

void ScrewLockRecipeForm::on_pushButton_GetCurrentHeight_clicked()
{
	ui->textEdit_CurrentHeight->clear();
	GetHeightData();
	return;
}

void ScrewLockRecipeForm::DriverBitChangeSlot()
{
	UpdateParamToUI();
	return;
}

void ScrewLockRecipeForm::GetHeightData()
{
	//获取当前传感器位移高度
	qint32 nValue = 0;
	QString sRealAddr = "D1000"; //实时高度地址
	QString sBeforeDriverBitAddr = "D1001"; //打螺丝前位移高度地址
	QString sAfterDriverBitAddr = "D1002"; //打螺丝后位移高度地址
	QString sHeightOffestAddr = "D1003";//高度差

	for (int i = 0; i != 4; i++)
	{
		QString sAddr = sRealAddr + QString::number(i);
		VisMotorInstance->ReadD(sAddr, nValue, ePlcDOne);
		ui->textEdit_CurrentHeight->append(QString::fromUtf8("工位%1实时高度： ").arg(i+1)
			+ QString::number(qAbs(nValue * 0.001)) + "\n");
		sAddr = sBeforeDriverBitAddr + QString::number(i);
		VisMotorInstance->ReadD(sAddr, nValue, ePlcDOne);
		ui->textEdit_CurrentHeight->append(QString::fromUtf8("工位%1打螺丝前位移高度： ").arg(i + 1)
			+ QString::number(qAbs(nValue * 0.001)) + "\n");
		sAddr = sAfterDriverBitAddr + QString::number(i);
		VisMotorInstance->ReadD(sAddr, nValue, ePlcDOne);
		ui->textEdit_CurrentHeight->append(QString::fromUtf8("工位%1打螺丝后位移高度： ").arg(i + 1)
			+ QString::number(qAbs(nValue * 0.001)) + "\n");
		sAddr = sHeightOffestAddr + QString::number(i);
		VisMotorInstance->ReadD(sAddr, nValue, ePlcDOne);
		ui->textEdit_CurrentHeight->append(QString::fromUtf8("工位%1打螺丝后高度差： ").arg(i + 1)
			+ QString::number(qAbs(nValue * 0.001)) + "\n");
	}
	return;
}

void ScrewLockRecipeForm::UpdateDataToTable(QList<ScrewLockHolePos_T> listPos)
{
	int nRowCnt = m_pItemModelScrewLockPos->rowCount();
	for (int i = nRowCnt - 1; i > -1; i--)
	{
		m_pItemModelScrewLockPos->removeRow(i);
	}

	int nCount = listPos.size();
	for (int i = 0; i != nCount; i++)
	{
		m_pItemModelScrewLockPos->setItem(i, 0,
			new QStandardItem(QString::number(listPos.at(i).dX)));
		m_pItemModelScrewLockPos->setItem(i, 1,
			new QStandardItem(QString::number(listPos.at(i).dY)));
		m_pItemModelScrewLockPos->setItem(i, 2,
			new QStandardItem(QString::number(listPos.at(i).dZ)));
	}
	return;
}

QList<ScrewLockHolePos_T> ScrewLockRecipeForm::GetTablePos()
{
	int nRowCnt = m_pItemModelScrewLockPos->rowCount();
	QList<ScrewLockHolePos_T> listPos;
	for (int i = 0; i != nRowCnt; i++)
	{
		ScrewLockHolePos_T pos;
		pos.dX = m_pItemModelScrewLockPos->item(i, 0)->text().toDouble();
		pos.dY = m_pItemModelScrewLockPos->item(i, 1)->text().toDouble();
		//pos.dZ = m_pItemModelScrewLockPos->item(i, 2)->text().toDouble();
		pos.dZ = ui->lineEdit_dDriverBitWorkHeight->text().toDouble();
		listPos.push_back(pos);
	}
	return listPos;
}
