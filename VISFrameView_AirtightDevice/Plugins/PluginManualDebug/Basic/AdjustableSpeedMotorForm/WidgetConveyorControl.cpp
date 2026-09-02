#include "WidgetConveyorControl.h"
#include "ui_WidgetConveyorControl.h"

ConveyorControlWidget::ConveyorControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConveyorControlWidget)
{
    setWindowTitle(u8"产线输送带控制系统");
            resize(1000, 600);
    // 样式表 (QSS) - 核心视觉还原
            QString qss = R"(
                QWidget {
                    font-family: "Microsoft YaHei";
                    font-size: 14px;
                }
                /* 左侧标题样式 */
                QLabel#TitleLabel {
                    background-color: #BDD7EE; /* 浅蓝背景 */
                    color: #2E5F8B;            /* 深蓝文字 */
                    font-weight: bold;
                    border: 1px solid #999;
                }
                /* 按钮通用样式 - 修改这里 */
                QPushButton {
                    background-color: #BDD7EE; /* 改为浅蓝色，和标题一致 */
                    border: 1px solid #2E5F8B; /* 边框改为深蓝色 */
                    font-weight: bold;
                    font-size: 16px;
                    color: #2E5F8B;            /* 文字也改为深蓝色 */
                }
                QPushButton:pressed {
                    background-color: #9BC2E6; /* 按下时稍深一点的蓝色 */
                }
                /* ID 标签样式 */
                QLabel#MotorIDLabel {
                    background-color: #2E5F8B; /* 浅蓝背景 */
                    color: #00FF00;            /* 亮绿文字 */
                    font-weight: bold;
                    font-size: 12px;
                }
            )";
            this->setStyleSheet(qss);

            // 主布局
            QVBoxLayout *mainLayout = new QVBoxLayout(this);

            // 滚动区域 (防止条目过多超出屏幕)
            QScrollArea *scrollArea = new QScrollArea(this);
            scrollArea->setWidgetResizable(true);

            // 内容容器
            QWidget *contentWidget = new QWidget();
            QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
            contentLayout->setAlignment(Qt::AlignTop); // 顶部对齐

            // --- 动态配置数据源 ---
            // 在实际项目中，这里可以是从数据库或 JSON 读取的列表
            QList<ConveyorRow::Config> pipelineConfigs = {
                {u8"上层输送电机","M8200", "M8201","","M8300","M8301",""},
                {u8"下层输送电机","M8202", "M8203","","M8302","M8303",""},
            };

            // 循环生成界面
            for (const auto &cfg : pipelineConfigs) {
                ConveyorRow *row = new ConveyorRow(cfg);
                // 添加分隔线
                QFrame *line = new QFrame();
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);

                contentLayout->addWidget(row);
                contentLayout->addWidget(line);
            }

            scrollArea->setWidget(contentWidget);
            mainLayout->addWidget(scrollArea);
}

ConveyorControlWidget::~ConveyorControlWidget()
{
    delete ui;
}
