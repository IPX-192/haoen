#ifndef CTIMESELECTWIDGET_H
#define CTIMESELECTWIDGET_H

#include <QWidget>
#include <QDateTime>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QPropertyAnimation>

class RollingBox;
class CTimeSelectWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CTimeSelectWidget(QWidget *parent = nullptr);
    void setDateTime(const QDateTime & dateTime);
    QDateTime getDateTime(void) const { return m_dateTime; }

signals:
    void valueChangeSign(void);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void valueChangeSlot(int value);

private:
    static int monthToDay(int year, int month);
    void setMaxDay(int year, int month);

private:
    RollingBox * m_yearRolling;
    RollingBox * m_monthRolling;
    RollingBox * m_dayRolling;
    RollingBox * m_hourRolling;
    RollingBox * m_minRolling;
    RollingBox * m_secRolling;

    QLabel * m_ymLabel;
    QLabel * m_mdLabel;
    QLabel * m_hmLabel;
    QLabel * m_msLabel;

    QDateTime m_dateTime;
    int m_minYear = 1990, m_maxYear = 2090;
};

class RollingBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int deviation READ readDeviation WRITE setDeviation)
public:
    enum ShowDirection
    {
        SD_HORIZONTAL,
        SD_VERTICAL
    };
    explicit RollingBox(QWidget *parent = nullptr);

    void setRange(int min,int max);
    void setDirection(ShowDirection dir);
    int getValue(void) { return m_currentValue; }
    void setValue(int value);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

signals:
    void currentValueChanged(int value);
    void deviationChange(float deviation);

private:
    void paintNum(QPainter &painter,int num,int deviation);
    void paintLine(QPainter &painter);
    void homing(void);
    int readDeviation(void);
    void setDeviation(int n);

private:
    int m_minRange;
    int m_maxRange;
    int m_currentValue;
    ShowDirection m_showDirection;
    bool isDragging;
    int m_deviation;
    int m_mouseSrcPos;
    int m_numSize;//计算所得的数字字符尺寸，以最长的情况为准
    QPropertyAnimation *homingAni;
};

#endif // TIMEWIDGET_H
