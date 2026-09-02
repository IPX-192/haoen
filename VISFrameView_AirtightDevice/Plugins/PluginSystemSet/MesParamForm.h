#ifndef MESPARAMFORM_H
#define MESPARAMFORM_H

#include <QWidget>

namespace Ui {
class MesParamForm;
}

class MesParamForm : public QWidget
{
    Q_OBJECT

public:
    explicit MesParamForm(QWidget *parent = nullptr);
    ~MesParamForm();
private:
    void InitMes();
public slots:
    void  LoadUIParam();
    void UpdateParamToUI();
    void SaveUIParam();

private slots:
    void on_PushButtonMesConnect_clicked();

    void on_PushButtonMesDisConnect_clicked();

    void on_PushButtonMesTest_clicked();
private:
    Ui::MesParamForm *ui;

};

#endif // MESPARAMFORM_H
