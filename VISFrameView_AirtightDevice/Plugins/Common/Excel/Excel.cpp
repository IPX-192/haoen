#include <QDir>
#include "Excel.h"
#include "xlsxdocument.h"
#pragma execution_character_set("utf-8")
using namespace FactorySpace;
Excel::Excel()
{
}

Excel::~Excel()
{
}

bool Excel::Open(const QString &sFilePath)
{
    if (QSharedPointer<QXlsx::Document>(nullptr) != mpDocumentSharedPoint) {
        mpDocumentSharedPoint->save();
    }
    /*QFile file(sFilePath);
    if (file.exists())
    {
        file.remove();
    }*/
    mpDocumentSharedPoint.reset(new QXlsx::Document(sFilePath));
    return true;
}

bool Excel::Close()
{
    if (QSharedPointer<QXlsx::Document>(nullptr) != mpDocumentSharedPoint) {
        mpDocumentSharedPoint->save();
        mpDocumentSharedPoint = QSharedPointer<QXlsx::Document>(nullptr);
    }
    return true;
}

bool Excel::SetCurrentSheet(const QString& sSheetName)
{
    if (QSharedPointer<QXlsx::Document>(nullptr) == mpDocumentSharedPoint) {
        return false;
    }

    QStringList sheetNames = mpDocumentSharedPoint->sheetNames();
    if (!sheetNames.contains(sSheetName)) {
        if (!mpDocumentSharedPoint->addSheet(sSheetName))
        {
            return false;
        }
    }

    if (false == mpDocumentSharedPoint->selectSheet(sSheetName))
    {
        msCurrentSheetName = "";
        return false;
    }
    msCurrentSheetName = sSheetName;
    return true;
}

QString Excel::GetCurrentSheet() const
{
     if (QSharedPointer<QXlsx::Document>(nullptr) == mpDocumentSharedPoint) {
        return QString();
    }
    return QString();
}

QStringList Excel::GetSheetNames() const
{
    if (QSharedPointer<QXlsx::Document>(nullptr) == mpDocumentSharedPoint) {
        return QStringList();
    }

    return mpDocumentSharedPoint->sheetNames();
}

bool Excel::Read(QList<SquaredData_T> &t_SquaredDataList)
{
    t_SquaredDataList.clear();
    if (QSharedPointer<QXlsx::Document>(nullptr) == mpDocumentSharedPoint) {
        return false;
    }
    int nRowCount = mpDocumentSharedPoint->dimension().rowCount();
    int nColCount = mpDocumentSharedPoint->dimension().columnCount();
    for (int i = 1; i <= nRowCount; ++i) {
        for (int j = 1; j <= nColCount; ++j) {
            SquaredData_T tSquaredData;
            tSquaredData.mnRow = i;
            tSquaredData.mnCol = j;
            tSquaredData.msData = mpDocumentSharedPoint->read(i, j).toString();
            t_SquaredDataList.append(tSquaredData);
        }
    }
    return true;
}

bool Excel::Read(QList<QString>& sDataList) {
    sDataList.clear();
    QList<SquaredData_T> t_SquaredDataList = {};
    if (!Read(t_SquaredDataList))
    {
        return false;
    }
    if (0 != ConvertDataToStrList(t_SquaredDataList, sDataList))
    {
        return false;
    }
    return true;
}

bool Excel::Write(const QList<SquaredData_T>& t_SquaredDataList)
{
    if (QSharedPointer<QXlsx::Document>(nullptr) == mpDocumentSharedPoint) {
        return false;
    }
    int nSize = t_SquaredDataList.size();
    
    for (int i = 0; i < nSize; ++i) {
        const SquaredData_T& tSquaredData = t_SquaredDataList[i];
        mpDocumentSharedPoint->setColumnWidth(tSquaredData.mnCol,tSquaredData.mnWidth);
        mpDocumentSharedPoint->setRowHeight(tSquaredData.mnRow,tSquaredData.mnHeight);
        QXlsx::Format format;
        // 设置水平和垂直居中
        format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        format.setFontSize(14);
        format.setFontBold(tSquaredData.mbBold);

        if (tSquaredData.mcColor != QColor(255, 255, 255))
        {
            format.setPatternBackgroundColor(tSquaredData.mcColor);
        }

        if (!mpDocumentSharedPoint->write(tSquaredData.mnRow, tSquaredData.mnCol, tSquaredData.msData, format)) {
            return false;
        }
    }
    return mpDocumentSharedPoint->save();
}

bool Excel::Write(const QList<QString>& sDataList)
{
    QList<SquaredData_T> t_SquaredDataList;
    if (0 != ConvertStrListToData(sDataList, t_SquaredDataList))
    {
        return false;
    }
    return Write(t_SquaredDataList);
}

bool Excel::SaveAs(const QString &sFilePath)
{
    if (QSharedPointer<QXlsx::Document>(nullptr) == mpDocumentSharedPoint) {
        return false;
    }
    return mpDocumentSharedPoint->saveAs(sFilePath);
}

int Excel::GetRowCount() const
{
    if (QSharedPointer<QXlsx::Document>(nullptr) == mpDocumentSharedPoint) {
        return -1;
    }
    int nRowCount = mpDocumentSharedPoint->dimension().rowCount();
    return nRowCount;
}

int Excel::GetColCount() const
{
    if (QSharedPointer<QXlsx::Document>(nullptr) == mpDocumentSharedPoint) {
        return -1;
    }
    int nColCount = mpDocumentSharedPoint->dimension().columnCount();
    return nColCount;
}

QString FactorySpace::Excel::GetCellValue(int nRow, int nCol)
{
    if (QSharedPointer<QXlsx::Document>(nullptr) == mpDocumentSharedPoint) {
        return "";
    }
    return mpDocumentSharedPoint->read(nRow, nCol).toString();
}

int Excel::ConvertDataToStrList(const QList<SquaredData_T>& t_SquaredDataList, QList<QString>& sDataList)
{
    sDataList.clear();
    int nSize = t_SquaredDataList.size();
    int nRowMax = 0;
    int nColMax = 0;
    for (int i = 0; i < nSize; ++i) {
        const SquaredData_T& tSquaredData = t_SquaredDataList[i];
        if (tSquaredData.mnRow > nRowMax) {
            nRowMax = tSquaredData.mnRow;
        }
        if (tSquaredData.mnCol > nColMax) {
            nColMax = tSquaredData.mnCol;
        }
    }

    for (int i = 1; i <= nRowMax; ++i) {
        QString strDataLine;
        for (int j = 1; j <= nColMax; ++j) {
            for (int k = 0; k < nSize; ++k) {
                const SquaredData_T& tSquaredData = t_SquaredDataList[k];
                if (tSquaredData.mnRow == i && tSquaredData.mnCol == j) {
                    strDataLine.append(tSquaredData.msData);
                    break;
                }
            }
            if (j != nColMax) {
                strDataLine.append(",");
            }
        }
        sDataList.append(strDataLine);
    }
    return 0;
}

int Excel::ConvertStrListToData(const QList<QString>& sDataList, QList<SquaredData_T>& t_SquaredDataList)
{
    t_SquaredDataList.clear();
    int nSize = sDataList.size();
    for (int i = 0; i < nSize; ++i) {
        QString strDataLine = sDataList[i];
        QStringList strList = strDataLine.split(",");
        for (int j = 0;j != strList.size();j++)
        {
            SquaredData_T tSquaredData;
            tSquaredData.mnRow = i+1;
            tSquaredData.mnCol = j+1;
            tSquaredData.msData = strList[j];
            t_SquaredDataList.append(tSquaredData);
        }
    }
    return 0;
}
