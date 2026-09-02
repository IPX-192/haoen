#include "ParamDef.h"
#include <QMetaObject>
#include <QMetaProperty>
#include "VisUIParam.h"

//SystemParam& SystemParam::operator=(const SystemParam&other)
//{
//	QObject* src = (QObject*) & other;
//	VisUIParam::QObjectCopy(src, this);
//	return *this;
//}

RecipeTray &RecipeTray::operator=(const RecipeTray &other)
{
    QObject* src = (QObject*) & other;
    VisUIParam::QObjectCopy(src, this);

    for(int i=0;i<2;i++){
        this->feedTrayPos[i]=other.feedTrayPos[i];
    }
    this->ngTrayPos=other.ngTrayPos;
    return *this;
}

bool RecipeTray::LoadPosList(QString sFilePath, QObject* uiObj)
{
    QSettings settings(sFilePath, QSettings::IniFormat);
    settings.beginGroup("Param");
    VisUIParam::LoadIniToUI(&settings, uiObj, this);
    settings.endGroup();
    for (int i = 0; i < 2; i++) {
        feedTrayPos[i].resize(feedTrayH * feedTrayW);
        for (int j = 0; j < feedTrayPos[i].size(); j++) {
            QStringList listValue = settings.value(QString("FeedTray%1/Pos_%2").arg(i + 1).arg(j + 1)).toString().split(" ");
            if (listValue.size() != 4)return false;
            feedTrayPos[i][j] = QVector4D(listValue[0].toDouble(), listValue[1].toDouble(), listValue[2].toDouble() , listValue[3].toDouble());
        }
    }
    ngTrayPos.resize(ngTrayH * ngTrayW);
    for (int j = 0; j < ngTrayPos.size(); j++) {
        QStringList listValue = settings.value(QString("NGTray/Pos_%1").arg(j + 1)).toString().split(" ");
        if (listValue.size() != 4)return false;
        ngTrayPos[j] = QVector4D(listValue[0].toDouble(), listValue[1].toDouble(), listValue[2].toDouble(), listValue[3].toDouble());
    }
    return true;
}

bool RecipeTray::SavePosList(QString sFilePath, QObject* uiObj)
{
    QSettings settings(sFilePath, QSettings::IniFormat);
    settings.beginGroup("Param");
    VisUIParam::SaveUIToIni(&settings, uiObj, this);
    settings.endGroup();
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < feedTrayPos[i].size(); j++) {
            QString strValue = QString("%1 %2 %3 %4").arg(feedTrayPos[i][j].x()).arg(feedTrayPos[i][j].y()).arg(feedTrayPos[i][j].z()).arg(feedTrayPos[i][j].w());
            settings.setValue(QString("FeedTray%1/Pos_%2").arg(i + 1).arg(j + 1), strValue);
        }
    }
    for (int j = 0; j < ngTrayPos.size(); j++) {
        QString strValue = QString("%1 %2 %3 %4").arg(ngTrayPos[j].x()).arg(ngTrayPos[j].y()).arg(ngTrayPos[j].z()).arg(ngTrayPos[j].w());
        settings.setValue(QString("NGTray/Pos_%1").arg(j + 1), strValue);
    }
    return true;
}
