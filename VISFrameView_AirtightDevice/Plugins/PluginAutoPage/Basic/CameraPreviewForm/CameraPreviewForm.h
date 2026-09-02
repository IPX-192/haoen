#ifndef CAMERAPREVIEWFORM_H
#define CAMERAPREVIEWFORM_H

#include <QWidget>
#include <QImage>
namespace Ui {
class CameraPreviewForm;
}

class CameraPreviewForm : public QWidget
{
    Q_OBJECT
public:
    explicit CameraPreviewForm(QWidget *parent = nullptr);
    ~CameraPreviewForm();
    void InitWidget();

public slots:
    int event_UpdateWorkStationOneLocationLoadImage(QImage image);
    int event_UpdateWorkStationTwoLocationLoadImage(QImage image);
    int event_UpdateWidgetTakePhotosForRecordImage(QImage image);

    void UpdateWorkStationOneLocationLoadLoadImageSlot(QImage image);
    void UpdateWidgetTakePhotosForRecordSlot(QImage image);
    void UpdateWorkStationTwoLocationLoadImageSlot(QImage image);

private:
    Ui::CameraPreviewForm *ui;
};

#endif // CAMERAPREVIEWFORM_H
