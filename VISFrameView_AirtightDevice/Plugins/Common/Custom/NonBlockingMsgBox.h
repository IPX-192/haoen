#ifndef NONBLOCKINGMSGBOX_H
#define NONBLOCKINGMSGBOX_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEventLoop>

class NonBlockingMsgBox : public QDialog
{
    Q_OBJECT

public:
    enum StandardButton {
        NoButton = 0x00000000,
        Ok = 0x00000400,
        Cancel = 0x00400000,
    };
    Q_ENUM(StandardButton)
    Q_DECLARE_FLAGS(StandardButtons, StandardButton)
    Q_FLAG(StandardButtons)

    explicit NonBlockingMsgBox(QWidget *parent = nullptr,
                              const QString &title = "",
                              const QString &text = "",
                              StandardButtons buttons = Ok);

    void setIcon(const QString &iconPath);
    StandardButton standardButton(QAbstractButton *button) const;

    StandardButton waitForResult();
signals:
    void buttonClicked(NonBlockingMsgBox::StandardButton button);

private slots:
    void onButtonClicked();

private:
    QLabel *m_iconLabel;
    QLabel *m_textLabel;
    QPushButton *createButton(StandardButton type);
    QHash<QPushButton*, StandardButton> m_buttonMap;
    QEventLoop *m_eventLoop;
    StandardButton m_result;
};

class MsgBoxManager : public QObject
{
    Q_OBJECT

public:
    static MsgBoxManager& instance();

    NonBlockingMsgBox::StandardButton showInfo(QWidget *parent,
                  const QString &title,
                  const QString &text,
                  NonBlockingMsgBox::StandardButtons buttons);

    NonBlockingMsgBox::StandardButton showWarning(QWidget *parent,
                     const QString &title,
                     const QString &text,
                     NonBlockingMsgBox::StandardButtons buttons);

    NonBlockingMsgBox::StandardButton showCritical(QWidget *parent,
                      const QString &title,
                      const QString &text,
                      NonBlockingMsgBox::StandardButtons buttons);

private:
    MsgBoxManager() = default;
    void centerOnScreen(NonBlockingMsgBox *msgBox);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NonBlockingMsgBox::StandardButtons)

#endif // NONBLOCKINGMSGBOX_H
