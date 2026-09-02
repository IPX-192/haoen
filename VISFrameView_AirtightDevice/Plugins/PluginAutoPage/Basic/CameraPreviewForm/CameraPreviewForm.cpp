#include "CameraPreviewForm.h"
#include "VisAppBus.h"
#include "ui_CameraPreviewForm.h"
#pragma execution_character_set("utf-8")

CameraPreviewForm::CameraPreviewForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraPreviewForm)
{
    ui->setupUi(this);
    ui->groupBox_3->setVisible(false);
    InitWidget();
}

CameraPreviewForm::~CameraPreviewForm()
{
    delete ui;
}

void CameraPreviewForm::InitWidget()
{
    VisAppBus::subscibeEvent(this,"UpdateWorkStationOneLocationLoadImage");
    VisAppBus::subscibeEvent(this,"UpdateWorkStationTwoLocationLoadImage");
    VisAppBus::subscibeEvent(this,"UpdateWidgetTakePhotosForRecordImage");
}

int CameraPreviewForm::event_UpdateWorkStationOneLocationLoadImage(QImage image)
{
    UpdateWorkStationOneLocationLoadLoadImageSlot(image);
    return 0;
}

int CameraPreviewForm::event_UpdateWidgetTakePhotosForRecordImage(QImage image)
{
    UpdateWidgetTakePhotosForRecordSlot(image);
    return 0;
}

int CameraPreviewForm::event_UpdateWorkStationTwoLocationLoadImage(QImage image)
{
    UpdateWorkStationTwoLocationLoadImageSlot(image);
    return 0;
}

void CameraPreviewForm::UpdateWidgetTakePhotosForRecordSlot(QImage image)
{
    ui->WidgetTakePhotosForRecord->SetImage(image);
    ui->WidgetTakePhotosForRecord->update();
}

void CameraPreviewForm::UpdateWorkStationTwoLocationLoadImageSlot(QImage image)
{
    ui->WidgetWorkStationTwoLocationLoad->SetImage(image);
    ui->WidgetWorkStationTwoLocationLoad->update();
}

void CameraPreviewForm::UpdateWorkStationOneLocationLoadLoadImageSlot(QImage image)
{
    ui->WidgetWorkStationOneLocationLoad->SetImage(image);
    ui->WidgetWorkStationOneLocationLoad->update();
}
