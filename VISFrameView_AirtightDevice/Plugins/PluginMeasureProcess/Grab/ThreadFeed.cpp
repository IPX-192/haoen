#include "ThreadFeed.h"
#include "VisAppBus.h"
#include "ParamManager.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"
#include "VisCameraTool.h"
#include "VisDMFind.h"
#include "CylinderCtrl.h"
#include "opencv2/opencv.hpp"
#include "VisAppThreadPool.h"
#include "ThreadBox.h"
#include <functional>
#include <QDebug>

using namespace  VisMotorToolSpace;

ThreadFeed::ThreadFeed(TrayFunc type,QObject *parent) :
    m_funcType(type),
    QObject(parent)
{
    m_logType=(m_funcType==FeedHolder)?Log_GrabHolder:Log_GrabPCB;
    m_threadBox = new ThreadBox(type,this);
    VisAppBus::subscibeEvent(this, "TrayReady");
    VisAppBus::subscibeEvent(this, "PipeLineTrayReady");
    VisAppBus::subscibeEvent(this, "SetClearFlag");
    VisAppBus::subscibeEvent(this, "HolderClearEnd");
    VisAppBus::subscibeEvent(this, "SimulateNG");
}

ThreadFeed::~ThreadFeed()
{
    delete m_threadBox;
    stop();
}

void ThreadFeed::InitParam()
{
    m_errInfo="";
    m_trayTotalNum = 0;
    m_clearFlag = false;
    m_pendingClear = false;
    IOLevel level1 = VisMotorInstance->GetIoInput((m_funcType == FeedHolder) ? IN_HolderTrayTilt1:IN_PCBTrayTilt1);
    IOLevel level2 = VisMotorInstance->GetIoInput((m_funcType == FeedHolder) ? IN_HolderTrayTilt2:IN_PCBTrayTilt2);

    m_waitFeedTray = (level1 == IO_OFF);
    m_waitBlankTray = true;
    m_grabIndex = 0;
    m_flagStopBlankHolder = false;
    m_okPCBTray = 0;
    m_emptyPCBTray = 0;
    m_blankGrabCount = 0;
    m_vecFeedModule.clear();
    m_threadBox->InitParam();

    start();
}

bool ThreadFeed::doTask()
{
    if (VisMotorInstance->IsEmgStop())return false;
    if (!Process()) {
        if (!m_errInfo.isEmpty())
            ShowSystemLog(Log_Error, m_errInfo);
        return false;
    }
    return true;
}

bool ThreadFeed::Process()
{
    //等待托盘/载具:载具未到上料位时不抓料(等下游要料期间不消耗料盘,不需要上新盘)
    if (m_waitFeedTray) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return true;
    }
    //清料模式:PCB夹爪结束找料,上台到安全位,退出本线程抓取循环(不再逐格检查)
    if (m_funcType == FeedPCB && m_clearFlag) {
        ShowLog(m_logType, 0, Log_Info, QString(u8"清料模式:PCB夹爪结束找料,移到安全位,停止抓取"));
        int nRes = VisMotorInstance->MovePositionAbs(PCBGripSafe);
        if (nRes != 0) {
            m_errInfo = QString(u8"清料安全位运动失败:%1").arg(nRes);
            return false;
        }
        m_waitFeedTray = true;
        return true;
    }
    //从料盘抓料
    int nRes = GrabTrayModule();
    if (nRes != 0) return false;
    if (!m_vecFeedModule.size())return true;
    //放置上料模组到测试位(PCB扫码在PlaceModule放置完成后执行,壳体扫码已屏蔽)
    nRes = PlaceModule();
    if (nRes != 0)return false;

    return true;
}

int ThreadFeed::GrabTrayModule()
{
    m_vecFeedModule.clear();
    int nRes=-1;
    ShowLog(m_logType, 0, Log_Info, QString(u8"开始抓取托盘模组%1").arg(m_grabIndex + 1));
    //检测托盘模组
    nRes = DetectTrayModule();
    if (nRes != 0) return nRes;
    //打开夹爪
    nRes = SetGripClose(false);
    if (nRes != 0)return nRes;
    //夹爪运动到托盘孔位
    nRes = MoveToTrayHole(m_grabIndex);
    if (nRes != 0)return nRes;
    //闭合夹爪
    nRes = SetGripClose(true);
    if (nRes != 0)return nRes;
    //移动到安全位
    nRes = VisMotorInstance->MovePositionAbs((m_funcType == FeedHolder) ? HolderGripSafe : PCBGripSafe);
    if (nRes != 0)return nRes;
    //检查夹爪模组,二次检查防止夹取后掉落
    bool exist=false;
    nRes = CheckGripModuleExist(exist);
    if (nRes != 0)return nRes;
    TrayFunc sendFunc = m_funcType;
    QString holeText = exist ? u8"空" : u8"无料";
    // 抓取后物料取走，blankFlag=true，走灰色空仓逻辑；ok=exist 区分夹到料(空)与空夹(无料)
    VisAppBus::sendEvent("TrayHoleUpdate", sendFunc, true, exist, m_grabIndex, holeText);
    m_grabIndex++;
    //检查是否托盘最后一个孔
    int totalHole = (m_funcType == FeedHolder) ? GlobalParam->recipeTray.feedTrayPosHolder.size() : GlobalParam->recipeTray.feedTrayPosPCB.size();
    if (m_grabIndex >= totalHole) {
        sigBlankTray(m_funcType);
    }
    if (!exist) {
          //上料PCB连续空抓3次触发清料(壳体不触发)
          if (m_funcType == FeedPCB) {
              m_blankGrabCount++;
              ShowLog(m_logType, 0, Log_Info, QString(u8"上料PCB空抓(连续%1次)").arg(m_blankGrabCount));
              if (m_blankGrabCount >= 3) {
                  m_blankGrabCount = 0;
                  ShowLog(m_logType, 0, Log_Error, QString(u8"上料PCB连续空抓3次,触发清料"));
                  VisAppBus::sendEvent("SetClearFlag");
              }
          }
          return 0;
      }
      //抓到料:空抓计数清零
    m_blankGrabCount = 0;
    m_vecFeedModule.push_back("");
    return 0;
}

int ThreadFeed::DetectTrayModule()
{
    return 0;
    RecipeTray& recipeTray = GlobalParam->recipeTray;
    QVector4D ptGrip = (m_funcType==FeedHolder)? recipeTray.feedTrayPosHolder.at(m_grabIndex): recipeTray.feedTrayPosPCB.at(m_grabIndex);
    QMap<QString, double> mapGroup;
    mapGroup[(m_funcType == FeedHolder) ? MotorHolderGantryX : MotorPCBGantryX] = ptGrip.x() + GlobalParam->hardwareParam.gripToCamX;
    mapGroup[(m_funcType == FeedHolder) ? MotorHolderGantryY : MotorPCBGantryY] = ptGrip.y() + GlobalParam->hardwareParam.gripToCamY;
    int nRes = VisMotorInstance->MoveAbsGroup(mapGroup);
    if (nRes != 0) return nRes;
    cv::Mat  img;
    if (!VisMotorDataInstance->m_flagOffline) {
        //		nRes = VisCameraTool::instance()->GrabImgFrame(0, PoseCam, img);
        //		if (nRes != 0) {
        //			m_errInfo = PoseCam + QString(u8"相机抓图失败:%1").arg(nRes);
        //			ShowLog(m_logType, 0, Log_Error, QString(u8"获取托盘码"));
        //			return nRes;
        //		}
    }
    else {

    }

    return 0;
}

int ThreadFeed::ScanCode()
{
    //移动到安全位
    int nRes = VisMotorInstance->MovePositionAbs((m_funcType == FeedHolder) ? HolderGripSafe : PCBGripSafe);
    if (nRes != 0)return nRes;

    //到扫码位
    nRes = VisMotorInstance->MovePositionAbs((m_funcType == FeedHolder) ? HolderScanCode : PCBScanCode);
    if (nRes != 0) return nRes;
    ShowLog(m_logType, 0, Log_Info, QString(u8"扫描产品条码"));
    QString barCode;
    if (m_funcType == FeedPCB) {
        //PCB扫码:相机拍图+DataMatrix识别(VisDataMatrixFind库)
        cv::Mat img;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        int nGrab = VisCameraTool::instance()->GrabImgFrame(0, FeedPCBCam, img);
        if (nGrab == 0 && !img.empty()) {
            //自动流程扫码图像同步到WidgetProdutData对位图像显示(station0=治具1)
            VisAppBus::postEvent("ShowCalibImage", 0, img);
            RecipeScanCode& scanRecipe = GlobalParam->recipeScanCode;
            cv::Rect roi(scanRecipe.roiX, scanRecipe.roiY, scanRecipe.roiW, scanRecipe.roiH);
            VisDMFind dmFind;
            std::string info;
            int nRet = dmFind.FindDM(img, roi, info);
            if (nRet == 0 && !info.empty()) {
                barCode = QString::fromStdString(info).trimmed();
                ShowLog(m_logType, 0, Log_Info, QString(u8"PCB DataMatrix识别:%1").arg(barCode));
            }
            else {
                ShowLog(m_logType, 0, Log_Error, QString(u8"PCB DataMatrix识别失败:%1(0正常,-1图像空,-2ROI超限,-3未识到)").arg(nRet));
            }
        }
        else {
            ShowLog(m_logType, 0, Log_Error, QString(u8"PCB扫码相机抓图失败:%1").arg(nGrab));
        }
    }
    else {
        //壳体扫码已屏蔽
        QString scanName = ScanHolderCode;
        VisAppBus::sendEvent("GetBarCode", scanName, barCode);
    }
    m_vecFeedModule[0] = barCode;


    //PCB扫码后发送MES条码校验
    if (m_funcType == FeedPCB && !barCode.isEmpty()) {
        bool validate = false;
        //启动生产任务，
        int vRes = VisAppBus::sendEvent("MesValidateNumber", barCode, validate);
         if(!validate)
         {
             m_trayInfo.result = false;
             m_trayInfo.errInfo = u8"条码校验NG_1_1";   //MES条码校验失败NG(后缀:固定_1 + PCB码_1)
         }
        ShowLog(m_logType, 0, vRes == 0 ? Log_Info : Log_Error,
                QString(u8"PCB条码校验%1:%2").arg(vRes == 0 ? u8"完成" : u8"失败").arg(validate ? u8"通过" : u8"不通过"));
    }

    return 0;
}

int ThreadFeed::PlaceModule()
{
    QMap<QString, double> moveGroup = VisMotorDataInstance->GetPosMap((m_funcType == FeedHolder) ? HolderPlace : PCBPlace);
    double moveZ = moveGroup[(m_funcType == FeedHolder) ? MotorHolderGantryZ : MotorPCBGantryZ];
    moveGroup.remove((m_funcType == FeedHolder) ? MotorHolderGantryZ : MotorPCBGantryZ);
    //执行放置点位xyr
    int nRes = VisMotorInstance->MoveAbsGroup(moveGroup);
    if (nRes != 0) return nRes;
    ShowLog(m_logType, 0, Log_Info, QString(u8"查询是否等待载具"));
    //检查是=是否处于等待托盘状态
    while (1) {
        if (!m_waitBlankTray) break;
        if(VisMotorInstance->IsEmgStop())return HardWareErr;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    //托盘已流走(空壳体),不执行放置,回到Process循环等下一盘
    if (m_traySkip) {
        m_traySkip = false;
        sigBlankPipeLineTray(m_funcType, m_trayInfo);
        return 0;
    }
    ShowLog(m_logType, 0, Log_Info, QString(u8"开始放置到流水线托盘"));
    //壳体上料:伸出接驳台固定托盘,防止放置时移位
    if (m_funcType == FeedHolder) {
        VisMotorInstance->SetDoubleIoOutput(Out_TransferExend, IO_ON, Out_TransferBack, IO_OFF);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    //执行放置点位Z
    nRes = VisMotorInstance->MotorMoveAbs((m_funcType == FeedHolder) ? MotorHolderGantryZ : MotorPCBGantryZ, moveZ);
    if (nRes != 0) return nRes;
    //松开夹爪
    nRes = SetGripClose(false);
    if (nRes != 0) return nRes;
    //移动到安全位
    nRes = VisMotorInstance->MovePositionAbs((m_funcType == FeedHolder) ? HolderGripSafe : PCBGripSafe);
    if (nRes != 0)return nRes;
    //检查模组
    bool exist=false;
    nRes = CheckGripModuleExist(exist);
    if (nRes != 0)return nRes;
    if (GlobalParam->flagOffline || GlobalParam->emptyRun || GlobalParam->Debug_)
        exist = false;
    if(exist){
        m_errInfo = QString(u8"%1夹爪松开后仍检测到模组").arg((m_funcType == FeedHolder) ? u8"壳体" : u8"PCB");
        ShowLog(m_logType, 0, Log_Error, m_errInfo);
        VisAppBus::sendEvent("PopupErrInfo", m_errInfo);
        return HardWareErr;
    }
    ShowLog(m_logType, 0, Log_Info, QString(u8"放置完毕"));
    //壳体上料完毕:缩回接驳台,托盘流走
    if (m_funcType == FeedHolder) {
        VisMotorInstance->SetDoubleIoOutput(Out_TransferBack, IO_ON, Out_TransferExend, IO_OFF);
    }
    //PCB放置完成后扫码
    if (m_funcType == FeedPCB) {
        nRes = ScanCode();
        if (nRes != 0)return nRes;
    }
    //发送流走托盘请求
    m_trayInfo.empty = false;
    //壳体无二维码,扫码屏蔽,holderBarCode给假数据占位;PCB条码取扫码结果
    if (m_funcType == FeedHolder)
        m_trayInfo.holderBarCode = QString("Holder%1").arg(m_grabIndex);
    else {
        m_trayInfo.pcbBarCode = m_vecFeedModule[0];
        //排查日志:上料写入载具条码
        ShowLog(m_logType, 0, Log_Info, QString(u8"PCB上料写入载具条码:[%1]").arg(m_trayInfo.pcbBarCode));
        //扫码失败:条码为空→result=false,errInfo=扫码失败
         if (m_vecFeedModule[0].isEmpty()) {
            m_trayInfo.result = false;
            m_trayInfo.errInfo = u8"扫码失败_1_1";
            ShowLog(m_logType, 0, Log_Error, QString(u8"PCB扫码失败,托盘标记NG"));
        }
        else {
            m_okPCBTray++;   //OK的PCB托盘计数,NG不计数
        }
    }
    sigBlankPipeLineTray(m_funcType, m_trayInfo);
    return 0;
}

int ThreadFeed::MoveToTrayHole(int hole)
{
    QMap<QString, double> mapGroup;
    QVector4D pos = (m_funcType == FeedHolder) ? GlobalParam->recipeTray.feedTrayPosHolder.at(hole) : GlobalParam->recipeTray.feedTrayPosPCB.at(hole);
    mapGroup[(m_funcType == FeedHolder) ? MotorHolderGantryX : MotorPCBGantryX] = pos.x();
    mapGroup[(m_funcType == FeedHolder) ? MotorHolderGantryY : MotorPCBGantryY] = pos.y();
    mapGroup[(m_funcType == FeedHolder) ? MotorHolderGripR : MotorPCBGripR] = pos.w();
    int nRes=VisMotorInstance->MoveAbsGroup(mapGroup);
    if (nRes != 0) return nRes;
    nRes = VisMotorInstance->MotorMoveAbs((m_funcType == FeedHolder) ? MotorHolderGantryZ : MotorPCBGantryZ,pos.z());
    if (nRes != 0) return nRes;

    return 0;
}

int ThreadFeed::SetGripClose(bool close)
{
    int nRes = 0;
    if (close) {
        //闭合夹爪:闭合位已进配方,按夹爪型号取闭合点位
        nRes = VisMotorInstance->MovePositionAbs((m_funcType == FeedHolder) ? HolderGripClose : PCBGripClose);
        if (nRes != 0) {
            //临时:闭合到位误差不阻断流程(夹持结果由CheckGripModuleExist兜底)
            ShowLog(m_logType, 0, Log_Error, QString(u8"夹爪闭合运动返回%1,忽略继续流程").arg(nRes));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    else {
        nRes = VisMotorInstance->MovePositionAbs((m_funcType == FeedHolder) ? HolderGripOpen : PCBGripOpen);
        if (nRes != 0) return nRes;
    }

    return 0;
}

int ThreadFeed::CheckGripModuleExist(bool& exist)
{
    if(m_funcType == FeedHolder && GlobalParam->holderDebug)
    {
        exist = true;
        return 0;
    }
    if (GlobalParam->flagOffline|| GlobalParam->emptyRun) {
        exist = true;
        return 0;
    }

    curSportState state;
    if (!VisMotorInstance->ReadCurSportState((m_funcType == FeedHolder) ? MotorHolderGripX : MotorPCBGripX, state)) {
        m_errInfo = QString(u8"获取上料%1夹取状态失败").arg((m_funcType == FeedHolder) ? u8"壳体" : u8"PCB");
        ShowSystemLog(Log_Error, m_errInfo);
        VisAppBus::sendEvent("PopupErrInfo", m_errInfo);
        return HardWareErr;
    }
    exist = (state == SPAORTSTOPANDNOCATCH);
    ShowLog(m_logType, 0, Log_Debug, QString(u8"%1夹爪夹取状态:state=%2,exist=%3")
        .arg((m_funcType == FeedHolder) ? u8"壳体" : u8"PCB").arg((int)state).arg(exist));

    return 0;
}

void ThreadFeed::sigBlankTray(TrayFunc type)
{
    ShowLog(m_logType, 0, Log_Info, QString(u8"请求退空托盘"));
    m_grabIndex = 0;
    m_waitFeedTray=true;
    VisAppBus::sendEventDirect("BlankTray",type);
}

void ThreadFeed::sigBlankPipeLineTray(TrayFunc type, TrayInfo info)
{
    m_waitBlankTray=true;
    VisAppBus::sendEventDirect("BlankPipeLineTray", type, info);
}

void ThreadFeed::sigHolderClearEnd()
{
    VisAppBus::sendEventDirect("HolderClearEnd");
}

int ThreadFeed::event_TrayReady(TrayFunc type)
{
    if (m_funcType != type)return 0;
    m_waitFeedTray = false;
    return 0;
}

int ThreadFeed::event_PipeLineTrayReady(TrayFunc type, TrayInfo info)
{
    if (m_funcType != type)return 0;
    m_trayTotalNum++;
    if (m_funcType == FeedHolder) {
        //前4个托盘不放壳体，或清料停止放壳体
        if (m_trayTotalNum < m_indexHolderTrayWork || m_flagStopBlankHolder) {
            sigBlankPipeLineTray(type, info);
            return 0;
        }
    }
    else if (m_funcType == FeedPCB) {
        //从第5个托盘开始，空盘空壳体直接退盘
        if (info.holderBarCode.isEmpty()&& m_trayTotalNum>= m_indexHolderTrayWork) {
            sigBlankPipeLineTray(type, info);
            return 0;
        }
        //清料模式后，流走清料盘(有壳体无PCB),数量=min(4, OK PCB托盘数),达到后告知上料壳体模块不再放壳体
        if (m_clearFlag) {
            m_emptyPCBTray++;
            uint64 maxClear = m_okPCBTray > 4 ? 4 : m_okPCBTray;
            if (m_emptyPCBTray >= maxClear) {
                m_clearFlag = false;
                sigHolderClearEnd();
            }
            sigBlankPipeLineTray(type, info);
            return 0;
        }
    }

    m_trayInfo = info;
    m_waitBlankTray=false;
    //待清料:载具到位,立即触发清料
    if (m_pendingClear) {
        m_pendingClear = false;
        return event_SetClearFlag();
    }
    return 0;
}

int ThreadFeed::event_HolderClearEnd()
{
    m_flagStopBlankHolder = true;
    return 0;
}

int ThreadFeed::event_SetClearFlag()
{
    ShowLog(m_logType, 0, Log_Info, QString(u8"收到清料事件:funcType=%1,clearFlag=%2,waitBlankTray=%3")
        .arg((int)m_funcType).arg(m_clearFlag).arg(m_waitBlankTray));
    if (m_funcType != FeedPCB) {
        ShowLog(m_logType, 0, Log_Info, QString(u8"清料忽略:funcType=%1非FeedPCB").arg((int)m_funcType));
        return 0;
    }
    if (m_clearFlag) {
        ShowLog(m_logType, 0, Log_Info, QString(u8"清料忽略:已在清料状态"));
        return 0;
    }
    //当前上料PCB流线载具未到位:先记下待清料,等载具到位后再触发(避免点清料时静默失败)
    if (m_waitBlankTray) {
        m_pendingClear = true;
        ShowLog(m_logType, 0, Log_Debug, QString(u8"上料PCB流线载具未到位,记录待清料"));
        return 0;
    }
    m_waitBlankTray = true;
    m_clearFlag = true;
    m_emptyPCBTray = 1;
    ShowLog(m_logType, 0, Log_Info, QString(u8"触发清料成功:clearFlag=true,发下料空载具信号"));
    sigBlankPipeLineTray(m_funcType, m_trayInfo);

    return 0;
}

int ThreadFeed::event_SimulateNG(bool simulate)
{
    if (m_funcType != FeedPCB)return 0;
    m_simulateNG = simulate;
    ShowLog(m_logType, 0, Log_Info, QString(u8"调试模拟扫码NG:%1").arg(simulate ? u8"开启" : u8"关闭"));
    return 0;
}
