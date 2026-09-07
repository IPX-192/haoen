#include "CutomSinBox.h"

CutomSinBox::CutomSinBox(QWidget *parent):QSpinBox(parent)
{

}

void CutomSinBox::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
    return;
}
