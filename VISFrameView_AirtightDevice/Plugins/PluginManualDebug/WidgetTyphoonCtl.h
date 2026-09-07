#ifndef WIDGETTYPHOONCTL_H
#define WIDGETTYPHOONCTL_H

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QSettings>
#include "VisUIParam.h"
#include "VisAppBus.h"

class WidgetTyphoonCtl : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetTyphoonCtl(QWidget *parent = nullptr);

    void LoadUIParam();
    void SaveUIParam();
    void UpdateParamToUI();

private:
    QWidget* CreateTyphoonGroup();
    bool m_typhoonSysOpened = false;
    bool m_typhoonCleanOpened = false;
};

#endif // WIDGETTYPHOONCTL_H
