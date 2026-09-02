#ifndef WIDGETMATCHPARAM_H
#define WIDGETMATCHPARAM_H

#include <QWidget>
#include <QSettings>
#include "ParamManager.h"
#include "ImageCanvas.h"

namespace Ui {
class WidgetMatchParam;
}

class WidgetMatchParam : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetMatchParam(int station, ImageCanvas* widgetImg,QWidget *parent = nullptr);
    ~WidgetMatchParam();

public:
    void  UpdateShapeMatch(ShapeMatch*matchParam);
	ShapeMatch* GetShapeMatch() { return &m_shapeMatch; }

protected:
	ShapeMatch  m_shapeMatch;
    int  m_station = 0;
    ImageCanvas* m_widgetImg;
    QRectF m_rectModel;

private slots:
    void on_horizontalSliderContrastLow_valueChanged(int value);

    void on_spinBox_contrastLow_valueChanged(int arg1);

    void on_spinBox_contrastHigh_valueChanged(int arg1);

    void on_horizontalSliderContrastHigh_valueChanged(int value);

    void on_spinBox_level_valueChanged(int arg1);

    void on_horizontalSliderLevel_valueChanged(int value);

    void on_doubleSpinBox_angleStep_valueChanged(double arg1);

    void on_horizontalSliderAngleStep_valueChanged(int value);

    void on_comboBox_metric_currentIndexChanged(const QString &arg1);

    void on_comboBox_optimiz_currentIndexChanged(const QString &arg1);

    void on_spinBox_exposure_valueChanged(int arg1);

    void on_spinBox_lightValue_editingFinished();

    void on_checkBoxROI_clicked();

    void on_btnMakeModel_clicked();

private:
    Ui::WidgetMatchParam *ui;
};

#endif // WIDGETMATCHPARAM_H
