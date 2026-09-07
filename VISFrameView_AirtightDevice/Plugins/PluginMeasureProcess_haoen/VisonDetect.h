#ifndef VISONDETECT_H
#define VISONDETECT_H

#include <QObject>
#include "ParamManager.h"

class VisonDetect : public QObject
{
    Q_OBJECT
    enum MatchType{
        MatchMaterial,   //检测有无
        MatchAngle,      //检测角度
    };
public:
    explicit VisonDetect(QObject *parent = nullptr);

protected:

signals:

public slots:

};

#endif // VISONDETECT_H
