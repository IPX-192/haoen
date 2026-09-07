#include "CPopTipScreen.h"
#include <QGuiApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QHBoxLayout>
#include <QApplication>

CPopTipScreen::CPopTipScreen(QWidget *parent) :
    QDialog(parent)
{
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags);  //去掉标题栏右上角的问号
    this->setWindowTitle(QStringLiteral("提示"));
    this->setFixedSize(300,100);
    m_pLabel = new QLabel;
    m_pLabel->setStyleSheet("background-color:transparent;border: none;");
    QFont t_font("", 12);
    m_pLabel->setFont(t_font);

    QHBoxLayout *h=new QHBoxLayout(this);
    h->addWidget(m_pLabel);

    m_nDesktopHeight = QApplication::desktop()->height();

    m_pShowTimer = new QTimer(this);
    m_pStayTimer = new QTimer(this);
    m_pCloseTimer = new QTimer(this);

    connect(m_pShowTimer, SIGNAL(timeout()), this, SLOT(onMove()));
    connect(m_pStayTimer, SIGNAL(timeout()), this, SLOT(onStay()));
    connect(m_pCloseTimer, SIGNAL(timeout()), this, SLOT(onClose()));
}

CPopTipScreen::~CPopTipScreen()
{

}

void CPopTipScreen::showMessage(QString str)
{
    m_pLabel->setText(str);
    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    QRect rect = primaryScreen->availableGeometry();
    m_point.setX(rect.width() - width());
    m_point.setY(rect.height() - height());
    move(m_point.x(), m_point.y());
    m_pShowTimer->start(5);
}

void CPopTipScreen::onMove()
{
    m_nDesktopHeight--;
    move(m_point.x(), m_nDesktopHeight);
    if (m_nDesktopHeight <= m_point.y())
    {
        m_pShowTimer->stop();
        m_pStayTimer->start(5000);
    }
}

void CPopTipScreen::onStay()
{
    m_pStayTimer->stop();
    m_pCloseTimer->start(10);
}

void CPopTipScreen::onClose()
{
    m_nDesktopHeight++;
    move(m_point.x(), m_nDesktopHeight);
    if (m_nDesktopHeight >= QApplication::desktop()->height())
    {
        m_pCloseTimer->stop();
        close();
    }
}
