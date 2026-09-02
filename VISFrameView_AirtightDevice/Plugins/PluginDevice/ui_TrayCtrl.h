/********************************************************************************
** Form generated from reading UI file 'TrayCtrl.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRAYCTRL_H
#define UI_TRAYCTRL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrayCtrl
{
public:
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBoxTray;
    QVBoxLayout *verticalLayout;
    QTableView *tableViewTray;
    QGroupBox *groupBoxSilos;
    QVBoxLayout *verticalLayout_2;
    QTableView *tableViewSilos;

    void setupUi(QWidget *TrayCtrl)
    {
        if (TrayCtrl->objectName().isEmpty())
            TrayCtrl->setObjectName(QString::fromUtf8("TrayCtrl"));
        TrayCtrl->resize(243, 394);
        TrayCtrl->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_3 = new QVBoxLayout(TrayCtrl);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        groupBoxTray = new QGroupBox(TrayCtrl);
        groupBoxTray->setObjectName(QString::fromUtf8("groupBoxTray"));
        groupBoxTray->setAlignment(Qt::AlignCenter);
        verticalLayout = new QVBoxLayout(groupBoxTray);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 4, 0, 4);
        tableViewTray = new QTableView(groupBoxTray);
        tableViewTray->setObjectName(QString::fromUtf8("tableViewTray"));
        tableViewTray->setFocusPolicy(Qt::NoFocus);
        tableViewTray->setContextMenuPolicy(Qt::NoContextMenu);
        tableViewTray->setStyleSheet(QString::fromUtf8("selection-color: rgb(171, 171, 171);"));
        tableViewTray->setFrameShape(QFrame::Box);
        tableViewTray->setSelectionMode(QAbstractItemView::NoSelection);
        tableViewTray->horizontalHeader()->setVisible(false);
        tableViewTray->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(tableViewTray);


        verticalLayout_3->addWidget(groupBoxTray);

        groupBoxSilos = new QGroupBox(TrayCtrl);
        groupBoxSilos->setObjectName(QString::fromUtf8("groupBoxSilos"));
        groupBoxSilos->setAlignment(Qt::AlignCenter);
        verticalLayout_2 = new QVBoxLayout(groupBoxSilos);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 4, 0, 4);
        tableViewSilos = new QTableView(groupBoxSilos);
        tableViewSilos->setObjectName(QString::fromUtf8("tableViewSilos"));
        tableViewSilos->setStyleSheet(QString::fromUtf8(""));
        tableViewSilos->setFrameShape(QFrame::Box);

        verticalLayout_2->addWidget(tableViewSilos);


        verticalLayout_3->addWidget(groupBoxSilos);

        verticalLayout_3->setStretch(0, 3);
        verticalLayout_3->setStretch(1, 2);

        retranslateUi(TrayCtrl);

        QMetaObject::connectSlotsByName(TrayCtrl);
    } // setupUi

    void retranslateUi(QWidget *TrayCtrl)
    {
        TrayCtrl->setWindowTitle(QCoreApplication::translate("TrayCtrl", "Form", nullptr));
        groupBoxTray->setTitle(QCoreApplication::translate("TrayCtrl", "\346\226\231\347\233\230", nullptr));
        groupBoxSilos->setTitle(QCoreApplication::translate("TrayCtrl", "\346\226\231\344\273\223", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TrayCtrl: public Ui_TrayCtrl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRAYCTRL_H
