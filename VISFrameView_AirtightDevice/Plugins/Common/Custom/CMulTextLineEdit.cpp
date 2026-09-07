#include "CMulTextLineEdit.h"
#include <QAction>
#include <QWidgetAction>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QScreen>
#include <QGuiApplication>

CMulTextLineEdit::CMulTextLineEdit(QWidget *parent):QLineEdit(parent)
{
    InitWidget();
    m_hideTimer.setInterval(100);  // 每100ms检查一次鼠标位置
    connect(&m_hideTimer, &QTimer::timeout, this, [this](){
        checkMousePosition();
    });
}

CMulTextLineEdit::~CMulTextLineEdit()
{

}

void CMulTextLineEdit::SetIcon(QString path)
{
    m_mulBtn->setIcon(QIcon(path));
}

void CMulTextLineEdit::SetText(QString text)
{
    if(text.isEmpty())
    {
        this->setToolTip("");
        this->setText("");
    }
    else
    {
        this->setToolTip(text);
        this->setText(ElideText(this->font(),this->width()-25,text));
    }
}

QString CMulTextLineEdit::GetText()
{
    return this->toolTip();
}

void CMulTextLineEdit::SetMaxLength(int length)
{
    if(m_mulWidget!=nullptr)
    {
       m_mulWidget->SetMaxLength(length);
    }
}

void CMulTextLineEdit::showEvent(QShowEvent *event)
{
    SetText(GetText());
    m_mulBtn->setFixedHeight(this->height()-2);
    QWidget::showEvent(event);
}

void CMulTextLineEdit::leaveEvent(QEvent *event)
{
    checkMousePosition();
    QLineEdit::leaveEvent(event);
}

bool CMulTextLineEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_mulWidget)
    {
        if (event->type() == QEvent::Enter)
        {
            m_hideTimer.stop();  // 鼠标进入弹窗时暂停检测
        }
        else if (event->type() == QEvent::Leave)
        {
            m_hideTimer.start(); // 鼠标离开弹窗时恢复检测
        }
    }
    return QLineEdit::eventFilter(watched, event);
}

QString CMulTextLineEdit::ElideText(QFont font,int width,QString strInfo)
{
    QFontMetrics fontMetrics(font);
    //如果当前字体下，字符串长度大于指定宽度
    if(fontMetrics.horizontalAdvance(strInfo) > width)
    {
        strInfo= QFontMetrics(font).elidedText(strInfo, Qt::ElideRight, width);
    }
    return strInfo;
}

void CMulTextLineEdit::checkMousePosition()
{
    if (!m_mulWidget->isVisible()) return;

    // 计算控件和弹窗的全局坐标范围
    QRect mainRect = QRect(
                this->mapToGlobal(QPoint(0, 0)),
                this->size()
                );
    QRect popupRect = QRect(
                m_mulWidget->mapToGlobal(QPoint(0, 0)),
                m_mulWidget->size()
                );

    // 获取当前鼠标全局坐标
    QPoint mousePos = QCursor::pos();

    // 如果鼠标不在控件和弹窗区域内则隐藏
    if (!mainRect.contains(mousePos) && !popupRect.contains(mousePos))
    {
        m_mulWidget->hide();
        m_hideTimer.stop();
    }
}


void CMulTextLineEdit::InitWidget()
{
    m_mulBtn=new QPushButton(this);
    m_mulBtn->setCursor(Qt::PointingHandCursor);
    m_mulBtn->setFixedSize(17, 20);
    m_mulBtn->setIcon(QIcon(":/Img/MyStyle/muladd.png"));

    //防止文本框输入内容位于按钮之下
    QMargins margins = this->textMargins();
    this->setTextMargins(margins.left(), margins.top(), m_mulBtn->width(), margins.bottom());
    QHBoxLayout *pLayout = new QHBoxLayout();
    pLayout->addStretch();
    pLayout->addWidget(m_mulBtn);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 1, 0);
    this->setLayout(pLayout);

    this->setFocusPolicy(Qt::NoFocus);
    m_mulWidget = new CMulTextLineEditItem(this);
    m_mulWidget->hide();

    connect(m_mulBtn,&QPushButton::clicked,this,&CMulTextLineEdit::slotOpenWidget);
    connect(m_mulWidget,&CMulTextLineEditItem::sigText,this,&CMulTextLineEdit::slotUpdateText);
}

void CMulTextLineEdit::slotOpenWidget()
{
    m_mulWidget->SetText(this->toolTip());
    //QPoint p = QCursor::pos();
    m_mulWidget->setFixedWidth(this->width());
    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
    //QPoint parentPos = this->mapToParent(QPoint(0, 0));
    int yoffset = this->height();
    m_mulWidget->move(globalPos.x(),globalPos.y()+yoffset);
    m_mulWidget->show();
    m_hideTimer.start();  // 新增
    //检测边缘
    QScreen* screen = QGuiApplication::screenAt(globalPos);
    if (screen)
    {
        QRect screenRect = screen->availableGeometry();
        if ((globalPos.y() + yoffset + m_mulWidget->height()) > screenRect.bottom())
        {
            // 向上弹出
            m_mulWidget->move(globalPos.x(), globalPos.y() - m_mulWidget->height());
        }
    }
}

void CMulTextLineEdit::slotUpdateText(QString text)
{
    SetText(text);
}


CMulTextLineEditItem::CMulTextLineEditItem(QWidget *parent):QDialog(parent)
{
    InitWidget();
}

CMulTextLineEditItem::~CMulTextLineEditItem()
{

}

void CMulTextLineEditItem::SetText(QString text)
{
    m_listWidget->clear();
    if(text.isEmpty())
    {
        return;
    }
    QStringList list = text.split(";");
    m_listWidget->addItems(list);
}

void CMulTextLineEditItem::SetMaxLength(int length)
{
    if(m_lineEdit!=nullptr)
    {
       m_lineEdit->setMaxLength(length);
    }
}

void CMulTextLineEditItem::InitWidget()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    m_listWidget = new QListWidget(this);
    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_listWidget,&QListWidget::customContextMenuRequested,this,&CMulTextLineEditItem::slotCustomContextMenuRequested);
    m_lineEdit = new QLineEdit;
    m_lineEdit->clear();

    m_btn = new QPushButton(this);
    connect(m_lineEdit,&QLineEdit::returnPressed,this,&CMulTextLineEditItem::slotAddText);
    connect(m_btn,&QPushButton::clicked,this,&CMulTextLineEditItem::slotAddText);
    m_btn->setText(QStringLiteral("确认"));
    m_btn->setFixedWidth(40);
    m_btn->setAutoDefault(false);
    QGridLayout *grid = new QGridLayout(this);
    QHBoxLayout *bbox = new QHBoxLayout;
    bbox->addWidget(m_lineEdit);
    bbox->addWidget(m_btn);
    bbox->setStretchFactor(m_lineEdit,5);
    bbox->setStretchFactor(m_btn,1);

    grid->addWidget(m_listWidget,0,0);
    grid->addLayout(bbox,1,0);
    grid->setContentsMargins(4,4,4,4);
}

void CMulTextLineEditItem::leaveEvent(QEvent *event)
{
    // 当鼠标离开部件时隐藏部件
    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
    QPoint pos = QCursor::pos();
    //qDebug()<<globalPos<<pos<<this->width()<<this->height();
    if(pos.x()>(globalPos.x()+this->width()) || pos.x()<(globalPos.x())
            || pos.y()<(globalPos.y()) || pos.y()>(globalPos.y()+this->height()))
    {
        hide();
    }
    QWidget::leaveEvent(event);
}

void CMulTextLineEditItem::slotAddText()
{
    m_textInfo.clear();
    for (int i=0;i<m_listWidget->count();++i)
    {
        m_textInfo.append(m_listWidget->item(i)->text());
    }
    if(!m_lineEdit->text().isEmpty())
    {
        QString linfo = m_lineEdit->text();
        if(!m_textInfo.contains(linfo))
        {
            m_textInfo.append(m_lineEdit->text());
        }
        m_lineEdit->clear();
    }

    m_listWidget->clear();
    //std::sort(m_textInfo.begin(),m_textInfo.end());

    m_listWidget->addItems(m_textInfo);
    QString text = "";
    if(m_textInfo.isEmpty())
    {
        text = "";
    }
    else
    {
        text = m_textInfo.join(";");
    }
    emit sigText(text);
}
void CMulTextLineEditItem::slotCustomContextMenuRequested(const QPoint &pos)
{
    QMenu *pMenu = new QMenu;
    QAction *pCopy = new QAction(QStringLiteral("复制"),pMenu);
    QAction *pDelete = new QAction(QStringLiteral("删除"), pMenu);
    connect(pCopy,&QAction::triggered,this,[=]{
        QListWidgetItem *item = m_listWidget->currentItem();
        if(item == nullptr)
        {
            return;
        }
        // 获取文本
        QString textToCopy = item->text();
        // 访问全局粘贴板对象
        QClipboard *clipboard = QApplication::clipboard();
        // 将文本复制到粘贴板
        clipboard->setText(textToCopy);
    });
    connect(pDelete,SIGNAL(triggered(bool)),this,SLOT(delListItem()));

    //当ListWidgetitem不为空时显示菜单
    if(m_listWidget->itemAt(pos)!=nullptr)
    {
        pMenu->addAction(pCopy);
        pMenu->addAction(pDelete);
    }
    //在鼠标位置显示菜单
    pMenu->exec(QCursor::pos());

    delete pMenu;
    pMenu = nullptr;
}

void CMulTextLineEditItem::delListItem()
{
    QListWidgetItem *item = m_listWidget->currentItem();
    if(item == nullptr)
    {
        return;
    }
    m_listWidget->takeItem(m_listWidget->row(item));
    delete item;
    item = nullptr;

    slotAddText();
}
