#ifndef CLOADINGWIDGET_H
#define CLOADINGWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QTimer>
#include <QWidget>

class CLoadingWidgetItem;
class CLoadingWidget : public QDialog
{
    Q_OBJECT
public:
    explicit CLoadingWidget(QWidget *parent = nullptr);
    ~CLoadingWidget();
    //设置提示文本
    void SetTextInfo(QString text);
    //开始播放
    void StartMovie();
    //停止播放
    void StopMovie();
private:
    QLabel *m_pLoadingLabelText = nullptr;
    CLoadingWidgetItem *m_pLoadingWidgetItem = nullptr;
};


class CLoadingWidgetItem : public QWidget
{
    Q_OBJECT
public:
    explicit CLoadingWidgetItem(QWidget *parent = nullptr);
    ~CLoadingWidgetItem() override;
protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void startUpdateTimer();
    void stopUpdateTimer();
private:
    QTimer *timer;
    int m_timerID = -1;
    int m_angle = 0;  // 旋转的角度

};

#endif // CLOADINGWIDGET_H
