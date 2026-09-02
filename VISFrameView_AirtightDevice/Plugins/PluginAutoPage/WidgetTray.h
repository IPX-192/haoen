#ifndef WIDGETTRAY_H
#define WIDGETTRAY_H

#include <QWidget>
#include "ParamManager.h"

namespace Ui {
class WidgetTray;
}

class TrayStatusForm;
class WidgetTray : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetTray(QWidget *parent = nullptr);
    ~WidgetTray();

public slots:
    int event_NotifyTrayUpdate(int trayIndex,int holeIndex,QString msg,bool bDownTray);
    int event_NotifyTrayUpdateUsed(int trayIndex,int usedHole);
    int event_NotifyTrayReset(int trayIndex);
private:
    void TabelInit();
    void UpdateTrayTable();
private:
    Ui::WidgetTray *ui;
    TrayStatusForm * m_leftUpDownTray = nullptr;
    TrayStatusForm * m_rightUpDownTray = nullptr;
    TrayStatusForm * m_ngTray = nullptr;
};

#endif // WIDGETTRAY_H
