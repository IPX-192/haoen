#include "PCBCleanNode.h"
#include "VisAppBus.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"
#include "VisAppThreadPool.h"
#include "ParamManager.h"
#include "CylinderCtrl.h"

using namespace  VisMotorToolSpace;

PCBCleanNode::PCBCleanNode(ModuleInfo* item,int station)
{
	m_item = item;
	m_station = station;
}

int PCBCleanNode::Process()
{
    //功能屏蔽:转盘清洗被屏蔽则跳过整个流程
    if (GlobalParam->systemParam.shieldParam.turntableClean) {
        ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"转盘清洗已屏蔽,跳过清洗流程%1").arg(m_station + 1));
        return 0;
    }
	ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"开始PCB清洗流程%1").arg(m_station + 1));
	int nRes = GrabPCB();
	if(nRes != 0)return nRes;
	nRes = CleanPCB();
	if (nRes != 0)return nRes;
	nRes = PlacePCB();
	if (nRes != 0)return nRes;
	ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"PCB清洗结束%1").arg(m_station + 1));
	return 0;
}

int PCBCleanNode::GrabPCB()
{
	//夹爪上升
	int nRes = CylinderCtrl::instance()->event_SetCleanPCBGripUp(1, true);
	if (nRes != 0)return nRes;
	//张开夹爪
	nRes = SetGripClose(false);
	if (nRes != 0)return nRes;
	//到抓取位
	nRes = VisMotorInstance->MovePositionAbs(TurntableCleanPCB_Grab);
	if (nRes != 0)return nRes;
	//夹爪下降
	nRes = CylinderCtrl::instance()->event_SetCleanPCBGripUp(1, false);
	if (nRes != 0)return nRes;
	//关闭夹爪
	nRes = SetGripClose(true);
	if (nRes != 0)return nRes;
	//夹爪上升
	nRes = CylinderCtrl::instance()->event_SetCleanPCBGripUp(1, true);
	if (nRes != 0)return nRes;
	//检查模组
	bool exist = false;
	nRes = CheckGripModuleExist(exist);
	if (nRes != 0)return nRes;
	if (!exist) {
		QString errInfo = QString(u8"转盘清洗夹爪夹取后未检测到PCB");
		ShowLog(Log_Fixture, m_station, Log_Error, errInfo);
		QStringList listBtn;
		listBtn << u8"重新夹取" << u8"停止生产";
		nRes = VisAppBus::sendEvent("PopupUserMsgBox", listBtn, errInfo);
		if (nRes == 0) {
			nRes = GrabPCB();
			if (nRes != 0)return nRes;
		}
		else {
			VisAppBus::sendEvent("AutoEmg");
			return HardWareErr;
		}
	}
	return 0;
}

int PCBCleanNode::CleanPCB()
{
	//到抓取位
	int nRes = VisMotorInstance->MovePositionAbs(TurntableCleanPCB_Clean);
	if (nRes != 0)return nRes;
	//清洗指令
    nRes = VisMotorInstance->MotorMoveInc(MotorTurntableCleanGripR, 100);

	return 0;
}

int PCBCleanNode::PlacePCB()
{
	//到抓取位
	int nRes = VisMotorInstance->MovePositionAbs(TurntableCleanPCB_Grab);
	if (nRes != 0)return nRes;
	//夹爪下降
	nRes = CylinderCtrl::instance()->event_SetCleanPCBGripUp(1, false);
	if (nRes != 0)return nRes;
	//张开夹爪
	nRes = SetGripClose(false);
	if (nRes != 0)return nRes;
	//夹爪上升
	nRes = CylinderCtrl::instance()->event_SetCleanPCBGripUp(1, true);
	if (nRes != 0)return nRes;
	return 0;
}

int PCBCleanNode::SetGripClose(bool close)
{
	int nRes = 0;
	if (close) {
		//闭合夹爪:闭合位已进配方
		nRes = VisMotorInstance->MovePositionAbs(TurntableCleanGripClose);
		if (nRes != 0) {
            //临时:闭合到位误差不阻断流程(夹持结果由CheckGripModuleExist兜底)
            ShowLog(Log_Fixture, m_station, Log_Error, QString(u8"夹爪闭合运动返回%1,忽略继续流程").arg(nRes));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	else {
		nRes = VisMotorInstance->MovePositionAbs(TurntableCleanGripOpen);
		if (nRes != 0) return nRes;
	}
	return 0;
}

int PCBCleanNode::CheckGripModuleExist(bool& exist)
{
    if(GlobalParam->Debug_)
    {
        exist = true;
        return 0;
    }
	curSportState state;
	if (!VisMotorInstance->ReadCurSportState(MotorTurntableCleanGripX, state)) {
		QString errInfo = QString(u8"获取流线清洗夹爪夹取状态失败");
		ShowLog(Log_Fixture, m_station, Log_Error, errInfo);
		VisAppBus::sendEvent("PopupErrInfo", errInfo);
		return HardWareErr;
	}
	exist = (state == SPAORTSTOPANDNOCATCH);

	return 0;
}

