#include "WidgetSingleTest.h"
#include "ui_WidgetSingleTest.h"
#include "VisAppBus.h"
#include "ParamManager.h"
#include <QPushButton>

WidgetSingleTest::WidgetSingleTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSingleTest)
{
    ui->setupUi(this);
    VisAppBus::subscibeEvent(this, "UiAutoMode");
    VisAppBus::subscibeEvent(this, "LoginUserChange");
    InitSlot();
}

WidgetSingleTest::~WidgetSingleTest()
{
    delete ui;
}

void WidgetSingleTest::InitSlot()
{
    connect(ui->btnEndProduct, &QPushButton::clicked, this, [=]{
        //结束生产:触发清料,剩余托盘做完不放新盘
        ShowSystemLog(Log_Info, QString(u8"点击结束生产按钮:发送SetClearFlag"));
        VisAppBus::sendEvent("SetProductEnd", true);
        VisAppBus::sendEvent("SetClearFlag");
    });

    //调试按钮:模拟下游要盘
    QPushButton* btnSimRequest = new QPushButton(QStringLiteral("模拟下游要盘"), this);
    btnSimRequest->setMinimumHeight(36);
    connect(btnSimRequest, &QPushButton::clicked, this, [=]{
        static bool simRequest = false;
        simRequest = !simRequest;
        VisAppBus::sendEvent("SimulateNextRequest", simRequest);
        btnSimRequest->setText(simRequest ? QStringLiteral("停止下游要盘") : QStringLiteral("模拟下游要盘"));
    });
    ui->verticalLayout->addWidget(btnSimRequest);

    //调试按钮:模拟扫码NG
    QPushButton* btnSimNG = new QPushButton(QStringLiteral("模拟扫码NG"), this);
    btnSimNG->setMinimumHeight(36);
    connect(btnSimNG, &QPushButton::clicked, this, [=]{
        static bool simNG = false;
        simNG = !simNG;
        VisAppBus::sendEvent("SimulateNG", simNG);
        btnSimNG->setText(simNG ? QStringLiteral("停止模拟NG") : QStringLiteral("模拟扫码NG"));
    });
    ui->verticalLayout->addWidget(btnSimNG);
}

int WidgetSingleTest::event_LoginUserChange()
{
    UserInfo& curUserInfo = GlobalParam->frameCore->curUserInfo;
    bool enable = (curUserInfo.authority == PARAMADMIN || curUserInfo.authority == SUPERADMIN);
    this->setEnabled(enable);
    return 0;
}

int WidgetSingleTest::event_UiAutoMode(bool flag)
{
    ui->editBarCodeAuto1->setEnabled(!flag);
    return 0;
}
