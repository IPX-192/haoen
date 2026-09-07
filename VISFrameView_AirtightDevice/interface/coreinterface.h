/*
Author : Wangwei
Data    : 2025-04-18
Version : 1.0.1
*/

#ifndef MAINFRAMEINTERFACE_H
#define MAINFRAMEINTERFACE_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QQueue>
#include <QThreadPool>
#include <QMetaType>
#include <QMutex>

#include "plugininterface.h"

#ifndef __FILENAME__
#define __FILENAME__  (strrchr("\\" __FILE__, '\\') + 1)
#endif

#define  SystemLog (QString("system"))  //系统窗口
#define  DebugLog (QString("debug"))    //手动调试日志窗口注册类型使用

//系统日志
#define ShowSystemLog(level,info) \
    VisAppBus::postEvent("AddSyetemLog",SystemLog,level,info,QString("[%1:%2:%3]").arg(__FILENAME__).arg(__LINE__).arg(__FUNCTION__))
//组件运行日志(QString,int,LogLevel,QString)
#define ShowLog(type,index,level,info) \
    VisAppBus::postEvent("AddLogShow",type,index,level,info,QString("[%1:%2:%3]").arg(__FILENAME__).arg(__LINE__).arg(__FUNCTION__))
#define PrintLog(type,index,level,info) \
    VisAppBus::postEvent("PrintLog",type,index,level,info,QString("[%1:%2:%3]").arg(__FILENAME__).arg(__LINE__).arg(__FUNCTION__))
//程序状态(QString,QColor,bool)
#define ChangeExeStatus(info,color,enable) \
    VisAppBus::sendEvent("ExeStatus",info,color,enable)

//参数修改日志
#define AddParamChangeLog(info) \
    VisAppBus::sendEvent("ParamChange",info)

class UserInfo : public QObject
{
public:
    QString userName = u8"操作员";
    QString password = "";
    QString detail = "";
    QString createDatetime = "";
    AuthorityType authority= OPERATOR;
};

enum RunMode
{
    RM_VIEW,//生产版
    RM_TRIAL_VIEW,//试用版
    RM_OFFLINE, //离线版本
};

class QVisF_Interface : public QObject
{
    Q_OBJECT
 public:
    //运行模式
    RunMode runMode;

    //基于UI的Application对象
    QApplication* application;
    //=====================当前配置文件目录和组件目录
    QString systemID;
    QString systemName;
    QString systemVersion;
    QString organizationName;
    QString applicationDirPath;
    QString configDirPath;
    QString pluginDirPath;
    //================================================Users
    //用户信息
    QList<UserInfo*> userInfoLst;
    UserInfo curUserInfo;

    //所有系统组件集合
    QList<Plugin_Interface*> listPlugins;
    //所有组件日志窗口集合
    QList<PluginLogInfo> listPluginLog;

signals:
    int sig_OutputInfo(tagOutputInfo& info);

public slots:
    virtual int slot_InputInfo(tagOutputInfo& info) { return 0; }
};

//这个宏将一个给定的字符串标识符和ClassName所表示的接口相关联，其中Identifier必须唯一。
#define QCPF_Interface_iid "io.qt.interface.qvisfInterface"
Q_DECLARE_INTERFACE(QVisF_Interface, QCPF_Interface_iid)
#endif // MAINFRAMEINTERFACE_H
