#ifndef CUTOMSINBOX_H
#define CUTOMSINBOX_H

#include <QSpinBox>

class CutomSinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit CutomSinBox(QWidget *parent = nullptr);
protected:
    void wheelEvent(QWheelEvent *event) override;
};

#endif // CUTOMSINBOX_H
