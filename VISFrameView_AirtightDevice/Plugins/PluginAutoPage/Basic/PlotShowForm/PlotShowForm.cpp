#include "PlotShowForm.h"
#include "ui_PlotShowForm.h"
#include <QString>
#include <QRandomGenerator>
#include "MachineStatus.h"
#include "ParamManager.h"
#pragma execution_character_set("utf-8")
PlotShowForm::PlotShowForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotShowForm)
{
    ui->setupUi(this);
    InitWidget();
}

PlotShowForm::~PlotShowForm()
{
    delete ui;
}

void PlotShowForm::InitWidget()
{
    InitTableView();
    VisAppBus::subscibeEvent(this, "ShowTighteningInfoStr");
    QList<AdvancedCurveViewer*> plotList;
    plotList.append(ui->WidgetPlotOne);
    plotList.append(ui->WidgetPlotTwo);
    plotList.append(ui->WidgetPlotThree);
    plotList.append(ui->WidgetPlotFour);

    QStringList strList;
    strList.append(QString::fromUtf8("曲线0"));

    QVector<QVector<QPointF>> DataVector = generateDiverseCurves(strList.size());
    for (int i = 0;i != plotList.size();i++)
    {
        AdvancedCurveViewer*  pMyPlot = plotList.at(i);
        pMyPlot->setAxisLabels(QString::fromUtf8("角度(°)"), QString::fromUtf8("扭矩(Nm)"));
        pMyPlot->setTitle(QString::fromUtf8("工位") + QString::number(i + 1) +
                QString::fromUtf8(" PCBA码:1201000271D9P00080V10H"));
        //设置曲线
        for (int j = 0; j != strList.size();j++)
        {
            pMyPlot->setCurveData(j, DataVector.at(j), QString::fromUtf8("曲线") + QString::number(j));
        }
    }

    connect(&mTimer, &QTimer::timeout, this, &PlotShowForm::TimerOutSlot);
    return;
}

void PlotShowForm::InitTableView()
{
    mpItemModelDriveerBitInfo = new QStandardItemModel();
    mpItemModelDriveerBitInfo->setColumnCount(3);
    mpItemModelDriveerBitInfo->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("电批序号"));
    mpItemModelDriveerBitInfo->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("ID"));
    mpItemModelDriveerBitInfo->setHeaderData(2, Qt::Horizontal, QString::fromUtf8("信息"));
    ui->TableViewDriverBitInfo->setModel(mpItemModelDriveerBitInfo);
    ui->TableViewDriverBitInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->TableViewDriverBitInfo->show();
}

void PlotShowForm::recvAngleAndTorqueDataOneSlot(QByteArray sTighteningId, QVector<QPointF> lstCurve)
{
    UpdatePlotData(ui->WidgetPlotOne,sTighteningId,lstCurve);
}

void PlotShowForm::recvAngleAndTorqueDataTwoSlot(QByteArray sTighteningId, QVector<QPointF> lstCurve)
{
    UpdatePlotData(ui->WidgetPlotTwo,sTighteningId,lstCurve);
}

void PlotShowForm::recvAngleAndTorqueDataThreeSlot(QByteArray sTighteningId, QVector<QPointF> lstCurve)
{
    UpdatePlotData(ui->WidgetPlotThree,sTighteningId,lstCurve);
}

void PlotShowForm::recvAngleAndTorqueDataFourSlot(QByteArray sTighteningId, QVector<QPointF> lstCurve)
{
    UpdatePlotData(ui->WidgetPlotFour,sTighteningId,lstCurve);
}

void PlotShowForm::TimerOutSlot()
{
    QList<ScrewLockLineParam> AllLineParam = 
        ScrewLockParamManagerIns.GetScrewLockLineParamAll();

    QList<AdvancedCurveViewer*> plotList;
    plotList.append(ui->WidgetPlotOne);
    plotList.append(ui->WidgetPlotTwo);
    plotList.append(ui->WidgetPlotThree);
    plotList.append(ui->WidgetPlotFour);

    for (int i = 0; i != AllLineParam.size(); i++)
    {
        ScrewLockLineParam lineParam = AllLineParam.at(i);
        if (lineParam.nStation < 0 || lineParam.nStation >= plotList.size())
        {
            continue;
        }

        UpdatePlotData(plotList.at(lineParam.nStation),
            lineParam.mListCurveData);
        plotList.at(lineParam.nStation)->
            setTitle(QString::fromUtf8("工位") + QString::number(lineParam.nStation + 1) +
                QString::fromUtf8(" PCBA码:") + lineParam.sChipCode);
    }
    return;
}

int PlotShowForm::event_ShowTighteningInfo(int nIndex, int nTighteningID, QString sInfo)
{
    AddTigheningInfo(nIndex, nTighteningID,sInfo);
    return 0;
}

void PlotShowForm::showEvent(QShowEvent* event)
{
    mTimer.start(300);
    QWidget::showEvent(event);
}

void PlotShowForm::hideEvent(QHideEvent* event)
{
    mTimer.stop();
    QWidget::hideEvent(event);
}

void PlotShowForm::AddTigheningInfo(int nIndex, int nTighteningID, QString sInfo)
{
    //检查大小，超过200条清除所有数据
    int nCount = mpItemModelDriveerBitInfo->rowCount();
    if (nCount > 200)
    {
        mpItemModelDriveerBitInfo->removeRows(0, nCount - 100);
    }

    //查找有没有相同的数据
    for (int i = 0; i != mpItemModelDriveerBitInfo->rowCount(); i++)
    {
        int nIndexSearch = mpItemModelDriveerBitInfo->item(i, 0)->text().toInt();
        int nTighteningId = mpItemModelDriveerBitInfo->item(i, 1)->text().toInt();
        if (nIndexSearch == nIndex && nTighteningId == nTighteningID)
        {
            return;
        }
    }

    //在第一行插入新的数据
    mpItemModelDriveerBitInfo->insertRow(0);
    mpItemModelDriveerBitInfo->setItem(0, 0,
        new QStandardItem(QString::number(nIndex)));
    mpItemModelDriveerBitInfo->setItem(0, 1,
        new QStandardItem(QString::number(nTighteningID)));
    mpItemModelDriveerBitInfo->setItem(0, 2,
        new QStandardItem(sInfo));
    return;
}

QVector<QVector<QPointF>> PlotShowForm::generateDiverseCurves(int nCount)
{
    QVector<QVector<QPointF>> curves;
    QRandomGenerator* rg = QRandomGenerator::global();

    for (int i = 0; i < nCount; ++i) {
        QVector<QPointF> curve;
        int pointCount = 1000 + rg->bounded(2000); // 1000-3000个点
        double xRange = 10.0;
        double yBase = rg->bounded(50.0);

        // 选择曲线类型
        int curveType = 0;

        for (int j = 0; j < pointCount; ++j) {
            double x = j * (xRange / pointCount);
            double y = yBase;

            switch (curveType) {
            case 0: // 正弦波动
                y += 20 * sin(x * 2.0);
                break;
            case 1: // 随机漫步
                y += rg->bounded(40.0);
                break;
            case 2: // 线性趋势
                y += x * (rg->bounded(10.0) - 5.0);
                break;
            case 3: // 指数变化
                y += pow(x, 1.0 + rg->generateDouble());
                break;
            }
            curve.append(QPointF(x, y));
        }
        curves.append(curve);
    }
    return curves;
}

void PlotShowForm::UpdatePlotData(AdvancedCurveViewer *pMyQwtPlot, QByteArray sTighteningId, QVector<QPointF> lstCurve)
{
    pMyQwtPlot->setCurveData(0, lstCurve, QString::fromUtf8("曲线") + QString::number(0));
    return;
}

void PlotShowForm::UpdatePlotData(AdvancedCurveViewer* pMyQwtPlot, QList<CurveData> listCurveData)
{
    QStringList strList;
    for (int i = 0; i != listCurveData.size(); i++)
    {
        strList.append(QString::number(i + 1) + "#");
    }

    if (pMyQwtPlot->GetCurveCount() != listCurveData.size())
    {
        pMyQwtPlot->clearAll();
    }

    for (int i = 0; i != listCurveData.size(); i++)
    {
        pMyQwtPlot->setCurveData(i, listCurveData.at(i).mCurveData, strList.at(i));
    }
    return;
}
