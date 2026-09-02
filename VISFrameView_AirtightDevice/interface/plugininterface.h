/*
Author : Wangwei
Data    : 2025-04-18
Version : 1.0.1
*/

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QString>
#include <QObject>
#include <QVector>
#include <QVariant>
#include <QIcon>
#include <QPluginLoader>
#include <QTranslator>

class Plugin_Interface;

enum InfoType
{
    INFT_INITIALIZE_FINISHED,

    INFT_LOG_INFO,
    INFT_STATUS_INFO,

    INFT_MSG_INFO,
    INFT_MSG_WARN,
    INFT_MSG_ERROR,
    INFT_MSG_QUESTION_YES_NO,
    INFT_MSG_QUESTION_OK_CANCEL,
    INFT_MSG_QUESTION_RESULT,

    INFT_PLUGIN_COLLECT,
    INFT_PLUGIN_COLLECT_FINISHED,

    INFT_ProductChange,      //设备机种变化
    INFT_MACHINE_Auto,       //设备进入生产模式

    INFT_EXT_BASE,//从这条开始往后扩展
};

enum LogLevel
{
	Log_Info,      //文字白色
    Log_Debug,     //文字蓝色
    Log_Error,     //文字红色
    Log_MessageBox   //文字红色，加弹窗
};
Q_DECLARE_METATYPE(LogLevel)

enum AuthorityType
{
    PARAMADMIN,         //参数管理员
    ENGINEERADMIN,      //工程管理员
    OPERATOR,           //操作员
    SUPERADMIN,         //超级管理员
};

enum PluginType
{
    PT_SYS,
    PT_NON_SYS,
};

struct tagOutputInfo
{
    int _type;
    QString _title;
    QString _content;
    QVariant _data;
    tagOutputInfo(){}
    tagOutputInfo(InfoType type, QString title, QString content){ _type=type;_title=title;_content=content;}
};

typedef void (Plugin_Interface::*FPTR_ACTION)(bool);
struct PluginActionInfo
{
    QString _actionName;
    QString _actionDetail;
    QIcon _actionIcon;
    FPTR_ACTION _pAction;//函数指针指向组件函数
};

struct PluginWidgetInfo
{
    QWidget* _widget = nullptr;
    QString _widgetDetail;
};

using LogFunc = std::function<void(QString, LogLevel)>;
struct PluginLogInfo
{
    QString type;   //类型
    int index;
    LogFunc _pLog;//函数指针指向打印函数
};

class QVisF_Interface;
class Plugin_Interface:public QObject
{
    Q_OBJECT
public:
    int            loadOrder = 1;   //加载顺序
	int            showOrder = 1;   //显示顺序
    QString        pluginID;//组件ID，该属性是组件间唯一的
    PluginType     pluginType;//组件类型
    QString        pluginVersion;//组件版本
    QString        pluginAuther;//组件开发者
    QString        pluginComment;//组件功能说明
    AuthorityType  pluginAuthority;//组件的访问权限

    QList<PluginActionInfo*> listAction;   //组件菜单功能
    QVector<PluginWidgetInfo*> listWidget; //组件功窗口功能

    //显示窗口组件信息
    QWidget*       page = nullptr;
    int            btnHeight = 30;
    QList<int>     icons;
    QSize          iconArea = QSize(20, 20);
    int            iconSize = 16;
    //=====================

    QVisF_Interface* frameCore = nullptr;
    tagOutputInfo outputInfo;

signals:
    int sig_OutputInfo(tagOutputInfo& info);

public slots:
    virtual int  slot_InputInfo(tagOutputInfo& info) { return 0; }
    virtual void slotAction(bool checkState) { }

public:
    virtual bool ConnectCore(QObject* core) { return true; }
    virtual void InitSubscibeEvent(Plugin_Interface* plugin){}
    virtual void InitActionList(Plugin_Interface* plugin){}
    virtual void InitWidgetList(Plugin_Interface* plugin){}

    //PluginConnected之后，登录界面显示前，此时系统只收集了插件
    virtual int  OnCoreInitialized() { return 0; }
    //OnCoreInitialized之后，登录界面显示结束，
    virtual int  OnInitialized() { return 0; }
    //当QMainWindow主视图构造完成后，显示前
    virtual int  OnViewCreated() { return 0; }
    //当QMainWindow主视图Closeing时要执行的过程
    virtual int  OnViewClosing() { return 0; }
};

//这个宏将一个给定的字符串标识符和ClassName所表示的接口相关联，其中Identifier必须唯一。
#define Plugin_Interface_iid "io.qt.interface.plugin_interface"
Q_DECLARE_INTERFACE(Plugin_Interface, Plugin_Interface_iid)

#endif // PLUGININTERFACE_H
