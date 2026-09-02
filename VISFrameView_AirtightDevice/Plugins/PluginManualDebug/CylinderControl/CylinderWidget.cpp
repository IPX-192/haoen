#include "CylinderWidget.h"
#include <QGraphicsDropShadowEffect>

CylinderWidget::CylinderWidget(const CylinderData &data, QWidget *parent)
    : QFrame(parent), m_data(data)
{
    this->setFrameShape(QFrame::Box);
    this->setStyleSheet("CylinderWidget { background-color: #EEEEEE; border: 1px solid #888; border-radius: 6px; }");

    // 1. 【放大整体气缸控件】
    this->setFixedSize(360, 180);
    SetupUi();
    UpdateRealTimeStatus(false, false, false, false); // 默认全灭状态
}

QLabel* CylinderWidget::CreateIndicatorLabel(QString str,const QString &text)
{
    QString txt = text;
    if(text.contains("X") || text.contains("Y"))
        txt = text;
    else
        txt = str+text;

    if(text.isEmpty())
        txt = text;

    QLabel *lbl = new QLabel(txt);
    lbl->setAlignment(Qt::AlignCenter);

    // 2. 【放大 IO 状态框】
    lbl->setFixedSize(80, 32);
    lbl->setStyleSheet(STYLE_INACTIVE);

    // 3. 【调大 IO 字体】
    QFont font = lbl->font();
    font.setPointSize(11);
    font.setBold(true);
    lbl->setFont(font);

    return lbl;
}

void CylinderWidget::SetupUi() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(0);

    // --- 顶部标题栏 ---
    QLabel *titleLbl = new QLabel(m_data.name);
    titleLbl->setAlignment(Qt::AlignCenter);

    // 4. 【调大标题栏高度和字体】
    titleLbl->setFixedHeight(35); // 高度从 30 调到 35
    QFont titleFont = titleLbl->font();
    titleFont.setPointSize(12);   // 字体大小设为 12
    titleFont.setBold(true);
    titleLbl->setFont(titleFont);
    titleLbl->setStyleSheet("background-color: #F0E68C; border-bottom: 1px solid #AAA;");
    mainLayout->addWidget(titleLbl);

    // --- 内容区域 ---
    QWidget *contentWidget = new QWidget();
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(5, 5, 5, 5); // 增加一点内边距

    // 5. 【准备统一的状态文字字体】
    QFont stateTextFont;
    stateTextFont.setPointSize(12); // "缩回"/"伸出" 字体设为 12
    stateTextFont.setBold(true);

    // --- 左侧区域 (状态0) ---
    QVBoxLayout *leftLayout = new QVBoxLayout();
    m_lblState0Text = new QLabel(m_data.s0_text);
    m_lblState0Text->setAlignment(Qt::AlignCenter);
    // 6. 【放大状态文字框】从 30x60 调大到 40x80
    m_lblState0Text->setFixedSize(40, 80);
    m_lblState0Text->setWordWrap(true);
    m_lblState0Text->setFont(stateTextFont); // 应用大字体
    m_lblState0Text->setStyleSheet(STYLE_TEXT_ACTIVE);

    QVBoxLayout *leftIOLayout = new QVBoxLayout();
    leftIOLayout->setSpacing(8); // IO 框之间的间距稍微拉开
    m_lblState0Out = CreateIndicatorLabel("OUT",m_data.s0_out);
    m_lblState0In = CreateIndicatorLabel("IN",m_data.s0_in);
    leftIOLayout->addWidget(m_lblState0Out);
    leftIOLayout->addWidget(m_lblState0In);

    QHBoxLayout *leftGroup = new QHBoxLayout();
    leftGroup->addWidget(m_lblState0Text);
    leftGroup->addLayout(leftIOLayout);

    // --- 中间图形 ---
    QLabel *pistonImage = new QLabel(m_data.addr);
    // 7. 【放大中间活塞示意图】从 40x20 调大到 60x25
    pistonImage->setFixedSize(60, 25);
    pistonImage->setAlignment(Qt::AlignCenter);
    pistonImage->setStyleSheet("background-color: #BBB; border: 1px solid #666;");
    //pistonImage->setStyleSheet(STYLE_TEXT_ACTIVE);

    // --- 右侧区域 (状态1) ---
    QVBoxLayout *rightLayout = new QVBoxLayout();
    m_lblState1Text = new QLabel(m_data.s1_text);
    m_lblState1Text->setAlignment(Qt::AlignCenter);
    m_lblState1Text->setFixedSize(40, 80); // 同左侧
    m_lblState1Text->setWordWrap(true);
    m_lblState1Text->setFont(stateTextFont); // 应用大字体
    m_lblState1Text->setStyleSheet(STYLE_TEXT_ACTIVE);

    QVBoxLayout *rightIOLayout = new QVBoxLayout();
    rightIOLayout->setSpacing(8); // IO 框之间的间距稍微拉开
    m_lblState1Out = CreateIndicatorLabel("OUT",m_data.s1_out);
    m_lblState1In = CreateIndicatorLabel("IN",m_data.s1_in);
    rightIOLayout->addWidget(m_lblState1Out);
    rightIOLayout->addWidget(m_lblState1In);

    QHBoxLayout *rightGroup = new QHBoxLayout();
    rightGroup->addLayout(rightIOLayout);
    rightGroup->addWidget(m_lblState1Text);

    // 组装所有部分
    contentLayout->addLayout(leftGroup);
    contentLayout->addWidget(pistonImage, 0, Qt::AlignCenter);
    contentLayout->addLayout(rightGroup);

    mainLayout->addWidget(contentWidget);
}


// 【新增】辅助函数：切换 Label 颜色
void CylinderWidget::SetLabelActive(QLabel* lbl, bool active)
{
    if(active) lbl->setStyleSheet(STYLE_ACTIVE);
    else lbl->setStyleSheet(STYLE_INACTIVE);
}

// 【新增】核心逻辑：接收实时状态并刷新 UI
void CylinderWidget::UpdateRealTimeStatus(bool out0, bool in0, bool out1, bool in1)
{
    SetLabelActive(m_lblState0Out, out0);
    SetLabelActive(m_lblState0In, in0);
    SetLabelActive(m_lblState1Out, out1);
    SetLabelActive(m_lblState1In, in1);
}

// 【新增】鼠标点击事件
void CylinderWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint clickPos = event->pos();
    // 简单的判断：点击左半边触发状态0，右半边触发状态1
    QWidget* clickedChild = this->childAt(clickPos);
	if (m_lblState0Text== clickedChild)
		emit commandTriggered(0); // 用户想缩回
	else if (m_lblState1Text== clickedChild)
		emit commandTriggered(1); //用户想伸出

    QFrame::mousePressEvent(event);
}
