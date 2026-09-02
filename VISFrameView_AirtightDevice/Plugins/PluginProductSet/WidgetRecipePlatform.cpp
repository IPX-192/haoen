#include "WidgetRecipePlatform.h"
#include "ui_WidgetRecipePlatform.h"
#include "ItemDelegate.h"
#include "ParamManager.h"
#include "VisAppBus.h"
#include "VisUIParam.h"

WidgetRecipePlatform::WidgetRecipePlatform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetRecipePlatform)
{
    ui->setupUi(this);
    InitTable();
}

WidgetRecipePlatform::~WidgetRecipePlatform()
{
    delete ui;
}

void WidgetRecipePlatform::InitTable()
{
    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);	//设置选择一行
    //tableView->horizontalHeader()->setStretchLastSection(true);		//设置最后一列自适应
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //自适应列宽
    ui->tableView->verticalHeader()->setDefaultSectionSize(35);			//设置默认行高

    QStringList headerList;
    headerList<< u8"产品名称"<< u8"伺服配方"<<u8"托盘配方"<<u8"电爪配方"<<u8"气密配方";
    m_pModel = new QStandardItemModel(this);
    m_pModel->setHorizontalHeaderLabels(headerList);
    ui->tableView->setModel(m_pModel);
	ItemDelegate* itemDelegate = new ItemDelegate(this);
    ui->tableView->setItemDelegate(itemDelegate);
}

void WidgetRecipePlatform::LoadUIParam()
{
    bool bRet = GlobalParam->LoadRecipeProduct();
	if (false == bRet)
	{
		ShowSystemLog(Log_Error, QString(u8"产品配方文件加载失败！"));
	}
	UpdateParamToUI();
}

void WidgetRecipePlatform::SaveUIParam()
{
	GlobalParam->recipeProduct.listRecipe.clear();
    for (int i = 0; i < m_pModel->rowCount(); i++) {
        ProductMatrix matrix;
		matrix.productName = m_pModel->data(m_pModel->index(i, 0)).toString();
        matrix.recipeMotor = m_pModel->data(m_pModel->index(i, 1)).toString();
        matrix.recipeTray = m_pModel->data(m_pModel->index(i, 2)).toString();
        matrix.recipeElectricSaw = m_pModel->data(m_pModel->index(i, 3)).toString();
        matrix.recipeAirtight = m_pModel->data(m_pModel->index(i, 4)).toString();
        GlobalParam->recipeProduct.listRecipe.push_back(matrix);
    }
	bool bRet = GlobalParam->SaveRecipeProduct();
    tagOutputInfo outInfo;
    outInfo._type = INFT_ProductChange;
    emit GlobalParam->frameCore->sig_OutputInfo(outInfo);
	ShowSystemLog(bRet ? Log_Info : Log_Error, QString(u8"产品配方文件保存%1！").arg(bRet ? u8"成功" : u8"失败"));
}

void WidgetRecipePlatform::UpdateParamToUI()
{
	QVector<MatrixSetting>& listPlatformMatrix = GlobalParam->recipeProduct.listPlatformMatrix;
	QVector<ProductMatrix>& listOldRecipe = GlobalParam->recipeProduct.listRecipe;
	m_pModel->setRowCount(listPlatformMatrix.size());

	QVector<ProductMatrix> listNewRecipe;
	for (int i = 0; i < listPlatformMatrix.size(); i++) {
		ProductMatrix productMatrix;
		productMatrix.productName = listPlatformMatrix[i].productName;
		for (int j = 0; j < listOldRecipe.size(); j++) {
			if (listOldRecipe[j].productName == productMatrix.productName) {
				productMatrix = listOldRecipe[j];
			}
		}
		listNewRecipe.push_back(productMatrix);
	}

	GlobalParam->recipeProduct.listRecipe = listNewRecipe;
	for (int i = 0; i < listNewRecipe.size(); i++) {
		ProductMatrix& productMatrix = listNewRecipe[i];
		m_pModel->setData(m_pModel->index(i, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
		m_pModel->setData(m_pModel->index(i, 0), productMatrix.productName);

        m_pModel->setData(m_pModel->index(i, 1), Qt::AlignCenter, Qt::TextAlignmentRole);
        m_pModel->setData(m_pModel->index(i, 1), productMatrix.recipeMotor);
        m_pModel->setData(m_pModel->index(i, 1), ComboBoxDelegate, ItemType);

        m_pModel->setData(m_pModel->index(i, 2), Qt::AlignCenter, Qt::TextAlignmentRole);
        m_pModel->setData(m_pModel->index(i, 2), productMatrix.recipeTray);
        m_pModel->setData(m_pModel->index(i, 2), ComboBoxDelegate, ItemType);

        m_pModel->setData(m_pModel->index(i, 3), Qt::AlignCenter, Qt::TextAlignmentRole);
        m_pModel->setData(m_pModel->index(i, 3), productMatrix.recipeElectricSaw);
        m_pModel->setData(m_pModel->index(i, 3), ComboBoxDelegate, ItemType);

        m_pModel->setData(m_pModel->index(i, 4), Qt::AlignCenter, Qt::TextAlignmentRole);
        m_pModel->setData(m_pModel->index(i, 4), productMatrix.recipeAirtight);
        m_pModel->setData(m_pModel->index(i, 4), ComboBoxDelegate, ItemType);
    }
}
