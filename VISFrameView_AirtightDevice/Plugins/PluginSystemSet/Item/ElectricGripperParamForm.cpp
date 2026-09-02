#include "ElectricGripperParamForm.h"
#include "ui_ElectricGripperParamForm.h"


ElectricGripperParamForm::ElectricGripperParamForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ElectricGripperParamForm)
{
    ui->setupUi(this);
}

ElectricGripperParamForm::~ElectricGripperParamForm()
{
    delete ui;
}

void ElectricGripperParamForm::SetTittle(QString name)
{
    ui->groupBox_3->setTitle(name);
}
