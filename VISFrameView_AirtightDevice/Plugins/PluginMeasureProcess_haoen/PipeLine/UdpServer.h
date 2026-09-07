#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <thread>
#include "PipeLineDef.h"

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = nullptr);

	bool  Start();

protected:
    bool  SendUdpRequest(PipeLineCommandType type);
    void  BeginTransTray();
protected:
    std::thread::id    m_initThreadID;
    QUdpSocket *m_uSocket;
	QTimer*m_timer;
    bool  m_existTray=false; //本站有盘
    bool  m_nextRequest=false; //下游请求托盘
    bool  m_transTray=false;

protected slots:
    void  slotUdpReceive();
    bool  slotPostData(QByteArray data);

signals:

public slots:
    int  event_NotifyNextStation();
};

#endif // UDPSERVER_H
