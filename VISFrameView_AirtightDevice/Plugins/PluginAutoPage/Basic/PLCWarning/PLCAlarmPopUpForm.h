#ifndef PLCALARMPOPUPFORM_H
#define PLCALARMPOPUPFORM_H

#include <QWidget>

namespace Ui {
class PLCAlarmPopUpForm;
}

class PLCAlarmPopUpForm : public QWidget
{
    Q_OBJECT

public:
    explicit PLCAlarmPopUpForm(QWidget *parent = nullptr);
    ~PLCAlarmPopUpForm();

    void AddWarningInfo(QString sInfo);
    int GetWarningCount();
    void ClearWarningInfo();
    void ShowPlcWarningForm();
private slots:
    void on_PushButtonConfirm_clicked();

private:
    Ui::PLCAlarmPopUpForm *ui;
    qint64 mnLastPLCWarningTime = 0;
};

#endif // PLCALARMPOPUPFORM_H
