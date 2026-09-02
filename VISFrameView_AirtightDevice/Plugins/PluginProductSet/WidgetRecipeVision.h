#ifndef WIDGETRECIPEVISION_H
#define WIDGETRECIPEVISION_H

#include <QWidget>

namespace Ui {
class WidgetRecipeVision;
}
class CameraDebugForm;
class WidgetRecipeVision : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetRecipeVision(QWidget *parent = nullptr);
    ~WidgetRecipeVision();

    void InitWidget();
public slots:
    void LoadUIParam();
    void UpdateParamToUI();
    void SaveUIParam();
    void LoadRecipeFile();
private slots:
    void on_btnSaveAs_clicked();


private:
    Ui::WidgetRecipeVision *ui;

};

#endif // WIDGETRECIPEVISION_H
