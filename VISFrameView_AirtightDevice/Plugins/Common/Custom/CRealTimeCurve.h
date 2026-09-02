#ifndef CREALTIMECURVE_H
#define CREALTIMECURVE_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <QWheelEvent>

namespace Ui {
class CRealTimeCurve;
}

class CRealTimeCurve : public QWidget
{
    Q_OBJECT

public:
    explicit CRealTimeCurve(QWidget *parent = nullptr);
    ~CRealTimeCurve();

    void  SetCurve(QString title,QStringList curveName,QString yName,int xRange,int yRange);

    void  UpdateData(std::vector<double> data);

    void  UpdateStep(int step);

    void  Clear();

    void  ClearData();
protected:
    void  wheelEvent(QWheelEvent *event) override;
    void  mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    void  zoomChart(qreal factor, QPointF center);
    void  resetZoom();
private:
    Ui::CRealTimeCurve *ui;

    QtCharts::QChart *m_chart = nullptr;
    QtCharts::QChartView *m_chartView = nullptr;
    QList<QtCharts::QLineSeries*> m_seriesList;
    QList<QtCharts::QLineSeries*> m_lineSeriesList;
    QtCharts::QValueAxis *m_axisX;
    QtCharts::QValueAxis *m_axisY;
    QTimer *m_timer;
    int m_timeCounter;
    int m_yRange = 0;

    QRectF m_originalRange;
    bool m_isZoomed = false;
};

#endif // CREALTIMECURVE_H
