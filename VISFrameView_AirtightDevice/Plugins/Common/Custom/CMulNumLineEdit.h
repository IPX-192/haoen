#ifndef CMULNUMLINEEDIT_H
#define CMULNUMLINEEDIT_H

#include <QLineEdit>
#include <QWidget>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>

//多填数字输入框
class CMulNumLineEditItem;
class CMulNumLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CMulNumLineEdit(QWidget *parent = nullptr);
    ~CMulNumLineEdit();
    //设置按钮图标
    void SetIcon(QString path);
    //设置默认文本
    void SetText(QString text);
    //获取文本内容
    QString GetText();
    //设置输入数据大小限制
    void SetRange(int min,int max);

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void leaveEvent(QEvent* event) override;  // 新增
    virtual bool eventFilter(QObject* watched, QEvent* event) override;  // 新增
private:
    void InitWidget();
    QString ElideText(QFont font,int width,QString strInfo);
    void checkMousePosition();
private slots:
    void slotOpenWidget();

    void slotUpdateText(QString text);
private:
    CMulNumLineEditItem *m_mulWidget = nullptr;
    QPushButton *m_mulBtn = nullptr;
    QTimer m_hideTimer;  // 新增定时器
};

class CMulNumLineEditItem : public QDialog
{
    Q_OBJECT
public:
    CMulNumLineEditItem(QWidget *parent = nullptr);
    ~CMulNumLineEditItem() override;

    void SetText(QString text);
    void SetRange(int min,int max);
private:
    void InitWidget();
protected:
    virtual void leaveEvent(QEvent *event) override;
private slots:
    void slotAddText();

    void slotCustomContextMenuRequested(const QPoint &pos);

    void delListItem();
signals:
    void sigText(QString text);
private:
    QListWidget *m_listWidget = nullptr;
    QSpinBox  *m_spinBox = nullptr;
    QPushButton *m_btn = nullptr;

    QList<int> m_textInfo;

    bool m_isShow = false;
};

#endif // CMULNUMLINEEDIT_H
