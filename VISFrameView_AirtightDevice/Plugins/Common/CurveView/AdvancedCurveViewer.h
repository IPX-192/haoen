#ifndef ADVANCEDCURVEVIEWER_H
#define ADVANCEDCURVEVIEWER_H
#include <QWidget>
#include <QToolBar>

class QCustomPlot;
class QCPItemStraightLine;
class QCPTextElement;
class  AdvancedCurveViewer : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedCurveViewer(QWidget *parent = nullptr);

    // 曲线数据设置
    void addCurveData(const QVector<double> &x, const QVector<double> &y,
                         const QString &name = "曲线", const QColor &color = Qt::color0);
    void setCurveData(int nIndex,const QVector<double> &x, const QVector<double> &y,
        const QString& name = "曲线", const QColor& color = Qt::color0);
    void setCurveData(int nIndex, const QVector<QPointF>& vecCurveData,
        const QString& name = "曲线", const QColor& color = Qt::color0);

    void clearAll();
    int GetCurveCount();

    // 图表配置
    void setTitle(const QString &title);
    void setAxisLabels(const QString &xLabel, const QString &yLabel);
    void setCrosshairVisible(bool visible);

    // 交互控制
    void zoomToFit();
    void setZoomMode(bool xOnly, bool yOnly);

    // 获取底层对象
    QCustomPlot *customPlot() const { return m_plot; }
    QToolBar *toolBar() const { return m_toolBar; }

signals:
    void featurePointClicked(double x, double y);
    void viewChanged(double xMin, double xMax, double yMin, double yMax);

private slots:
private:
    void setupUI();
    void setupPlot();
    void setupToolBar();
    void setupInteractions();

    QCustomPlot *m_plot;
    QToolBar *m_toolBar;

    // 交互元素
    QCPItemStraightLine *m_vLine;
    QCPItemStraightLine *m_hLine;

    QCPTextElement* mptitleElement = nullptr;

    // 状态
    bool m_featurePointsVisible;
    bool m_crosshairVisible;
    bool m_xZoomOnly;
    bool m_yZoomOnly;
};

#endif // ADVANCEDCURVEVIEWER_H
