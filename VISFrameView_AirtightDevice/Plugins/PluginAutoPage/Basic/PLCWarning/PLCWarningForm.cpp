#include "PLCWarningForm.h"
#include "ui_PLCWarningForm.h"
#include <QDateTime>
#pragma execution_character_set("utf-8")
PLCWarningForm::PLCWarningForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PLCWarningForm)
{
    ui->setupUi(this);
	InitWnd();
}

PLCWarningForm::~PLCWarningForm()
{
    delete ui;
}

int PLCWarningForm::GetWarningCount()
{
	return m_pItemModelWarnInfo->rowCount();
}

void PLCWarningForm::AddWarningInfo(QString sInfo)
{
	int nFind = FindItem(sInfo);
	if (nFind == -1)
	{
		QStandardItem* pItemTime = new QStandardItem(QDateTime::currentDateTime().toString("HH:mm:ss"));
		QStandardItem* pItemInfo = new QStandardItem(sInfo);
		m_pItemModelWarnInfo->appendRow(QList<QStandardItem*>() << pItemTime << pItemInfo);
	}
	else
	{
		QStandardItem* pItemInfo = m_pItemModelWarnInfo->item(nFind, 0);
		pItemInfo->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
	}
	return;
}

void PLCWarningForm::ClearWarningInfo()
{
	m_pItemModelWarnInfo->clear();
}

void PLCWarningForm::InitWnd()
{
	InitTableWidget();
}

void PLCWarningForm::InitTableWidget()
{
	m_pItemModelWarnInfo = new QStandardItemModel();
	m_pItemModelWarnInfo->setColumnCount(2);
	m_pItemModelWarnInfo->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("时间"));
	m_pItemModelWarnInfo->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("报警信息"));
	ui->TableViewPlcWarningInfo->setModel(m_pItemModelWarnInfo);
	ui->TableViewPlcWarningInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->TableViewPlcWarningInfo->show();
	QFont font = ui->TableViewPlcWarningInfo->font();
	font.setPointSize(12); // 设置字体大小为12
	ui->TableViewPlcWarningInfo->setFont(font);
	ui->TableViewPlcWarningInfo->setStyleSheet("#TableViewPlcWarningInfo{color: rgb(255, 0, 0);}");
	return;
}

int PLCWarningForm::FindItem(QString sInfo)
{
	//遍历Table查找相同的报警信息
	int nRow = m_pItemModelWarnInfo->rowCount();
	for (int i = 0; i < nRow; i++)
	{
		QModelIndex index = m_pItemModelWarnInfo->index(i, 1);
		if (m_pItemModelWarnInfo->data(index).toString() == sInfo)
		{
			return i;
		}
	}
	return -1;
}
