#ifndef CONVEYORROW_H
#define CONVEYORROW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include "VisMotorManager.h"
#include "coreinterface.h"
#include <QPainter>
#include <QScrollArea>

#pragma execution_character_set("utf-8")

// 状态指示灯类
class StatusLed : public QWidget
{
    Q_OBJECT
public:
    explicit StatusLed(QWidget *parent = nullptr) : QWidget(parent), m_state(false) {
        setFixedSize(20, 20);
    }

    void setState(bool on) {
        m_state = on;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        if (m_state) {
            painter.setBrush(Qt::green);
            painter.setPen(Qt::darkGreen);
        } else {
            painter.setBrush(Qt::gray);
            painter.setPen(Qt::darkGray);
        }
        painter.drawEllipse(rect().adjusted(1, 1, -1, -1));
    }

private:
    bool m_state;
};

// 按钮组类
class MotorBtnGroup : public QWidget
{
    Q_OBJECT
public:
    MotorBtnGroup(const QString &btnText, const QString &idText, const QString &iconChar, QWidget *parent = nullptr);

    QString getBtnText() const { return m_btn->text(); }
    QString getIdText() const { return idText; }

signals:
    void clicked();
    void pressed();
    void released();

private:
    QPushButton *m_btn;
    QString idText;
};

// 输送带行类
class ConveyorRow : public QWidget
{
    Q_OBJECT
public:
    struct Config {
        QString title;
        QString fwdId;    // 正转地址
        QString revId;    // 反转地址
        QString stopId;   // 停止地址

        QString readfwdId;
        QString readrevId;
        QString readstopId;
    };

    explicit ConveyorRow(const Config &config, QWidget *parent = nullptr);

private slots:
    void onForwardPressed();
    void onForwardReleased();
    void onReversePressed();
    void onReverseReleased();
//    void onStopClicked();

private:
    Config m_config;
    StatusLed *m_ledFwd;
    StatusLed *m_ledRev;
    MotorBtnGroup *btnFwd;
    MotorBtnGroup *btnRev;
    MotorBtnGroup *btnStop;
};

#endif // CONVEYORROW_H


