#ifndef WIDGETAUTOPAGE_H
#define WIDGETAUTOPAGE_H

#include <QWidget>
#include "../../interface/coreinterface.h"
#include <QTimer>
#include <QSharedPointer>
#include "ParamDef.h"

namespace Ui {
class WidgetAutoPage;
}

class QAbstractButton;
class ProcessLogForm;
class SwitchButton;
class WidgetAutoPage :public QWidget
{
    Q_OBJECT

public:
    explicit WidgetAutoPage(QWidget *parent = nullptr);
    ~WidgetAutoPage();
    void InitWidget();
  
public:
    void  InitLog();

public slots:
    int  event_ProductTotal(AritightTask item);
protected:
    virtual void showEvent(QShowEvent* event);
    virtual void hideEvent(QHideEvent* event);

private:
    Ui::WidgetAutoPage *ui;
    ProcessLogForm *pProcessLogForm = nullptr;
    SwitchButton *mpSwitchButtonAutoModel = nullptr;

};

#endif // WIDGETAUTOPAGE_H
