#include "TrayCalculate.h"
#pragma execution_character_set("utf-8")

FactorySpace::TrayCalculate::TrayCalculate()
{
    return;
}

bool FactorySpace::TrayCalculate::JudgedPointIsInABCPoint(int nTrayW, int nTrayH, QPointF point, int &nSearchIndex)
{
    nSearchIndex = -1;
    if (JudgeClickPointIsA(point.y(), point.x()))
    {
        nSearchIndex = 0;
        return true;
    }

    if (JudgeClickPointIsB(nTrayW, point.y(), point.x()))
    {
        nSearchIndex = 1;
        return true;
    }

    if (JudgeClickPointIsC(nTrayH, point.y(), point.x()))
    {
        nSearchIndex = 2;
        return true;
    }

    return false;
}

bool FactorySpace::TrayCalculate::JudgeClickPointIsA(int row, int col)
{
    if (0 == row && 0 == col)
    {
        return true;
    }
    return false;
}

bool FactorySpace::TrayCalculate::JudgeClickPointIsB(int nTrayW, int row, int col)
{
    if (1 == nTrayW)
    {
        return false;
    }

    if (0 == row && (col == (nTrayW - 1)))
    {
        return true;
    }
    return false;
}

bool FactorySpace::TrayCalculate::JudgeClickPointIsC(int nTrayH, int row, int col)
{
    if (1 == nTrayH)
    {
        return false;
    }

    if (0 == col && (row == (nTrayH - 1)))
    {
        return true;
    }
    return false;
}

QPointF FactorySpace::TrayCalculate::CalculateTrayPoint(int nTrayW, int nTrayH, const QList<QPointF>& ThreePoints, QPoint point)
{
    QList<QPointF> dAllTrayPoints;
    int nPointSearchIndex = point.x() + point.y() * nTrayW;
    if (CalculateTrayAllPoint(nTrayW, nTrayH, ThreePoints, dAllTrayPoints))
    {
        if (nPointSearchIndex < dAllTrayPoints.size())
        {
            return dAllTrayPoints[nPointSearchIndex];
        }
    }
    return QPointF(0,0);
}

bool FactorySpace::TrayCalculate::CalculateTrayAllPoint(int nTrayW, int nTrayH,
    const QList<QPointF>& ThreePoints, QList<QPointF>& AllTrayPoints)
{
    AllTrayPoints.clear();

    if (3!= ThreePoints.size())
    {
        return false;
    }

    if (2 > nTrayW || 2> nTrayH)
    {
        return false;
    }

    QPointF A = ThreePoints[0];
    QPointF B = ThreePoints[1];
    QPointF C = ThreePoints[2];

    //强制要求三个点是不同的三个点
    if (A == B || A == C || B == C)
    {
        return false;
    }

    //calculate the span of each column and row
    double colSpanX = (B.x() - A.x()) / (nTrayW - 1);
    double colSpanY = (B.y() - A.y()) / (nTrayW - 1);
    double rowSpanX = (C.x() - A.x()) / (nTrayH - 1);
    double rowSpanY = (C.y() - A.y()) / (nTrayH - 1);
    for (int j = 0; j < nTrayH; j++) {
        double startX = A.x() + j * rowSpanX;
        double startY = A.y() + j * rowSpanY;
        for (int i = 0; i < nTrayW; i++) {
            QPointF point;
            point.setX(startX + i * colSpanX);
            point.setY(startY + i * colSpanY);
            AllTrayPoints.append(point);
        }
    }
    return true;
}

bool FactorySpace::TrayCalculate::ReCalculateTrayAllPointByThreePoint(int nTrayW, int nTrayH, QList<QPointF>& AllTrayPoints,QList<QPointF> &ResPointList)
{
    if (nTrayW * nTrayH !=
        AllTrayPoints.size())
    {
        return false;
    }
    QList<QPointF> threePointList = FactorySpace::TrayCalculate::GetThreePointList(nTrayW,
        nTrayW,AllTrayPoints);

    QList<QPointF> allPointList;
    if (false == FactorySpace::TrayCalculate::CalculateTrayAllPoint(nTrayW,
        nTrayH, threePointList, allPointList))
    {
        return false;
    }

    ResPointList = allPointList;
    return true;
}

double FactorySpace::TrayCalculate::CalculateOneTrayPoint(int nCount, const QList<double>& TwoPoints, QPoint point)
{
    QList<double> dAllTrayPoints;
    int nPointSearchIndex = point.x() + point.y();
    if (CalculateOneTrayAllPoint(nCount, TwoPoints, dAllTrayPoints))
    {
        if (nPointSearchIndex < dAllTrayPoints.size())
        {
            return dAllTrayPoints[nPointSearchIndex];
        }
    }
    return 0.0;
}

bool FactorySpace::TrayCalculate::CalculateOneTrayAllPoint(int nCount, const QList<double>& TwoPoints, QList<double>& AllTrayPoints)
{
    AllTrayPoints.clear();
    if (2 != TwoPoints.size())
    {
        return false;
    }

    if (0 >= nCount)
    {
        return false;
    }

    double dSpan = (TwoPoints[1] - TwoPoints[0]) / (nCount - 1);
    for (int i = 0; i < nCount; i++) {
        AllTrayPoints.append(TwoPoints[0] + i * dSpan);
    }
    return true;
}

void FactorySpace::TrayCalculate::ResizeOneTrayList(int nCount, QList<double>& TrayPoints)
{
    if (TrayPoints.size() != nCount)
    {
        //重新设置大小
        int nSize = TrayPoints.size();
        if (nSize > nCount)
        {
            QList<double> TempList;
            for (int i = 0; i != nCount; i++)
            {
                TempList.append(TrayPoints[i]);
            }
            TrayPoints = TempList;
        }
        else
        {
            for (int i = nSize; i < nCount; i++)
            {
                TrayPoints.append(0.0);
            }
        }
    }
}

QList<double> FactorySpace::TrayCalculate::GetTwoPointList(int nCount, const QList<double>& TrayPoints)
{
    if (TrayPoints.size() != nCount)
    {
        return QList<double>();
    }

    QList<double> TwoPoints;
    TwoPoints.clear();
    TwoPoints.append(TrayPoints[0]);
    TwoPoints.append(TrayPoints[nCount - 1]);
    return TwoPoints;
}

double FactorySpace::TrayCalculate::GetSiloClickPos(QList<double>& dPosList, int nTrayCount, QPoint point)
{
    //根据当前点击的格子，自动获取坐标格子的坐标
    if (0 == nTrayCount)
    {
        return 0.0;
    }
    if (dPosList.size() != nTrayCount)
    {
        FactorySpace::TrayCalculate::ResizeOneTrayList(nTrayCount, dPosList);
    }

    int nPointSearchIndex = point.x() + point.y();
    if (nPointSearchIndex >= nTrayCount || 
        nPointSearchIndex < 0)
    {
        return 0.0;
    }

    return dPosList[nPointSearchIndex];
}

bool FactorySpace::TrayCalculate::SavePosToPosList(QList<double>& dPosList, QPoint point, double dPos)
{
    //根据当前点击的格子，自动保存坐标格子的坐标
    int nPointSearchIndex = point.x() + point.y();
    if (nPointSearchIndex >= dPosList.size() || 
        nPointSearchIndex < 0)
    {
        return false;
    }

    dPosList[nPointSearchIndex] = dPos;
    return true;
}

void FactorySpace::TrayCalculate::ResizeTrayList(int nTrayW, int nTrayH, QList<QPointF>& TrayPoints)
{
    if (TrayPoints.size() != nTrayW * nTrayH)
    {
        //重新设置大小
        int nSize = TrayPoints.size();
        if (nSize > nTrayW * nTrayH)
        {
            QList<QPointF> TempList;
            for (int i = 0; i != nTrayW * nTrayH; i++)
            {
                TempList.append(TrayPoints[i]);
            }
            TrayPoints = TempList;
        }
        else
        {
            for (int i = nSize; i < nTrayW * nTrayH; i++)
            {
                QPointF point;
                point.setX(0);
                point.setY(0);
                TrayPoints.append(point);
            }
        }
    }
}

QList<QPointF> FactorySpace::TrayCalculate::GetThreePointList(int nTrayW, int nTrayH, const QList<QPointF>& TrayPoints)
{
    if (TrayPoints.size() != nTrayW * nTrayH)
    {
        return QList<QPointF>();
    }

    QList<QPointF> ThreePoints;
    ThreePoints.clear();
    ThreePoints.append(TrayPoints[0]);
    ThreePoints.append(TrayPoints[nTrayW - 1]);
    ThreePoints.append(TrayPoints[nTrayW * (nTrayH - 1)]);
    return ThreePoints;
}

QPointF FactorySpace::TrayCalculate::GetTrayClickPos(QList<QPointF>& trayPosList, 
    int nW, int nH, QPoint point)
{
    if (nW * nH != trayPosList.size())
    {
        FactorySpace::TrayCalculate::ResizeTrayList(nW,
            nH, trayPosList);
    }

    if (0 == trayPosList.size())
    {
        return QPointF();
    }

    int nPointSearchIndex = point.x() +
        point.y() * nW;
    if (nPointSearchIndex >= trayPosList.size() ||
        nPointSearchIndex < 0)
    {
        return QPointF();
    }
    return trayPosList[nPointSearchIndex];
}

bool FactorySpace::TrayCalculate::SaveTrayPos(QList<QPointF>& trayPosList, 
    int nW, int nH, QPoint point, QPointF pos)
{
    if (nW * nH != trayPosList.size())
    {
        FactorySpace::TrayCalculate::ResizeTrayList(nW,
            nH, trayPosList);
    }

    if (0 == trayPosList.size())
    {
        return false;
    }

    int nPointSearchIndex = point.x() +
        point.y() * nW;
    if (nPointSearchIndex >= trayPosList.size() ||
        nPointSearchIndex < 0)
    {
        return false;
    }
    trayPosList[nPointSearchIndex] = pos;
    return true;
}
