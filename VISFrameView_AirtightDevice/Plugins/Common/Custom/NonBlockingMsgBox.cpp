#include "NonBlockingMsgBox.h"
#include <QApplication>
#include <QScreen>

NonBlockingMsgBox::NonBlockingMsgBox(QWidget *parent,
                                     const QString &title,
                                     const QString &text,
                                     StandardButtons buttons)
    : QDialog(parent)
{
    setWindowTitle(title);
    setModal(false); // 明确设置为非模态

    // 设置窗口标志，确保对话框始终可见但不会阻塞
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint);

    this->setStyleSheet("font:24pt");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 图标和文本区域
    QHBoxLayout *contentLayout = new QHBoxLayout();
    m_iconLabel = new QLabel();
    m_iconLabel->setFixedSize(64, 64);
    contentLayout->addWidget(m_iconLabel);

    m_textLabel = new QLabel(text);
    m_textLabel->setWordWrap(true);
    contentLayout->addWidget(m_textLabel);
    contentLayout->setStretchFactor(m_textLabel, 1);

    mainLayout->addLayout(contentLayout);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    if (buttons & Ok)
        buttonLayout->addWidget(createButton(Ok));
    buttonLayout->addStretch();
    if (buttons & Cancel)
        buttonLayout->addWidget(createButton(Cancel));
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    setFixedSize(600, 300);
}

void NonBlockingMsgBox::setIcon(const QString &iconPath)
{
    QPixmap pixmap(iconPath);
    if (!pixmap.isNull()) {
        m_iconLabel->setPixmap(pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

NonBlockingMsgBox::StandardButton NonBlockingMsgBox::standardButton(QAbstractButton *button) const
{
    return m_buttonMap.value(static_cast<QPushButton*>(button), NoButton);
}

NonBlockingMsgBox::StandardButton NonBlockingMsgBox::waitForResult()
{
    m_eventLoop = new QEventLoop(this);
    m_eventLoop->exec();
    return m_result;
}

QPushButton* NonBlockingMsgBox::createButton(StandardButton type)
{
    QPushButton *button = new QPushButton();
    button->setFixedSize(120,60);
    switch (type) {
    case Ok:
        button->setText(QStringLiteral("确定"));
        break;
    case Cancel:
        button->setText(QStringLiteral("取消"));
        break;
    default:
        break;
    }

    m_buttonMap.insert(button, type);
    connect(button, &QPushButton::clicked, this, &NonBlockingMsgBox::onButtonClicked);

    return button;
}

void NonBlockingMsgBox::onButtonClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (clickedButton && m_buttonMap.contains(clickedButton)) {
        m_result = m_buttonMap.value(clickedButton);
        emit buttonClicked(m_result);

        if (m_eventLoop && m_eventLoop->isRunning()) {
            m_eventLoop->exit();
        }
    }
    close();
}

MsgBoxManager& MsgBoxManager::instance()
{
    static MsgBoxManager instance;
    return instance;
}

NonBlockingMsgBox::StandardButton MsgBoxManager::showInfo(QWidget *parent,
                                                          const QString &title,
                                                          const QString &text,
                                                          NonBlockingMsgBox::StandardButtons buttons)
{
    NonBlockingMsgBox *msgBox = new NonBlockingMsgBox(parent, title, text, buttons);
    msgBox->setIcon(":/Img/MyStyle/info.png");
    centerOnScreen(msgBox);

    // 连接信号处理用户响应
    connect(msgBox, &NonBlockingMsgBox::buttonClicked, [msgBox](NonBlockingMsgBox::StandardButton button) {
        msgBox->deleteLater(); // 确保内存正确释放
    });

    msgBox->setAttribute(Qt::WA_DeleteOnClose, false);
    msgBox->show();
    msgBox->activateWindow();
    msgBox->raise();

    return msgBox->waitForResult();

}

NonBlockingMsgBox::StandardButton MsgBoxManager::showWarning(QWidget *parent,
                                                             const QString &title,
                                                             const QString &text,
                                                             NonBlockingMsgBox::StandardButtons buttons)
{
    NonBlockingMsgBox *msgBox = new NonBlockingMsgBox(parent, title, text, buttons);
    msgBox->setIcon(":/Img/MyStyle/warnning.png");
    centerOnScreen(msgBox);

    connect(msgBox, &NonBlockingMsgBox::buttonClicked, [msgBox](NonBlockingMsgBox::StandardButton button) {
        msgBox->deleteLater();
    });

    msgBox->setAttribute(Qt::WA_DeleteOnClose, false);
    msgBox->show();
    msgBox->activateWindow();
    msgBox->raise();

    // 使用QEventLoop等待结果
    return msgBox->waitForResult();
}

NonBlockingMsgBox::StandardButton MsgBoxManager::showCritical(QWidget *parent,
                                                              const QString &title,
                                                              const QString &text,
                                                              NonBlockingMsgBox::StandardButtons buttons)
{
    NonBlockingMsgBox *msgBox = new NonBlockingMsgBox(parent, title, text, buttons);
    msgBox->setIcon(":/Img/MyStyle/fatal.png");
    centerOnScreen(msgBox);

    connect(msgBox, &NonBlockingMsgBox::buttonClicked, [msgBox](NonBlockingMsgBox::StandardButton button) {
        msgBox->deleteLater();
    });

    msgBox->setAttribute(Qt::WA_DeleteOnClose, false);
    msgBox->show();
    msgBox->activateWindow();
    msgBox->raise();

    // 使用QEventLoop等待结果
    return msgBox->waitForResult();
}

void MsgBoxManager::centerOnScreen(NonBlockingMsgBox *msgBox)
{
    QList<QScreen*> screens = QGuiApplication::screens();
    if (!screens.isEmpty()) {
        QScreen *primaryScreen = screens.first();
        QRect screenGeometry = primaryScreen->geometry();

        int x = screenGeometry.x() + (screenGeometry.width() - msgBox->width()) / 2;
        int y = screenGeometry.y() + (screenGeometry.height() - msgBox->height()) / 2;
        msgBox->move(x, y);
    }
}
