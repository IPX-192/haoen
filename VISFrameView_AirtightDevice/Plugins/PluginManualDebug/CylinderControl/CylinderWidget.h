#ifndef CYLINDERWIDGET_H
#define CYLINDERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMouseEvent>


struct CylinderData {
    QString name;
    QString addr;   //地址(IO序号/寄存器)
    QString s0_text; QString s0_out; QString s0_in;
    QString s1_text; QString s1_out; QString s1_in;
	QMap<QString, QString> io_name; // 控制卡专用，IO序号实际对应的IO英文名
};

class CylinderWidget : public QFrame
{
    Q_OBJECT

public:
    explicit CylinderWidget(const CylinderData &data, QWidget *parent = nullptr);

    // 暴露获取 IO 名称的接口，供主程序查询状态
    QString GetOut0Name() const { return m_data.s0_out; }
    QString GetIn0Name()  const { return m_data.s0_in; }
    QString GetOut1Name() const { return m_data.s1_out; }
    QString GetIn1Name()  const { return m_data.s1_in; }
    CylinderData GetData() const { return m_data; }

    // 【新增】实时更新状态接口
    // 参数分别代表：左侧输出(Y), 左侧到位(X), 右侧输出(Y), 右侧到位(X) 是否有电
    void UpdateRealTimeStatus(bool out0, bool in0, bool out1, bool in1);

signals:
    // 【新增】用户点击信号，cmdIndex: 0=左侧动作, 1=右侧动作
    void commandTriggered(int cmdIndex);

protected:
    // 【新增】鼠标点击事件处理
    void mousePressEvent(QMouseEvent *event) override;

private:
    void SetupUi();
    QLabel* CreateIndicatorLabel(QString str,const QString &text);
    void SetLabelActive(QLabel* lbl, bool active);

    CylinderData m_data;

    // UI 指示器指针，用于后续改变颜色
    QLabel *m_lblState0Text;
    QLabel *m_lblState0Out;
    QLabel *m_lblState0In;

    QLabel *m_lblState1Text;
    QLabel *m_lblState1Out;
    QLabel *m_lblState1In;

    // 样式常量
    const QString STYLE_ACTIVE = "background-color: #00AA00; color: white; border: 1px solid #444;";
    const QString STYLE_INACTIVE = "background-color: #D3D3D3; color: black; border: 1px solid gray;";
    const QString STYLE_TEXT_ACTIVE = "font-weight: bold; background-color: #E0E0E0; border: 1px solid gray;";
};

#endif // CYLINDERWIDGET_H
