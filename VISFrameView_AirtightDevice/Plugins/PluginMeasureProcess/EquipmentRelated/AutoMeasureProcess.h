#ifndef AUTOMEASUREPROCESS_H
#define AUTOMEASUREPROCESS_H

#include <QObject>
#include <QSharedPointer>
#include "ParamManager.h"
#include "ThreadWorkOrder.h"
#include "ThreadMonitor.h"

#define AutoMeasureProcessIns AutoMeasureProcess::instance()

class AutoMeasureProcess : public QObject
{
    Q_OBJECT
    SINGLETON_DECL(AutoMeasureProcess)
    public:
        void StartMonitor();

    void StopMonitor();

    bool InitRunParam(); //初始化运行参数

private:
    explicit AutoMeasureProcess(QObject *parent = nullptr);
    ~AutoMeasureProcess();

signals:

public slots:
    //设备复位
    int event_AutoReset();
    //启动
    int event_AutoStart();
    //停止
    int event_AutoStop();
    //暂停
    int event_AutoPause();
    //恢复
    int event_AutoResume();
    //报警清除
    int event_ClearWarning();
    //设备急停
    int event_AutoEmg();
    //刷新PLC数据
    int event_InitPlcData();

    int  event_NotifyTrayRequest(bool isOpen);
protected:
    std::shared_ptr<ThreadWorkOrder>m_threadWorkOrder;
    QSharedPointer<ThreadMonitor> m_threadMonitor;
};

#endif // AUTOMEASUREPROCESS_H
