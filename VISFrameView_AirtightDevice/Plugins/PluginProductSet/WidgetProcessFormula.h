#ifndef WIDGETPROCESSFORMULA_H
#define WIDGETPROCESSFORMULA_H

#include <QWidget>

namespace Ui {
class WidgetProcessFormula;
}

class WidgetProcessFormula : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetProcessFormula(QWidget *parent = nullptr);
    ~WidgetProcessFormula();

private:
    Ui::WidgetProcessFormula *ui;
};

#endif // WIDGETPROCESSFORMULA_H
