#include "WidgetDetectCam.h"
#include "ui_WidgetDetectCam.h"
#include "WidgetMatchParam.h"
#include "VisUIParam.h"
#include "ParamManager.h"
#include "VisCameraTool.h"
#include "VisAppThreadPool.h"

WidgetDetectCam::WidgetDetectCam(int station,QWidget *parent) :
    m_station(station),
    QWidget(parent),
    ui(new Ui::WidgetDetectCam)
{
    ui->setupUi(this);
    WidgetMatchParam*widgetMaterial=new WidgetMatchParam(0,ui->widget_img,ui->widget_tab);
    WidgetMatchParam*widgetAngle=new WidgetMatchParam(1,ui->widget_img,ui->widget_tab);
    ui->widget_tab->addTab(widgetMaterial,u8"物料检测");
    ui->widget_tab->addTab(widgetAngle,u8"角度检测");
}

WidgetDetectCam::~WidgetDetectCam()
{
    delete ui;
}

void WidgetDetectCam::LoadUIParam(QSettings*setting)
{
    setting->beginGroup(QString("Material%1").arg(m_station+1));
    VisUIParam::LoadIniToUI(setting, ui->widget_tab->widget(0), &GlobalParam->recipeVison.shapeMaterial[m_station]);
    setting->endGroup();
    setting->beginGroup(QString("Angle%1").arg(m_station+1));
    VisUIParam::LoadIniToUI(setting, ui->widget_tab->widget(1), &GlobalParam->recipeVison.shapeAngle[m_station]);
    setting->endGroup();
}

void WidgetDetectCam::UpdateParamToUI()
{

}

void WidgetDetectCam::SaveUIParam(QSettings*setting)
{

}


void WidgetDetectCam::on_btnOpenCam_clicked()
{
    int nRes=VisCameraTool::instance()->OpenCamera(m_station,m_station ? StationTwo : StationOne);
	if (nRes != 0) {
		ShowLog(DebugLog, 0, Log_Error, QString(u8"打开相机失败"));
		return;
	}
    nRes=VisCameraTool::instance()->StartStream(m_station,m_station ? StationTwo : StationOne,false);
    if(nRes!=0)return;
	ShowLog(DebugLog, 0, Log_Debug, QString(u8"开始出图"));
    GlobalThreadPool->Commit(std::bind(&WidgetDetectCam::RealTimeShowImg, this));
}

void WidgetDetectCam::on_btnCloseCam_clicked()
{
    VisCameraTool::instance()->CloseCamera(m_station,m_station ? StationTwo : StationOne);
	ShowLog(DebugLog, 0, Log_Debug, QString(u8"关闭相机"));
}

void WidgetDetectCam::on_btnSaveImg_clicked()
{

}

void WidgetDetectCam::on_btnLoadImg_clicked()
{

}

void WidgetDetectCam::RealTimeShowImg()
{
	while (m_showImg)
	{
		cv::Mat img;
		QImage imgshow;
		if (!GlobalParam->flagOffline)
		{
			int nRes = VisCameraTool::instance()->GrabImgFrame(m_station, m_station ? StationTwo : StationOne, img, &imgshow);
			if (nRes != 0) {
				ShowLog(DebugLog, 0, Log_Error, QString(u8"%1相机抓图失败%2").arg(m_station ? StationTwo : StationOne).arg(nRes));
				break;
			}
		}
		else {
			img = cv::imread("1.bmp", -1);
		}
	}
}
