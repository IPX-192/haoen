#ifndef ELECTRICGRIPPERPARAMFORM_H
#define ELECTRICGRIPPERPARAMFORM_H

#include <QWidget>

namespace Ui {
class ElectricGripperParamForm;
}

class ElectricGripperParamForm : public QWidget
{
    Q_OBJECT

public:
    explicit ElectricGripperParamForm(QWidget *parent = nullptr);
    ~ElectricGripperParamForm();
    void SetTittle(QString name);
private:
    Ui::ElectricGripperParamForm *ui;
};

#endif // ELECTRICGRIPPERPARAMFORM_H
