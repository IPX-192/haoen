#ifndef EXCEL_H
#define EXCEL_H
#include <QString>
#include <QSharedPointer>
#include <QColor>
#include "Factory_global.h"
namespace QXlsx {
    class Document;
}
namespace FactorySpace {

    //每一个方格的数据和位置
    struct SquaredData_T
    {
        QColor mcColor = QColor(255, 255, 255);
        bool mbBold = false;
        int mnWidth = 20;
        int mnHeight = 15;
        int mnRow = 0;
        int mnCol = 0;
        QString msData = QString();
    };

    class FACTORY_EXPORT Excel
    {
    public:
        Excel();
        ~Excel();

    public:
        bool Open(const QString &sFilePath);
        bool Close();

        bool SetCurrentSheet(const QString &sSheetName);
        QString GetCurrentSheet() const;
        QStringList GetSheetNames() const;

        bool Read(QList<SquaredData_T> &t_SquaredDataList);
        bool Write(const QList<SquaredData_T> &t_SquaredDataList);

        bool Read(QList<QString>& sDataList);
        bool Write(const QList<QString>& sDataList);

        bool SaveAs(const QString &sFilePath);

        int GetRowCount() const;
        int GetColCount() const;

        QString GetCellValue(int nRow, int nCol);
    private:
        int ConvertDataToStrList(const QList<SquaredData_T>& t_SquaredDataList, QList<QString>& sDataList);
        int ConvertStrListToData(const QList<QString>& sDataList, QList<SquaredData_T>& t_SquaredDataList);
    private:
        QString msCurrentSheetName = "";
        QSharedPointer<QXlsx::Document> mpDocumentSharedPoint =
                QSharedPointer<QXlsx::Document>(nullptr);
    };
}
#endif // EXCEL_H
