#include "ThreadWorkOrder.h"
#include <QThread>
#include "PlcControl.h"
#include "VisAppThreadPool.h"
#include "VisMotorManager.h"

ThreadWorkOrder::ThreadWorkOrder(QObject *parent) : QObject(parent)
{
    setSleepPolicy(SLEEP_FOR,100);

    VisAppBus::subscibeEvent(this,"AritightFunTest");
}

ThreadWorkOrder::~ThreadWorkOrder()
{
    stop();
}

void ThreadWorkOrder::InitParam()
{
    m_statusExcute["ExcuteTrayScan"] = false;
    m_statusExcute["ExcuteProduceScan"] = false;
    m_statusExcute["ExcutePlaceStation"] = false;
    m_statusExcute["ExcuteAritightTest1"] = false;
    m_statusExcute["ExcuteAritightTest2"] = false;
    m_statusExcute["ExcuteOutTray"] = false;
}

bool ThreadWorkOrder::doTask()
{
    bool bRet = false;
    bRet = ExcuteScan();
    if (false == bRet)
    {
        return bRet;
    }

    return true;
}

bool ThreadWorkOrder::ExcuteScan()
{
    QString regName = "D9602";  //D9602~D9609
    int length = 8;
    QVector<qint32>blockValue;
    if (!PlcControl::Instance().ReadPageD(regName, length, blockValue))
        return false;
    ParseProduceScan(blockValue);
    ParseTrayScan(blockValue);
    ParseAritightTest(blockValue);
    ParseOutTray(blockValue);
    ParsePlaceStation(blockValue);

    return true;
}

void ThreadWorkOrder::ParseTrayScan(QVector<qint32> &blockValue)
{
    int flag = blockValue.mid(1,1).at(0);
    if(flag != 1 || m_statusExcute["ExcuteTrayScan"])
        return;

    m_statusExcute["ExcuteTrayScan"] = true;

    GlobalThreadPool->Commit(std::bind(&ThreadWorkOrder::ExcuteTrayScan,this));
}

void ThreadWorkOrder::ParseProduceScan(QVector<qint32> &blockValue)
{
    int flag = blockValue.mid(0,1).at(0);
    if(flag == false || m_statusExcute["ExcuteProduceScan"])
        return;

    m_statusExcute["ExcuteProduceScan"] = true;

    GlobalThreadPool->Commit(std::bind(&ThreadWorkOrder::ExcuteProduceScan,this));
}

void ThreadWorkOrder::ParsePlaceStation(QVector<qint32> &blockValue)
{
    bool flag = blockValue.mid(7,1).at(0);
    if(flag == false || m_statusExcute["ExcutePlaceStation"])
        return;

    m_statusExcute["ExcutePlaceStation"] = true;

    GlobalThreadPool->Commit(std::bind(&ThreadWorkOrder::ExcutePlaceStation,this));
}

void ThreadWorkOrder::ParseOutTray(QVector<qint32> &blockValue)
{
    int flag = blockValue.mid(5,1).at(0);
    if(flag != 1 || m_statusExcute["ExcuteOutTray"])
        return;

    m_statusExcute["ExcuteOutTray"] = true;

    GlobalThreadPool->Commit(std::bind(&ThreadWorkOrder::ExcuteOutTray,this));
}

void ThreadWorkOrder::ParseAritightTest(QVector<qint32> &blockValue)
{
    QVector<qint32> doubleBlockValue = blockValue.mid(2,2);

    for(int i = 0;i<doubleBlockValue.size();++i)
    {
        int flag = doubleBlockValue.at(i);
        QString testStation = QString("ExcuteAritightTest%1").arg(i+1);
        if(flag != 1 || m_statusExcute[testStation])
            continue;
        m_statusExcute[testStation] = true;
        GlobalThreadPool->Commit(std::bind(&ThreadWorkOrder::ExcuteAritightTest,this,i));
    }
}

void ThreadWorkOrder::ExcuteTrayScan()
{
    QString barCode;
    QString regName = "D9603";
    ShowLog(Log_Process,0,Log_Info,QStringLiteral("托盘扫码"));

    VisAppBus::sendEvent("GetBarCode",TrayScanCode,barCode);

    ShowLog(Log_Process,0,Log_Info,QStringLiteral("托盘二维码:%1").arg(barCode));

    AritightTask item;
    item.trayBarCode = barCode;
    if(barCode.isEmpty()){
        item.result = false;
        item.errorMsg = QStringLiteral("托盘扫码失败");
    }
    m_itemTask = item;

    PlcControl::Instance().WriteD(regName,item.result?2:1);
    m_statusExcute["ExcuteTrayScan"] = false;
}

void ThreadWorkOrder::ExcuteProduceScan()
{
    QString barCode;
    QString regName = "D9602";
    ShowLog(Log_Process,0,Log_Info,QStringLiteral("产品扫码"));
    VisAppBus::sendEvent("GetBarCode",ModuleScanCode,barCode);
    ShowLog(Log_Process,0,Log_Info,QStringLiteral("产品二维码:%1").arg(barCode));

    AritightTask item;
    item.barCode = barCode;
    if(barCode.isEmpty()){
        item.result = false;
        item.errorMsg = QStringLiteral("产品扫码失败");
    }
    item.trayBarCode = m_itemTask.trayBarCode;
    m_itemTask = item;

    PlcControl::Instance().WriteD(regName,item.result?2:1);

    m_statusExcute["ExcuteProduceScan"] = false;
}

void ThreadWorkOrder::ExcuteOutTray()
{
    QString regName = "D9607";
    VisAppBus::sendEvent("NotifyNextStation");
    m_statusExcute["ExcuteOutTray"] = false;
}

void ThreadWorkOrder::ExcutePlaceStation()
{
    QString barCode;
    QString regName = "D9609";
    QVector<int> blockValue;
    if (!PlcControl::Instance().ReadPageD(regName, 1, blockValue)){
        ShowSystemLog(Log_Error, QString(u8"工位查询失败,寄存器:%").arg(regName));
        return;
    }
    PlcControl::Instance().WriteD(regName,0);
    m_itemTask.station = blockValue.at(0);
    ShowLog(Log_Station,m_itemTask.station,Log_Info,QStringLiteral("%1放入测试工位").arg(m_itemTask.barCode));
    m_mapItemTask.insert(m_itemTask.station,m_itemTask);
    m_statusExcute["ExcutePlaceStation"] = false;
}

void ThreadWorkOrder::ExcuteAritightTest(int station)
{
    AritightTask item = m_mapItemTask.value(station);
    QString regName = station==0?"D9504":"D9505";
    QString testStation = QString("ExcuteAritightTest%1").arg(station+1);
    item.station = station;
    event_AritightFunTest(item);
    //记录最终结果
    PlcControl::Instance().WriteD(regName,item.result?1:2);

    if(!item.result)
    {
        //D7500NG满盘信息
        regName = "D7500";
        QVector<int> blockValue;
        if (!PlcControl::Instance().ReadPageD(regName, 1, blockValue)){
            ShowSystemLog(Log_Error, QString(u8"NG满盘信息失败,寄存器:%").arg(regName));
            return;
        }
        regName = "D4500";//NG物料计数
        if(blockValue.at(0) == 1)
        {
            QString msg = QString(u8"NG盘已满,请更换料盘后点击确认!");
            int flag = VisAppBus::sendEvent("ShowMessageBox",msg);
            if(flag == 1)
            {
                m_ngIndex = 0;
                PlcControl::Instance().WriteD(regName,m_ngIndex);
            }
            else
            {
                ShowSystemLog(Log_Error, QString(u8"未清理NG盘"));
                return;
            }
        }
        VisAppBus::sendEvent("NotifyTrayUpdate",2,m_ngIndex,item.errorMsg,false);
        m_ngIndex++;
    }
    m_statusExcute[testStation] = false;
}

int ThreadWorkOrder::event_AritightFunTest(AritightTask &item)
{
    ShowLog(Log_Station,item.station,Log_Info,QStringLiteral("%1启动测试").arg(item.barCode));
    item.startDateTime = QDateTime::currentDateTime();
    AritightTest(item.station,item);
    item.endDateTime = QDateTime::currentDateTime();
    VisAppBus::sendEvent("ProductTotal",item);
    //记录数据
    WriteLastCsv(item);
    ShowLog(Log_Station,item.station,Log_Info,QStringLiteral("%1测试结束").arg(item.barCode));
    return 0;
}

int ThreadWorkOrder::AritightTest(int station, AritightTask &item)
{
    //开始测试
    VisAppBus::sendEvent("ClearPlot",station);
    int res = VisAppBus::sendEvent("StartAirtightTest",station);
    if(res != 0){
        item.result = false;
        item.errorMsg = QStringLiteral("启动测试失败");
        item.errorCode = -1;
        ShowLog(Log_Station,station,Log_Error,item.errorMsg);
        return res;
    }
    int testStage = -1;
    //测试过程，并通知绘制曲线
    while (1)
    {
        res = VisAppBus::sendEvent("ReadAirtightRealResult",station,item);
        if(res == -1 || res == 2){//测试终止
            item.result = false;
            item.errorMsg = QStringLiteral("测试终止");
            item.errorCode = -2;
            ShowLog(Log_Station,station,Log_Error,item.errorMsg);
            return res;
        }

        if( res == 0)//测试结束
            break;

        //判断阶段
        if(item.TestStage != testStage)
            testStage = item.TestStage;
        else
            continue;
        //绘图
        VisAppBus::sendEvent("DrawAirtightPlot",station,item);

        //记录过程数据
        WriteItemCsv(item);
    }
    res = VisAppBus::sendEvent("ReadAirtightLastResult",station,item);
    if(res != 0 ) {
        ShowLog(Log_Station,station,Log_Error,QStringLiteral("读取最终数据失败"));
        return -1;
    }
    //绘图
    VisAppBus::sendEvent("DrawAirtightPlot",station,item);

    return 0;
}

void ThreadWorkOrder::WriteItemCsv(AritightTask &item)
{
    QString datapath = GlobalParam->systemParam.dataStorage.sProductSavePath;
    QString csvFileName = datapath+QString("/%1.csv").arg(item.barCode);
    QFile file(csvFileName);
    int res = FileFilter(file,item.station);
    if(res!=0)return;
    QTextStream out(&file);
    if(file.size()<1)
    {
        out<<QStringLiteral("二维码")<<",";
        out<<QStringLiteral("测试时间")<< ",";
        out<<QStringLiteral("治具")<<",";
        out<<QStringLiteral("阶段")<<",";
        out<<QStringLiteral("压力值")<<",";
        out<<QStringLiteral("泄漏值")<<",";
        out << "\n"; // 每行结束后换行
    }
    out << ("'"+item.barCode) << ",";
    out << QDateTime::currentDateTime().toString(DateTimeStr) << ",";
    out << GetTestStation(item.station) << ",";
    out << item.TestStage  << ",";
    out << item.pressValue  << ",";
    out << item.leakageValue  << ",";
    out << "\n"; // 每行结束后换行
    file.close();
}

void ThreadWorkOrder::WriteLastCsv(AritightTask &item)
{
    QString datapath = GlobalParam->systemParam.dataStorage.sProductSavePath;
    QString csvFileName = datapath+QString("/%1.csv").arg(item.barCode);
    QFile file(csvFileName);
    int res = FileFilter(file,item.station);
    if(res!=0)return;
    QTextStream out(&file);
    if(file.size()<1)
    {
        out<<QStringLiteral("二维码")<<",";
        out<<QStringLiteral("测试时间")<< ",";
        out<<QStringLiteral("治具")<<",";
        out<<QStringLiteral("阶段")<<",";
        out<<QStringLiteral("压力值")<<",";
        out<<QStringLiteral("泄漏值")<<",";
        out<<QStringLiteral("结果")<<",";
        out << "\n"; // 每行结束后换行
    }
    out << ("'"+item.barCode) << ",";
    out << QDateTime::currentDateTime().toString(DateTimeStr) << ",";
    out << GetTestStation(item.station) << ",";
    out << item.TestStage  << ",";
    out << item.pressValue  << ",";
    out << item.leakageValue  << ",";
    out << (item.result?QStringLiteral("OK"):QStringLiteral("NG"))<< ",";
    out << "\n"; // 每行结束后换行
    file.close();
}

int ThreadWorkOrder::FileFilter(QFile &file,int station,bool reWrite)
{
    QIODevice::OpenMode flags;
    if(reWrite)
    {
        flags = (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    }
    else
    {
        flags = (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
    }
    if (!file.open(flags))
    {
        QFileInfo info(file.fileName());
        QString fileErrMsg = QStringLiteral("请关闭%1文件,否则无法写入数据").arg(info.fileName());
        int res = VisAppBus::sendEvent("PopupWarning",fileErrMsg);
        if(res != 0)return res;
        else
        {
            if (!file.open(flags))
            {
                QString errorMsg = QStringLiteral("%1文件被外部占用,无法写入数据").arg(info.fileName());
                ShowLog(Log_Station,station,Log_Error,errorMsg);
                return -1;
            }
        }
    }
    return 0;
}

QString ThreadWorkOrder::GetTestStation(int index)
{
    QStringList listtest;
    listtest<<QStringLiteral("治具1")<<QStringLiteral("治具2");
    return listtest.at(index);
}
