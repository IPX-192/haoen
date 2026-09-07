#include "WidgetCylinderTab.h"
#include "ui_WidgetCylinderTab.h"
#include <QDomDocument>
#include <QFile>
#include <QGridLayout>
#include <QScrollArea>
#include "CylinderWidget.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"
#include "ParamManager.h"

WidgetCylinderTab::WidgetCylinderTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetCylinderTab)
{
    ui->setupUi(this);

    // 设置 200ms 定时器
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &WidgetCylinderTab::slotRefreshTimer);
    m_refreshTimer->start(200); // 200ms 刷新周期
}

WidgetCylinderTab::~WidgetCylinderTab()
{
    delete ui;
}

void WidgetCylinderTab::LoadUIParam()
{
    QString filePath=GlobalParam->frameCore->configDirPath+"Cylinder.xml";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)){
        file.close();
        return;
    }
    file.close();

    QDomElement root = doc.documentElement();
    QDomNodeList tabs = root.elementsByTagName("Tab");

    for (int i = 0; i < tabs.count(); i++) {
        QDomElement tabElem = tabs.at(i).toElement();
        QString tabName = tabElem.attribute("name");

        // 创建 Tab 页的内容容器
        QWidget *tabPage = new QWidget();
        // 使用 Grid 布局，每行显示 3 个气缸
        QGridLayout *gridLayout = new QGridLayout(tabPage);
        gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        // 2. 【调大网格布局的间距】从 15 调大到 25
        gridLayout->setSpacing(25);
        gridLayout->setContentsMargins(20, 20, 20, 20); // 增加页面的外边距

        QList<CylinderWidget*>listCylinders;
        QDomNodeList cylinders = tabElem.elementsByTagName("Cylinder");
        for (int j = 0; j < cylinders.count(); j++) {
            QDomElement cylElem = cylinders.at(j).toElement();
            QDomElement s0 = cylElem.firstChildElement("State0");
            QDomElement s1 = cylElem.firstChildElement("State1");

            CylinderData data;
            data.name = cylElem.attribute("name");
            data.s0_text = s0.attribute("text");
            data.s0_out = s0.attribute("out");
            data.s0_in = s0.attribute("in");
            data.s1_text = s1.attribute("text");
            data.s1_out = s1.attribute("out");
            data.s1_in = s1.attribute("in");
            data.addr = cylElem.attribute("addr");
            GetCardIoName(data.io_name, data.s0_out,false);
            GetCardIoName(data.io_name, data.s1_out,false);
			GetCardIoName(data.io_name, data.s0_in, true);
			GetCardIoName(data.io_name, data.s1_in, true);

            CylinderWidget *cylWidget = new CylinderWidget(data);

            // 【重要】连接点击信号
            // 使用 Lambda 表达式捕获指针，方便知道是哪个气缸被点了
            connect(cylWidget, &CylinderWidget::commandTriggered, this, [=](int cmd){
                this->slotCylinderCommand(cylWidget, cmd);
            });
            // 计算行列位置: 每行 3 个
            int row = j / m_cols;
            int col = j % m_cols;
            gridLayout->addWidget(cylWidget, row, col);
            listCylinders.append(cylWidget); // 加入列表管理
        }

        // 如果气缸很多，最好放入 ScrollArea
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidget(tabPage);
        scrollArea->setWidgetResizable(true);
        m_cylinders.insert(ui->tabWidget->count(),listCylinders);
        ui->tabWidget->addTab(scrollArea, tabName);
    }
}

void WidgetCylinderTab::SetIoState(QString name, bool state)
{
    VisMotorToolSpace::VisMotorInstance->SetIoOutput(name, state ? VisMotorToolSpace::IO_ON : VisMotorToolSpace::IO_OFF);
}

void WidgetCylinderTab::GetIoState(QString name, bool*state)
{
    if(name.isEmpty()){
        * state = false;
        return;
    }
    * state = (VisMotorToolSpace::VisMotorInstance->GetIoInput(name) == VisMotorToolSpace::IO_ON) ? true : false;
}

void WidgetCylinderTab::GetCardIoName(QMap<QString, QString>& io_name, QString ioIndex,bool inIO)
{
    int index = ioIndex.toInt();
    if(ioIndex.isEmpty())
        index = -1;
    QMap<QString, StIOParam>& IoMapParam = inIO ? VisMotorToolSpace::VisMotorDataInstance->m_inIoMap : VisMotorToolSpace::VisMotorDataInstance->m_outIoMap;
	for (auto it = IoMapParam.begin(); it != IoMapParam.end(); ++it) {
		if (it.value().stIoOnID == index) {
            io_name.insert(ioIndex, it.key());
			break;
		}
	}
}

// 【用户交互】处理点击
void WidgetCylinderTab::slotCylinderCommand(CylinderWidget* sender, int cmd)
{
    // 获取相关的 IO 名称
    QString out0 = sender->GetOut0Name(); // 左侧输出 (如 Y213)
    QString out1 = sender->GetOut1Name(); // 右侧输出 (如 Y214)
    QString addr = sender->GetData().addr; // 地址(IO序号) 
	out0 = sender->GetData().io_name[out0];
	out1 = sender->GetData().io_name[out1];
    SetIoState(addr, cmd == 0 ? false : true);
	////双控气缸才有互锁逻辑
 //   if (out0 != out1) {
 //       SetIoState((cmd == 0) ? out1 : out0, false);
 //       SetIoState((cmd == 0) ? out0 : out1, true);
 //   }
 //   else { //单控气缸
	//	SetIoState(out0, cmd == 0 ? false : true);
 //   }
}

// 【定时刷新】界面轮询
void WidgetCylinderTab::slotRefreshTimer()
{
    if (!this->isVisible())return;
    for(auto it=m_cylinders.begin();it!=m_cylinders.end();++it){
        if(it.key()!=ui->tabWidget->currentIndex())continue;
        for (CylinderWidget *cyl : it.value()) {
            // 从数据源获取当前状态
            bool s0_out = false,s0_in = false,s1_out=false,s1_in=false;
            QStringList listIoName;
            QMap<QString, QString> io_name= cyl->GetData().io_name;
            listIoName << io_name[cyl->GetOut0Name()] << io_name[cyl->GetOut1Name()]
                      << io_name[cyl->GetIn0Name()] << io_name[cyl->GetIn1Name()];
            
            QVector<bool*> result;
            result<<&s0_out<<&s1_out<<&s0_in<<&s1_in;
            for(int i=0;i< listIoName.size();i++){
                GetIoState(listIoName.at(i),result.at(i));
            }
            // 更新 UI
            cyl->UpdateRealTimeStatus(s0_out, s0_in, s1_out, s1_in);
        }
    }
}
