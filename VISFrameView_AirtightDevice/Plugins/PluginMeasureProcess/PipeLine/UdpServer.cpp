#include "UdpServer.h"
#include <QApplication>
#include "VisAppBus.h"
#include "VisAppThreadPool.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"
#include "ParamManager.h"
#include <QTime>

using namespace  VisMotorToolSpace;

UdpServer::UdpServer(QObject *parent) : QObject(parent)
{
    m_initThreadID = std::this_thread::get_id();
    m_uSocket = new QUdpSocket(this);
    connect(m_uSocket, &QUdpSocket::readyRead, this, &UdpServer::slotUdpReceive);
    VisAppBus::subscibeEvent(this, "NotifyNextStation");
}


bool UdpServer::Start()
{
    m_existTray = true;
    m_uSocket->bind(UdpDevPort + 1, QUdpSocket::ShareAddress);
    return true;
}

bool UdpServer::SendUdpRequest(PipeLineCommandType type)
{
    QString udpdata = QString("%1_%2_").arg(Log_PipeLine).arg(type);
    if(type==PreExistTray)
        udpdata+=QString("%1").arg(m_existTray);
    if (std::this_thread::get_id() == m_initThreadID) {
        return slotPostData(udpdata.toLocal8Bit());
    }
    else {
        bool bRet = false;
        QMetaObject::invokeMethod(this, "slotPostData", Qt::BlockingQueuedConnection
                                  , Q_RETURN_ARG(bool, bRet)
                                  , Q_ARG(QByteArray, udpdata.toLocal8Bit()));
        return bRet;
    }

    return true;
}

bool UdpServer::slotPostData(QByteArray data)
{
    QString ip=GlobalParam->systemParam.nextDeviceIp;
    quint16 port=UdpDevPort;
    qint64 nRes = m_uSocket->writeDatagram(data, QHostAddress(ip), port);
    m_uSocket->flush();
    return true;
}

void UdpServer::slotUdpReceive()
{
    QByteArray ba;
    QHostAddress address;
    quint16 port;
    while(m_uSocket->hasPendingDatagrams()){
        ba.resize(m_uSocket->pendingDatagramSize());
        m_uSocket->readDatagram(ba.data(), ba.size(), &address, &port);
    }
    QString receiveData = QString::fromLocal8Bit(ba);
    QStringList listPipeLine;
    listPipeLine << Log_PipeLine;
    for (int i = 0; i < listPipeLine.size(); i++) {
        QString pipeLineName = listPipeLine.at(i);
        int index = receiveData.indexOf(pipeLineName);
        if (index != -1) {
            QString strCmd = receiveData.mid(index, receiveData.length() - index);
            QStringList cmdList = strCmd.split("_");
            if (cmdList.size() < 3)return;
            PipeLineCommandType type = (PipeLineCommandType)cmdList[1].toInt();
            if(type==RequestTray){
                m_nextRequest = cmdList[2].toInt();
                ShowLog(pipeLineName, 0, Log_Info, QString(u8"收到下一站新盘请求"));
                //VisAppBus::postEvent("RequestNewTray");
                VisAppBus::postEvent("NotifyTrayRequest",true);
            }
            else if(type==TrayArrive){
                if(!VisMotorDataInstance->m_running)continue;
                ShowLog(pipeLineName, 0, Log_Info, QString(u8"托盘已到下一站"));
                //VisAppBus::postEvent("NewTrayArrive");
                VisAppBus::postEvent("NotifyTrayRequest",false);
            }
        }
    }
}

int UdpServer::event_NotifyNextStation()
{
    ShowLog(Log_PipeLine, 0, Log_Info, QString(u8"通知下站接收料盘"));
    SendUdpRequest(PreExistTray);
    return 0;
}
