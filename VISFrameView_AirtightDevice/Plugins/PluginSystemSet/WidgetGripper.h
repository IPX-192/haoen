#ifndef WIDGETGRIPPER_H
#define WIDGETGRIPPER_H

#include <QWidget>

namespace Ui {
class WidgetGripper;
}

class ElectricGripperParamForm;
class WidgetGripper : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetGripper(QWidget *parent = nullptr);
    ~WidgetGripper();
public slots:
    void LoadUIParam();
    void UpdateParamToUI();
    void SaveUIParam();
protected:
    void InitWidget();
private:
    Ui::WidgetGripper *ui;
    QList<ElectricGripperParamForm *> m_gripperList;
};

#endif // WIDGETGRIPPER_H
