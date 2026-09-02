#ifndef WidgetRecipeGrip_H
#define WidgetRecipeGrip_H

#include <QWidget>
#include "ParamManager.h"
namespace Ui {
class WidgetRecipeGrip;
}

class WidgetRecipeGrip : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetRecipeGrip(QWidget *parent = nullptr);
    ~WidgetRecipeGrip();
public slots:
    void LoadUIParam();
    void UpdateParamToUI();
    void SaveUIParam();
private slots:
    void on_btnSave_clicked();
    void on_comboBox_sRecipeName_currentIndexChanged(const QString &arg1);
private:
    bool CopyFile(const QString &src, const QString &dst);
private:
    Ui::WidgetRecipeGrip *ui;
};

#endif // WidgetRecipeGrip_H
