#ifndef WIDGETRECIPETRAY_H
#define WIDGETRECIPETRAY_H

#include <QWidget>
#include <QPointF>
#include <QVector4D>
#include "ParamManager.h"

namespace Ui {
class WidgetRecipeTray;
}

class TrayCtrl;
class WidgetRecipeTray : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetRecipeTray(QWidget *parent = nullptr);
    ~WidgetRecipeTray();
    enum TrayType {
        TestTray1,
		TestTray2,
        NgTray
    };
	enum GripType {
		FeedGrip,
        BlankGrip
	};

public:
    void  InitWidget();
    void  InitTrayCell(TrayType type);

protected:
    void  ShowCurHoleInfo(GripType gripType, TrayType trayType,int index);

public slots:
	void  LoadUIParam();
	void  SaveUIParam();
	void  UpdateParamToUI();

protected:
    RecipeTray m_recipeTray;
    QMap<GripType, QMap<TrayType, TrayCtrl*>>m_mapTray;
    QMap<TrayCtrl*, QVector<QVector4D>*>m_trayToPos;
    TrayCtrl* m_selectTray = nullptr;     //当前鼠标点击的托盘
    int  m_curHole = 0;

private slots:
    void on_comboBox_Recipe_currentIndexChanged(const QString &arg1);

    void on_spinBox_feedTrayH_editingFinished();

    void on_spinBox_feedTrayW_editingFinished();

    void on_spinBox_ngTrayW_editingFinished();

    void on_spinBox_ngTrayH_editingFinished();

    void on_btnSave_clicked();

    void on_btnLoadPosToHole_clicked();

    void on_btnCalcuTrayPos_clicked();

    void slotTrayClick(int index, int silos, int row, int col);

    void on_doubleSpinBoxPosX_editingFinished();

    void on_doubleSpinBoxPosY_editingFinished();

private:
    Ui::WidgetRecipeTray *ui;
};

#endif // WIDGETRECIPETRAY_H
