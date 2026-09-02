#ifndef WIDGETMANUALDEBUG_H
#define WIDGETMANUALDEBUG_H

#include <QWidget>
#include "../../interface/coreinterface.h"
#include "VisMotorTool.h"
#include "VisMotorToolData.h"
#include "VisMotorManager.h"

namespace Ui {
class WidgetManualDebug;
}

class QToolButton;
class WidgetManualDebug :public QWidget
{
    Q_OBJECT

public:
    explicit WidgetManualDebug(QWidget *parent = nullptr);
    ~WidgetManualDebug();

public:
    void  AddLog(QString msg, LogLevel level);
    void  LoadUIParam();
    void  UpdateUIParam();

private slots:
    void on_btnUpdateUI_clicked();

    void on_btnSaveParam_clicked();

private:
    Ui::WidgetManualDebug *ui;
};

#endif // WIDGETMANUALDEBUG_H
