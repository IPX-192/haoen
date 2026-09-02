#ifndef WIDGETCYLINDERTAB_H
#define WIDGETCYLINDERTAB_H

#include <QWidget>
#include <QTimer>
#include <QMap>

namespace Ui {
class WidgetCylinderTab;
}
class CylinderWidget;
class WidgetCylinderTab : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetCylinderTab(QWidget *parent = nullptr);
    ~WidgetCylinderTab();

protected:
    Q_INVOKABLE void LoadUIParam();
    void SetIoState(QString name,bool state);
    void GetIoState(QString name,bool* state);
    //通过IO序号获取IO英文名，控制卡专用
    void GetCardIoName(QMap<QString, QString>& io_name, QString ioIndex, bool inIO);

private slots:
    // 200ms 定时器槽
    void slotRefreshTimer();
    // 处理气缸点击
    void slotCylinderCommand(CylinderWidget* sender, int cmd);

private:
    int     m_cols = 4;  // 每行显示几个气缸
    QTimer *m_refreshTimer;
    // 存储所有气缸指针，用于遍历刷新
    QMap<int,QList<CylinderWidget*>> m_cylinders;

private:
    Ui::WidgetCylinderTab *ui;
};

#endif // WIDGETCYLINDERTAB_H
