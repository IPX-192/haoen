#ifndef PLOTSHOWFORM_H
#define PLOTSHOWFORM_H

#include <QWidget>
#include <QTimer>
#include <QStandardItemModel>
#include "ParamManager.h"
#include "../Common/CurveView/AdvancedCurveViewer.h"
namespace Ui {
class PlotShowForm;
}
//class MyQwtPlot;
//class AdvancedCurveViewer;
class PlotShowForm : public QWidget
{
    Q_OBJECT
public:
    explicit PlotShowForm(QWidget *parent = nullptr);
    ~PlotShowForm();
    void InitWidget();
    void InitTableView();
public slots:
    void recvAngleAndTorqueDataOneSlot(QByteArray sTighteningId, QVector<QPointF> lstCurve);
    void recvAngleAndTorqueDataTwoSlot(QByteArray sTighteningId, QVector<QPointF> lstCurve);
    void recvAngleAndTorqueDataThreeSlot(QByteArray sTighteningId, QVector<QPointF> lstCurve);
    void recvAngleAndTorqueDataFourSlot(QByteArray sTighteningId, QVector<QPointF> lstCurve);
    void TimerOutSlot();
    int event_ShowTighteningInfo(int nIndex, int nTighteningID, QString sInfo);
protected:
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);
private:
    void AddTigheningInfo(int nIndex, int nTighteningID, QString sInfo);
    // 生成具有不同特征的随机曲线
    QVector<QVector<QPointF>> generateDiverseCurves(int nCount = 10);
    void UpdatePlotData(AdvancedCurveViewer* pMyQwtPlot,QByteArray sTighteningId, QVector<QPointF> lstCurve);
    void UpdatePlotData(AdvancedCurveViewer* pMyQwtPlot,QList<CurveData> listCurveData);
private:
    Ui::PlotShowForm *ui;
    QTimer mTimer;
    QStandardItemModel* mpItemModelDriveerBitInfo = nullptr;
};

#endif // PLOTSHOWFORM_H
