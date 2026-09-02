#ifndef MTFPARAMFORM_H
#define MTFPARAMFORM_H
#include <QWidget>
#include <QSharedPointer>
#include <QStandardItemModel>
namespace Ui {
class MtfParamForm;
}
class OpenProtocolMtf6000;
class MtfParamForm : public QWidget
{
    Q_OBJECT

public:
    explicit MtfParamForm(QWidget *parent = nullptr);
    ~MtfParamForm();

    void InitWidget();
    void SetOpenProtocolMtf(QSharedPointer<OpenProtocolMtf6000> pOpenProtocolMtf);
    void AddTigheningInfo(int nIndex, int nTightrningID, QString sInfo);
private slots:
    void on_PushButtonTightening_clicked();
    void on_PushButtonRelease_clicked();
    void on_PushButtonRefresh_clicked();
    void on_PushButtonDriverBitReset_clicked();
    void onTighteningResultInfoClicked(const QModelIndex& index);
    void on_PushButtonLoadTightenData_clicked();
private:
    void InitTableView();
    void ShowInfoToTighteningTable(QString sInfo,bool bClear = true);
private:
    Ui::MtfParamForm *ui;
    QSharedPointer<OpenProtocolMtf6000> mpOpenProtocolMtf = nullptr;
    QStandardItemModel* m_pItemModelMtfTightening = nullptr;
    QStandardItemModel* m_pItemModelMtfTighteningInfo = nullptr;
};

#endif // MTFPARAMFORM_H
