#ifndef SERIALCOMFORM_H
#define SERIALCOMFORM_H

#include <QWidget>
#include "ParamDef.h"

namespace Ui {
class SerialComForm;
}

class SerialComForm : public QWidget
{
    Q_OBJECT

public:
    explicit SerialComForm(QWidget *parent = nullptr);
    ~SerialComForm();

    void SetStation(int station);
    SerialComStruct GetParam();
    void SetParam(SerialComStruct param);
private slots:
    void on_PushButtonOpenCom_clicked();
    void on_PushButtonCloseCom_clicked();
private:
    QStringList GetPortNameList();
private:
    Ui::SerialComForm *ui;
    int m_station = 0;
};

#endif // SERIALCOMFORM_H
