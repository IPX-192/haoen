#include "CutomDoubleSinBox.h"

CutomDoubleSinBox::CutomDoubleSinBox(QWidget *parent):QDoubleSpinBox(parent)
{

}

void CutomDoubleSinBox::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
    return;
}
