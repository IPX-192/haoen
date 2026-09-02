#ifndef ELECTRICGRIPPERDEBUGFORM_H
#define ELECTRICGRIPPERDEBUGFORM_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
namespace Ui {
class ElectricGripperDebugForm;
}
class ElectricSawModuleParam;
class ElectricGripperDebugForm : public QWidget
{
    Q_OBJECT

public:
    explicit ElectricGripperDebugForm(QWidget *parent = nullptr);
    ~ElectricGripperDebugForm();

    void SetPointer(ElectricSawModuleParam* pElectricSawModuleParam);
private slots:
    void on_PushButtonRead_clicked();

    void on_PushButtonWrite_clicked();

    void on_PushButtonClampGroupRelease_clicked();
private:
    void ReadParam(QString sAddr,QLineEdit* pLineEdit);
    void WriteParam(QString sAddr, QLineEdit* pLineEdit);
private:

    Ui::ElectricGripperDebugForm *ui;
    ElectricSawModuleParam* mpElectricSawModuleParam;
};

#endif // ELECTRICGRIPPERDEBUGFORM_H
