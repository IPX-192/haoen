#include "AdvancedCurveViewer.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QActionGroup>
#include "Chart/qcustomplot.h"
#pragma execution_character_set("utf-8")
AdvancedCurveViewer::AdvancedCurveViewer(QWidget *parent)
    : QWidget(parent),
      m_featurePointsVisible(true),
      m_crosshairVisible(true),
      m_xZoomOnly(false),
      m_yZoomOnly(false)
{
    setupUI();
    setupPlot();
    setupToolBar();
    setupInteractions();
}

const QVector<QColor> PREDEFINED_COLORS = {
    QColor(31, 119, 180),   // 蓝色
    QColor(255, 127, 14),   // 橙色
    QColor(44, 160, 44),    // 绿色
    QColor(214, 39, 40),    // 红色
    QColor(148, 103, 189),  // 紫色
    QColor(140, 86, 75),    // 棕色
    QColor(227, 119, 194),  // 粉色
    QColor(127, 127, 127),  // 灰色
    QColor(188, 189, 34),   // 黄绿色
    QColor(23, 190, 207)    // 青色
};

QColor getPredefinedColor(int index) {
    return PREDEFINED_COLORS.at(index % PREDEFINED_COLORS.size());
}

void AdvancedCurveViewer::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 添加工具栏
    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);
    layout->addWidget(m_toolBar);

    // 添加绘图区域
    m_plot = new QCustomPlot(this);
    layout->addWidget(m_plot);
}

void AdvancedCurveViewer::setupPlot()
{
    // 基本配置
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    m_plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    // 默认标题和轴标签
    setTitle(QString::fromUtf8(""));
    setAxisLabels(QString::fromUtf8("X轴"), QString::fromUtf8("Y轴"));

    // 图例
    m_plot->legend->setVisible(true);
    m_plot->legend->setFont(QFont("Helvetica", 9));
    m_plot->legend->setBrush(Qt::NoBrush);  // 无背景填充
    m_plot->legend->setBorderPen(Qt::NoPen);
    // 十字线
    m_vLine = new QCPItemStraightLine(m_plot);
    m_hLine = new QCPItemStraightLine(m_plot);
    m_vLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    m_hLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    m_vLine->setVisible(m_crosshairVisible);
    m_hLine->setVisible(m_crosshairVisible);
}

void AdvancedCurveViewer::setupToolBar()
{
    // 重置视图
    QAction *resetAction = m_toolBar->addAction(QString::fromUtf8("重置视图"));
    resetAction->setToolTip(QString::fromUtf8("重置视图以显示所有数据"));
    connect(resetAction, &QAction::triggered, this, &AdvancedCurveViewer::zoomToFit);

    //// 十字线显示切换
    //QAction *crosshairAction = m_toolBar->addAction(QString::fromUtf8("十字线"));
    //crosshairAction->setCheckable(true);
    //crosshairAction->setChecked(m_crosshairVisible);
    //crosshairAction->setToolTip(QString::fromUtf8("显示/隐藏十字线"));
    //connect(crosshairAction, &QAction::toggled, this, &AdvancedCurveViewer::setCrosshairVisible);

    //m_toolBar->addSeparator();

    //// 缩放模式
    //QActionGroup *zoomGroup = new QActionGroup(this);

    //QAction *zoomBothAction = m_toolBar->addAction(QString::fromUtf8("双向缩放"));
    //zoomBothAction->setCheckable(true);
    //zoomBothAction->setChecked(true);
    //zoomBothAction->setToolTip(QString::fromUtf8("同时缩放X和Y轴"));
    //connect(zoomBothAction, &QAction::triggered, [this]() {
    //    setZoomMode(false, false);
    //});
    //zoomGroup->addAction(zoomBothAction);

    //QAction *zoomXAction = m_toolBar->addAction(QString::fromUtf8("X轴缩放"));
    //zoomXAction->setCheckable(true);
    //zoomXAction->setToolTip(QString::fromUtf8("仅缩放X轴"));
    //connect(zoomXAction, &QAction::triggered, [this]() {
    //    setZoomMode(true, false);
    //});
    //zoomGroup->addAction(zoomXAction);

    //QAction *zoomYAction = m_toolBar->addAction(QString::fromUtf8("Y轴缩放"));
    //zoomYAction->setCheckable(true);
    //zoomYAction->setToolTip(QString::fromUtf8("仅缩放Y轴"));
    //connect(zoomYAction, &QAction::triggered, [this]() {
    //    setZoomMode(false, true);
    //});
    //zoomGroup->addAction(zoomYAction);

    //m_toolBar->addSeparator();

    // 导出图像
    QAction *exportAction = m_toolBar->addAction("导出");
    exportAction->setToolTip("导出当前视图为图像");
    connect(exportAction, &QAction::triggered, [this]() {
        QString fileName = QFileDialog::getSaveFileName(
            this, QString::fromUtf8("保存图像"), "", "PNG (*.png);;JPG (*.jpg);;PDF (*.pdf)");

        if (fileName.endsWith(".png", Qt::CaseInsensitive)) {
            m_plot->savePng(fileName);
        } else if (fileName.endsWith(".jpg", Qt::CaseInsensitive)) {
            m_plot->saveJpg(fileName);
        } else if (fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
            m_plot->savePdf(fileName);
        }
    });
}

void AdvancedCurveViewer::setupInteractions()
{
    // 十字线移动
    connect(m_plot, &QCustomPlot::mouseMove, [this](QMouseEvent *event) {
        if (!m_crosshairVisible) return;

        double x = m_plot->xAxis->pixelToCoord(event->pos().x());
        double y = m_plot->yAxis->pixelToCoord(event->pos().y());

        m_vLine->point1->setCoords(x, m_plot->yAxis->range().lower);
        m_vLine->point2->setCoords(x, m_plot->yAxis->range().upper);

        m_hLine->point1->setCoords(m_plot->xAxis->range().lower, y);
        m_hLine->point2->setCoords(m_plot->xAxis->range().upper, y);

        m_plot->replot();
    });

    // 双击重置视图
    connect(m_plot, &QCustomPlot::mouseDoubleClick, this, &AdvancedCurveViewer::zoomToFit);
    // 视图变化信号
    connect(m_plot->xAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), [this]() {
        emit viewChanged(m_plot->xAxis->range().lower, m_plot->xAxis->range().upper,
                        m_plot->yAxis->range().lower, m_plot->yAxis->range().upper);
    });
}

// 公共方法实现
void AdvancedCurveViewer::addCurveData(const QVector<double> &x, const QVector<double> &y,
                                         const QString &name, const QColor &color)
{
    int nCount = m_plot->graphCount();
    m_plot->addGraph();
    QCPGraph *graph = m_plot->graph(nCount);
    graph->setData(x, y);
    graph->setName(name);
    QColor colorUseDef = Qt::color0;
    if (colorUseDef == color)
    {
        graph->setPen(QPen(getPredefinedColor(nCount), 2));
    }
    else
    {
        graph->setPen(QPen(color, 2));
    }
    graph->setLineStyle(QCPGraph::lsLine);

    if (!x.isEmpty()) {
        zoomToFit();
    }
    return;
}

void AdvancedCurveViewer::setCurveData(int nIndex, const QVector<double>& x, const QVector<double>& y, const QString& name, const QColor& color)
{
    if (nIndex < 0)
    {
        return;
    }

    int nCurveCount = m_plot->graphCount();
    if (nIndex >= nCurveCount)
    {
        int nAddCount = nIndex - nCurveCount + 1;
        for (int i = 0; i < nAddCount; ++i)
        {
            m_plot->addGraph();
        }
    }

    QCPGraph *graph = m_plot->graph(nIndex);
    graph->setData(x, y);
    graph->setName(name);
    QColor colorUseDef = Qt::color0;
    if (colorUseDef == color)
    {
        graph->setPen(QPen(getPredefinedColor(nIndex), 2));
    }
    else
    {
        graph->setPen(QPen(color, 2));
    }
    graph->setLineStyle(QCPGraph::lsLine);

    if (!x.isEmpty()) {
        zoomToFit();
    }
    return;
}

int AdvancedCurveViewer::GetCurveCount()
{
    return m_plot->graphCount();
}

void AdvancedCurveViewer::setCurveData(int nIndex, const QVector<QPointF>& vecCurveData, const QString& name, const QColor& color)
{
    QVector<double> x, y;
    for (int i = 0; i < vecCurveData.size(); ++i)
    {
        x.append(vecCurveData[i].x());
        y.append(vecCurveData[i].y());
    }
    setCurveData(nIndex, x, y, name, color);
    return;
}

void AdvancedCurveViewer::clearAll()
{
    m_plot->clearGraphs();
    m_plot->replot();
}

void AdvancedCurveViewer::setTitle(const QString &title)
{
    if (nullptr == mptitleElement)
    {
        m_plot->plotLayout()->insertRow(0);
        mptitleElement = new QCPTextElement(m_plot, title, QFont("sans", 12, QFont::Bold));
        m_plot->plotLayout()->addElement(0, 0, mptitleElement);
    }
    else
    {
        mptitleElement->setText(title);
    }
}

void AdvancedCurveViewer::setAxisLabels(const QString &xLabel, const QString &yLabel)
{
    m_plot->xAxis->setLabel(xLabel);
    m_plot->yAxis->setLabel(yLabel);
}

void AdvancedCurveViewer::setCrosshairVisible(bool visible)
{
    m_crosshairVisible = visible;
    m_vLine->setVisible(visible);
    m_hLine->setVisible(visible);
    m_plot->replot();
}

void AdvancedCurveViewer::zoomToFit()
{
    m_plot->rescaleAxes();

    QCPRange Xrange = m_plot->xAxis->range();
    double dXexpansion = Xrange.size() * 0.2;
    m_plot->xAxis->setRange(Xrange.lower - dXexpansion / 2, Xrange.upper + dXexpansion / 2);

    QCPRange Yrange = m_plot->yAxis->range();
    double dYexpansion = Yrange.size() * 0.2;
    m_plot->yAxis->setRange(Yrange.lower - dYexpansion / 2, Yrange.upper + dYexpansion / 2);

    m_plot->replot();
}

void AdvancedCurveViewer::setZoomMode(bool xOnly, bool yOnly)
{
    m_xZoomOnly = xOnly;
    m_yZoomOnly = yOnly;

    if (xOnly) {
        m_plot->axisRect()->setRangeZoom(Qt::Horizontal);
    } else if (yOnly) {
        m_plot->axisRect()->setRangeZoom(Qt::Vertical);
    } else {
        m_plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
}

