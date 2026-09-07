#ifndef SERIALCOMFORM_H
#define SERIALCOMFORM_H

#include <QWidget>
#include "ParamDef.h"

namespace Ui {
class SerialComForm;
}

//气密仪 TCP 连接配置表单(每台双通道仪一个实例)
class SerialComForm : public QWidget
{
    Q_OBJECT

public:
    explicit SerialComForm(QWidget *parent = nullptr);
    ~SerialComForm();

    void SetDevice(int device);
    TcpComStruct GetParam();
    void SetParam(TcpComStruct param);
private slots:
    void on_PushButtonOpenCom_clicked();
    void on_PushButtonCloseCom_clicked();
private:
    Ui::SerialComForm *ui;
    int m_device = 0;
};

#endif // SERIALCOMFORM_H
