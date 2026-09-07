#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "FilmTearNode.h"
#include "VisAppBus.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"
#include "VisAppThreadPool.h"
#include "ParamManager.h"

using namespace  VisMotorToolSpace;

FilmTearNode::FilmTearNode(ModuleInfo* item,int station)
{
	m_item = item;
	m_station = station;
}

int FilmTearNode::Process()
{
	ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"开始撕膜流程%1").arg(m_station + 1));
	int nRes = GrabPCB();
	if (nRes != 0)return nRes;
	nRes = FilmTear();
	if (nRes != 0)return nRes;
	nRes = PlacePCB();
	if (nRes != 0)return nRes;
	ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"撕膜结束%1").arg(m_station + 1));
	return 0;
}

int FilmTearNode::GrabPCB()
{
	//移动到安全位
	int nRes = VisMotorInstance->MovePositionAbs(FilmTearGripSafe);
	if (nRes != 0)return nRes;
	//张开夹爪
	nRes = SetGripClose(false);
	if (nRes != 0)return nRes;
	//到抓取位
	nRes = VisMotorInstance->MovePositionAbs(FileTearFeed);
	if (nRes != 0)return nRes;
	//闭合夹爪
	nRes = SetGripClose(true);
	if (nRes != 0)return nRes;
	//移动到安全位
	nRes = VisMotorInstance->MovePositionAbs(FilmTearGripSafe);
	if (nRes != 0)return nRes;
	bool exist = false;
	nRes = CheckGripModuleExist(exist);
	if (nRes != 0)return nRes;
	//如果没抓到
	if (!exist) {
		ShowLog(Log_Fixture, m_station, Log_Error, QString(u8"未抓取到转盘PCB"));
		QStringList listBtn;
		listBtn << u8"重新夹取" << u8"停止生产";
		nRes = VisAppBus::sendEvent("PopupUserMsgBox", listBtn, QString(u8"未抓取到转盘PCB"));
		if (nRes == 0) {
			nRes = GrabPCB();
			if (nRes != 0)return nRes;
			return 0;
		}
		else {
			VisAppBus::sendEvent("AutoEmg");
			return HardWareErr;
		}
		return 0;
	}
	return 0;
}

int FilmTearNode::FilmTear()
{
    //到撕膜位
    int nRes = VisMotorInstance->MovePositionAbs(FileTearWork);
    if (nRes != 0)return nRes;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // 获取撕膜峰值压力
    float pressure = 0.0f;
    int ret = VisAppBus::sendEvent("PressureSensorGetPeakPressure", QString("TearStation"), pressure);
    m_item->pressure = static_cast<double>(pressure);

    // 获取撕膜位移高度
    QString sensorId = "00";
    double h = 0.0;
    VisAppBus::sendEvent("DisplacementSensorReadHeight", QString("TearStation"), sensorId, h);
    m_item->displacement  = static_cast<double>(h);
    nRes = VisMotorInstance->MovePositionAbs(FilmTearGripSafe);
    if (nRes != 0)return nRes;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    //移动到检测位
    nRes = VisMotorInstance->MovePositionAbs(FileTearDetect);
    if (nRes != 0)return nRes;

    //检测位到位后延时稳定IO信号,避免刚撕完就读不到IN49(绿灯=已撕掉)
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    //抬起后检测IN49:红灯(IO_OFF)=没撕掉,绿灯(IO_ON)=已撕掉
    if (VisMotorInstance->GetIoInput(IN_FilmTearDetect) == IO_OFF) {

        ShowLog(Log_Fixture, m_station, Log_Error, QString(u8"撕膜未撕下(IN49=OFF),R1喂新胶带后重新撕膜一次"));
        //R1顺时针转一点,把用过的胶带送走、新胶带喂到位
        VisMotorInstance->InitUserParamSpeed(MotorFilmtearR1, 0.1);   //bool
        nRes = VisMotorInstance->MotorMoveInc(MotorFilmtearR1, 0.3);
        if (nRes != 0)return nRes;
        nRes = VisMotorInstance->MovePositionAbs(FileTearWork);
        if (nRes != 0)return nRes;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        nRes = VisMotorInstance->MovePositionAbs(FilmTearGripSafe);
        if (nRes != 0)return nRes;
        nRes = VisMotorInstance->MovePositionAbs(FileTearDetect);
        if (nRes != 0)return nRes;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));   //重撕后同样等信号稳定
    }

    //旋转R1:IN49绿灯(ON)时顺时针旋转把撕下来的膜送走,变红灯(OFF)表示膜已送走,停止旋转
    if (VisMotorInstance->GetIoInput(IN_FilmTearDetect) == IO_ON) {
        ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"撕膜成功(IN49=ON),旋转R1送走胶带"));
        VisMotorInstance->InitUserParamSpeed(MotorFilmtearR1, 0.1);   //bool
        nRes = VisMotorInstance->MotorMoveAbs(MotorFilmtearR1, 1, false, true);
        if (nRes != 0)return nRes;
        //等待IN49变红灯(IO_OFF)表示膜已送走;10秒超时防止长时间卡住,超时也强制停止并报警
        bool bTapeMoved = VisMotorInstance->SelectIoInput(IN_FilmTearDetect, IO_OFF, 10000);
        VisMotorInstance->StopContinuous(MotorFilmtearR1);   //void,直接调用
        if (!bTapeMoved) {
            ShowLog(Log_Fixture, m_station, Log_Error, QString(u8"撕膜送膜超时:IN49在10秒内未变OFF(膜可能未送走),已停止R1,请检查胶带是否卡住"));
        }
    }
    else {
        //重撕后仍是红灯:撕膜失败NG
        ShowLog(Log_Fixture, m_station, Log_Error, QString(u8"撕膜失败:重撕后仍未撕下,标记NG"));
        m_item->result = false;
        m_item->ngReason = u8"撕膜失败_1_3";   //后缀:固定_1 + PCB码_1
        VisMotorInstance->StopContinuous(MotorFilmtearR1);
    }

    CheckFilmTearResult();
    SaveFilmDataToCsv();
    return 0;
}

int FilmTearNode::PlacePCB()
{
	//到放置位
	int nRes = VisMotorInstance->MovePositionAbs(FileTearBlank);
	if (nRes != 0)return nRes;
	//张开夹爪
	nRes = SetGripClose(false);
	if (nRes != 0)return nRes;
	//移动到安全位
	nRes = VisMotorInstance->MovePositionAbs(FilmTearGripSafe);
	if (nRes != 0)return nRes;
	bool exist = false;
	//检查模组
	nRes = CheckGripModuleExist(exist);
	if (nRes != 0)return nRes;
    if (GlobalParam->flagOffline || GlobalParam->emptyRun || GlobalParam->Debug_ ) {
		exist = false;
	}
	if (exist) {
		QString errInfo = QString(u8"撕膜夹爪松开后仍检测到模组");
		ShowLog(Log_Fixture, m_station, Log_Error, errInfo);
		VisAppBus::sendEvent("PopupErrInfo", errInfo);
		return HardWareErr;
	}
	return 0;
}

int FilmTearNode::SetGripClose(bool close)
{
	int nRes = 0;
	if (close) {
		//闭合夹爪:闭合位已进配方
		nRes = VisMotorInstance->MovePositionAbs(FilmtearGripClose);
		if (nRes != 0) {
            return nRes;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	else {
		nRes = VisMotorInstance->MovePositionAbs(FilmtearGripOpen);
		if (nRes != 0) return nRes;
	}
	return 0;
}

int FilmTearNode::CheckGripModuleExist(bool& exist)
{
    if (GlobalParam->flagOffline || GlobalParam->emptyRun || GlobalParam->Debug_) {
		exist = true;
		return 0;
	}

	curSportState state;
	if (!VisMotorInstance->ReadCurSportState(MotorFilmtearGripX, state)) {
		QString errInfo = QString(u8"获取撕膜夹爪夹取状态失败");
		ShowSystemLog(Log_Error, errInfo);
		VisAppBus::sendEvent("PopupErrInfo", errInfo);
		return HardWareErr;
	}
	exist = (state == SPAORTSTOPANDNOCATCH);

	return 0;
}

int FilmTearNode::CheckFilmTearResult()
{
    //测试模式
    if (!GlobalParam->ng_1) {
        m_item->result = true;
        ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"空跑模式:撕膜结果默认通过"));
        return 0;
    }
    const RecipeFilmTear& recipe = GlobalParam->recipeFilmTear;
    bool pressureOk = (m_item->pressure >= recipe.dPressMin && m_item->pressure <= recipe.dPressMax);
    bool heightOk   = (m_item->displacement >= recipe.dHeightMin && m_item->displacement <= recipe.dHeightMax);

    if (!pressureOk || !heightOk)
    {
        m_item->result = false;
        QStringList ngList;
        if (!pressureOk) ngList << QString(u8"压力越限:%1(%2~%3)")
                                   .arg(m_item->pressure).arg(recipe.dPressMin).arg(recipe.dPressMax);
        if (!heightOk)   ngList << QString(u8"位移越限:%1(%2~%3)")
                                   .arg(m_item->displacement).arg(recipe.dHeightMin).arg(recipe.dHeightMax);
        m_item->ngReason = QString(u8"撕膜NG:%1_1_3").arg(ngList.join(";"));   //后缀:固定_1 + PCB码_1
        ShowLog(Log_Fixture, m_station, Log_Error, QString(u8"撕膜检测NG:") + m_item->ngReason);
    }
    return 0;
}

int FilmTearNode::SaveFilmDataToCsv()
{
    QString productFolder = "DefaultProduct";
    if (GlobalParam->recipeProduct.curMatrix != nullptr)
    {
        productFolder = GlobalParam->recipeProduct.curMatrix->productName;
    }
    QString exePath = QCoreApplication::applicationDirPath();
    QString dataPath = exePath + "/Data/" + productFolder;

    QDir dir;
    if (!dir.exists(dataPath))
        dir.mkpath(dataPath);

    QString dateStr = QDate::currentDate().toString("yyyy-MM-dd");
    QString filePath = dataPath + "/" + dateStr + "_film.csv";

    bool fileExist = QFile::exists(filePath);
    QFile file(filePath);
    if (!file.open(QIODevice::Append))
    {
        ShowLog(Log_Fixture, m_station, Log_Error,
                QStringLiteral("CSV open failed: %1").arg(filePath));
        return -1;
    }

    const bool isNewFile = (file.size() == 0);

    if (isNewFile)
    {
        // UTF-8 BOM
        file.write("\xEF\xBB\xBF", 3);

        // 使用 Unicode 转义，避免源码按 GBK/ANSI 编译。
        const QString header = QStringLiteral(
                    "\u65F6\u95F4,"             // 时间
                    "\u6761\u7801,"             // 条码
                    "\u538B\u529B(N),"          // 压力(N)
                    "\u4F4D\u79FB(mm)\r\n");    // 位移(mm)

        file.write(header.toUtf8());
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    const QString timeStr =
            QDateTime::currentDateTime().toString(
                QStringLiteral("yyyy-MM-dd hh:mm:ss"));

    out << timeStr << ','
        << m_item->pcbCode << ','
        << QString::number(m_item->pressure, 'f', 3) << ','
        << QString::number(m_item->displacement, 'f', 3) << "\r\n";

    out.flush();

    if (out.status() != QTextStream::Ok)
    {
        file.close();
        return -1;
    }

    file.close();
    return 0;
}
