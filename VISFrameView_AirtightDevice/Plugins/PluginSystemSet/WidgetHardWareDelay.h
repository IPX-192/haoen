#ifndef WIDGETHARDWAREDELAY_H
#define WIDGETHARDWAREDELAY_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>

namespace Ui {
class WidgetHardWareDelay;
}

class WidgetHardWareDelay : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetHardWareDelay(QWidget *parent = nullptr);
    ~WidgetHardWareDelay();

    void  LoadConfig();
    void  SaveConfig();
protected slots:
    void  LoadUIParam();
	void  UpdateParamToUI();
	void  SaveUIParam();

protected:
   // QTableView *tableView;
    QMap<QString,QStandardItemModel* >m_pViewModel;


private:
    Ui::WidgetHardWareDelay *ui;
};

#endif // WIDGETHARDWAREDELAY_H
