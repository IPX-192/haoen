#ifndef WIDGETPRODUCTSET_H
#define WIDGETPRODUCTSET_H

#include <QWidget>

#include "../../interface/coreinterface.h"
namespace Ui {
class WidgetProductSet;
}
//生产配方设置界面
class WidgetProductSet :public QWidget
{
    Q_OBJECT

public:
    explicit WidgetProductSet(QWidget *parent = nullptr);
    ~WidgetProductSet();

public:
    void  AddLog(QString msg, LogLevel level);
    void  Init();
    void  LoadUIParam();
    void  UpdateUIParam();

public slots:
    int   slot_InputInfo(tagOutputInfo& info);
  
private slots:
    void on_btnUpdateUI_clicked();

    void on_btnSaveParam_clicked();

private:
    Ui::WidgetProductSet *ui;
};

#endif // WIDGETPRODUCTSET_H
