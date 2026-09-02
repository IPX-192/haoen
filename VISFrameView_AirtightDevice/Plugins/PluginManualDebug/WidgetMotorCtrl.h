#ifndef WIDGETMOTORCTRL_H
#define WIDGETMOTORCTRL_H

#include <QWidget>
#include "Singleton.h"
#include <QTimer>

namespace Ui {
class WidgetMotorCtrl;
}

class WidgetMotorCtrl : public QWidget
{
    Q_OBJECT
    SINGLETON_DECL(WidgetMotorCtrl)
public:
    explicit WidgetMotorCtrl(QWidget *parent = nullptr);
    ~WidgetMotorCtrl();
    void  InitMotor();
protected:
    void showEvent(QShowEvent *event);
private:
    Ui::WidgetMotorCtrl *ui;

    QTimer *m_timer = nullptr;
    int     m_heartIndex = 0;
};

#endif // WIDGETMOTORCTRL_H
