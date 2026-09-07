#include "WidgetProdutData.h"
#include "ui_WidgetProdutData.h"
#include "ProductDetailForm.h"
#include "VisAppBus.h"
#include "VisCameraTool.h"
#include "VisDMFind.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

WidgetProdutData::WidgetProdutData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetProdutData)
{
    ui->setupUi(this);
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<QImage>("QImage");
    InitTabs();

    VisAppBus::subscibeEvent(this, "ClearProductData");
    VisAppBus::subscibeEvent(this, "AddProductData");
    VisAppBus::subscibeEvent(this, "ShowCalibImage");
    VisAppBus::subscibeEvent(this, "ShowDirtyImg");
    VisAppBus::subscibeEvent(this, "ShowResultImage");
}

WidgetProdutData::~WidgetProdutData()
{
    delete ui;
}

void WidgetProdutData::InitTabs()
{
    m_tabWidget = new QTabWidget(this);
    ui->mainLayout->addWidget(m_tabWidget);

    for (int i = 0; i < 4; i++) {
        // 为每个治具创建一个 tab 页面
        QWidget* tabPage = new QWidget();
        QHBoxLayout* hLayout = new QHBoxLayout(tabPage);
        hLayout->setContentsMargins(5, 5, 5, 5);
        hLayout->setSpacing(8);

        // 左侧：数据表格（占 60% 宽度）
        m_vecProductDetail[i] = new ProductDetailForm(i, tabPage);
        hLayout->addWidget(m_vecProductDetail[i], 3);

        // 右侧：对位图像 + 脏污图像（垂直排列）
        QWidget* rightPanel = new QWidget(tabPage);
        QVBoxLayout* vLayout = new QVBoxLayout(rightPanel);
        vLayout->setContentsMargins(0, 0, 0, 0);
        vLayout->setSpacing(6);

        // 对位图像(临时用于PCB扫码测试:抓图+ROI框+DataMatrix识别)
        QGroupBox* groupAlign = new QGroupBox(u8"对位图像", rightPanel);
        QVBoxLayout* alignLayout = new QVBoxLayout(groupAlign);
        m_vecAlignImg[i] = new ImageCanvas(groupAlign);
        m_vecAlignImg[i]->setMinimumSize(280, 200);
        alignLayout->addWidget(m_vecAlignImg[i]);

        QPushButton* btnScanTest = new QPushButton(u8"PCB扫码测试", groupAlign);
        int scanStation = i;
        QObject::connect(btnScanTest, &QPushButton::clicked, this, [=](){
            m_vecAlignImg[scanStation]->ClearAll();
            //抓PCB扫码相机图像
            cv::Mat img;
            int nGrab = VisCameraTool::instance()->GrabImgFrame(0, FeedPCBCam, img);
            if (nGrab != 0 || img.empty()) {
                UserImageCanvas::TextParam textItem;
                textItem.text = QString(u8"PCB扫码相机抓图失败:%1").arg(nGrab);
                textItem.color = Qt::red;
                textItem.drawAtTop = true;
                textItem.posIsImg = false;
                textItem.rect = QRectF(0, 0, 1000, 200);
                textItem.font.setPointSizeF(20);
                m_vecAlignImg[scanStation]->AddText(textItem);
                m_vecAlignImg[scanStation]->update();
                return;
            }
            //显示图像(转RGB避免偏色)
            cv::Mat imgRgb;
            cv::cvtColor(img, imgRgb, cv::COLOR_BGR2RGB);
            QImage image = QImage(static_cast<const unsigned char *>(imgRgb.data),
                                  imgRgb.cols, imgRgb.rows, static_cast<int>(imgRgb.step),
                                  QImage::Format_RGB888).copy();
            m_vecAlignImg[scanStation]->SetImage(image, false);
            //画扫码ROI框
            RecipeScanCode& scanRecipe = GlobalParam->recipeScanCode;
            m_vecAlignImg[scanStation]->AddRect(
                QRectF(scanRecipe.roiX, scanRecipe.roiY, scanRecipe.roiW, scanRecipe.roiH), Qt::yellow, 2);
            //DataMatrix识别
            VisDMFind dmFind;
            std::string info;
            int nRet = dmFind.FindDM(img, cv::Rect(scanRecipe.roiX, scanRecipe.roiY, scanRecipe.roiW, scanRecipe.roiH), info);
            UserImageCanvas::TextParam textItem;
            if (nRet == 0 && !info.empty()) {
                QString barCode = QString::fromStdString(info).trimmed();
                textItem.text = QString(u8"识别成功:%1").arg(barCode);
                textItem.color = Qt::green;

                //PCB扫码测试附带MES条码校验
                if (!barCode.isEmpty()) {
                    bool validate = false;
                    int vRes = VisAppBus::sendEvent("MesValidateNumber", barCode, validate);
                    ShowSystemLog(vRes == 0 ? Log_Info : Log_Error,
                        QString(u8"PCB条码校验%1:%2").arg(vRes == 0 ? u8"完成" : u8"失败").arg(validate ? u8"通过" : u8"不通过"));
                }
            }
            else {
                textItem.text = QString(u8"识别失败:%1(0正常,-1图像空,-2ROI超限,-3未识到)").arg(nRet);
                textItem.color = Qt::red;
            }
            textItem.drawAtTop = true;
            textItem.posIsImg = false;
            textItem.rect = QRectF(0, 0, 1000, 200);
            textItem.font.setPointSizeF(20);
            m_vecAlignImg[scanStation]->AddText(textItem);
            m_vecAlignImg[scanStation]->update();
        });
        alignLayout->addWidget(btnScanTest);
        vLayout->addWidget(groupAlign);

        // 脏污图像 + 测试按钮
        QGroupBox* groupDirty = new QGroupBox(u8"脏污图像", rightPanel);
        QVBoxLayout* dirtyLayout = new QVBoxLayout(groupDirty);
        m_vecDirtyImg[i] = new ImageCanvas(groupDirty);
        m_vecDirtyImg[i]->setMinimumSize(280, 200);
        dirtyLayout->addWidget(m_vecDirtyImg[i]);

        QPushButton* btnTest = new QPushButton(u8"测试", groupDirty);
        int station = i;
        QObject::connect(btnTest, &QPushButton::clicked, this, [=](){
            //真实脏污相机采集+检测:走 TestDirtyDetect → DirtyNode::event_TestDetect(抓图→预处理→检测→显示)
            VisAppBus::sendEvent("TestDirtyDetect", station);
        });
        dirtyLayout->addWidget(btnTest);
        vLayout->addWidget(groupDirty);

        hLayout->addWidget(rightPanel, 2);

        m_tabWidget->addTab(tabPage, QString(u8"治具%1").arg(i + 1));
    }
}

int WidgetProdutData::event_InitProductData(int station, QString barCode)
{
    if (station >= 0 && station < 4)
        m_vecProductDetail[station]->InitData(barCode);
    return 0;
}

int WidgetProdutData::event_AddProductData(int station, double pressure, double displacement)
{
    if (station < 0 || station >= 4) return 0;
    ProductDetailForm* form = m_vecProductDetail[station];
    form->InitData("");

    //压力:只填测试项和测试值,最大最小值暂不填
    ItemDetail infoPress;
    infoPress.name = u8"压力";
    infoPress.sUnit = "N";
    infoPress.testValue = pressure;
    infoPress.fillLimit = false;
    infoPress.result = true;
    form->AddData(infoPress);

    //位移:只填测试项和测试值,最大最小值暂不填
    ItemDetail infoDisp;
    infoDisp.name = u8"位移";
    infoDisp.sUnit = "mm";
    infoDisp.testValue = displacement;
    infoDisp.fillLimit = false;
    infoDisp.result = true;
    form->AddData(infoDisp);
    return 0;
}

int WidgetProdutData::event_ShowCalibImage(int station, cv::Mat mat)
{
    if (station < 0 || station >= 4) return 0;
    if (mat.empty()) return 0;
    //相机图像为BGR,转RGB显示避免偏色
    cv::Mat matRgb;
    cv::cvtColor(mat, matRgb, cv::COLOR_BGR2RGB);
    QImage image = QImage(static_cast<const unsigned char *>(matRgb.data),
                          matRgb.cols, matRgb.rows, static_cast<int>(matRgb.step),
                          QImage::Format_RGB888).copy();
    m_vecAlignImg[station]->ClearText();
    m_vecAlignImg[station]->SetImage(image, false);
    m_vecAlignImg[station]->update();
    return 0;
}

int WidgetProdutData::event_ShowDirtyImg(int station, QImage img)
{
    if (station < 0 || station >= 4) return 0;
    if (img.isNull()) return 0;
    m_vecDirtyImg[station]->ClearText();
    m_vecDirtyImg[station]->SetImage(img, false);
    m_vecDirtyImg[station]->update();
    return 0;
}

int WidgetProdutData::event_ShowResultImage(int station, QString ngInfo)
{
    if (station < 0 || station >= 4) return 0;
    ImageCanvas* widgetImg = m_vecDirtyImg[station];
    widgetImg->ClearText();
    UserImageCanvas::TextParam textItem;
    textItem.text = ngInfo.contains("OK") ? u8"OK" : QString(u8"NG:%1").arg(ngInfo);
    textItem.color = ngInfo.contains("OK") ? Qt::green : Qt::red;
    textItem.drawAtTop = true;
    textItem.posIsImg = false;
    textItem.rect = QRectF(0, 0, 1000, 200);
    textItem.font.setPointSizeF(20);
    widgetImg->AddText(textItem);
    widgetImg->update();
    return 0;
}
