#include "CTimeSelectWidget.h"

const int m_monthday[] = {31,28,31,30,31,30,31,31,30,31,30,31};

CTimeSelectWidget::CTimeSelectWidget(QWidget *parent) : QWidget(parent)
{
    this->setMinimumSize(405, 160);

    m_dateTime = QDateTime::currentDateTime();

    m_yearRolling = new RollingBox(this);
    m_yearRolling->setDirection(RollingBox::SD_VERTICAL);
    m_yearRolling->setRange(m_minYear, m_maxYear);
    m_yearRolling->setValue(m_dateTime.date().year());
    connect(m_yearRolling, &RollingBox::currentValueChanged, this, &CTimeSelectWidget::valueChangeSlot);

    m_ymLabel = new QLabel("-", this);
    m_ymLabel->show();

    m_monthRolling = new RollingBox(this);
    m_monthRolling->setDirection(RollingBox::SD_VERTICAL);
    m_monthRolling->setRange(1, 12);
    m_monthRolling->setValue(m_dateTime.date().month());
    connect(m_monthRolling, &RollingBox::currentValueChanged, this, &CTimeSelectWidget::valueChangeSlot);

    m_mdLabel = new QLabel("-", this);
    m_mdLabel->show();

    m_dayRolling = new RollingBox(this);
    m_dayRolling->setDirection(RollingBox::SD_VERTICAL);
    setMaxDay(m_dateTime.date().year(), m_dateTime.date().month());
    m_dayRolling->setValue(m_dateTime.date().day());
    connect(m_dayRolling, &RollingBox::currentValueChanged, this, &CTimeSelectWidget::valueChangeSlot);

    m_hourRolling = new RollingBox(this);
    m_hourRolling->setDirection(RollingBox::SD_VERTICAL);
    m_hourRolling->setRange(0, 23);
    m_hourRolling->setValue(m_dateTime.time().hour());
    connect(m_hourRolling, &RollingBox::currentValueChanged, this, &CTimeSelectWidget::valueChangeSlot);

    m_hmLabel = new QLabel(":", this);
    m_hmLabel->show();

    m_minRolling = new RollingBox(this);
    m_minRolling->setDirection(RollingBox::SD_VERTICAL);
    m_minRolling->setRange(0, 59);
    m_minRolling->setValue(m_dateTime.time().minute());
    connect(m_minRolling, &RollingBox::currentValueChanged, this, &CTimeSelectWidget::valueChangeSlot);

    m_msLabel = new QLabel(":", this);
    m_msLabel->show();

    m_secRolling = new RollingBox(this);
    m_secRolling->setDirection(RollingBox::SD_VERTICAL);
    m_secRolling->setRange(0, 59);
    m_secRolling->setValue(m_dateTime.time().second());
    connect(m_secRolling, &RollingBox::currentValueChanged, this, &CTimeSelectWidget::valueChangeSlot);
}

void CTimeSelectWidget::setDateTime(const QDateTime &dateTime)
{
    if (dateTime.date().year() < m_minYear || dateTime.date().year() > m_maxYear)
        return ;
    m_yearRolling->setValue(dateTime.date().year());
    m_monthRolling->setValue(dateTime.date().month());
    m_dayRolling->setValue(dateTime.date().day());
    m_hourRolling->setValue(dateTime.time().hour());
    m_minRolling->setValue(dateTime.time().minute());
    m_secRolling->setValue(dateTime.time().second());
}

void CTimeSelectWidget::resizeEvent(QResizeEvent *event)
{
    int xPos = 0;
    int height = event->size().height();
    int yearWidth = event->size().width() / 6;
    if (yearWidth < 70)
        yearWidth = 70;

    m_yearRolling->resize(yearWidth, height);
    m_yearRolling->move(xPos, 0);

    xPos += m_yearRolling->size().width();
    m_ymLabel->move(xPos + 5, (height / 2) - 10);

    xPos += 15;
    m_monthRolling->resize(yearWidth * 3 / 4, height);
    m_monthRolling->move(xPos, 0);

    xPos += m_monthRolling->size().width();
    m_mdLabel->move(xPos + 5, (height / 2) - 10);

    xPos += 15;
    m_dayRolling->resize(yearWidth * 3 / 4, height);
    m_dayRolling->move(xPos, 0);

    xPos += 15;
    xPos += m_dayRolling->size().width();
    m_hourRolling->resize(yearWidth * 3 / 4, height);
    m_hourRolling->move(xPos, 0);

    xPos += m_hourRolling->size().width();
    m_hmLabel->move(xPos + 5, (height / 2) - 10);

    xPos += 15;
    m_minRolling->resize(yearWidth * 3 / 4, height);
    m_minRolling->move(xPos, 0);

    xPos += m_minRolling->size().width();
    m_msLabel->move(xPos + 5, (height / 2) - 10);

    xPos += 15;
    m_secRolling->resize(yearWidth * 3 / 4, height);
    m_secRolling->move(xPos, 0);
}

void CTimeSelectWidget::valueChangeSlot(int value)
{
    RollingBox * box = static_cast<RollingBox *>(this->sender());
    if (box == m_yearRolling)
    {
        if (value == m_dateTime.date().year())
            return ;
        int month = m_dateTime.date().month();
        setMaxDay(value, month);
        int day   = m_dateTime.date().day();
        m_dateTime.setDate(QDate(value, month, day));
        emit valueChangeSign();
    }
    else if (box == m_monthRolling)
    {
        if (value == m_dateTime.date().month())
            return ;
        int year = m_dateTime.date().year();
        setMaxDay(year, value);
        int day   = m_dateTime.date().day();
        m_dateTime.setDate(QDate(year, value, day));
        emit valueChangeSign();
    }
    else if (box == m_dayRolling)
    {
        if (value == m_dateTime.date().day())
            return ;
        int year = m_dateTime.date().year();
        int month   = m_dateTime.date().month();
        m_dateTime.setDate(QDate(year, month, value));
        emit valueChangeSign();
    }
    else if (box == m_hourRolling)
    {
        if (value == m_dateTime.time().hour())
            return ;
        int min = m_dateTime.time().minute();
        int sec = m_dateTime.time().second();
        m_dateTime.setTime(QTime(value, min, sec));
        emit valueChangeSign();
    }
    else if (box == m_minRolling)
    {
        if (value == m_dateTime.time().minute())
            return ;
        int hour = m_dateTime.time().hour();
        int sec = m_dateTime.time().second();
        m_dateTime.setTime(QTime(hour, value, sec));
        emit valueChangeSign();
    }
    else if (box == m_secRolling)
    {
        if (value == m_dateTime.time().second())
            return ;
        int hour = m_dateTime.time().hour();
        int min = m_dateTime.time().minute();
        m_dateTime.setTime(QTime(hour, min, value));
        emit valueChangeSign();
    }
}

int CTimeSelectWidget::monthToDay(int year, int month)
{
    if (month == 2) {
        return (((year%4 == 0) && ((year%100 != 0) || (year%400 == 0)))? 29: 28);
    }else{
        return(m_monthday[month-1]);
    }
}

void CTimeSelectWidget::setMaxDay(int year, int month)
{
    int maxDay = monthToDay(year, month);
    m_dayRolling->setRange(1, maxDay);
    if (m_dateTime.date().day() > maxDay)
        m_dateTime.setDate(QDate(year, month, maxDay));
}


RollingBox::RollingBox(QWidget *parent) : QWidget(parent),
    m_minRange(0),
    m_maxRange(100),
    m_currentValue(50),
    m_showDirection(SD_HORIZONTAL),
    isDragging(false),
    m_deviation(0),
    m_mouseSrcPos(0),
    m_numSize(6)
{
    homingAni  = new QPropertyAnimation(this, "deviation");
    //定义动画持续时间
    homingAni->setDuration(300);
    //设置动画效果
    homingAni->setEasingCurve(QEasingCurve::OutQuad);
}

void RollingBox::setRange(int min, int max)
{
    m_minRange = min;
    m_maxRange = max;

    if(m_currentValue < min)
        m_currentValue = min;
    if(m_currentValue > max)
        m_currentValue = max;

    //计算字符尺寸
    m_numSize = 3;
    int temp = m_maxRange;
    while(temp > 0)
    {
        temp /= 10;
        m_numSize++;
    }

    repaint();
}

void RollingBox::setDirection(RollingBox::ShowDirection dir)
{
    m_showDirection = dir;
    repaint();
}

void RollingBox::setValue(int value)
{
    if (value > m_maxRange)
        value = m_maxRange;
    if (value < m_minRange)
        value = m_minRange;
    m_currentValue = value;
}

/*
 * 鼠标按住事件
 */
void RollingBox::mousePressEvent(QMouseEvent *e)
{
    homingAni->stop();
    isDragging = true;
    m_mouseSrcPos = (m_showDirection == SD_HORIZONTAL)? (e->pos().x()):(e->pos().y());
}

/*
 * 鼠标移动事件
 */
void RollingBox::mouseMoveEvent(QMouseEvent *e)
{
    if(!isDragging)
        return ;

    int pos = (m_showDirection == SD_HORIZONTAL)? (e->pos().x()) : (e->pos().y());
    int length = (m_showDirection == SD_HORIZONTAL)? (width()) : (height());

    m_deviation = pos - m_mouseSrcPos;

    //若移动速度过快时进行限制
    if(m_deviation > (length - 1) / 4)
        m_deviation = (length - 1) / 4;
    else if(m_deviation < -(length - 1) / 4)
        m_deviation = -(length - 1) / 4;

    emit deviationChange((float)m_deviation / ((length - 1) / 4));
    repaint();
}

/*
 * 鼠标松开事件
 */
void RollingBox::mouseReleaseEvent(QMouseEvent *)
{
    if(!isDragging)
        return ;
    isDragging = false;
    homing();
}

void RollingBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int measurement = (m_showDirection == SD_HORIZONTAL)? (width()-1) : (height()-1);

    if(m_deviation >= measurement/4)
    {
        m_mouseSrcPos   += measurement/4;
        m_deviation     -= measurement/4;
        m_currentValue  -= 1;
        if (m_currentValue < m_minRange)
            m_currentValue = m_maxRange;
    }
    if(m_deviation <= -measurement/4)
    {
        m_mouseSrcPos   -= measurement/4;
        m_deviation     += measurement/4;
        m_currentValue  += 1;
        if (m_currentValue > m_maxRange)
            m_currentValue = m_minRange;
    }

    //中间数字
    paintNum(painter, m_currentValue, m_deviation);

    //两侧数字1
    int temp = m_currentValue - 1;
    if (temp < m_minRange)
        temp = m_maxRange;
    paintNum(painter, temp, m_deviation - measurement / 4);
    temp = m_currentValue + 1;
    if (temp > m_maxRange)
        temp = m_minRange;
    paintNum(painter, temp, m_deviation + measurement / 4);

    //两侧数字2,超出则不显示
    if(m_deviation >= 0)
    {
        temp = m_currentValue - 2;
        if (temp == m_minRange - 1)
            temp = m_maxRange;
        else if (temp == m_minRange - 2)
            temp = m_maxRange - 1;
        else if (temp < m_minRange)
            temp = m_maxRange - 2;
        paintNum(painter, temp, m_deviation - measurement / 2);
    }
    if(m_deviation <= 0)
    {
        temp = m_currentValue + 2;
        if (temp == m_maxRange + 1)
            temp = m_minRange;
        else if (temp == m_maxRange + 2)
            temp = m_minRange + 1;
        else if (temp > m_maxRange)
            temp = m_minRange + 2;
        paintNum(painter, temp, m_deviation + measurement / 2);
    }

    //边框
    paintLine(painter);
}

/*
 * 绘制数字
 */
void RollingBox::paintNum(QPainter &painter,int num,int deviation)
{
    painter.save();

    int measurement = (m_showDirection == SD_HORIZONTAL)? (width()-1) : (height()-1);

    int size = (measurement - qAbs(deviation)) / m_numSize;
    //颜色透明度
    int transparency = 255 - 510 * qAbs(deviation) / measurement;
    int len = measurement / 2 - 3 * qAbs(deviation) / 4;
    int pos = measurement / 2 + deviation - len / 2;

    QFont font;
    font.setPixelSize(size);
    painter.setFont(font);
    painter.setPen(QColor(0, 0, 0, transparency));

    if (m_showDirection == SD_HORIZONTAL)
        painter.drawText(QRectF(pos, 0, len, height()-1), Qt::AlignCenter, QString::number(num));
    else
        painter.drawText(QRectF(0, pos, width()-1, len), Qt::AlignCenter, QString::number(num));

    painter.restore();
}

/*
 * 画线
 */
void RollingBox::paintLine(QPainter &painter)
{
    painter.save();

    QPen pen;
    pen.setBrush(QColor(0, 0, 0, 120));
    pen.setWidth(4);
    pen.setCapStyle(Qt::RoundCap);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    int space = 10;
    if(m_showDirection == SD_HORIZONTAL)
    {
        int up_down_space = height() / 10;
        QPoint left_up=QPoint(space + (width() - space * 2) / 3, up_down_space);
        QPoint left_down = QPoint(space + (width() - space * 2) / 3, height() - up_down_space);
        QPoint right_up = QPoint(space + ((width() - space * 2) / 3) * 2, up_down_space);
        QPoint right_down = QPoint(space + ((width() - space * 2) / 3) * 2, height() - up_down_space);
        painter.drawLine(left_up, left_down);
        painter.drawLine(right_up, right_down);
    }else{
        int up_down_space = width() / 10;
        QPoint left_up = QPoint(up_down_space, space + (height() - space * 2) / 3);
        QPoint left_down = QPoint(2 * space + (width() - space * 2) - up_down_space, space + (height() - space * 2) / 3);
        QPoint right_up = QPoint(up_down_space, space + ((height() - space * 2) / 3) * 2);
        QPoint right_down = QPoint(2 * space + (width() - space * 2) - up_down_space, space + ((height() - space * 2) / 3) * 2);
        painter.drawLine(left_up, left_down);
        painter.drawLine(right_up, right_down);
    }

    painter.restore();
}

/*
 * 将数字矫正到中心
 */
void RollingBox::homing()
{
    if(m_deviation > width()/8)
    {
        homingAni->setStartValue((width() - 1) / 8 - m_deviation);
        homingAni->setEndValue(0);
        m_currentValue--;
        if (m_currentValue < m_minRange)
            m_currentValue = m_maxRange;
    }
    else if(m_deviation > -width()/8)
    {
        homingAni->setStartValue(m_deviation);
        homingAni->setEndValue(0);
    }
    else if(m_deviation < -width()/8)
    {
        homingAni->setStartValue(-(width() - 1) / 8 - m_deviation);
        homingAni->setEndValue(0);
        m_currentValue++;
        if (m_currentValue > m_maxRange)
            m_currentValue = m_minRange;
    }
    emit currentValueChanged(m_currentValue);
    homingAni->start();
}

int RollingBox::readDeviation()
{
    return m_deviation;
}

void RollingBox::setDeviation(int n)
{
    m_deviation = n;
    repaint();
}
