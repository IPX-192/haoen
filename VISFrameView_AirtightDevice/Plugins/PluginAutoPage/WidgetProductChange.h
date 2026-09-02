#ifndef WidgetProductChange_H
#define WidgetProductChange_H

#include <QWidget>
#include "ParamDef.h"
#include "../../interface/coreinterface.h"

namespace Ui {
class WidgetProductChange;
}

class WidgetProductChange : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetProductChange(QWidget *parent = nullptr);
    ~WidgetProductChange();

public slots:
    void  slot_InputInfo(tagOutputInfo& inputInfo);

private slots:
    void on_pushButton_clicked();

private:
    void SaveChangeInfo();

private:
    Ui::WidgetProductChange *ui;
};

#endif // WidgetProductChange_H
