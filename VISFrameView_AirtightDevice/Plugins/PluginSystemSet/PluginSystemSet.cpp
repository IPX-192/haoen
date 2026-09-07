#include "PluginSystemSet.h"
#include "WidgetSystemSet.h"
#include "WidgetMesLogin.h"
#include "VisAppBus.h"
#include "MesHttpPost.h"
#include "ParamManager.h"
PluginSystemSet::PluginSystemSet()
{
    pluginID = "PluginSystemSet";
    pluginVersion = "1.0.1";
    pluginAuther = "wangwei";
    pluginAuthority = OPERATOR;
    loadOrder = 0;
    showOrder = 4;
    VisAppBus::subscibeEvent(this, "ShowLogin");
    //MES接口总线
    VisAppBus::subscibeEvent(this, "MesValidateNumber");
    VisAppBus::subscibeEvent(this, "MesSaveProcessOpResult");
    VisAppBus::subscibeEvent(this, "MesCompleteTask");
    m_heartTimer.setInterval(5000);
    connect(&m_heartTimer, &QTimer::timeout, this, &PluginSystemSet::SlotPostHeartBeatTimeout);
    m_heartTimer.start();
}

void PluginSystemSet::InitWidgetList(Plugin_Interface *plugin)
{
    WidgetSystemSet* widget=new WidgetSystemSet();
    plugin->page=widget;
    plugin->icons << 0xf031 << 0xf036 << 0xf036;
    plugin->iconArea = QSize(40, 40);
    plugin->iconSize = 25;
    plugin->btnHeight = 45;
}

void PluginSystemSet::InitActionList(Plugin_Interface *plugin)
{

}

int PluginSystemSet::OnInitialized()
{
    WidgetSystemSet* widget = (WidgetSystemSet*)this->page;
    widget->LoadUIParam();
    return 0;
}


int PluginSystemSet::event_ShowLogin()
{
    //WidgetMesLogin widgetMesLogin;
   // QEventLoop* eventloop = new QEventLoop(this);

    if (m_widgetMesLogin == nullptr) {
        m_widgetMesLogin = new WidgetMesLogin();
        m_eventloop = new QEventLoop(this);
        connect(m_widgetMesLogin, &WidgetMesLogin::sigLogin, [&](QString name, int level) {
            this->frameCore->curUserInfo.userName = name;
            this->frameCore->curUserInfo.authority = (AuthorityType)level;
            m_eventloop->exit(0);
            });
        connect(m_widgetMesLogin, &WidgetMesLogin::sigCloseEvent, [&]() {
            exit(0);
        });
    }
    QString errMsg;
    m_widgetMesLogin->LoadAllWorkOrderData(errMsg);
    m_widgetMesLogin->show();
    m_widgetMesLogin->raise();          // 确保提到最前
    m_widgetMesLogin->activateWindow(); // 激活窗口获得焦点

    m_eventloop->exec();
    return 0;
}

//==================== MES接口总线 ====================
int PluginSystemSet::event_MesValidateNumber(QString sn, bool& outValidate)
{
    outValidate = false;
    // 免MES登录:不请求MES接口,直接校验通过
    //if (!MesHttpPost::IsMesLoggedIn()) {
        if (!GlobalParam->isMes) {
        outValidate = true;
        ShowSystemLog(Log_Info, QString(u8"免MES登录,跳过条码校验"));
        return 0;
    }
    //验证打印:请求实际地址(基址+接口名)
    ShowSystemLog(Log_Info, QString(u8"条码校验请求地址:%1validateNumber").arg(MesHttpPost::Instance()->GetMesBaseUrl()));
    //条码校验前先启动生产任务
    bool startResult = false;
    QString startErr = MesHttpPost::Instance()->StartProduction(sn, 0, startResult);   //boardNum连板数固定1
    if (!startErr.isEmpty() || !startResult) {
        ShowSystemLog(Log_Error, QString(u8"启动生产任务失败:%1").arg(startErr.isEmpty() ? u8"result=false" : startErr));
        return -1;
    }
    ShowSystemLog(Log_Info, QString(u8"启动生产任务成功:%1").arg(sn));
    QString errMsg = MesHttpPost::Instance()->ValidateNumber(sn, outValidate);
    if (!errMsg.isEmpty()) {
        ShowSystemLog(Log_Error, QString(u8"条码校验失败:%1").arg(errMsg));
        return -1;
    }
    ShowSystemLog(Log_Info, QString(u8"条码校验完成:%1,校验结果:%2").arg(sn).arg(outValidate ? u8"通过" : u8"不通过"));
    return 0;
}

int PluginSystemSet::event_MesSaveProcessOpResult(QString sn, int opResult, QList<DataDetail> detailAll, QString& outMainId, bool& outSuccess)
{
    outMainId.clear();
    outSuccess = true;   //免MES登录视为成功(跳过上传不NG)
    // 免MES登录:不请求MES接口
    if (!GlobalParam->isMes) {
        return 0;
    }
    QString errMsg = MesHttpPost::Instance()->SaveProcessOpResult(sn, opResult, detailAll, outMainId);
    if (!errMsg.isEmpty()) {
        outSuccess = false;   //HTTP失败:业务成败由errMsg决定,经输出参数返回
        ShowSystemLog(Log_Error, QString(u8"保存工序操作结果失败:%1").arg(errMsg));
        return -1;
    }
    ShowSystemLog(Log_Info, QString(u8"保存工序操作结果完成:%1,mainId:%2").arg(sn).arg(outMainId));
    return 0;
}

int PluginSystemSet::event_MesCompleteTask(QString sn, bool isSuccess, QString errCode, QString errInfo, bool bindMat, bool& outTaskResult)
{
    outTaskResult = false;
    // 免MES登录:不请求MES接口,直接返回OK
      if (!GlobalParam->isMes) {
        outTaskResult = true;
        return 0;
    }
    QString errMsg = MesHttpPost::Instance()->CompleteTask(sn, isSuccess, errCode, errInfo, bindMat, outTaskResult);
    if (!errMsg.isEmpty()) {
        ShowSystemLog(Log_Error, QString(u8"工序过站失败:%1").arg(errMsg));
        return -1;
    }
    ShowSystemLog(Log_Info, QString(u8"工序过站完成:%1,结果:%2").arg(sn).arg(outTaskResult ? u8"OK" : u8"NG"));
    return 0;
}

void PluginSystemSet::SlotPostHeartBeatTimeout()
{
    if (!GlobalParam->isMes) {
        return ;
    }
    QString heartBeatMsg = MesHttpPost::Instance()->StationHeartbeat();
    if(heartBeatMsg.isEmpty ())
    {
        return;
    }
    ShowSystemLog(Log_Error,heartBeatMsg);
}

