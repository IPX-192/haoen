#ifndef SCREWLOCKRECIPEFORM_H
#define SCREWLOCKRECIPEFORM_H
#include <QWidget>
#include <QStandardItemModel>
#include "ParamManager.h"
namespace Ui {
class ScrewLockRecipeForm;
}

class ScrewLockRecipeForm : public QWidget
{
    Q_OBJECT

public:
    explicit ScrewLockRecipeForm(QWidget *parent = nullptr);
    ~ScrewLockRecipeForm();

    void InitWidget();

public slots:
    void  LoadUIParam();
    void  SaveUIParam();
    void  UpdateParamToUI();

    void recvTighteningResultOneSlot(QByteArray sTighteningId, QString sResultInfo);
    void recvTighteningResultTwoSlot(QByteArray sTighteningId, QString sResultInfo);
    void recvTighteningResultThreeSlot(QByteArray sTighteningId, QString sResultInfo);
    void recvTighteningResultFourSlot(QByteArray sTighteningId, QString sResultInfo);

private slots:
    void on_btnSave_clicked();
    void on_comboBox_Recipe_currentIndexChanged(const QString &arg1);
    void on_pushButton_Add_clicked();
    void on_pushButton_Del_clicked();
    void on_pushButton_MoveTo_clicked();
    void on_pushButton_MoveToTighteningH_clicked();
    void on_pushButton_MoveToSafeH_clicked();
    void on_pushButton_GetCurrentHeight_clicked();
    void DriverBitChangeSlot();
private:
    void GetHeightData();
    void UpdateDataToTable(QList<ScrewLockHolePos_T> listPos);
    QList<ScrewLockHolePos_T> GetTablePos();

    void LoadRecipeParam(QString sPath, RecipeScrewLock *pRecipe);
    void SaveRecipeParam(QString sPath);
    void CopyParamToTemp(RecipeScrewLock *pSrc, RecipeScrewLock *pDst);
    int GetCurrentStation();
    void GetPos(int nStation, double& dPosX, double& dPosY, double& dPosZ);
    void GetPosByAxisName(QString sAxisName,double &dPos);
    void MoveToPos(int nStation, double dPosX, double dPosY, double dPosZ);
    void MoveToTighteningH(int nStation,double dPosZ);
    void MoveToSafeH(int nStation,double dPosZ);
    bool GetCurrentChosePos(ScrewLockHolePos_T & tScrewLockHolePos);
    void InitPestChose();
private:
    Ui::ScrewLockRecipeForm *ui;
    RecipeScrewLock m_RecipeScrewLockTemp;
    QStandardItemModel *m_pItemModelScrewLockPos = nullptr;
};

#endif // SCREWLOCKRECIPEFORM_H
