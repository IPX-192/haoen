/********************************************************************************
** Form generated from reading UI file 'BoxStatusForm.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BOXSTATUSFORM_H
#define UI_BOXSTATUSFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BoxStatusForm
{
public:
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;

    void setupUi(QWidget *BoxStatusForm)
    {
        if (BoxStatusForm->objectName().isEmpty())
            BoxStatusForm->setObjectName(QString::fromUtf8("BoxStatusForm"));
        BoxStatusForm->resize(157, 300);
        gridLayout = new QGridLayout(BoxStatusForm);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        tableWidget = new QTableWidget(BoxStatusForm);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        gridLayout->addWidget(tableWidget, 0, 0, 1, 1);


        retranslateUi(BoxStatusForm);

        QMetaObject::connectSlotsByName(BoxStatusForm);
    } // setupUi

    void retranslateUi(QWidget *BoxStatusForm)
    {
        BoxStatusForm->setWindowTitle(QCoreApplication::translate("BoxStatusForm", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BoxStatusForm: public Ui_BoxStatusForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BOXSTATUSFORM_H
