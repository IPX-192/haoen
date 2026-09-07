#include "SwitchButton.h"
#include "qpainter.h"
#include "qevent.h"
#include "qtimer.h"
#include "qdebug.h"

SwitchButton::SwitchButton(QWidget *parent): QWidget(parent)
{
    checked = false;
    buttonStyle    = ButtonStyle_Rect;

    bgColorOff = QColor(190, 190, 190);//设置按钮内部颜色
    bgColorOn = QColor(102, 205, 170);

    sliderColorOff =QColor(255, 255, 255); //QColor(100, 100, 100);
    sliderColorOn = QColor(255, 255, 255); //QColor(100, 184, 255);

    textColorOff = QColor(255, 255, 255);
    textColorOn =  QColor(255, 255, 255); //QColor(10, 10, 10);

    textOff = u8"人工模式";//设置按钮中的文字
    textOn = u8"自动模式";

    imageOff = ":/image/btncheckoff1.png";
    imageOn = ":/image/btncheckon1.png";

    space = 2;
    rectRadius = 5;

    step = width() / 10;
    startX = 0;
    endX = 0;

    timer = new QTimer(this);
    timer->setInterval(20);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateValue()));

    setFont(QFont("Microsoft Yahei", 10));
}

SwitchButton::~SwitchButton()
{

}

void SwitchButton::ChangeState(bool flag)
{
    QMouseEvent *event = nullptr;
    mousePressEvent(event);
}

void SwitchButton::mousePressEvent(QMouseEvent *)
{
    checked = !checked;
    emit checkedChanged(checked);

    //每次移动的步长为宽度的 10分之一
    step = width() / 10;

    //状态切换改变后自动计算终点坐标
    if (checked) {
        endX = width() - height();
    } else {
        endX = 0;
    }

    timer->start();
}

void SwitchButton::resizeEvent(QResizeEvent *)
{
    //每次移动的步长为宽度的 10分之一
    step = width() / 10;

    //尺寸大小改变后自动设置起点坐标为终点
    if (checked) {
        startX = width() - height();
    } else {
        startX = 0;
    }

    update();
}

void SwitchButton::paintEvent(QPaintEvent *)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (buttonStyle == ButtonStyle_Image) {
        //绘制图片
        drawImage(&painter);
    } else {
        //绘制背景
        drawBg(&painter);
        //绘制滑块
        drawSlider(&painter);
        //绘制文字
        drawText(&painter);
    }
}

void SwitchButton::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    if (!checked) {
        painter->setBrush(bgColorOff);
    } else {
        painter->setBrush(bgColorOn);
    }

    if (buttonStyle == ButtonStyle_Rect)
    {
        painter->drawRoundedRect(rect(), rectRadius, rectRadius);
    }
    painter->restore();
}

void SwitchButton::drawSlider(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    if (!checked) {
        painter->setBrush(sliderColorOff);
    } else {
        painter->setBrush(sliderColorOn);
    }

    if (buttonStyle == ButtonStyle_Rect) {
        //int sliderWidth = width() / 2 - space * 2;
        int sliderHeight = height() - space * 2;
        QRect sliderRect(startX + space, space, sliderHeight , sliderHeight);
        painter->drawRoundedRect(sliderRect, rectRadius, rectRadius);
    }

    painter->restore();
}

void SwitchButton::drawText(QPainter *painter)
{
    painter->save();

    QFont font;
    font.setBold(true);
    font.setPointSize(11);
    painter->setFont(font);

    //滑块半径
    int sliderWidth = qMin(height(), width()) - space * 2 - 5;
    if (checked){
        QRect textRect(0, 0, width() - sliderWidth, height());
        painter->setPen(QPen(textColorOn));
        painter->drawText(textRect, Qt::AlignCenter, textOn);
    } else {
        QRect textRect(sliderWidth, 0, width() - sliderWidth, height());
        painter->setPen(QPen(textColorOff));
        painter->drawText(textRect, Qt::AlignCenter, textOff);
    }
    painter->restore();
}

void SwitchButton::drawImage(QPainter *painter)
{
    painter->save();

    QPixmap pix;

    if (!checked) {
        pix = QPixmap(imageOff);
    } else {
        pix = QPixmap(imageOn);
    }

    //自动等比例平滑缩放居中显示
    int targetWidth = pix.width();
    int targetHeight = pix.height();
    pix = pix.scaled(targetWidth, targetHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    int pixX = rect().center().x() - targetWidth / 2;
    int pixY = rect().center().y() - targetHeight / 2;
    QPoint point(pixX, pixY);
    painter->drawPixmap(point, pix);

    painter->restore();
}

void SwitchButton::updateValue()
{
    if (checked) {
        if (startX < endX) {
            startX = startX + step;
        } else {
            startX = endX;
            timer->stop();
        }
    } else {
        if (startX > endX) {
            startX = startX - step;
        } else {
            startX = endX;
            timer->stop();
        }
    }
    update();
}

void SwitchButton::setChecked(bool checked)
{
    if (this->checked != checked) {
        this->checked = checked;
        emit checkedChanged(checked);

        //每次移动的步长为宽度的 10分之一
        step = width() / 10;

        //状态切换改变后自动计算终点坐标
        if (checked) {
            endX = width() - height();
        } else {
            endX = 0;
        }

        timer->start();
        update();
    }
}

void SwitchButton::setButtonStyle(SwitchButton::ButtonStyle buttonStyle)
{
    this->buttonStyle = buttonStyle;
    update();
}

void SwitchButton::setBgColor(QColor bgColorOff, QColor bgColorOn)
{
    this->bgColorOff = bgColorOff;
    this->bgColorOn = bgColorOn;
    update();
}

void SwitchButton::setSliderColor(QColor sliderColorOff, QColor sliderColorOn)
{
    this->sliderColorOff = sliderColorOff;
    this->sliderColorOn = sliderColorOn;
    update();
}

void SwitchButton::setTextColor(QColor textColorOff, QColor textColorOn)
{
    this->textColorOff = textColorOff;
    this->textColorOn = textColorOn;
    update();
}

void SwitchButton::setText(QString textOff, QString textOn)
{
    this->textOff = textOff;
    this->textOn = textOn;
    update();
}

void SwitchButton::setImage(QString imageOff, QString imageOn)
{
    this->imageOff = imageOff;
    this->imageOn = imageOn;
    update();
}

void SwitchButton::setSpace(int space)
{
    this->space = space;
    update();
}

void SwitchButton::setRectRadius(int rectRadius)
{
    this->rectRadius = rectRadius;
    update();
}
