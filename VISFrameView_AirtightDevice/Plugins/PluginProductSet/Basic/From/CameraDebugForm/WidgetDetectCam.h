#ifndef WIDGETDETECTCAM_H
#define WIDGETDETECTCAM_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class WidgetDetectCam;
}

class WidgetDetectCam : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetDetectCam(int station,QWidget *parent = nullptr);
    ~WidgetDetectCam();

public slots:
    void LoadUIParam(QSettings*setting);
    void UpdateParamToUI();
    void SaveUIParam(QSettings*setting);

protected:
    void RealTimeShowImg();

protected:
    int  m_station;
    bool m_showImg = false;

private slots:
    void on_btnOpenCam_clicked();

    void on_btnCloseCam_clicked();

    void on_btnSaveImg_clicked();

    void on_btnLoadImg_clicked();

private:
    Ui::WidgetDetectCam *ui;
};

#endif // WIDGETDETECTCAM_H
