#ifndef CKDCLAMPDEBUGFORM_H
#define CKDCLAMPDEBUGFORM_H

#include <QWidget>
#include <QLineEdit>
namespace Ui {
class CKDClampDebugForm;
}
class CKDClampingAddrParam;
class CKDClampDebugForm : public QWidget
{
    Q_OBJECT

public:
    explicit CKDClampDebugForm(QWidget *parent = nullptr);
    ~CKDClampDebugForm();

    void InitWidget();
    void SetPointer(CKDClampingAddrParam* pCKDClampingAddrParam);
private slots:

    void on_PushButtonRead_clicked();

    void on_PushButtonWrite_clicked();

    void on_PushButtonClampGroupRelease_clicked();
private:
    void ReadParam(QString sAddr, QLineEdit* pLineEdit);
    void WriteParam(QString sAddr, QLineEdit* pLineEdit);
private:
    Ui::CKDClampDebugForm *ui;
    CKDClampingAddrParam* mpCKDClampingAddrParam;
};

#endif // CKDCLAMPDEBUGFORM_H
