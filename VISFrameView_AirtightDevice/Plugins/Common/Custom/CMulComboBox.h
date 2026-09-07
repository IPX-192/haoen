#ifndef CMULCOMBOBOX_H
#define CMULCOMBOBOX_H

#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QEvent>
#include <QStandardItemModel>

class CMulComboBox : public QComboBox
{
    Q_OBJECT
public:
    CMulComboBox(QWidget* parent = nullptr);
    ~CMulComboBox();
    //添加一条选项
    void AddItem(const QString& text, const QVariant& userData = QVariant());
    //添加多条选项
    void AddItems(const QStringList& texts);
    //定义"全部"选中字段,传入则提供一键全选能力
    void SetAllSelectFeild(QString allstr);
    //返回当前选中选项
    QStringList CurrentText();
    //清空所有内容
    void Clear();
    //设置选中文本--单
    void SetSelectText(const QString& text);
    //设置选中文本--多
    void SetSelectText(const QStringList& text_list);
protected:
    //隐藏下拉框
    virtual void hidePopup();
    //事件过滤器
    virtual bool eventFilter(QObject *watched,QEvent *event);
    //滚轮事件
    virtual void wheelEvent(QWheelEvent *event);
    //按键事件
    virtual void keyPressEvent(QKeyEvent *event);
signals:
    void sigUpdateText();
private:
    void initWidget();
    QString ElideText(QFont font,int width,QString strInfo);
    void updateText();
public slots:
    //槽函数：点击下拉框选项
    void slotItemClicked(QListWidgetItem *item);
private:
    QListWidget* m_listWidget = nullptr;
    QString m_allstr = "";
};

#endif // CMULCOMBOBOX_H

