#ifndef CONVEYORCONTROLWIDGET_H
#define CONVEYORCONTROLWIDGET_H
#include <QWidget>
#include "ConveyorRow.h"
namespace Ui {
class ConveyorControlWidget;
}

class ConveyorControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConveyorControlWidget(QWidget *parent = nullptr);
    ~ConveyorControlWidget();

private:
    Ui::ConveyorControlWidget *ui;
};

#endif // CONVEYORCONTROLWIDGET_H
