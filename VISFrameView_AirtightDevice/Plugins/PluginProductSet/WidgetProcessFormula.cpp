#include "WidgetProcessFormula.h"
#include "ui_WidgetProcessFormula.h"

WidgetProcessFormula::WidgetProcessFormula(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetProcessFormula)
{
    ui->setupUi(this);
}

WidgetProcessFormula::~WidgetProcessFormula()
{
    delete ui;
}
