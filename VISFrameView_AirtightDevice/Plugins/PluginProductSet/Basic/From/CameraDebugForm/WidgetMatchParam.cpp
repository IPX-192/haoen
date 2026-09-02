#include "WidgetMatchParam.h"
#include "ui_WidgetMatchParam.h"
#include "VisCameraTool.h"
#include "VisUIParam.h"
//#include "LightManager.h"


WidgetMatchParam::WidgetMatchParam(int station, ImageCanvas*widgetImg,QWidget *parent) :
    m_widgetImg(widgetImg),
    m_station(station),
    QWidget(parent),
    ui(new Ui::WidgetMatchParam)
{
    ui->setupUi(this);
    connect(m_widgetImg, &ImageCanvas::emitRectSelect, [=](QRectF rect) {
        m_rectModel = rect;
        m_widgetImg->AddRect(rect, Qt::red);
        m_widgetImg->update();
       });
}

WidgetMatchParam::~WidgetMatchParam()
{
    delete ui;
}

void WidgetMatchParam::UpdateShapeMatch(ShapeMatch* matchParam)
{
    VisUIParam::QObjectCopy(matchParam, &m_shapeMatch);
}

void WidgetMatchParam::on_horizontalSliderContrastLow_valueChanged(int value)
{
    ui->spinBox_contrastLow->setValue(value);
    m_shapeMatch.contrastLow = value;
    if (m_shapeMatch.contrastLow > m_shapeMatch.contrastHigh) {
        ui->spinBox_contrastHigh->setValue(value);
    }
}

void WidgetMatchParam::on_spinBox_contrastLow_valueChanged(int arg1)
{
    ui->horizontalSliderContrastLow->setValue(arg1);
    m_shapeMatch.contrastLow = arg1;
}

void WidgetMatchParam::on_spinBox_contrastHigh_valueChanged(int arg1)
{
    ui->horizontalSliderContrastHigh->setValue(arg1);
    m_shapeMatch.contrastHigh = arg1;
}

void WidgetMatchParam::on_horizontalSliderContrastHigh_valueChanged(int value)
{
    ui->spinBox_contrastHigh->setValue(value);
    m_shapeMatch.contrastHigh = value;
}

void WidgetMatchParam::on_spinBox_level_valueChanged(int arg1)
{
    ui->horizontalSliderLevel->setValue(arg1);
    m_shapeMatch.level = arg1;
}

void WidgetMatchParam::on_horizontalSliderLevel_valueChanged(int value)
{
	ui->spinBox_level->setValue(value);
	m_shapeMatch.level = value;
}

void WidgetMatchParam::on_doubleSpinBox_angleStep_valueChanged(double arg1)
{
    double stepValue = (ui->doubleSpinBox_angleStep->maximum() - ui->doubleSpinBox_angleStep->minimum()) / 100.0;;
	int step = (arg1 - ui->doubleSpinBox_angleStep->minimum()) / stepValue;
	ui->horizontalSliderAngleStep->setValue(step);
	m_shapeMatch.angleStep = arg1;
}

void WidgetMatchParam::on_horizontalSliderAngleStep_valueChanged(int step)
{
    double stepValue = (ui->doubleSpinBox_angleStep->maximum() - ui->doubleSpinBox_angleStep->minimum()) / 100.0;
    double angleStep = ui->doubleSpinBox_angleStep->minimum() + step * stepValue;
	ui->doubleSpinBox_angleStep->setValue(angleStep);
	m_shapeMatch.angleStep = angleStep;
}

void WidgetMatchParam::on_comboBox_metric_currentIndexChanged(const QString &arg1)
{
    m_shapeMatch.metricStr = arg1;
}

void WidgetMatchParam::on_comboBox_optimiz_currentIndexChanged(const QString &arg1)
{
    m_shapeMatch.optimizStr = arg1;
}

void WidgetMatchParam::on_spinBox_exposure_valueChanged(int arg1)
{
    VisCameraTool::instance()->SetCamParam(m_station, m_station ? StationTwo : StationOne, VisCameraTool::ShutterTime, arg1);
}

void WidgetMatchParam::on_spinBox_lightValue_editingFinished()
{
//    LightManagerIns.SetLightValue(GlobalParam->mLightStruct.portName, 0, ui->spinBox_lightValue->value());
}

void WidgetMatchParam::on_checkBoxROI_clicked()
{
    m_widgetImg->SetCurMode(ui->checkBoxROI->isChecked() ? UserImageCanvas::RectMode : UserImageCanvas::NormalMode);
}

void WidgetMatchParam::on_btnMakeModel_clicked()
{

}

