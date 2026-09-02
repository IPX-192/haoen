#ifndef ARITIGHTFORM_H
#define ARITIGHTFORM_H

#include <QWidget>

namespace Ui {
class AritightForm;
}

class AritightForm : public QWidget
{
    Q_OBJECT

public:
    explicit AritightForm(QWidget *parent = nullptr);
    ~AritightForm();
    void SetStation(int station);
private slots:
    void on_pushButton_start_clicked();

    void on_pushButton_reset_clicked();

private:
    Ui::AritightForm *ui;
    int  m_station = 0;
};

#endif // ARITIGHTFORM_H
