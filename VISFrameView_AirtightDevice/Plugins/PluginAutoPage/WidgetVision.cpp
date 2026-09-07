#include "WidgetVision.h"
#include "ui_WidgetVision.h"
#include "VisAppBus.h"
#include "ParamManager.h"

WidgetVision::WidgetVision(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetVision)
{
    ui->setupUi(this);
    qRegisterMetaType<QImage>("QImage");
    VisAppBus::subscibeEvent(this, "ShowDirtyImg");
    VisAppBus::subscibeEvent(this, "ShowResultImage");
}

WidgetVision::~WidgetVision()
{
    delete ui;
}

int WidgetVision::event_ShowDirtyImg(int station, QImage img)
{
    if (img.isNull()) return 0;
    ui->widgetDirtyImg->ClearText();
    ui->widgetDirtyImg->SetImage(img, false);
    ui->widgetDirtyImg->update();
    return 0;
}

void WidgetVision::on_btnTestDirty_clicked()
{
    //真实脏污相机采集+检测(默认治具1):走 TestDirtyDetect → DirtyNode::event_TestDetect(抓图→预处理→检测→显示)
    VisAppBus::sendEvent("TestDirtyDetect", 0);
}

int WidgetVision::event_ShowResultImage(int station, QString ngInfo)
{
    ui->widgetDirtyImg->ClearText();
    UserImageCanvas::TextParam textItem;
    textItem.text = ngInfo.contains("OK") ? u8"OK" : QString(u8"NG:%1").arg(ngInfo);
    textItem.color = ngInfo.contains("OK") ? Qt::green : Qt::red;
    textItem.drawAtTop = true;
    textItem.posIsImg = false;
    textItem.rect = QRectF(0, 0, 1000, 200);
    textItem.font.setPointSizeF(20);
    ui->widgetDirtyImg->AddText(textItem);
    ui->widgetDirtyImg->update();
    return 0;
}
