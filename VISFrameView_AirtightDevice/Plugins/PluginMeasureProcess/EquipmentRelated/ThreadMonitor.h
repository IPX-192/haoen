#ifndef ThreadMonitor_H
#define ThreadMonitor_H

#include <QObject>
#include "hthread.h"
#include <QMutex>

class ThreadMonitor : public QObject, public HThread
{
    Q_OBJECT
public:
    explicit ThreadMonitor(QObject *parent = nullptr);
    ~ThreadMonitor();
signals:

protected:
    virtual bool doTask();

private:
    void ScanPlcIsHome();   //查询回零是否完成
    void ScanPlcRun();      //查询是否在自动生产状态

public slots:

};

#endif // ThreadMonitor_H
