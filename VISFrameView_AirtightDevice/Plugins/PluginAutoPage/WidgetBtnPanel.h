#ifndef WIDGETBTNPANEL_H
#define WIDGETBTNPANEL_H

#include <QWidget>
#include "ParamManager.h"

namespace Ui {
class WidgetBtnPanel;
}

class SwitchButton;
class WidgetBtnPanel : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetBtnPanel(QWidget *parent = nullptr);
    ~WidgetBtnPanel();

protected:
    void  SetState(MachineRunStatus state);

protected:
    SwitchButton*m_autoModel=nullptr;

private slots:
    void on_btnReset_clicked();

    void on_btnAutoStop_clicked();

    void on_btnAutoPause_clicked();

    void on_btnAutoRestore_clicked();

    void on_btnAlarmCleared_clicked();

    void on_btnEstop_clicked();

    void on_btnClearModule_clicked();

    void on_btnSpotCheck_clicked();

    void on_btnAutoStart_clicked();

public slots:
    int event_UpdateDevStatus(MachineRunStatus status);

private:
    Ui::WidgetBtnPanel *ui;
};

#endif // WIDGETBTNPANEL_H
