#ifndef WIDGETARITIGHTPLOTFORM_H
#define WIDGETARITIGHTPLOTFORM_H

#include <QWidget>
#include "ParamDef.h"
namespace Ui {
class WidgetAritightPlotForm;
}

class AritightPlotItem;
class WidgetAritightPlotForm : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetAritightPlotForm(QWidget *parent = nullptr);
    ~WidgetAritightPlotForm();
public slots:
    int event_DrawAirtightPlot(int station,AritightTask data);
    int event_SetAirtightParam(int station);
    int event_ClearPlot(int station);
private:
    Ui::WidgetAritightPlotForm *ui;
    QList<AritightPlotItem *> m_listPlot;
};

#endif // WIDGETARITIGHTPLOTFORM_H
