#ifndef WIDGETRECIPEMOTOR_H
#define WIDGETRECIPEMOTOR_H

#include <QWidget>

namespace Ui {
class WidgetRecipeMotor;
}

class WidgetRecipeMotor : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetRecipeMotor(QWidget *parent = nullptr);
    ~WidgetRecipeMotor();


public slots:
    void  LoadUIParam();
    void  SaveUIParam();
    void  UpdateParamToUI();
protected:
    void showEvent(QShowEvent *event);
private slots:
    void on_btnSave_clicked();

    void on_comboBox_Recipe_currentIndexChanged(const QString &arg1);

private:
    Ui::WidgetRecipeMotor *ui;
};

#endif // WIDGETRECIPEMOTOR_H
