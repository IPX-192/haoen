#include "WidgetHardWareDelay.h"
#include "ui_WidgetHardWareDelay.h"
#include <QTextStream>
#include <QHeaderView>
#include <QFile>
#include <QMessageBox>
#include "VisAppBus.h"
#include "ParamManager.h"

WidgetHardWareDelay::WidgetHardWareDelay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetHardWareDelay)
{
    ui->setupUi(this);
    ui->tabWidget->setStyleSheet("QTabBar::tab { height: 26px; }");
    LoadConfig();
}

WidgetHardWareDelay::~WidgetHardWareDelay()
{
    delete ui;
}

void WidgetHardWareDelay::LoadConfig()
{
    QString filename=GlobalParam->systemParam.filepath + "HardWareDelay.csv";
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) return;
    QTextStream stream(&file);
    QTableView *tableView=nullptr;
    while (!stream.atEnd()) {
       QString line = stream.readLine();
       if (line.isEmpty())continue;
       QStringList values = line.split(',');
       // 处理每一行数据
       // ...
       if(values.size()<4)continue;
       if(values.at(0)==QString::fromLocal8Bit("name")) {
           QString name=values.at(1);
           QStringList listHeader; //列名
           listHeader << u8"类型" << values.at(2) << values.at(3) << values.at(2) + u8"延时"  << values.at(3) + u8"延时";
           tableView=new QTableView(ui->tabWidget);
           tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //自适应列宽
           tableView->verticalHeader()->setDefaultSectionSize(40);			//设置默认行高
         //  tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);	//设置不可编辑
           QStandardItemModel* pModel = new QStandardItemModel(this);
           pModel->setHorizontalHeaderLabels(listHeader);
           tableView->setModel(pModel);
           ui->tabWidget->addTab(tableView,name);
           m_pViewModel[name] = pModel;
       }
       else {
           if(tableView==nullptr)continue;
           QStandardItemModel* pModel=(QStandardItemModel*)tableView->model();
           pModel->appendRow(new QStandardItem());
           int rowTotal = pModel->rowCount() - 1;
           for(int i=0;i<values.size();i++){
               pModel->setData(pModel->index(rowTotal,i),values.at(i));
               pModel->item(rowTotal, i)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
           }
		   if (values.size()==5) {
			   pModel->setData(pModel->index(rowTotal, 3), values.at(3));
			   pModel->setData(pModel->index(rowTotal, 4), values.at(4));
		   }
		   else {
			   pModel->setData(pModel->index(rowTotal, 3), 0);
			   pModel->setData(pModel->index(rowTotal, 4), 0);
		   }
           pModel->item(rowTotal, 3)->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
           pModel->item(rowTotal, 4)->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
       }
    }
}

void WidgetHardWareDelay::SaveConfig()
{
    QString filename = GlobalParam->systemParam.filepath + "HardWareDelay.csv";
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
	QTextStream stream(&file);
    for (int i = 0; i < ui->tabWidget->count(); i++) {
		QTableView* tableView = (QTableView*)ui->tabWidget->widget(i);
		QStandardItemModel* pModel = (QStandardItemModel*)tableView->model();
		stream << "name" << "," << ui->tabWidget->tabText(i) << ","
			<< pModel->headerData(1, Qt::Horizontal).toString() << ","
			<< pModel->headerData(2, Qt::Horizontal).toString() << "\n";
		int rowTotal = pModel->rowCount();
		for (int j = 0; j < rowTotal; j++) {
			QString name = pModel->data(pModel->index(j, 0)).toString();
			QString reg1 = pModel->data(pModel->index(j, 1)).toString();
			QString reg2 = pModel->data(pModel->index(j, 2)).toString();
			QString value1 = QString::number(pModel->data(pModel->index(j, 3)).toInt());
			QString value2 = QString::number(pModel->data(pModel->index(j, 4)).toInt());
			stream << name <<"," << reg1 << "," << reg2 << "," << value1 << "," << value2 << "\n";
		}
		stream << "\n";
    }
	file.close();
}

void WidgetHardWareDelay::SaveUIParam()
{
	SaveConfig();
    QTableView* tableView = (QTableView*)ui->tabWidget->currentWidget();
    QStandardItemModel* pModel = (QStandardItemModel*)tableView->model();
    int rowTotal = pModel->rowCount();
    if (!rowTotal)return;
	for (int i = 0; i < 2; i++) {
		QString regName = pModel->data(pModel->index(0, 1 + i)).toString();
        QVector<int> data;
		for (int j = 0; j < rowTotal; j++) {
            data << pModel->data(pModel->index(j, 3 + i)).toUInt();
		}
		if (0 != VisAppBus::sendEvent("WritePagePLC", regName, rowTotal, data)) {
			QMessageBox::information(this, u8"提示", u8"PLC写入失败");
			return;
		}
	}
    QMessageBox::information(this, u8"提示", u8"PLC写入完毕");
}

void WidgetHardWareDelay::LoadUIParam()
{
    for (int k = 0; k < ui->tabWidget->count(); k++) {
        QTableView* tableView = (QTableView*)ui->tabWidget->widget(k);
		QStandardItemModel* pModel = (QStandardItemModel*)tableView->model();
		int rowTotal = pModel->rowCount();
		if (!rowTotal)return;

		for (int i = 0; i < 2; i++) {
			QString regName = pModel->data(pModel->index(0, 1 + i)).toString();
			QVector<int> data;
			int nRes = VisAppBus::sendEvent("ReadPagePLC", regName, rowTotal, data);
			if (0 != nRes) {
				//QMessageBox::information(this, u8"提示", u8"PLC读取失败");
				return;
			}
			for (int j = 0; j < rowTotal; j++) {
				pModel->setData(pModel->index(j, 3 + i), data.at(j));
			}
		}
	}
}

void WidgetHardWareDelay::UpdateParamToUI()
{
	QTableView* tableView = (QTableView*)ui->tabWidget->currentWidget();
	QStandardItemModel* pModel = (QStandardItemModel*)tableView->model();
	int rowTotal = pModel->rowCount();
	if (!rowTotal)return;

    for (int i = 0; i < 2; i++) {
        QString regName = pModel->data(pModel->index(0, 1 + i)).toString();
		QVector<int> data;
		int nRes = VisAppBus::sendEvent("ReadPagePLC", regName, rowTotal, data);
		if (0 != nRes) {
			QMessageBox::information(this, u8"提示", u8"PLC读取失败");
			return;
		}
		for (int j = 0; j < rowTotal; j++) {
            pModel->setData(pModel->index(j, 3 + i), data.at(j));
		}
    }
}
