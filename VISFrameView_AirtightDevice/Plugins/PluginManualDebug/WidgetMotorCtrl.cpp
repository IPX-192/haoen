#include "WidgetMotorCtrl.h"
#include "ui_WidgetMotorCtrl.h"
#include "VisMotorTool.h"
#include "VisMotorToolData.h"
#include "VisMotorManager.h"
#include "ParamManager.h"

SINGLETON_IMPL(WidgetMotorCtrl)
WidgetMotorCtrl::WidgetMotorCtrl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetMotorCtrl)
{
    ui->setupUi(this);
    this->setWindowTitle(u8"电机调试");
    setAttribute(Qt::WA_QuitOnClose, false);
    this->layout()->addWidget(VisMotorToolSpace::VisMotorToolIns->GetWidget(VisMotorToolSpace::VisMotorTool::F_MotorDebugForm));

}

WidgetMotorCtrl::~WidgetMotorCtrl()
{
    delete ui;
}

void WidgetMotorCtrl::InitMotor()
{
    QString appPath = QCoreApplication::applicationDirPath();
    VisMotorToolSpace::VisMotorToolIns->SetMotorFile(appPath + "/Config/MotorParam.xml");
    VisMotorToolSpace::VisMotorToolIns->SetPointFile(appPath + "/Config/pos.xml");
    VisMotorToolSpace::VisMotorDataInstance->m_flagOffline = false;
    if (false == VisMotorToolSpace::VisMotorInstance->InitMotor())
    {
        ShowSystemLog(Log_Error, QString(u8"初始化电机失败！"));
        return;
    }

    if (false == VisMotorToolSpace::VisMotorInstance->ConnectMotor())
    {
        ShowSystemLog(Log_Error, QString(u8"连接电机失败！"));
        return;
    }

    VisMotorToolSpace::VisMotorToolIns->StartAxisMonitor();
    VisMotorToolSpace::VisMotorToolIns->StartIoMonitor();

    FuncAddr funAddr;
    funAddr.listHomeAddr.append(QString("M504"));
    funAddr.listHomeAddr.append(QString("M514"));
    funAddr.pauseAddr = QString("M501");
    funAddr.emgStopAddr = QString("M503");

    QString strRegName = "M900";
    VisMotorToolSpace::VisMotorInstance->WriteML(strRegName, true);
    VisMotorToolSpace::VisMotorInstance->SetFuncAddr(funAddr);

    m_timer = new QTimer(this);
    m_timer->start(1000);
    connect(m_timer,&QTimer::timeout,this,[=](){
         QString regName = "D9509";
         VisMotorToolSpace::VisMotorInstance->WriteD(regName, m_heartIndex);
         if(m_heartIndex == 0)
            m_heartIndex = 1;
         else
            m_heartIndex = 0;
    });
}
void WidgetMotorCtrl::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    //初始化点位窗口
    QWidget* widgetPoint = VisMotorToolSpace::VisMotorToolIns->GetWidget(VisMotorToolSpace::VisMotorTool::F_PointDebugForm);
    widgetPoint->setStyleSheet(QString("font:%1pt;").arg(qApp->font().pointSize()));
    this->layout()->addWidget(widgetPoint);
}
