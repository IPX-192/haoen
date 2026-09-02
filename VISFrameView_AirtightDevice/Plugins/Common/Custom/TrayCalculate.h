#ifndef TRAYCALCULATE_H
#define TRAYCALCULATE_H
#include <QPointF>
#include <QList>
namespace FactorySpace {
    class  TrayCalculate
    {
    public:
        TrayCalculate();

        //判断点是否在ABC点内 如果是则返回true，否则返回false  nSearchIndex返回的三点的位置索引
        static bool JudgedPointIsInABCPoint(int nTrayW, int nTrayH,QPointF point,int& nSearchIndex);

        static bool JudgeClickPointIsA(int row, int col);
        static bool JudgeClickPointIsB(int nTrayW, int row, int col);
        static bool JudgeClickPointIsC(int nTrayH, int row, int col);

        //通过已知的三个点的位置计算出所有点的位置
        static QPointF CalculateTrayPoint(int nTrayW, int nTrayH, const QList<QPointF>& ThreePoints,QPoint point);
        static bool CalculateTrayAllPoint(int nTrayW, int nTrayH,const QList<QPointF> &ThreePoints, QList<QPointF>& AllTrayPoints);
        static bool ReCalculateTrayAllPointByThreePoint(int nTrayW, int nTrayH, QList<QPointF>& AllTrayPoints, QList<QPointF>& ResPointList);
        static void ResizeTrayList(int nTrayW, int nTrayH, QList<QPointF>& TrayPoints);
        static QList<QPointF> GetThreePointList(int nTrayW, int nTrayH, const QList<QPointF>& TrayPoints);
        static QPointF GetTrayClickPos(QList<QPointF>& trayPosList, int nW, int nH, QPoint point);
        static bool SaveTrayPos(QList<QPointF>& trayPosList, int nW, int nH, QPoint point, QPointF pos);

        //通过已知两个点计算出只有宽度或者高度为一的格子坐标
        static double CalculateOneTrayPoint(int nCount, const QList<double>& TwoPoints,QPoint point);
        static bool CalculateOneTrayAllPoint(int nCount,const QList<double> &TwoPoints, QList<double>& AllTrayPoints);
        static void ResizeOneTrayList(int nCount, QList<double>& TrayPoints);
        static QList<double> GetTwoPointList(int nCount, const QList<double>& TrayPoints);
        static double GetSiloClickPos(QList<double>& dPosList, int nTrayCount, QPoint point);
        static bool SavePosToPosList(QList<double>& dPosList, QPoint point, double dPos);
    };
}
#endif // TRAYCALCULATE_H
