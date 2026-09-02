#ifndef THREADWORKORDER_H
#define THREADWORKORDER_H

#include <QObject>
#include "hthread.h"
#include "ParamDef.h"
#include <QFile>
#include <QFileInfo>

class ThreadWorkOrder : public QObject, public HThread
{
    Q_OBJECT
public:
    explicit ThreadWorkOrder(QObject *parent = nullptr);
    ~ThreadWorkOrder();
    void InitParam();
signals:

public slots:
    int event_AritightFunTest(AritightTask &item);
protected:
    virtual bool doTask();
    bool ExcuteScan();

    void ParseTrayScan(QVector<qint32> &blockValue);
    void ParseProduceScan(QVector<qint32> &blockValue);
    void ParsePlaceStation(QVector<qint32> &blockValue);
    void ParseOutTray(QVector<qint32> &blockValue);
    void ParseAritightTest(QVector<qint32> &blockValue);

    void ExcuteTrayScan();
    void ExcuteProduceScan();
    void ExcuteOutTray();
    void ExcutePlaceStation();
    void ExcuteAritightTest(int station);

    int AritightTest(int station,AritightTask &item);

    void WriteItemCsv(AritightTask &item);
    void WriteLastCsv(AritightTask &item);

    int FileFilter(QFile &file, int station, bool reWrite = false);
    QString GetTestStation(int index);
private:
    QMap<QString, bool>m_statusExcute;
    AritightTask m_itemTask;
    QMap<int,AritightTask> m_mapItemTask;
    int m_ngIndex = 0;
};

#endif // THREADWORKORDER_H
