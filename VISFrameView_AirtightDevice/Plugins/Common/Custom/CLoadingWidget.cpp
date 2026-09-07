#include "CLoadingWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>

CLoadingWidget::CLoadingWidget(QWidget *parent) : QDialog(parent)
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setModal(true);

    m_pLoadingWidgetItem = new CLoadingWidgetItem();
    m_pLoadingLabelText = new QLabel();
    QHBoxLayout *h=new QHBoxLayout;
    QVBoxLayout *v=new QVBoxLayout(this);

    h->addStretch();
    h->addWidget(m_pLoadingWidgetItem);
    h->addStretch();
    v->addLayout(h);
    v->addWidget(m_pLoadingLabelText);
    QFont font;
    font.setBold(true);
    font.setPointSize(18);
    m_pLoadingLabelText->setFont(font);
    m_pLoadingLabelText->setText(QStringLiteral("加载中,请稍后..."));
    m_pLoadingLabelText->setAlignment(Qt::AlignCenter);

    m_pLoadingWidgetItem->setFixedSize(80,80);
    m_pLoadingWidgetItem->hide();
}

CLoadingWidget::~CLoadingWidget()
{

}

void CLoadingWidget::SetTextInfo(QString text)
{
    m_pLoadingLabelText->setText(text);
}

void CLoadingWidget::StartMovie()
{
    m_pLoadingWidgetItem->show();
}

void CLoadingWidget::StopMovie()
{
    this->close();
}


CLoadingWidgetItem::CLoadingWidgetItem(QWidget *parent)
    : QWidget{parent}
{

}

CLoadingWidgetItem::~CLoadingWidgetItem()
{

}

void CLoadingWidgetItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 定义旋转中心和旋转角度
    painter.translate(width() / 2, height() / 2);
    painter.rotate(m_angle);

    // 设置笔的宽度和颜色，并画圆
    int radius = (width()-10) / 2;    // 圆的半径
    QPen pen;
    pen.setWidth(radius / 5);
    pen.setColor(QColor(0, 0, 0, 50));
    painter.setPen(pen);
    painter.drawEllipse(-radius, -radius, 2*radius, 2*radius);

    // 在圆上画一个定义好颜色的扇形外边框
    pen.setColor(QColor(0, 255, 20));
    painter.setPen(pen);
    painter.drawArc(-radius, -radius, 2*radius, 2*radius, 0 * 16, 80 * 16);
}


void CLoadingWidgetItem::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    m_angle = (m_angle + 10) % 360; // 每次旋转10度
    update();
}


void CLoadingWidgetItem::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    QMetaObject::invokeMethod(this, "startUpdateTimer");
}

void CLoadingWidgetItem::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    QMetaObject::invokeMethod(this, "stopUpdateTimer");
}

void CLoadingWidgetItem::startUpdateTimer()
{
    if(m_timerID == -1){
        m_timerID = startTimer(15);
    }
}

void CLoadingWidgetItem::stopUpdateTimer()
{
    if (m_timerID != -1) {
        killTimer(m_timerID);
        m_timerID = -1;
    }
}
