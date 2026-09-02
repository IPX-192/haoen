#ifndef ARITIGHTPLOTITEM_H
#define ARITIGHTPLOTITEM_H

#include <QWidget>
#include "ParamDef.h"

namespace Ui {
class AritightPlotItem;
}

class CRealTimeCurve;
class AritightPlotItem : public QWidget
{
    Q_OBJECT

public:
    explicit AritightPlotItem(QWidget *parent = nullptr);
    ~AritightPlotItem();
    void SetTittle(int station,QString name);

    void DrawAirtightPlot(int station,AritightTask data);
    void SetAirtightParam(int station);
    void ClearPlot(int station);
private:
    void InitWidget();
    void InsertTable(int row);
    void UpdateTable(AritightTask data);
private:
    Ui::AritightPlotItem *ui;
    CRealTimeCurve *m_realTimePressCurve = nullptr;
    CRealTimeCurve *m_realTimeLeaksCurve = nullptr;
    int m_station = 0;
    int m_tableRow = 0;
};

#endif // ARITIGHTPLOTITEM_H
