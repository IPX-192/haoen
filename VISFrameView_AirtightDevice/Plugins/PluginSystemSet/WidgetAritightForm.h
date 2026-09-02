#ifndef WIDGETARITIGHTFORM_H
#define WIDGETARITIGHTFORM_H

#include <QWidget>

namespace Ui {
class WidgetAritightForm;
}

class SerialComForm;
class WidgetAritightForm : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetAritightForm(QWidget *parent = nullptr);
    ~WidgetAritightForm();
private:
    void InitWidget();
public slots:
    void  LoadUIParam();
    void  UpdateParamToUI();
    void  SaveUIParam();
private:
    Ui::WidgetAritightForm *ui;
    QList<SerialComForm *> m_serialComFormList;
};

#endif // WIDGETARITIGHTFORM_H
