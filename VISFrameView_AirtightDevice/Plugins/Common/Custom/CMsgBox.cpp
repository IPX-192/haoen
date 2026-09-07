#include "CMsgBox.h"
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QEvent>
#include <QApplication>
#include <QScreen>

CMsgBox::CMsgBox(QWidget *parent, const QString &title, const QString &text,CMsgBox::StandardButtons buttons): QDialog(parent)
{
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);  //去掉标题栏右上角的问号
    setWindowTitle(title);
    setMinimumSize(250, 120);
    setMaximumHeight(120);

    m_pButtonBox = new QDialogButtonBox(this);
    m_pButtonBox->setStandardButtons(QDialogButtonBox::StandardButtons(int(buttons)));

    QPushButton *pYesButton = m_pButtonBox->button(QDialogButtonBox::Yes);

    if (pYesButton != nullptr)
    {
        pYesButton->setObjectName("blueButton");
        pYesButton->setStyle(QApplication::style());
        pYesButton->setMinimumSize(120,30);
    }

    m_pIconLabel = new QLabel(this);
    m_pLabel = new QLabel(this);
    m_pLabel->setStyleSheet("QLabel{font-size:12pt;font-weight:Bold;}");

//    QPixmap pixmap(":/image/Resourse/Image/info.png");
//    m_pIconLabel->setPixmap(pixmap);
    m_pIconLabel->setFixedSize(35, 35);
    m_pIconLabel->setScaledContents(true);

    m_pLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pLabel->setObjectName("whiteLabel");
    m_pLabel->setOpenExternalLinks(true);
    m_pLabel->setText(text);

    m_pLine = new QFrame(this);
    m_pLine->setStyleSheet("QFrame{background-color: rgb(220, 255, 255);}");
    m_pLine->setFrameStyle(QFrame::HLine | QFrame::Raised);  //Sunken:凹陷，Raised：凸起

    QHBoxLayout* HLay = new QHBoxLayout;
    HLay->addWidget(m_pIconLabel, 1, Qt::AlignVCenter | Qt::AlignRight);
    HLay->addWidget(m_pLabel, 5, Qt::AlignCenter);
    HLay->setSpacing(10);
    HLay->setContentsMargins(2, 2, 2, 2);
    QHBoxLayout* HLay1 = new QHBoxLayout;
    HLay1->addWidget(m_pButtonBox, Qt::AlignRight);
    QVBoxLayout* VLay = new QVBoxLayout(this);
    VLay->addLayout(HLay, 10);
    VLay->addWidget(m_pLine, 1);
    VLay->addLayout(HLay1, 4);
    VLay->setSpacing(10);

    this->setLayout(VLay);
    translateUI();
    connect(m_pButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
}

CMsgBox::~CMsgBox()
{

}

void CMsgBox::changeEvent(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::LanguageChange:
        translateUI();
        break;
    default:
        break;
    }
}

void CMsgBox::translateUI()
{
    QPushButton *pOkButton = m_pButtonBox->button(QDialogButtonBox::Ok);
    if (pOkButton != nullptr){
        pOkButton->setText(QStringLiteral("确认"));
        pOkButton->setStyleSheet("QPushButton {font-size:12pt;width:80px;height:25px;background-color: rgb(240, 255, 255);border-radius:2px;}" \
        "QPushButton:hover {background-color: rgb(220, 255, 255);}" \
        "QPushButton:pressed {padding-top: 2px;padding-left: 1px;background-color: rgb(240, 255, 255);}" \
        "QPushButton:disabled {background-color: lightgray;color: gray;}"                          );
    }

    QPushButton *pCancelButton = m_pButtonBox->button(QDialogButtonBox::Cancel);
    if (pCancelButton != nullptr)
    {
        pCancelButton->setText(QStringLiteral("取消"));
        pCancelButton->setStyleSheet("QPushButton {font-size:12pt;width:80px;height:25px;background-color: rgb(240, 255, 255);border-radius:2px;}" \
        "QPushButton:hover {background-color: rgb(220, 255, 255);}" \
        "QPushButton:pressed {padding-top: 2px;padding-left: 1px;background-color: rgb(240, 255, 255);}" \
        "QPushButton:disabled {background-color: lightgray;color: gray;}"                          );
    }

}

CMsgBox::StandardButton CMsgBox::standardButton(QAbstractButton *button) const
{
    StandardButton flag = Cancel;
    if( QDialogButtonBox::Ok == m_pButtonBox->standardButton(button))
    {
        flag = Ok;
    }
    else if( QDialogButtonBox::Cancel == m_pButtonBox->standardButton(button))
    {
        flag = Cancel;
    }
    return flag;
}

QAbstractButton *CMsgBox::clickedButton() const
{
    return m_pClickedButton;
}

int CMsgBox::execReturnCode(QAbstractButton *button)
{
    int nResult = m_pButtonBox->standardButton(button);
    return nResult;
}

void CMsgBox::onButtonClicked(QAbstractButton *button)
{
    m_pClickedButton = button;
    done(execReturnCode(button));
}

void CMsgBox::setDefaultButton(QPushButton *button)
{
    if (!m_pButtonBox->buttons().contains(button))
        return;
    m_pDefaultButton = button;
    button->setDefault(true);
    button->setFocus();
}

void CMsgBox::setDefaultButton(CMsgBox::StandardButton button)
{
    setDefaultButton(m_pButtonBox->button(QDialogButtonBox::StandardButton(button)));
}

void CMsgBox::setIcon(const QString &icon)
{
    m_pIconLabel->setPixmap(QPixmap(icon));
}

CMsgBox::StandardButton CMsgBox::showInfo(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons)
{

    CMsgBox msgBox(parent, title, text, buttons);
    //msgBox.setModal(true);
    msgBox.setWindowFlag(Qt::WindowStaysOnTopHint);

    QList<QScreen *> screen = QGuiApplication::screens();
    if (screen.size() > 1)
    {
       msgBox.move(screen[0]->geometry().x() + screen[0]->geometry().width() / 2 - 100,
               screen[0]->geometry().y() + screen[0]->geometry().height() / 2 - 100);
    }
    msgBox.setIcon(":/Img/MyStyle/info.png");
    if (msgBox.exec() == -1)
        return Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

CMsgBox::StandardButton CMsgBox::showFatal(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons)
{
    CMsgBox msgBox(parent, title, text, buttons);
    //msgBox.setModal(true);
    msgBox.setWindowFlag(Qt::WindowStaysOnTopHint);

    QList<QScreen *> screen = QGuiApplication::screens();
    if (screen.size() > 1)
    {
       msgBox.move(screen[0]->geometry().x() + screen[0]->geometry().width() / 2 - 100,
               screen[0]->geometry().y() + screen[0]->geometry().height() / 2 - 100);
    }
    msgBox.setIcon(":/Img/MyStyle/fatal.png");
    if (msgBox.exec() == -1)
        return Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

CMsgBox::StandardButton CMsgBox::showSuccess(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons)
{
    CMsgBox msgBox(parent, title, text, buttons);
    //msgBox.setModal(true);
    msgBox.setWindowFlag(Qt::WindowStaysOnTopHint);

    QList<QScreen *> screen = QGuiApplication::screens();
    if (screen.size() > 1)
    {
       msgBox.move(screen[0]->geometry().x() + screen[0]->geometry().width() / 2 - 100,
               screen[0]->geometry().y() + screen[0]->geometry().height() / 2 - 100);
    }
    msgBox.setIcon(":/Img/MyStyle/chenggong.png");
    if (msgBox.exec() == -1)
        return Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

CMsgBox::StandardButton CMsgBox::showQuestion(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons)
{

    CMsgBox msgBox(parent, title, text, buttons);
    //msgBox.setModal(true);
    msgBox.setWindowFlag(Qt::WindowStaysOnTopHint);

    QList<QScreen *> screen = QGuiApplication::screens();
    if (screen.size() > 1)
    {
       msgBox.move(screen[0]->geometry().x() + screen[0]->geometry().width() / 2 - 100,
               screen[0]->geometry().y() + screen[0]->geometry().height() / 2 - 100);
    }
    msgBox.setIcon(":/Img/MyStyle/yiwenhao.png");
    if (msgBox.exec() == -1)
        return Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

CMsgBox::StandardButton CMsgBox::showWarning(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons)
{
    CMsgBox msgBox(parent, title, text, buttons);
    //msgBox.setModal(true);
    msgBox.setWindowFlag(Qt::WindowStaysOnTopHint);

    QList<QScreen *> screen = QGuiApplication::screens();
    if (screen.size() > 1)
    {
       msgBox.move(screen[0]->geometry().x() + screen[0]->geometry().width() / 2 - 100,
               screen[0]->geometry().y() + screen[0]->geometry().height() / 2 - 100);
    }
    msgBox.setIcon(":/Img/MyStyle/warnning.png");
    if (msgBox.exec() == -1)
        return Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

CMsgBox::StandardButton CMsgBox::showCritical(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons)
{
    CMsgBox msgBox(parent, title, text, buttons);
    //msgBox.setModal(true);
    msgBox.setWindowFlag(Qt::WindowStaysOnTopHint);

    QList<QScreen *> screen = QGuiApplication::screens();
    if (screen.size() > 1)
    {
       msgBox.move(screen[0]->geometry().x() + screen[0]->geometry().width() / 2 - 100,
               screen[0]->geometry().y() + screen[0]->geometry().height() / 2 - 100);
    }
    msgBox.setIcon(":/Img/MyStyle/fatal.png");
    if (msgBox.exec() == -1)
        return Cancel;
    return msgBox.standardButton(msgBox.clickedButton());
}

