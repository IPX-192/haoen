/********************************************************************************
** Form generated from reading UI file 'TrayStatusForm.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRAYSTATUSFORM_H
#define UI_TRAYSTATUSFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrayStatusForm
{
public:
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;

    void setupUi(QWidget *TrayStatusForm)
    {
        if (TrayStatusForm->objectName().isEmpty())
            TrayStatusForm->setObjectName(QString::fromUtf8("TrayStatusForm"));
        TrayStatusForm->resize(400, 300);
        gridLayout = new QGridLayout(TrayStatusForm);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        tableWidget = new QTableWidget(TrayStatusForm);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        gridLayout->addWidget(tableWidget, 0, 0, 1, 1);


        retranslateUi(TrayStatusForm);

        QMetaObject::connectSlotsByName(TrayStatusForm);
    } // setupUi

    void retranslateUi(QWidget *TrayStatusForm)
    {
        TrayStatusForm->setWindowTitle(QCoreApplication::translate("TrayStatusForm", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrayStatusForm: public Ui_TrayStatusForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRAYSTATUSFORM_H
