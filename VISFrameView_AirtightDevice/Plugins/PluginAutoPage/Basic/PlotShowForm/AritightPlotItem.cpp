#include "AritightPlotItem.h"
#include "ui_AritightPlotItem.h"
#include "../../Common/Custom/CRealTimeCurve.h"
#include "ParamManager.h"
#include <QTableWidgetItem>

#define StageNum 8
AritightPlotItem::AritightPlotItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AritightPlotItem)
{
    ui->setupUi(this);
    InitWidget();
}

AritightPlotItem::~AritightPlotItem()
{
    delete ui;
}

void AritightPlotItem::SetTittle(int station,QString name)
{
    ui->groupBox->setTitle(name);
    m_station = station;
}

void AritightPlotItem::InitWidget()
{
    m_realTimePressCurve = new CRealTimeCurve();
    m_realTimeLeaksCurve = new CRealTimeCurve();
    ui->gridLayout_plot->addWidget(m_realTimePressCurve,0,0);
    ui->gridLayout_plot->addWidget(m_realTimeLeaksCurve,1,0);

    //初始化表格
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setRowCount(StageNum);

    for (int i = 0; i<StageNum;++i)
    {
        InsertTable(i);
    }
}

void AritightPlotItem::DrawAirtightPlot(int station,AritightTask data)
{
    if(m_station != station)return ;

    std::vector<double> pressdata,leaksdata;
    pressdata.push_back(data.pressValue);
    leaksdata.push_back(data.leakageValue);
    m_realTimePressCurve->UpdateData(pressdata);
    m_realTimeLeaksCurve->UpdateData(leaksdata);
    UpdateTable(data);
}

void AritightPlotItem::SetAirtightParam(int station)
{
    if(m_station != station)return ;

    QStringList pressCurveName;
    QStringList leaksCurveName;

    pressCurveName<<QStringLiteral("压力");
    leaksCurveName<<QStringLiteral("泄漏");

    QStringList pressunit;
    pressunit<<QStringLiteral("bar")<<QStringLiteral("mbar")<<QStringLiteral("PSI")
            <<QStringLiteral("Pa")<<QStringLiteral("Kpa")<<QStringLiteral("Mpa");
    QString pressName = pressunit.at(GlobalParam->recipeAirtight.airtightParam[station].PressUnit);

    QStringList leaksunit;
    leaksunit<<QStringLiteral("cm3/s")<<QStringLiteral("cm3/min")<<QStringLiteral("cm3/h")
            <<QStringLiteral("mm3/s");
    QString leakName = leaksunit.at(GlobalParam->recipeAirtight.airtightParam[station].LeakUnit);

    m_realTimePressCurve->Clear();
    m_realTimePressCurve->SetCurve(QStringLiteral("压力曲线"),pressCurveName,pressName,StageNum,9999);

    m_realTimeLeaksCurve->Clear();
    m_realTimeLeaksCurve->SetCurve(QStringLiteral("泄漏曲线"),leaksCurveName,leakName,StageNum,9999);
}
void AritightPlotItem::ClearPlot(int station)
{
    if(m_station != station)return;
    m_realTimeLeaksCurve->ClearData();
    m_realTimePressCurve->ClearData();
    // 清空内容 + 清空所有行（彻底重置）
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0); // 关键：清空行，避免残留
    m_tableRow = 0;
}

void AritightPlotItem::InsertTable(int row)
{
    // 辅助函数添加文本项
    auto addTextItem = [this, row](int col, QString value) {
        QTableWidgetItem *item = new QTableWidgetItem(value);
        item->setTextAlignment(Qt::AlignCenter);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, col, item);
    };

    // 添加各参数项
    addTextItem(1, "");
    addTextItem(2, "");
    addTextItem(3, "");
}

void AritightPlotItem::UpdateTable(AritightTask data)
{
    // 安全判断：如果列数不对，直接返回（防止崩溃）
    if(ui->tableWidget->columnCount() < 3)
        return;

    // 关键：插入新行，不存在就创建（不会崩溃）
    if(m_tableRow >= ui->tableWidget->rowCount())
    {
        ui->tableWidget->insertRow(m_tableRow);
    }

    // 给每一列创建 Item（防止空指针）
    auto setTableItem = [&](int row, int col, const QString& text)
    {
        QTableWidgetItem* item = ui->tableWidget->item(row, col);
        if(!item)
        {
            item = new QTableWidgetItem;
            ui->tableWidget->setItem(row, col, item);
        }
        item->setText(text);
    };
    // 赋值数据
    setTableItem(m_tableRow, 0, QString::number(data.TestStage));
    setTableItem(m_tableRow, 1, QString::number(data.pressValue));
    setTableItem(m_tableRow, 2, QString::number(data.leakageValue));

    m_tableRow++;
}
