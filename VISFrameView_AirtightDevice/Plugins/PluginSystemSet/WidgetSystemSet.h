#ifndef WIDGETSYSTEMSET_H
#define WIDGETSYSTEMSET_H
#include <QWidget>
#include "../../interface/coreinterface.h"

namespace Ui {
class WidgetSystemSet;
}

class WidgetSystemSet :public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSystemSet(QWidget *parent = nullptr);
    ~WidgetSystemSet();

    void InitWidget();
    void LoadUIParam();
    void AddLog(QString msg, LogLevel level);

private slots:
    void on_btnUpdateUI_clicked();

    void on_btnSaveParam_clicked();
private:
    Ui::WidgetSystemSet *ui;
};

#endif // WIDGETSYSTEMSET_H
