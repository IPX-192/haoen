#ifndef CMSGBOX_H
#define CMSGBOX_H

#include <QDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QGridLayout>

class QLabel;
class QFrame;
class CMsgBox : public QDialog
{
    Q_OBJECT
public:
    enum StandardButton {
        Ok = 0x00000400,
        Cancel = 0x00400000,
    };
    Q_ENUM(StandardButton)
    Q_DECLARE_FLAGS(StandardButtons, StandardButton)
    Q_FLAG(StandardButtons)

    explicit CMsgBox(QWidget *parent = nullptr, const QString &title = tr("Tip"), const QString &text = "",CMsgBox::StandardButtons buttons = CMsgBox::Ok);

    ~CMsgBox();

    QAbstractButton *clickedButton() const;
    StandardButton standardButton(QAbstractButton *button) const;
    // 设置默认按钮
    void setDefaultButton(QPushButton *button);
    void setDefaultButton(CMsgBox::StandardButton button);

    // 设置窗体图标
    void setIcon(const QString &icon);

    static StandardButton showInfo(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons);
    static StandardButton showFatal(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons);
    static StandardButton showSuccess(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons);
    static StandardButton showQuestion(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons);
    static StandardButton showWarning(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons);
    static StandardButton showCritical(QWidget *parent, const QString &title,const QString &text, CMsgBox::StandardButtons buttons);

protected:
    // 多语言翻译
    void changeEvent(QEvent *event);

private slots:
    void onButtonClicked(QAbstractButton *button);

private:
    void translateUI();
    int execReturnCode(QAbstractButton *button);
private:
    QLabel *m_pIconLabel;
    QLabel *m_pLabel;
    QFrame *m_pLine;
    QDialogButtonBox *m_pButtonBox;
    QAbstractButton *m_pClickedButton;
    QAbstractButton *m_pDefaultButton;

};

//在全局任意地方使用"|"操作符计算自定义的枚举量，需要使用Q_DECLARE_OPERATORS_FOR_FLAGS宏
Q_DECLARE_OPERATORS_FOR_FLAGS(CMsgBox::StandardButtons)

#endif // CMSGBOX_H
