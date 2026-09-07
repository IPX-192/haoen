#include "WidgetTyphoonCtl.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QButtonGroup>
#include <QMessageBox>
#include <QTextCodec>
#include "ParamManager.h"
#pragma execution_character_set("utf-8")

WidgetTyphoonCtl::WidgetTyphoonCtl(QWidget *parent) : QWidget(parent)
{
    this->setMaximumWidth(800);
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);

    this->setStyleSheet(R"(
QLabel#TitleLabel {
background-color: #BBD6EE;
border: 1px solid #A0C0E0;
padding: 5px;
font-size: 14px;
}
QPushButton {
background-color: #A9A9A9;
color: white;
border: 2px solid #D0D0D0;
padding: 8px 20px;
font-size: 14px;
font-weight: bold;
}
QPushButton:checked {
background-color: #32CD32;
border: 2px solid #228B22;
}
QGroupBox {
border: 1px solid #D0D0D0;
margin-top: 10px;
font-size: 13px;
color: #333333;
}
QGroupBox::title {
subcontrol-origin: margin;
left: 10px;
padding: 0 3px;
}
)");

    mainLayout->addWidget(CreateTyphoonGroup());
    mainLayout->addStretch();
    this->setLayout(mainLayout);
}

QWidget* WidgetTyphoonCtl::CreateTyphoonGroup()
{
    QGroupBox *group = new QGroupBox("台风控制系统");
    QVBoxLayout *vLayout = new QVBoxLayout(group);
    vLayout->setSpacing(12);

    // 台风系统启动
    {
        QWidget *rowWidget = new QWidget();
        QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(0,0,0,0);

        QLabel *titleLabel = new QLabel("台风系统启动");
        titleLabel->setObjectName("TitleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);

        QPushButton *btnSysOpen = new QPushButton("打开");
        QPushButton *btnSysClose = new QPushButton("关闭");
        btnSysOpen->setCheckable(true);
        btnSysClose->setCheckable(true);
        btnSysClose->setChecked(true);

        QButtonGroup *sysBtnGroup = new QButtonGroup(rowWidget);
        sysBtnGroup->addButton(btnSysOpen,1);
        sysBtnGroup->addButton(btnSysClose,0);

        rowLayout->addWidget(titleLabel,3);
        rowLayout->addWidget(btnSysOpen,1);
        rowLayout->addWidget(btnSysClose,1);

        connect(sysBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [=](int btnId){
            if(btnId == 1)
            {
                int ret = VisAppBus::sendEvent("TyphoonSysOpen");
                if(ret == 0)
                {
                    m_typhoonSysOpened = true;
                }
                else
                {
                    QMessageBox::warning(this,"提示","台风系统启动失败！");
                    btnSysClose->setChecked(true);
                }
            }
            else
            {
                VisAppBus::sendEvent("TyphoonSysClose");
                m_typhoonSysOpened = false;
            }
        });
        vLayout->addWidget(rowWidget);
    }

    // 台风系统清灰
    {
        QWidget *rowWidget = new QWidget();
        QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(0,0,0,0);

        QLabel *titleLabel = new QLabel("台风系统清灰");
        titleLabel->setObjectName("TitleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);

        QPushButton *btnCleanOpen = new QPushButton("打开");
        QPushButton *btnCleanClose = new QPushButton("关闭");
        btnCleanOpen->setCheckable(true);
        btnCleanClose->setCheckable(true);
        btnCleanClose->setChecked(true);

        QButtonGroup *cleanBtnGroup = new QButtonGroup(rowWidget);
        cleanBtnGroup->addButton(btnCleanOpen,1);
        cleanBtnGroup->addButton(btnCleanClose,0);

        rowLayout->addWidget(titleLabel,3);
        rowLayout->addWidget(btnCleanOpen,1);
        rowLayout->addWidget(btnCleanClose,1);

        connect(cleanBtnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [=](int btnId){
            if(!m_typhoonSysOpened)
            {
                QMessageBox::warning(this,"提示","请先启动台风系统！");
                btnCleanClose->setChecked(true);
                return;
            }
            if(btnId == 1)
            {
                int ret = VisAppBus::sendEvent("TyphoonCleanOpen");
                if(ret ==0)
                {
                    m_typhoonCleanOpened = true;
                }
                else
                {
                    QMessageBox::warning(this,"提示","台风清灰开启失败！");
                    btnCleanClose->setChecked(true);
                }
            }
            else
            {
                VisAppBus::sendEvent("TyphoonCleanClose");
                m_typhoonCleanOpened = false;
            }
        });
        vLayout->addWidget(rowWidget);
    }
    return group;
}

void WidgetTyphoonCtl::LoadUIParam()
{
    QString filename = GlobalParam->hardwareParam.fileName;
    QSettings *config = new QSettings(filename, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));
    config->beginGroup(QString("TyphoonParam"));
   // VisUIParam::LoadIniToUI(config, this, &GlobalParam->hardwareParam.typhoonParam);
    config->endGroup();
    delete config;
}

void WidgetTyphoonCtl::UpdateParamToUI()
{
    LoadUIParam();
}

void WidgetTyphoonCtl::SaveUIParam()
{
    QString filename = GlobalParam->hardwareParam.fileName;
    QSettings *config = new QSettings(filename, QSettings::IniFormat);
    config->setIniCodec(QTextCodec::codecForName("UTF-8"));
    config->beginGroup(QString("TyphoonParam"));
//   VisUIParam::SaveUIToIni(config, this, &GlobalParam->hardwareParam.typhoonParam);
    config->endGroup();
    delete config;
}
