#ifndef CUTOMDOUBLESINBOX_H
#define CUTOMDOUBLESINBOX_H

#include <QDoubleSpinBox>

class CutomDoubleSinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    explicit CutomDoubleSinBox(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
};

#endif // CUTOMDOUBLESINBOX_H
