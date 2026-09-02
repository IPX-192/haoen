#include "WidgetTray.h"
#include "ui_WidgetTray.h"
#include <QPushButton>
#include <QTextCodec>
#include <QDir>
#include <QDateTime>
#include "TrayStatusForm.h"

WidgetTray::WidgetTray(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetTray)
{
    ui->setupUi(this);

    VisAppBus::subscibeEvent(this, "NotifyTrayUpdate");
    VisAppBus::subscibeEvent(this, "NotifyTrayUpdateUsed");
    VisAppBus::subscibeEvent(this, "NotifyTrayReset");

    TabelInit();
}

WidgetTray::~WidgetTray()
{
    delete ui;
}

void WidgetTray::TabelInit()
{
    //料盘界面初始化
    if(nullptr == m_leftUpDownTray)
        m_leftUpDownTray = new TrayStatusForm(TrayStatusForm::UpDownTray);
    if(nullptr == m_rightUpDownTray)
        m_rightUpDownTray = new TrayStatusForm(TrayStatusForm::UpDownTray);
    if(nullptr == m_ngTray)
        m_ngTray = new TrayStatusForm(TrayStatusForm::NgTray);

    ui->gridLayout_leftTray->addWidget(m_leftUpDownTray,0,0);
    ui->gridLayout_rightTray->addWidget(m_rightUpDownTray,0,0);
    ui->gridLayout_ngTray->addWidget(m_ngTray,0,0);

    //UpdateTrayTable();
}
void WidgetTray::UpdateTrayTable()
{
    QStringList unUsedSenor;
    unUsedSenor.clear();
    m_leftUpDownTray->InitTray(QStringLiteral("料盘"),GlobalParam->recipeTray.feedTrayH,
                               GlobalParam->recipeTray.feedTrayW,unUsedSenor);
    m_rightUpDownTray->InitTray(QStringLiteral("料盘"),GlobalParam->recipeTray.feedTrayH,
                                GlobalParam->recipeTray.feedTrayW,unUsedSenor);

    m_ngTray->InitTray(QStringLiteral("料盘"),GlobalParam->recipeTray.ngTrayH,
                                GlobalParam->recipeTray.ngTrayW,unUsedSenor);
}
int WidgetTray::event_NotifyTrayUpdate(int trayIndex,int holeIndex,QString msg,bool bDownTray)
{
    if(trayIndex == 0)
    {
        m_leftUpDownTray->UpdateTray(holeIndex,msg,bDownTray);
    }
    else if(trayIndex == 1)
    {
        m_rightUpDownTray->UpdateTray(holeIndex,msg,bDownTray);
    }
    else if(trayIndex == 2)
    {
        m_ngTray->UpdateTray(holeIndex,msg,false);
    }
    return 0;
}

int WidgetTray::event_NotifyTrayUpdateUsed(int trayIndex, int usedHole)
{
    if(trayIndex == 0)
    {
        m_leftUpDownTray->UpdateInit(usedHole);
    }
    else if(trayIndex == 1)
    {
        m_rightUpDownTray->UpdateInit(usedHole);
    }
    else if(trayIndex == 2)
    {
        m_ngTray->UpdateInit(usedHole);
    }
    return 0;
}

int WidgetTray::event_NotifyTrayReset(int trayIndex)
{
    if(trayIndex == -1)
    {
       UpdateTrayTable();
    }
    QStringList unUsedSenor;
    unUsedSenor.clear();

    if(trayIndex == 0)
    {
        m_leftUpDownTray->InitTray(QStringLiteral("料盘"),GlobalParam->recipeTray.feedTrayH,
                                   GlobalParam->recipeTray.feedTrayW,unUsedSenor);
    }
    else if(trayIndex == 1)
    {
        m_rightUpDownTray->InitTray(QStringLiteral("料盘"),GlobalParam->recipeTray.feedTrayH,
                                    GlobalParam->recipeTray.feedTrayW,unUsedSenor);
    }
    else if(trayIndex == 2)
    {
        m_ngTray->InitTray(QStringLiteral("料盘"),GlobalParam->recipeTray.ngTrayH,
                                    GlobalParam->recipeTray.ngTrayW,unUsedSenor);
    }
    return 0;
}
