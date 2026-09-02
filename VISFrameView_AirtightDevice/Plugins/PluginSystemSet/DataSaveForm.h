#ifndef DATASAVEFORM_H
#define DATASAVEFORM_H

#include <QWidget>

namespace Ui {
class DataSaveForm;
}

class DataSaveForm : public QWidget
{
    Q_OBJECT

public:
    explicit DataSaveForm(QWidget *parent = nullptr);
    ~DataSaveForm();

public slots:
    void LoadUIParam();
    void UpdateParamToUI();
    void SaveUIParam();

private slots:
    void on_PushButtonChosePath_clicked();
private:
    Ui::DataSaveForm *ui;
};
#endif // DATASAVEFORM_H
