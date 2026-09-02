#include "CRealTimeCurve.h"
#include "ui_CRealTimeCurve.h"

CRealTimeCurve::CRealTimeCurve(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CRealTimeCurve)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);

    m_chart = new QtCharts::QChart();
    m_axisX = new QtCharts::QValueAxis();
    m_axisY = new QtCharts::QValueAxis();
    m_chartView = new QtCharts::QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    // 启用多种缩放模式
    m_chartView->setRubberBand(QtCharts::QChartView::RectangleRubberBand); // 矩形选择缩放
    m_chartView->setInteractive(true); // 启用交互
    m_chartView->setDragMode(QGraphicsView::NoDrag); // 禁用手势拖动
    setMouseTracking(true);

    // 添加坐标轴到图表
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_chart->setMargins(QMargins(10, 5, 10, 5));
    ui->gridLayout->addWidget(m_chartView);
}

CRealTimeCurve::~CRealTimeCurve()
{
    delete ui;
}

void CRealTimeCurve::SetCurve(QString title,QStringList curveName,QString yName,int xRange,int yRange)
{
    m_chart->setTitle(title);
    m_chart->legend()->setVisible(false);
    m_chart->legend()->setAlignment(Qt::AlignTop);
    m_yRange = yRange;

    // 保存原始范围
    m_originalRange = QRectF(0, 0, xRange, yRange);

    // 创建曲线
    for (int i = 0; i < curveName.size(); ++i) {
        QtCharts::QLineSeries *series = new QtCharts::QLineSeries();
        //series->setName(curveName.at(i));
        m_chart->addSeries(series);
        m_seriesList.append(series);
    }

//    QtCharts::QLineSeries *bestSeries = new QtCharts::QLineSeries();
//    m_chart->addSeries(bestSeries);
//    bestSeries->setName(QStringLiteral("最佳"));
//    m_lineSeriesList.append(bestSeries);

    // 设置坐标轴
    m_axisX->setRange(0, xRange);
    if(xRange > 20)
    {
       m_axisX->setTickCount(xRange/8+1);
    }
    else if(xRange > 10 && xRange< 20)
    {
       m_axisX->setTickCount(xRange/4+1);
    }
    else
    {
        m_axisX->setTickCount(xRange/1+1);
    }

    m_axisX->setTitleText(QStringLiteral("阶段"));

    m_axisY->setRange(0, m_yRange);
    m_axisY->setTitleText(yName);

    // 将系列连接到轴
    for (auto series : m_seriesList) {
        series->attachAxis(m_axisX);
        series->attachAxis(m_axisY);
    }
//    for (auto series : m_lineSeriesList) {
//        series->attachAxis(m_axisX);
//        series->attachAxis(m_axisY);
//    }
}

void CRealTimeCurve::UpdateData(std::vector<double> data)
{
    if (data.size() != static_cast<size_t>(m_seriesList.size())) {
        return; // 数据数量不匹配
    }
    for (int i = 0; i < m_seriesList.size(); ++i)
    {
        QtCharts::QLineSeries *series = m_seriesList[i];
        // 添加新点
        double value = data.at(i);
        series->append(m_timeCounter, value);
        // 保持只显示最近100个点
        if (series->count() > 200) {
            series->remove(0);
        }
    }
    m_timeCounter++;
}

void CRealTimeCurve::UpdateStep(int step)
{
    for (int i = 0; i < m_lineSeriesList.size(); ++i)
    {
        QtCharts::QLineSeries *series = m_lineSeriesList[i];
        series->append(step, 0);
        series->append(step, m_yRange); // 垂直线段
        series->setColor(Qt::green);
        series->pen().setWidth(10); // 设置线段粗细为2像素
    }
}

void CRealTimeCurve::Clear()
{
    // 清理之前的资源
    m_chart->removeAllSeries();
    m_seriesList.clear();
    //m_lineSeriesList.clear();
    m_timeCounter = 0;
}

void CRealTimeCurve::ClearData()
{
    // 清除所有数据点而不是删除series
    for (auto series : m_seriesList) {
        series->clear();
    }
//    for (auto series : m_lineSeriesList) {
//        series->clear();
//    }
    m_timeCounter = 0;
}

void CRealTimeCurve::wheelEvent(QWheelEvent *event)
{
    // 检查是否在图表视图上进行滚轮操作
    QPoint pos = m_chartView->mapFromParent(event->pos());
    if (m_chartView->rect().contains(pos)) {
        // 计算缩放因子
        qreal factor = 1.0;
        if (event->angleDelta().y() > 0) {
            factor = 1.2; // 放大
        } else {
            factor = 0.8; // 缩小
        }

        // 获取鼠标位置作为缩放中心
        QPointF center = m_chart->mapToValue(event->pos());
        zoomChart(factor, center);
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}
void CRealTimeCurve::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        resetZoom();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void CRealTimeCurve::zoomChart(qreal factor, QPointF center)
{
    // 获取当前坐标轴范围
    QRectF currentRange = QRectF(m_axisX->min(), m_axisY->min(),
                                 m_axisX->max() - m_axisX->min(),
                                 m_axisY->max() - m_axisY->min());

    // 计算新的范围
    qreal newWidth = currentRange.width() / factor;
    qreal newHeight = currentRange.height() / factor;

    // 计算新的位置，使缩放中心保持不变
    qreal newX = center.x() - (center.x() - currentRange.left()) / factor;
    qreal newY = center.y() - (center.y() - currentRange.top()) / factor;

    // 限制缩放范围，防止过度缩放
    if (newWidth > m_originalRange.width() * 10 || newHeight > m_originalRange.height() * 10) {
        return; // 防止过度放大
    }

    if (newWidth < m_originalRange.width() / 100 || newHeight < m_originalRange.height() / 100) {
        return; // 防止过度缩小
    }

    // 设置新的坐标轴范围
    m_axisX->setRange(newX, newX + newWidth);
    m_axisY->setRange(newY, newY + newHeight);
}

void CRealTimeCurve::resetZoom()
{
    // 重置到原始范围
    m_axisX->setRange(m_originalRange.left(), m_originalRange.right());
    m_axisY->setRange(m_originalRange.top(), m_originalRange.bottom());
}
