#ifndef CMULTEXTLINEEDIT_H
#define CMULTEXTLINEEDIT_H

#include <QLineEdit>
#include <QWidget>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
//多填文本输入框
class CMulTextLineEditItem;
class CMulTextLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CMulTextLineEdit(QWidget *parent = nullptr);
    ~CMulTextLineEdit();
    //设置按钮图标
    void SetIcon(QString path);
    //设置默认文本
    void SetText(QString text);
    //获取文本内容
    QString GetText();
    //设置输入单个文本长度限制
    void SetMaxLength(int length);

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
    CMulTextLineEditItem *m_mulWidget = nullptr;
    QPushButton *m_mulBtn=nullptr;
    QTimer m_hideTimer;  // 新增定时器
};

class CMulTextLineEditItem : public QDialog
{
    Q_OBJECT
public:
    CMulTextLineEditItem(QWidget *parent = nullptr);
    ~CMulTextLineEditItem() override;

    void SetText(QString text);
    void SetMaxLength(int length);
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
    QLineEdit  *m_lineEdit = nullptr;
    QPushButton *m_btn = nullptr;

    QList<QString> m_textInfo;

    bool m_isShow = false;
};

#endif // CMULTEXTLINEEDIT_H
