#include "WidgetMatrixCfg.h"
#include "ui_WidgetMatrixCfg.h"
#include "ParamManager.h"
#include "VisAppBus.h"
#include <QMessageBox>
#include "CMsgBox.h"

WidgetMatrixCfg::WidgetMatrixCfg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetMatrixCfg)
{
    ui->setupUi(this);
    InitTable();
}

WidgetMatrixCfg::~WidgetMatrixCfg()
{
    delete ui;
}

void WidgetMatrixCfg::InitTable()
{
	//创建一个特定的字体对象
	QFont customFont;
	customFont.setFamily("SimSun"); // 设置字体族
    customFont.setPointSize(15);             // 设置大小
    //ui->widget->setFont(customFont);
     this->setFont(customFont);
    //ui->tableView->verticalHeader()->setVisible(false);
    //ui->tableView->horizontalHeader()->setVisible(false);
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);	//设置选择一行
    //tableView->horizontalHeader()->setStretchLastSection(true);		//设置最后一列自适应
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //自适应列宽
    ui->tableView->verticalHeader()->setDefaultSectionSize(35);			//设置默认行高
	//ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);	//设置不可编辑
    QStringList headerList;
    headerList<< u8"平台名称" << u8"产品型号"<<u8"上位机"<<u8"装配序列号"
              <<u8"1条码固定字符"<<u8"1起始位置"<<u8"2条码固定字符"<<u8"2起始位置"
              <<u8"托盘1"<<u8"治具防错码1"<<u8"托盘2"<<u8"治具防错码2"
              <<u8"镜头水洗序列号"<<u8"镜头码防错" ;
    m_pModel = new QStandardItemModel(this);
    m_pModel->setHorizontalHeaderLabels(headerList);
    ui->tableView->setModel(m_pModel);
}

void WidgetMatrixCfg::on_btnNewMatrix_clicked()
{
    QString platform=ui->lineEdit_Platform->text();
    QString productName=ui->lineEdit_productName->text();
    QString productShow=ui->lineEdit_productShow->text();

    if(platform.isEmpty() || productName.isEmpty() || productShow.isEmpty())
    {
        QString str = u8"平台名称";
        if(platform.isEmpty())
            str = u8"平台名称";
        else if(productName.isEmpty())
            str = u8"产品型号";
        else if(productShow.isEmpty())
            str = u8"上位机";
        CMsgBox::showWarning(this,QString(u8"提示"),str+u8"输入为空，请重新输入",CMsgBox::Ok);
        return;
    }

    QString productNo=ui->lineEdit_ProductNo->text();

    QString barCodeLimit1=ui->lineEdit_BarCodeLimit1->text();
    QString barCodeStartPos1=ui->lineEdit_BarCodeStartPos1->text();
    QString barCodeLimit2=ui->lineEdit_BarCodeLimit2->text();
    QString barCodeStartPos2=ui->lineEdit_BarCodeStartPos2->text();

    QString productFamily1=ui->lineEdit_ProductFamily1->text();
    QString fixtureSn1=ui->lineEdit_FixtureSn1->text();
    QString productFamily2=ui->lineEdit_ProductFamily2->text();
    QString fixtureSn2=ui->lineEdit_FixtureSn2->text();

    QString lensProductNo=ui->lineEdit_LensProductNo->text();
    QString lensCodeEnable=ui->lineEdit_LensCodeEnable->text();

    int rowCount = m_pModel->rowCount();
    m_pModel->insertRow(rowCount);
    QStringList listValue;
    listValue << platform << productName << productShow << productNo
              << barCodeLimit1 << barCodeStartPos1<< barCodeLimit2 << barCodeStartPos2
              << productFamily1 << fixtureSn1<< productFamily2 << fixtureSn2
              << lensProductNo << lensCodeEnable;
    m_vecMatrix.push_back(listValue);
    for (int i = 0; i < listValue.size(); i++) {
        m_pModel->setData(m_pModel->index(rowCount, i), Qt::AlignCenter, Qt::TextAlignmentRole);
        m_pModel->setData(m_pModel->index(rowCount, i), listValue[i]);
    }
    ui->tableView->setCurrentIndex(m_pModel->index(rowCount, 0));
}

void WidgetMatrixCfg::on_btnDeleteMatrix_clicked()
{
    int curRow = ui->tableView->currentIndex().row();
    //判断是否为当前作业型号
	QString curText = m_pModel->item(curRow, 2)->text();
	QString curText1 = GlobalParam->recipeProduct.curProduct;
    if (curText == curText1)
    {
        QMessageBox::warning(nullptr, u8"警告", u8"不能删除正在作业的型号");
        return ;
    }

    m_pModel->removeRow(curRow);
    ui->tableView->setCurrentIndex(m_pModel->index(m_pModel->rowCount() - 1, 0));
    m_vecMatrix.remove(curRow);
}

int WidgetMatrixCfg::LoadMatrixFile()
{
    bool bRet = GlobalParam->LoadPlatformMatrix();
    ShowSystemLog(bRet ? Log_Info : Log_Error, QString(u8"产品Matrix文件加载%1！").arg(bRet ? u8"成功" : u8"失败"));
    UpdateParamToUI();
    return 0;
}

void WidgetMatrixCfg::LoadUIParam()
{

}

void WidgetMatrixCfg::SaveUIParam()
{
    GlobalParam->recipeProduct.listPlatformMatrix.clear();
    m_vecMatrix.clear();
    for (int i = 0; i < m_pModel->rowCount(); i++) {
        QStringList listValue;
        for (int j = 0; j < m_pModel->columnCount(); j++) {
            listValue << m_pModel->data(m_pModel->index(i, j)).toString();
        }
        m_vecMatrix.push_back(listValue);
    }
    for (int i = 0; i < m_vecMatrix.size(); i++) {
        QStringList listValue = m_vecMatrix[i];
        MatrixSetting matrixSetting;
        matrixSetting.platformName = listValue[0];
        matrixSetting.productName= listValue[1];
        matrixSetting.productShow = listValue[2];
        matrixSetting.productNo = listValue[3];

        matrixSetting.barCodeLimit1 = listValue[4];
        matrixSetting.barCodeStartPos1 = listValue[5];
        matrixSetting.barCodeLimit2 = listValue[6];
        matrixSetting.barCodeStartPos2 = listValue[7];

        matrixSetting.productFamily1 = listValue[8];
        matrixSetting.fixtureSn1 = listValue[9];
        matrixSetting.productFamily2 = listValue[10];
        matrixSetting.fixtureSn2 = listValue[11];

        matrixSetting.lensProductNo = listValue[12];
        matrixSetting.lensCodeEnable = listValue[13];
        GlobalParam->recipeProduct.listPlatformMatrix.push_back(matrixSetting);
    }
    GlobalParam->SavePlatformMatrix();
    emit sigAddPlatfrom();
    tagOutputInfo outInfo;
    outInfo._type = INFT_ProductChange;
    emit GlobalParam->frameCore->sig_OutputInfo(outInfo);
}

void WidgetMatrixCfg::UpdateParamToUI()
{
    m_vecMatrix.clear();
    QVector<MatrixSetting>& listPlatformMatrix = GlobalParam->recipeProduct.listPlatformMatrix;
    m_pModel->setRowCount(listPlatformMatrix.size());
    for (int i = 0; i < listPlatformMatrix.size(); i++) {
		MatrixSetting& matrixSetting = listPlatformMatrix[i];
		QStringList listValue;
        listValue << matrixSetting.platformName << matrixSetting.productName << matrixSetting.productShow << matrixSetting.productNo
                  << matrixSetting.barCodeLimit1 << matrixSetting.barCodeStartPos1<< matrixSetting.barCodeLimit2 << matrixSetting.barCodeStartPos2
                  << matrixSetting.productFamily1 << matrixSetting.fixtureSn1 << matrixSetting.productFamily2 << matrixSetting.fixtureSn2
                  << matrixSetting.lensProductNo << matrixSetting.lensCodeEnable;
		m_vecMatrix.push_back(listValue);
		for (int j = 0; j < listValue.size(); j++) {
			m_pModel->setData(m_pModel->index(i, j), Qt::AlignCenter, Qt::TextAlignmentRole);
			m_pModel->setData(m_pModel->index(i, j), listValue[j]);
		}
    }
    ui->tableView->setCurrentIndex(m_pModel->index(m_pModel->rowCount() - 1, 0));
}


