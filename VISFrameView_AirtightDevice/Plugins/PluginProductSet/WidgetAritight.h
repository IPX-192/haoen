#ifndef WIDGETARITIGHT_H
#define WIDGETARITIGHT_H

#include <QWidget>
#include "ParamManager.h"
namespace Ui {
class WidgetAritight;
}

class AritightForm;
class WidgetAritight : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetAritight(QWidget *parent = nullptr);
    ~WidgetAritight();
    void  InitWidget();
public slots:
    void LoadUIParam();
    void UpdateParamToUI();
    void SaveUIParam();
private slots:
    void on_btnSave_clicked();
    void on_comboBox_Recipe_currentIndexChanged(const QString &arg1);
private:
    bool CopyFile(const QString &src, const QString &dst);
private:
    Ui::WidgetAritight *ui;
    QList<AritightForm *> m_aritightFormList;
};

#endif // WIDGETARITIGHT_H
