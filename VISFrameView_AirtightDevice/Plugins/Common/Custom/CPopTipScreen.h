#ifndef CPOPTIPSCREEN_H
#define CPOPTIPSCREEN_H

#include <QDialog>
#include <QTimer>
#include <QLabel>

class CPopTipScreen : public QDialog
{
    Q_OBJECT

public:
    explicit CPopTipScreen(QWidget *parent = nullptr);
    ~CPopTipScreen();

    void showMessage(QString str);
private slots:
    void onMove();
    void onStay();
    void onClose();
private:
    QTimer * m_pShowTimer;
    QTimer * m_pStayTimer;
    QTimer * m_pCloseTimer;
    QPoint	 m_point;
    QLabel*  m_pLabel;
    int      m_nDesktopHeight;
};

#endif
