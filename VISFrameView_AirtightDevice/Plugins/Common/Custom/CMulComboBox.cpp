#include "CMulComboBox.h"

CMulComboBox::CMulComboBox(QWidget *parent):QComboBox(parent)
{
    initWidget();
}

CMulComboBox::~CMulComboBox()
{

}

void CMulComboBox::hidePopup()
{
    int width = this->width();
    int height = this->height();
    int x = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x() + geometry().x();
    int y = QCursor::pos().y() - mapToGlobal(geometry().topLeft()).y() + geometry().y();
    if (x >= 0 && x <= width && y >= this->height() && y <= height + this->height())
    {
    }
    else
    {
        QComboBox::hidePopup();
    }
}

void CMulComboBox::AddItem(const QString& text, const QVariant& userData)
{
    Q_UNUSED(userData)

    m_listWidget->blockSignals(true);
    QListWidgetItem *pItem = new QListWidgetItem(text, m_listWidget);
    pItem->setData(Qt::UserRole, userData);
    pItem->setCheckState(Qt::Unchecked);
    //设置QListWidgetItem 可交互且可以选中和取消选中
    pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    QComboBox::addItem(text);
    QComboBox::setCurrentText("");
    m_listWidget->blockSignals(false);
}

void CMulComboBox::AddItems(const QStringList &texts)
{
    foreach (auto text, texts)
    {
        AddItem(text);
    }
}

void CMulComboBox::SetAllSelectFeild(QString allstr)
{
    m_allstr = allstr;
}

void CMulComboBox::initWidget()
{
    m_listWidget = new QListWidget;
    m_listWidget->setFixedHeight(100);
    connect(m_listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotItemClicked(QListWidgetItem *)));
    this->setView(m_listWidget);
    //设置Editable为true，lineEdit() 函数才不返回空
    this->setEditable(true);
    this->lineEdit()->setReadOnly(true);
    this->lineEdit()->installEventFilter(this);
}

QStringList CMulComboBox::CurrentText()
{
    QStringList list;
    list.clear();
    if(!lineEdit()->toolTip().isEmpty())
    {
        list = lineEdit()->toolTip().split(";");
    }
    return list;
}

void CMulComboBox::Clear()
{
    this->lineEdit()->clear();
    m_listWidget->clear();
}

void CMulComboBox::SetSelectText(const QString& text)
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
            if( Qt::Checked == m_listWidget->item(i)->checkState() && m_allstr != m_listWidget->item(i)->text())
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

void CMulComboBox::SetSelectText(const QStringList& text_list)
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

bool CMulComboBox::eventFilter(QObject *watched, QEvent *event)
{
    //设置点击输入框也可以弹出下拉框
    if (watched == lineEdit() && event->type() == QEvent::MouseButtonRelease && this->isEnabled())
    {
        showPopup();
        return true;
    }
    return false;
}

void CMulComboBox::wheelEvent(QWheelEvent *event)
{
    //禁用QComboBox默认的滚轮事件
    Q_UNUSED(event)
}

void CMulComboBox::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
}

void CMulComboBox::slotItemClicked(QListWidgetItem *item)
{
    int count = m_listWidget->count();
    QString text = item->text();

    if(m_allstr == text)
    {
        if(item->checkState() == Qt::Checked)
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
        if(item->checkState() == Qt::Checked)
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
            if( Qt::Checked == m_listWidget->item(i)->checkState() && m_allstr != m_listWidget->item(i)->text())
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
                if(m_allstr == m_listWidget->item(i)->text())
                {
                    m_listWidget->item(i)->setCheckState(Qt::Unchecked);
                }
            }
        }
    }
    updateText();
}

void CMulComboBox::updateText()
{
    QStringList selectDataList;
    int count = m_listWidget->count();
    for (int i = 0; i < count; i++)
    {
        if( Qt::Checked == m_listWidget->item(i)->checkState())
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
    lineEdit()->setToolTip(textshow);
    lineEdit()->setText(ElideText(this->font(),this->width()-25,textshow));

    emit sigUpdateText();
}

QString CMulComboBox::ElideText(QFont font,int width,QString strInfo)
{
    QFontMetrics fontMetrics(font);
    //如果当前字体下，字符串长度大于指定宽度
    if(fontMetrics.horizontalAdvance(strInfo) > width)
    {
        strInfo= QFontMetrics(font).elidedText(strInfo, Qt::ElideRight, width);
    }
    return strInfo;
}
