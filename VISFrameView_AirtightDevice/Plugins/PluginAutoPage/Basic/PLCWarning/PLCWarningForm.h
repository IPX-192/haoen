#ifndef PLCWARNINGFORM_H
#define PLCWARNINGFORM_H

#include <QWidget>
#include <QStandardItemModel>
namespace Ui {
class PLCWarningForm;
}

class PLCWarningForm : public QWidget
{
    Q_OBJECT

public:
    explicit PLCWarningForm(QWidget *parent = nullptr);
    ~PLCWarningForm();

    void AddWarningInfo(QString sInfo);
    int GetWarningCount();
    void ClearWarningInfo();
private:
    void InitWnd();
    void InitTableWidget();
    int FindItem(QString sInfo);
private:
    Ui::PLCWarningForm *ui;
    QStandardItemModel *m_pItemModelWarnInfo = nullptr;
};
#endif // PLCWARNINGFORM_H
