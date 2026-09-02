#include "MtfParamForm.h"
#include "ui_MtfParamForm.h"
#include "OpenProtocolMtf6000.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QStyledItemDelegate>
#include <QPainter>
#pragma execution_character_set("utf-8")
class CustomDelegate : public QStyledItemDelegate
{
public:
    CustomDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        // 1. 可以根据index从模型获取数据
        QVariant value = index.data(Qt::DisplayRole);
        if (mPaintColor != Qt::white) {
            // 如果值大于100，用红色填充背景
            painter->fillRect(option.rect, mPaintColor);
            // 然后设置文字颜色为白色
            painter->setPen(Qt::white);
        }
        else {
            // 否则，使用默认的背景和文字颜色（通常来自样式表）
            // 调用基类的paint以确保其他元素（如焦点框）被正确绘制
            QStyledItemDelegate::paint(painter, option, index);
            return; // 让基类处理完就直接返回
        }
        QString text = value.toString();
        painter->drawText(option.rect, Qt::AlignCenter, text);

    }

    void SetPaintColor(Qt::GlobalColor color)
    {
        mPaintColor = color;
    }

private:
    Qt::GlobalColor mPaintColor = Qt::white;
};

MtfParamForm::MtfParamForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MtfParamForm)
{
    ui->setupUi(this);
    InitWidget();
}

MtfParamForm::~MtfParamForm()
{
    delete ui;
}

void MtfParamForm::InitWidget()
{
    ui->ShowCurveWidget->setAxisLabels(QString::fromUtf8("角度(°)"), QString::fromUtf8("扭矩(Nm)"));
    InitTableView();
    connect(ui->TableView_TighteningResultInfo, &QTableView::clicked, this, &MtfParamForm::onTighteningResultInfoClicked);
}

void MtfParamForm::SetOpenProtocolMtf(QSharedPointer<OpenProtocolMtf6000> pOpenProtocolMtf)
{
    mpOpenProtocolMtf = pOpenProtocolMtf;
}

void MtfParamForm::AddTigheningInfo(int nIndex, int nTightrningID, QString sInfo)
{
    //检查大小，超过200条清除所有数据
    int nCount = m_pItemModelMtfTightening->rowCount();
    if(nCount > 200)
    {
        m_pItemModelMtfTightening->removeRows(0, nCount - 100);
    }

    //查找有没有相同的数据
    for (int i = 0; i != m_pItemModelMtfTightening->rowCount(); i++)
    {
        int nIndexSearch = m_pItemModelMtfTightening->item(i, 0)->text().toInt();
        int nTighteningId = m_pItemModelMtfTightening->item(i, 1)->text().toInt();
        if (nIndexSearch == nIndex && nTighteningId == nTightrningID)
        {
            return;
        }
    }

    QStringList sInfoList = sInfo.split(" ");

    //在第一行插入新的数据
    m_pItemModelMtfTightening->insertRow(0);
    m_pItemModelMtfTightening->setItem(0, 0,
        new QStandardItem(QString::number(nIndex)));
    m_pItemModelMtfTightening->setItem(0, 1,
        new QStandardItem(QString::number(nTightrningID)));

    for (int i = 0; i != sInfoList.size(); i++)
    {
        if (sInfoList.at(i).contains("开始时间"))
        {
            m_pItemModelMtfTightening->setItem(0, 2,
                new QStandardItem(sInfoList.at(i)));
            break;
        }
    }
    return;
}

void MtfParamForm::on_PushButtonTightening_clicked()
{
    if(nullptr == mpOpenProtocolMtf)
    {
        QMessageBox::information(this,QString::fromUtf8("提示"),
                                 QString::fromUtf8("指针为空"));
        return;
    }
    bool bRet = mpOpenProtocolMtf->startTightening();
    if (false == bRet)
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("开始拧紧失败"));
    }
    return;
}

void MtfParamForm::on_PushButtonRelease_clicked()
{
    if(nullptr == mpOpenProtocolMtf)
    {
        QMessageBox::information(this,QString::fromUtf8("提示"),
                                 QString::fromUtf8("指针为空"));
        return;
    }
    bool bRet = mpOpenProtocolMtf->startRelease();
    if (false == bRet)
    {
        QMessageBox::information(this, QString::fromUtf8("提示"),QString::fromUtf8("开始拧紧失败"));
    }
    return;
}

void MtfParamForm::on_PushButtonRefresh_clicked()
{
    if(nullptr == mpOpenProtocolMtf)
    {
        QMessageBox::information(this,QString::fromUtf8("提示"),
                                 QString::fromUtf8("指针为空"));
        return;
    }
    bool bRet = mpOpenProtocolMtf->startRelease();
    if (false == bRet)
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("开始反松失败"));
    }
    return;
}

void MtfParamForm::on_PushButtonDriverBitReset_clicked()
{

}

void MtfParamForm::onTighteningResultInfoClicked(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }
    int row = index.row();
    int column = index.column();
    int nDraiverBitIndex = m_pItemModelMtfTightening->item(row, 0)->text().toInt();
    QByteArray nTighteningId = QByteArray::number(m_pItemModelMtfTightening->item(row, 1)->text().toInt());

    ui->ShowCurveWidget->clearAll();

    //重新获取拧紧曲线信息
    QVector<QPointF> curvePoints;
    QList<double> dTorqueList;
    QList<double> dAngleList;
    bool bRet = false;
    bRet = OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(nDraiverBitIndex)->getCurveTorqueData(nTighteningId, dTorqueList);
    if (false == bRet)
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("获取扭矩曲线数据失败"));
        return;
    }

    bRet = OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(nDraiverBitIndex)->getCurveAngleData(nTighteningId, dAngleList);
    if (false == bRet)
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("获取角度曲线数据失败"));
        return;
    }

    if (dAngleList.size() != dTorqueList.size())
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("角度曲线数据和扭矩曲线数据长度不一致"));
        return;
    }

    for (int i = 0; i < dTorqueList.size(); i++)
    {
        curvePoints.append(QPointF(dAngleList[i], dTorqueList[i]));
    }

    QString sTighteningResultInfo;
    Mtf6000InfoAll_T tTighteningInfo;
    bRet = OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(nDraiverBitIndex)->
        getCurrentTighteningResult(nTighteningId, sTighteningResultInfo, tTighteningInfo);
    if (false == bRet)
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("获取拧紧结果数据失败"));
        return;
    }

    if (sTighteningResultInfo.contains("Error"))
    {
        ui->ShowCurveWidget->setCurveData(0, curvePoints,
            QString::fromUtf8("电批：") + QString::number(nDraiverBitIndex) +
            QString::fromUtf8("  ID：") + QString(nTighteningId),Qt::red);
    }
    else
    {
        ui->ShowCurveWidget->setCurveData(0, curvePoints,
            QString::fromUtf8("电批：") + QString::number(nDraiverBitIndex) +
            QString::fromUtf8("  ID：") + QString(nTighteningId));
    }
    ShowInfoToTighteningTable(sTighteningResultInfo);
    return;
}

void MtfParamForm::on_PushButtonLoadTightenData_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        QString::fromUtf8("选择曲线文件"), qApp->applicationDirPath()
    );

    if (fileName.isEmpty()) {  // 返回空图像
        return;
    }
    ui->ShowCurveWidget->clearAll();
    ShowInfoToTighteningTable("",false);
    QList<CurveData_T> tCurveDataList;
    if (false == OpenProtocolMtf6000ManagerIns.GetOpenProtocolMtf6000(1)->ReadTighteningData(fileName, tCurveDataList))
    {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("加载曲线数据失败"));
        return;
    }

    for (int i = 0; i < tCurveDataList.size(); i++)
    {
        QVector<QPointF> curvePoints = tCurveDataList[i].lstCurve;
        ui->ShowCurveWidget->setCurveData(i, curvePoints,
            QString::fromUtf8("电批：") + QString::number(i + 1));
        ShowInfoToTighteningTable("CurveIndex:" + QString::number(i), false);
        ShowInfoToTighteningTable(tCurveDataList[i].sInfo,false);
    }
    return;
}

void MtfParamForm::InitTableView()
{
    m_pItemModelMtfTightening = new QStandardItemModel();
    m_pItemModelMtfTightening->setColumnCount(3);
    m_pItemModelMtfTightening->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("电批序号"));
    m_pItemModelMtfTightening->setHeaderData(1, Qt::Horizontal, QString::fromUtf8("ID"));
    m_pItemModelMtfTightening->setHeaderData(2, Qt::Horizontal, QString::fromUtf8("信息"));

    ui->TableView_TighteningResultInfo->setModel(m_pItemModelMtfTightening);
    ui->TableView_TighteningResultInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->TableView_TighteningResultInfo->show();

    m_pItemModelMtfTighteningInfo = new QStandardItemModel();
    m_pItemModelMtfTighteningInfo->setColumnCount(1);
    m_pItemModelMtfTighteningInfo->setHeaderData(0, Qt::Horizontal, QString::fromUtf8("预览"));

    CustomDelegate* pCustomDelegate = new CustomDelegate(ui->TtableViewInfo);
    ui->TtableViewInfo->setItemDelegate(pCustomDelegate);
    ui->TtableViewInfo->setModel(m_pItemModelMtfTighteningInfo);
    ui->TtableViewInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->TtableViewInfo->show();
    return;
}

void MtfParamForm::ShowInfoToTighteningTable(QString sInfo,bool bClear)
{
    if (bClear)
    {
        m_pItemModelMtfTighteningInfo->clear();
    }

    if (sInfo.contains("Error"))
    {
        CustomDelegate* pCustomDelegate = (CustomDelegate*)(ui->TtableViewInfo->itemDelegate());
        pCustomDelegate->SetPaintColor(Qt::red);
    }
    else
    {
        CustomDelegate* pCustomDelegate = (CustomDelegate*)(ui->TtableViewInfo->itemDelegate());
        pCustomDelegate->SetPaintColor(Qt::white);
    }

    QStringList strList = sInfo.split(" ");
    int nCount = m_pItemModelMtfTighteningInfo->rowCount();
    for (int i = 0; i < strList.size(); i++)
    {
        m_pItemModelMtfTighteningInfo->setItem(i + nCount, 0, new QStandardItem(strList[i]));
    }
    return;
}
