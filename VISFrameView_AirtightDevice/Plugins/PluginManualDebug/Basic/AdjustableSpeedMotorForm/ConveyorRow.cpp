#include "ConveyorRow.h"
#include <QStyleOption>
#include <QPainter>

#pragma execution_character_set("utf-8")

MotorBtnGroup::MotorBtnGroup(const QString &btnText, const QString &idText, const QString &iconChar, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setFixedHeight(70);

    // 上方按钮
    m_btn = new QPushButton(this);
    m_btn->setText(btnText);
    m_btn->setFixedHeight(40);
    m_btn->setCursor(Qt::PointingHandCursor);

    m_btn->setStyleSheet(
        "QPushButton {"
        "background-color: #BDD7EE;"
        "border: 1px solid #2E5F8B;"
        "font-weight: bold;"
        "font-size: 16px;"
        "color: #2E5F8B;"
        "}"
        "QPushButton:pressed {"
        "background-color: #9BC2E6;"
        "}"
    );

    QLabel *lblId = new QLabel(idText, this);
    lblId->setAlignment(Qt::AlignCenter);
    lblId->setFixedHeight(20);
    lblId->setObjectName("MotorIDLabel");

    layout->addWidget(m_btn);
    layout->addWidget(lblId);

    connect(m_btn, &QPushButton::pressed, this, &MotorBtnGroup::pressed);
    connect(m_btn, &QPushButton::released, this, &MotorBtnGroup::released);
}

// ConveyorRow实现
ConveyorRow::ConveyorRow(const ConveyorRow::Config &config, QWidget *parent)
    : QWidget(parent), m_config(config)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(15);

    // --- 左侧：标题 ---
    QLabel *titleLabel = new QLabel(config.title, this);
    titleLabel->setFixedSize(180, 60);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    titleLabel->setObjectName("TitleLabel");
    layout->addWidget(titleLabel);

    // 按钮文本和原表格一样
    QString forwardBtnText = QString::fromUtf8("正转_%1").arg(config.fwdId);
    QString reverseBtnText = QString::fromUtf8("反转_%1").arg(config.revId);
    btnFwd = new MotorBtnGroup(forwardBtnText, config.fwdId, "", this);
    btnRev = new MotorBtnGroup(reverseBtnText, config.revId, "", this);
    // 设置按钮宽度
    btnFwd->setFixedWidth(200);
    btnRev->setFixedWidth(200);

    layout->addWidget(btnFwd);
    layout->addSpacing(10);
    layout->addWidget(btnRev);
    layout->addSpacing(10);

    // 添加一个弹簧，把状态指示推到右边
    layout->addStretch();

    // --- 右侧：状态指示---
    QWidget *statusContainer = new QWidget(this);
    QGridLayout *gridLayout = new QGridLayout(statusContainer);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setHorizontalSpacing(20);
    gridLayout->setVerticalSpacing(5);

    // 创建标签和LED
    QLabel *labelFwd = new QLabel(u8"正转");
    labelFwd->setAlignment(Qt::AlignCenter);
    labelFwd->setFixedWidth(50);

    QLabel *labelRev = new QLabel(u8"反转");
    labelRev->setAlignment(Qt::AlignCenter);
    labelRev->setFixedWidth(50);

    m_ledFwd = new StatusLed();
    m_ledRev = new StatusLed();

    // 第一行：标签
    gridLayout->addWidget(labelFwd, 0, 0, Qt::AlignCenter);
    gridLayout->addWidget(labelRev, 0, 1, Qt::AlignCenter);

    // 第二行：LED指示灯
    gridLayout->addWidget(m_ledFwd, 1, 0, Qt::AlignCenter);
    gridLayout->addWidget(m_ledRev, 1, 1, Qt::AlignCenter);

    // 将状态容器添加到主布局
    layout->addWidget(statusContainer);
    layout->addSpacing(210);

    connect(btnFwd, &MotorBtnGroup::pressed, this, &ConveyorRow::onForwardPressed);
    connect(btnFwd, &MotorBtnGroup::released, this, &ConveyorRow::onForwardReleased);
    connect(btnRev, &MotorBtnGroup::pressed, this, &ConveyorRow::onReversePressed);
    connect(btnRev, &MotorBtnGroup::released, this, &ConveyorRow::onReverseReleased);
}

// 正转按钮按下
void ConveyorRow::onForwardPressed()
{
    QString buttonText = m_config.fwdId;

    // if (false == VisMotorInstance->IsConnected())
    // {
    //     ShowSystemLog(Log_Error, QString::fromUtf8("请先连接PLC"));
    //     return;
    // }

    bool bRet = VisMotorToolSpace::VisMotorInstance->WriteML(buttonText, true);

    if (false == bRet)
    {
        ShowSystemLog(Log_Error, buttonText + QString::fromUtf8(" 正转执行"));
    }
    else
    {
        ShowSystemLog(Log_Info, buttonText + QString::fromUtf8(" 写入成功"));
    }
    qDebug() << "Button PRESSED:" << buttonText << "LED亮";

    QString strRegName = m_config.readfwdId;
    bool bStatus = false;
    if (false !=  VisMotorToolSpace::VisMotorInstance->ReadML(strRegName, bStatus))
    {
         m_ledFwd->setState(true);
    }
}

// 正转按钮释放
void ConveyorRow::onForwardReleased()
{
    QString buttonText = m_config.fwdId;

    // if (false == VisMotorInstance->IsConnected())
    // {
    //     ShowSystemLog(Log_Error, QString::fromUtf8("请先连接PLC"));
    //     m_ledFwd->setState(false);
    //     return;
    // }

    bool bRet = VisMotorToolSpace::VisMotorInstance->WriteML(buttonText, false);

    if (false == bRet)
    {
        ShowSystemLog(Log_Error, buttonText + QString::fromUtf8(" 正转释放"));
    }
    else
    {
        ShowSystemLog(Log_Info, buttonText + QString::fromUtf8(" 写入成功"));
    }
    qDebug() << "Button RELEASED:" << buttonText << "LED灭";

    QString strRegName = m_config.readfwdId;
    bool bStatus = false;
    if (false !=  VisMotorToolSpace::VisMotorInstance->ReadML(strRegName, bStatus))
    {
         m_ledFwd->setState(false);
    }
}

// 反转按钮按下
void ConveyorRow::onReversePressed()
{
    QString buttonText = m_config.revId;

    // if (false == VisMotorInstance->IsConnected())
    // {
    //     ShowSystemLog(Log_Error, QString::fromUtf8("请先连接PLC"));
    //     return;
    // }

    bool bRet = VisMotorToolSpace::VisMotorInstance->WriteML(buttonText, true);

    if (false == bRet)
    {
        ShowSystemLog(Log_Error, buttonText + QString::fromUtf8(" 反转执行"));
    }
    else
    {
        ShowSystemLog(Log_Info, buttonText + QString::fromUtf8(" 写入成功"));
    }
    qDebug() << "Button PRESSED:" << buttonText << "LED亮";

    QString strRegName = m_config.readrevId;
    bool bStatus = false;
    if (false !=  VisMotorToolSpace::VisMotorInstance->ReadML(strRegName, bStatus))
    {
         m_ledRev->setState(true);
    }
}

// 反转按钮释放
void ConveyorRow::onReverseReleased()
{
    QString buttonText = m_config.revId;

    // if (false == VisMotorInstance->IsConnected())
    // {
    //     ShowSystemLog(Log_Error, QString::fromUtf8("请先连接PLC"));
    //     m_ledRev->setState(false);
    //     return;
    // }

    bool bRet = VisMotorToolSpace::VisMotorInstance->WriteML(buttonText, false);

    if (false == bRet)
    {
        ShowSystemLog(Log_Error, buttonText + QString::fromUtf8(" 反转释放"));
    }
    else
    {
        ShowSystemLog(Log_Info, buttonText + QString::fromUtf8(" 写入成功"));
    }
    qDebug() << "Button RELEASED:" << buttonText << "LED灭";

    QString strRegName = m_config.readrevId;
    bool bStatus = false;
    if (false !=  VisMotorToolSpace::VisMotorInstance->ReadML(strRegName, bStatus))
    {
         m_ledRev->setState(false);
    }
}

