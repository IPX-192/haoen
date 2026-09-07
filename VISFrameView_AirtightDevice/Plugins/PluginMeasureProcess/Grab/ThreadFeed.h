#ifndef THREADFEED_H
#define THREADFEED_H

#include <QObject>
#include "ThreadGrabDef.h"
#include "hthread.h"
#include "ParamDef.h"
#include <atomic>

class ThreadBox;
class ThreadFeed : public QObject, public HThread
{
    Q_OBJECT
public:
    explicit ThreadFeed(TrayFunc type,QObject *parent = nullptr);
    ~ThreadFeed();

    void  InitParam();

protected:
    virtual bool doTask();
    bool       Process();
    int        GrabTrayModule();       //抓取托盘产品
    int        DetectTrayModule();     //视觉检测产品有无
    int        ScanCode();             //扫描二维码
    int        PlaceModule();          //放置产品到流水线载具
    int        MoveToTrayHole(int hole);
	int        SetGripClose(bool close);
    int        CheckGripModuleExist(bool& exist);

protected:
    const TrayFunc m_funcType;         
    QString   m_logType;
    QString   m_errInfo;
    ThreadBox* m_threadBox=nullptr;
	std::atomic<bool> m_waitFeedTray{true};    //等待产品托盘(跨线程:event_TrayReady/Process)
    std::atomic<bool> m_waitBlankTray{true};         //等待流水线载具(跨线程读写)
    std::atomic<bool> m_traySkip{false};           //跳过本次放置(托盘已流走)
    QVector<QString>m_vecFeedModule;        //产品条码队列
	int       m_grabIndex = 0;              //抓取托盘孔位序号
    TrayInfo  m_trayInfo;                   //流线载具信息

    uint64    m_trayTotalNum = 0;             //工作托盘计数
    const uint    m_indexHolderTrayWork = 5;   //壳体托盘开始作业序号

    std::atomic<bool> m_clearFlag{false};      //清料标志(跨线程:event_SetClearFlag/Process)
    bool      m_pendingClear = false;          //待清料标志(点清料时载具未到位,先记下等载具到位再清)
    uint64    m_okPCBTray = 0;
    bool      m_simulateNG = false;             //调试:模拟扫码NG(强制NG)
    uint64    m_emptyPCBTray = 0;              //清料状态空PCB盘计数
    bool      m_flagStopBlankHolder = true;    //清料状态停止下料壳体
    int       m_blankGrabCount = 0;            //上料PCB连续空抓次数(>=3触发清料)

protected:
    void   sigBlankTray(TrayFunc type);
    void   sigBlankPipeLineTray(TrayFunc type, TrayInfo info);
    void   sigHolderClearEnd();

public slots:
   int    event_TrayReady(TrayFunc type);
   int    event_PipeLineTrayReady(TrayFunc type, TrayInfo info);
   int    event_HolderClearEnd();
   int    event_SetClearFlag();
   int    event_SimulateNG(bool simulate);
};

#endif // THREADFEED_H
