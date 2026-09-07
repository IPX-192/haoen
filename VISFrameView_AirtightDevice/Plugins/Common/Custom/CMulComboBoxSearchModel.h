#ifndef CMULCOMBOBOXSEARCHMODEL_H
#define CMULCOMBOBOXSEARCHMODEL_H

#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QEvent>
#include <QWidget>
#include <QDialog>
#include <QTimer>

class CMulComboBoxSearchItemModel;
class CMulComboBoxSearchModel : public QLineEdit
{
    Q_OBJECT
public:
    CMulComboBoxSearchModel(QWidget* parent = nullptr);
    ~CMulComboBoxSearchModel();
    //设置按钮图标
    void SetIcon(QString path);
    //添加一条选项
    void AddItem(const QString& text);
    //添加多条选项
    void AddItems(const QStringList& texts);
    //定义"全部"选中字段,传入则提供一键全选能力
    void SetAllSelectFeild(QString allstr);
    //返回当前选中选项
    QStringList CurrentText();
    //返回所有项
    QStringList GetAllText();
    //清空所有内容
    void ClearSelect();
    //清理文本
    void Clear();
    //设置选中文本--单
    void SetSelectText(const QString& text);
    //设置选中文本--多
    void SetSelectText(const QStringList& text_list);
    //是否显示搜索框
    void SetSearchEnable(bool eable);
protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void leaveEvent(QEvent* event) override;  // 新增
    virtual bool eventFilter(QObject* watched, QEvent* event) override;  // 新增
private slots:
    void slotOpenWidget();
    void slotUpdateText(QString text);
signals:
    void sigUpdateText(QString text);
private:
    void initWidget();
    QString ElideText(QFont font,int width,QString strInfo);
    void checkMousePosition();
private:
    CMulComboBoxSearchItemModel *m_mulWidget = nullptr;
    QPushButton *m_mulBtn = nullptr;
    QStringList m_itemTexts;
    QTimer m_hideTimer;  // 新增定时器
};

class CMulComboBoxSearchItemModel : public QDialog
{
    Q_OBJECT
public:
    CMulComboBoxSearchItemModel(QWidget *parent = nullptr);
    ~CMulComboBoxSearchItemModel() override;

    //添加多条选项
    void AddItems(const QStringList& texts);
    //定义"全部"选中字段,传入则提供一键全选能力
    void SetAllSelectFeild(QString allstr);
    //设置选中文本--单
    void SetSelectText(const QString& text);
    //设置选中文本--多
    void SetSelectText(const QStringList& text_list);
    //清理选中
    void ClearSelect();
    //清理文本
    void Clear();
    //是否显示搜索框
    void SetSearchEnable(bool eable);
private:
    void initWidget();
    void updateText();
protected:
    virtual void leaveEvent(QEvent *event) override;
signals:
    void sigText(QString text);
private slots:
    void slotItemClicked(QListWidgetItem *item);
    void slotSearch(const QString &text);
private:
    QListWidget *m_listWidget = nullptr;
    QLineEdit  *m_lineEdit = nullptr;
    QString m_allstr = "";
};

#endif // CMULCOMBOBOXSEARCHMODEL_H

