#include "CMulComboBoxSearch.h"
#include <QAction>
#include <QWidgetAction>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>

CMulComboBoxSearch::CMulComboBoxSearch(QWidget *parent):QLineEdit(parent)
{
    initWidget();
    m_hideTimer.setInterval(100);  // 每100ms检查一次鼠标位置
    connect(&m_hideTimer, &QTimer::timeout, this, [this](){
        checkMousePosition();
    });
}

CMulComboBoxSearch::~CMulComboBoxSearch()
{

}

void CMulComboBoxSearch::SetIcon(QString path)
{
    m_mulBtn->setIcon(QIcon(path));
}

void CMulComboBoxSearch::AddItem(const QString& text)
{
    if(text.isEmpty())
    {
        return;
    }
    QStringList list;
    list.append(text);
    m_itemTexts.append(text);
    m_mulWidget->AddItems(list);
}

void CMulComboBoxSearch::AddItems(const QStringList &texts)
{
    if(texts.isEmpty())
    {
        return;
    }
    m_itemTexts = texts;
    m_mulWidget->AddItems(texts);
}

void CMulComboBoxSearch::SetAllSelectFeild(QString allstr)
{
    m_mulWidget->SetAllSelectFeild(allstr);
}

void CMulComboBoxSearch::initWidget()
{
    m_itemTexts.clear();
    m_mulBtn=new QPushButton(this);
    m_mulBtn->setCursor(Qt::PointingHandCursor);
    m_mulBtn->setFixedSize(17, 20);
    m_mulBtn->setIcon(QIcon(":/Img/MyStyle/muldown.png"));

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
    m_mulWidget = new CMulComboBoxSearchItem(this);
    m_mulWidget->hide();
    m_mulWidget->installEventFilter(this);  // 新增

    connect(m_mulBtn,&QPushButton::clicked,this,&CMulComboBoxSearch::slotOpenWidget);
    connect(m_mulWidget,&CMulComboBoxSearchItem::sigText,this,&CMulComboBoxSearch::slotUpdateText);
}

QStringList CMulComboBoxSearch::CurrentText()
{
    QStringList list;
    list.clear();
    if(!this->toolTip().isEmpty())
    {
        list = this->toolTip().split(";");
    }
    return list;
}

QStringList CMulComboBoxSearch::GetAllText()
{
    return m_itemTexts;
}

void CMulComboBoxSearch::ClearSelect()
{
    m_mulWidget->ClearSelect();
}

void CMulComboBoxSearch::Clear()
{
    m_itemTexts.clear();
    m_mulWidget->ClearSelect();
    m_mulWidget->Clear();
}

void CMulComboBoxSearch::SetSelectText(const QString& text)
{
    m_mulWidget->SetSelectText(text);
}

void CMulComboBoxSearch::SetSelectText(const QStringList& text_list)
{
    m_mulWidget->SetSelectText(text_list);
}

void CMulComboBoxSearch::SetSearchEnable(bool eable)
{
    if( nullptr == m_mulWidget)
    {
        return;
    }
    m_mulWidget->SetSearchEnable(eable);
}

void CMulComboBoxSearch::showEvent(QShowEvent *event)
{
    m_mulWidget->SetSelectText(CurrentText());
    m_mulBtn->setFixedHeight(this->height()-2);
    QWidget::showEvent(event);
}

void CMulComboBoxSearch::leaveEvent(QEvent *event)
{
    checkMousePosition();
    QLineEdit::leaveEvent(event);
}

bool CMulComboBoxSearch::eventFilter(QObject *watched, QEvent *event)
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
// 核心检测逻辑
void CMulComboBoxSearch::checkMousePosition()
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
void CMulComboBoxSearch::slotOpenWidget()
{
    //正常显示
    m_mulWidget->SetSelectText(CurrentText());
    int w = this->width();
    m_mulWidget->setFixedWidth(w);
    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
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

void CMulComboBoxSearch::slotUpdateText(QString text)
{
    this->setToolTip(text);
    this->setText(ElideText(this->font(),this->width()-25,text));
}

QString CMulComboBoxSearch::ElideText(QFont font,int width,QString strInfo)
{
    QFontMetrics fontMetrics(font);
    //如果当前字体下，字符串长度大于指定宽度
    if(fontMetrics.horizontalAdvance(strInfo) > width)
    {
        strInfo= QFontMetrics(font).elidedText(strInfo, Qt::ElideRight, width);
    }
    return strInfo;
}


CMulComboBoxSearchItem::CMulComboBoxSearchItem(QWidget *parent):QDialog(parent)
{
    initWidget();
}

CMulComboBoxSearchItem::~CMulComboBoxSearchItem()
{

}

void CMulComboBoxSearchItem::AddItems(const QStringList &texts)
{
    m_listWidget->blockSignals(true);
    m_listWidget->clear();
    foreach(auto text,texts)
    {
        QListWidgetItem * item = new QListWidgetItem(m_listWidget);
        item->setText(text);
        item->setCheckState(Qt::Unchecked);
        m_listWidget->addItem(item);
    }
    m_listWidget->blockSignals(false);
}

void CMulComboBoxSearchItem::SetAllSelectFeild(QString allstr)
{
    m_allstr = allstr;
}

void CMulComboBoxSearchItem::SetSelectText(const QString &text)
{
    if(m_allstr == text)
    {
        int count = m_listWidget->count();
        for (int i = 0; i < count; i++)
        {
            m_listWidget->item(i)->setCheckState(Qt::Checked);
        }
    }
    else
    {
        int count = m_listWidget->count();
        for (int i = 0; i < count; i++)
        {
            if(text == m_listWidget->item(i)->text())
            {
                m_listWidget->item(i)->setCheckState(Qt::Checked);
            }
        }
        int num = 0;
        for (int i = 0; i < count; i++)
        {
            if(Qt::Checked == m_listWidget->item(i)->checkState() && m_allstr != m_listWidget->item(i)->text())
            {
                num++;
            }
        }
        if(num == (count-1))
        {
            for (int i = 0; i < count; i++)
            {
                if(m_allstr == m_listWidget->item(i)->text())
                {
                    m_listWidget->item(i)->setCheckState(Qt::Checked);
                }
            }
        }
    }
    updateText();
}

void CMulComboBoxSearchItem::SetSelectText(const QStringList &text_list)
{
    if(text_list.isEmpty())
    {
        return;
    }
    if(text_list.contains(m_allstr))
    {
        SetSelectText(m_allstr);
    }
    else
    {
        foreach (auto text,text_list)
        {
            SetSelectText(text);
        }
    }
}

void CMulComboBoxSearchItem::ClearSelect()
{
    for (int i = 0; i < m_listWidget->count(); i++)
    {
        m_listWidget->item(i)->setCheckState(Qt::Unchecked);
    }
    updateText();
}

void CMulComboBoxSearchItem::Clear()
{
    m_listWidget->blockSignals(true);
    m_listWidget->clear();
    m_listWidget->blockSignals(false);
}

void CMulComboBoxSearchItem::SetSearchEnable(bool eable)
{
    if(m_lineEdit !=nullptr)
    {
        m_lineEdit->setVisible(eable);
    }
}

void CMulComboBoxSearchItem::initWidget()
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    m_listWidget = new QListWidget(this);
    m_lineEdit = new QLineEdit;
    m_lineEdit->clear();
    m_lineEdit->setPlaceholderText(QStringLiteral("查找..."));
    m_lineEdit->setClearButtonEnabled(true);
    connect(m_lineEdit,&QLineEdit::textChanged,this,&CMulComboBoxSearchItem::slotSearch);
    connect(m_listWidget,&QListWidget::itemClicked,this,&CMulComboBoxSearchItem::slotItemClicked);

    QGridLayout *grid = new QGridLayout(this);

    grid->addWidget(m_listWidget,0,0);
    grid->addWidget(m_lineEdit,1,0);
    grid->setContentsMargins(4,4,4,4);
}

void CMulComboBoxSearchItem::leaveEvent(QEvent *event)
{
    // 计算窗口全局坐标范围
    QRect globalRect = QRect(
                this->mapToGlobal(QPoint(0, 0)),
                this->size()
                );

    // 如果鼠标不在窗口区域内，立即隐藏
    if (!globalRect.contains(QCursor::pos()))
    {
        this->hide();
    }
    QWidget::leaveEvent(event);
}

void CMulComboBoxSearchItem::slotItemClicked(QListWidgetItem *item)
{
    if( nullptr == item)
    {
        return;
    }
    int count = m_listWidget->count();
    QString text = item->text();

    if(m_allstr == text)
    {
        if(Qt::Checked == item->checkState())
        {
            for (int i = 0; i < count; i++)
            {
                m_listWidget->item(i)->setCheckState(Qt::Unchecked);
            }
        }
        else
        {
            for (int i = 0; i < count; i++)
            {
                m_listWidget->item(i)->setCheckState(Qt::Checked);
            }
        }
    }
    else
    {
        if(Qt::Checked == item->checkState())
        {
            item->setCheckState(Qt::Unchecked);
        }
        else
        {
            item->setCheckState(Qt::Checked);
        }
        int num = 0;
        for (int i = 0; i < count; i++)
        {
            if(Qt::Checked == m_listWidget->item(i)->checkState() && m_allstr != m_listWidget->item(i)->text())
            {
                num++;
            }
        }
        if(num == (count-1))
        {
            for (int i = 0; i < count; i++)
            {
                if(m_allstr == m_listWidget->item(i)->text())
                {
                    m_listWidget->item(i)->setCheckState(Qt::Checked);
                }
            }
        }
        if(num<(count-1))
        {
            for (int i = 0; i < count; i++)
            {
                QString textTmp = m_listWidget->item(i)->text();
                if(m_allstr == textTmp)
                {
                    m_listWidget->item(i)->setCheckState(Qt::Unchecked);
                }
            }
        }
    }
    updateText();
}

void CMulComboBoxSearchItem::slotSearch(const QString &text)
{
    for(int i = 0; i < m_listWidget->count(); i++)
    {
        if(m_listWidget->item(i)->text().contains(text, Qt::CaseInsensitive))
        {
            m_listWidget->item(i)->setHidden(false);
        }
        else
        {
            m_listWidget->item(i)->setHidden(true);
        }
    }
}

void CMulComboBoxSearchItem::updateText()
{
    QStringList selectDataList;
    int count = m_listWidget->count();
    for (int i = 0; i < count; i++)
    {
        if(Qt::Checked == m_listWidget->item(i)->checkState())
        {
            selectDataList.append(m_listWidget->item(i)->text());
        }
    }
    if(selectDataList.contains(m_allstr))
    {
        selectDataList.removeOne(m_allstr);
    }

    QString textshow = selectDataList.join(";");
    if(selectDataList.isEmpty())
    {
        textshow = "";
    }
    emit sigText(textshow);
}
