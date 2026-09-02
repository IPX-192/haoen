#include "WidgetRecipeTray.h"
#include "ui_WidgetRecipeTray.h"
#include <QSettings>
#include <QMessageBox>
#include "VisUIParam.h"
#include "TrayCtrl.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"

WidgetRecipeTray::WidgetRecipeTray(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetRecipeTray)
{
    ui->setupUi(this);
    InitWidget();
}

WidgetRecipeTray::~WidgetRecipeTray()
{
    delete ui;
}

void WidgetRecipeTray::InitWidget()
{
    QStringList listGripName;
    listGripName << u8"上料夹爪";
    QMap<int,QVector<TrayType>>mapTrayType;
    mapTrayType[0] << TestTray1 << TestTray2 << NgTray;
    QVector<QStringList>vecGripTrayName;
    vecGripTrayName.resize(1);   //0:上料夹爪  1:下料夹爪
    vecGripTrayName[0] << u8"工位1上料盘" << u8"工位2上料盘" << u8"NG料盘";
    QVector<QVector<QVector4D>*>vecTrayToPos;
    vecTrayToPos << &m_recipeTray.feedTrayPos[0] << &m_recipeTray.feedTrayPos[1] << &m_recipeTray.ngTrayPos;
    int indexTray = 0;
    for (int i = 0; i < listGripName.size(); i++) {
        QGridLayout* layout = new QGridLayout();
        layout->setMargin(2);
        layout->setSpacing(0);
        //QTabWidget* tabWidget = new QTabWidget(ui->tabWidget);
        for (int j = 0; j < vecGripTrayName[i].size(); j++) {
            //TrayCtrl* trayWidget = new TrayCtrl(tabWidget);
            TrayCtrl* trayWidget = new TrayCtrl(ui->tabWidget);
            trayWidget->setCtrlIndex(indexTray);
            trayWidget->setTrayName(QString::fromLocal8Bit(""));
            trayWidget->setSilosName(QString::fromLocal8Bit(""));
            trayWidget->initTrayModel(4, 4);
            trayWidget->initSilosModel(4);
            trayWidget->SetSilosVisible(false);
            trayWidget->RenameCellName();
            trayWidget->setProperty("GripType", i);
            trayWidget->setProperty("TrayType", (int)mapTrayType[i][j]);
            connect(trayWidget, &TrayCtrl::sigTraySelected, this, &WidgetRecipeTray::slotTrayClick);
            //tabWidget->addTab(trayWidget, vecGripTrayName[i][j]);
            ui->tabWidget->addTab(trayWidget, vecGripTrayName[i][j]);
            TrayType trayType = mapTrayType[i][j];
            m_mapTray[(GripType)i][trayType] = trayWidget;
            m_trayToPos[trayWidget] = vecTrayToPos[indexTray++];
        }
        //ui->tabWidget->addTab(tabWidget, listGripName[i]);
    }
}

void WidgetRecipeTray::LoadUIParam()
{   
    QString filename = GlobalParam->recipeTray.filepath +"/Tray.xml";
    GlobalParam->LoadRecipeList(filename, GlobalParam->recipeTray.listRecipe, ui->comboBox_Recipe);

    m_recipeTray = GlobalParam->recipeTray;
    ui->comboBox_Recipe->blockSignals(true);
    int index = GlobalParam->recipeTray.listRecipe.indexOf(GlobalParam->recipeTray.curRecipe);
    ui->comboBox_Recipe->setCurrentIndex(index);
    ui->comboBox_Recipe->blockSignals(false);

    UpdateParamToUI();

    VisAppBus::sendEvent("NotifyTrayReset",-1);

    ShowSystemLog(Log_Info, QString(u8"料盘配方文件加载成功！"));
}

void WidgetRecipeTray::SaveUIParam()
{
    QString filename = m_recipeTray.filepath + m_recipeTray.curRecipe + ".ini";
    m_recipeTray.SavePosList(filename, this);
    if (GlobalParam->recipeTray.curRecipe == m_recipeTray.curRecipe){
        //点位特殊数据赋值
        GlobalParam->recipeTray =m_recipeTray;
    }
    VisAppBus::sendEventDirect("NotifyTrayReset",-1);
}

void WidgetRecipeTray::UpdateParamToUI()
{
    QString filename = m_recipeTray.filepath + m_recipeTray.curRecipe + ".ini";
    m_recipeTray.LoadPosList(filename,this);
    for (int i = 0; i <= NgTray; i++)
        InitTrayCell((TrayType)i);
}

void WidgetRecipeTray::on_btnSave_clicked()
{
    QString recipeName = ui->lineEdit->text();
    if (recipeName.isEmpty()) {
        QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能为空");
        return;
    }
    if (m_recipeTray.listRecipe.contains(recipeName)) {
        QMessageBox::information(this, u8"提示信息", u8"保存的配方名不能重复");
        return;
    }
    m_recipeTray.listRecipe.push_back(recipeName);
    GlobalParam->recipeTray.listRecipe = m_recipeTray.listRecipe;
    QString filename = m_recipeTray.filepath + "Tray.xml";
    GlobalParam->SaveRecipeList(filename, m_recipeTray.listRecipe);
    filename = m_recipeTray.filepath + recipeName + ".ini";
    m_recipeTray.SavePosList(filename,this);

    ui->lineEdit->setText("");
    ui->comboBox_Recipe->addItem(recipeName);
}

void WidgetRecipeTray::on_comboBox_Recipe_currentIndexChanged(const QString &arg1)
{
    m_recipeTray.curRecipe = arg1;
    QString filename = m_recipeTray.filepath + arg1 + ".ini";
    m_recipeTray.LoadPosList(filename,this);
    for (int i = 0; i <= NgTray; i++)
        InitTrayCell((TrayType)i);
}

void WidgetRecipeTray::InitTrayCell(TrayType type)
{
    if (m_selectTray != nullptr)
        m_selectTray->ClearSelect();
    m_selectTray = nullptr;
    int rows = 0, cols = 0;
    if (type == TestTray1|| type == TestTray2) {
        rows = m_recipeTray.feedTrayH;
        cols = m_recipeTray.feedTrayW;
        m_recipeTray.feedTrayPos[type- TestTray1].resize(rows* cols);
    }
    else if (type == NgTray) {
        rows = m_recipeTray.ngTrayH;
        cols = m_recipeTray.ngTrayW;
        m_recipeTray.ngTrayPos.resize(rows * cols);
    }

    for (int i = FeedGrip; i <= BlankGrip; i++) {
        if (!m_mapTray.contains((GripType)i))continue;
        if (!m_mapTray[(GripType)i].contains(type))continue;
        QList<int> unUsedList;
        m_mapTray[(GripType)i][type]->initTrayModel(rows, cols,unUsedList);
        m_mapTray[(GripType)i][type]->RenameCellName();
    }
}

void WidgetRecipeTray::on_spinBox_feedTrayH_editingFinished()
{
    double value = ui->spinBox_feedTrayH->value();
    if (value != m_recipeTray.feedTrayH) {
        m_recipeTray.feedTrayH = ui->spinBox_feedTrayH->value();
        InitTrayCell(TestTray1);
        InitTrayCell(TestTray2);
    }
}

void WidgetRecipeTray::on_spinBox_feedTrayW_editingFinished()
{
    double value = ui->spinBox_feedTrayW->value();
    if (value != m_recipeTray.feedTrayW) {
        m_recipeTray.feedTrayW = ui->spinBox_feedTrayW->value();
        InitTrayCell(TestTray1);
        InitTrayCell(TestTray2);
    }
}

void WidgetRecipeTray::on_spinBox_ngTrayW_editingFinished()
{
    double value = ui->spinBox_ngTrayW->value();
    if (value != m_recipeTray.ngTrayW) {
        m_recipeTray.ngTrayW = ui->spinBox_ngTrayW->value();
        InitTrayCell(NgTray);
    }
}

void WidgetRecipeTray::on_spinBox_ngTrayH_editingFinished()
{
    double value = ui->spinBox_ngTrayH->value();
    if (value != m_recipeTray.ngTrayH) {
        m_recipeTray.ngTrayH = ui->spinBox_ngTrayH->value();
        InitTrayCell(NgTray);
    }
}


void WidgetRecipeTray::ShowCurHoleInfo(GripType gripType, TrayType trayType, int index)
{
    m_curHole = index;
    QVector4D movePos = m_trayToPos[m_selectTray]->at(m_curHole);
    ui->doubleSpinBoxPosX->setValue(movePos.x());
    ui->doubleSpinBoxPosY->setValue(movePos.y());
    if (ui->checkBoxMove->isChecked()) {
        QMap<QString, double>moveGroup;
        moveGroup.clear();
        moveGroup[Axis1_MotorGantryX] = movePos.x();
        moveGroup[Axis2_MotorGantryY] = movePos.y();
        int nRes =VisMotorToolSpace::VisMotorInstance->MoveAbsGroup(moveGroup);
        if (nRes != 0)return;
        if (!ui->checkBoxMoveZ->isChecked())return;
        moveGroup.clear();
        moveGroup[Axis3_MotorGantryZ] = movePos.z();
        VisMotorToolSpace::VisMotorInstance->MoveAbsGroup(moveGroup);
    }
}

void WidgetRecipeTray::on_btnLoadPosToHole_clicked()
{
    if (m_selectTray == nullptr)return;
    int gripType = m_selectTray->property("GripType").toInt();
    int trayType = m_selectTray->property("TrayType").toInt();
    double posX, posY,posZ, posR;
    VisMotorToolSpace::VisMotorInstance->GetCurPos(Axis1_MotorGantryX, posX);
    VisMotorToolSpace::VisMotorInstance->GetCurPos(Axis2_MotorGantryY, posY);
    VisMotorToolSpace::VisMotorInstance->GetCurPos(Axis3_MotorGantryZ, posZ);


    (*m_trayToPos[m_selectTray])[m_curHole]= QVector4D(posX, posY, posZ, posR);
    ui->doubleSpinBoxPosX->setValue(posX);
    ui->doubleSpinBoxPosY->setValue(posY);
    //ui->doubleSpinBoxPosZ->setValue(posZ);
    //ui->doubleSpinBoxPosR->setValue(posR);
}

void WidgetRecipeTray::on_btnCalcuTrayPos_clicked()
{
    TrayCtrl* curTray = nullptr;
    for (auto it = m_mapTray.begin(); it != m_mapTray.end(); ++it) {
        for (auto iter = it.value().begin(); iter != it.value().end(); ++iter) {
            if (iter.value()->isVisible()) {
                curTray = iter.value();
                break;
            }
        }
    }
    if (curTray == nullptr)return;
    int rows, cols;
    curTray->GetTrayRowCol(rows, cols);
    QVector<QVector4D>* curVecPos = m_trayToPos[curTray];
    QVector4D posA = curVecPos->at(0);
    QVector4D posB = curVecPos->at(cols - 1);
    QVector4D posC = curVecPos->at((rows - 1) * cols);

    double colSpanX = (posB.x() - posA.x()) / (cols - 1);
    double colSpanY = (posB.y() - posA.y()) / (cols - 1);
    double colSpanZ = (posB.z() - posA.z()) / (cols - 1);
    double rowSpanX = (posC.x() - posA.x()) / (rows - 1);
    double rowSpanY = (posC.y() - posA.y()) / (rows - 1);
    double rowSpanZ = (posC.z() - posA.z()) / (rows - 1);

    for (int j = 0; j < rows; j++) {
		//每行起始点
        double startX = posA.x() + j * rowSpanX;
        double startY = posA.y() + j * rowSpanY;
        double startZ = posA.z() + j * rowSpanZ;
		//每行点位
        for (int i = 0; i < cols; i++) {
            (*curVecPos)[j * cols + i].setX(startX + i * colSpanX);
            (*curVecPos)[j * cols + i].setY(startY + i * colSpanY);
            (*curVecPos)[j * cols + i].setZ(startZ + i * colSpanZ);
            (*curVecPos)[j * cols + i].setW(posA.w());
        }
    }
}

void WidgetRecipeTray::slotTrayClick(int index, int silos, int row, int col)
{
    TrayCtrl* pSender = (TrayCtrl*)sender();
    if (m_selectTray != nullptr && pSender != m_selectTray) {
        m_selectTray->ClearSelect();
    }
    m_selectTray = pSender;
    int trayRows, trayCols;
    m_selectTray->GetTrayRowCol(trayRows, trayCols);
    int gripType = m_selectTray->property("GripType").toInt();
    int trayType = m_selectTray->property("TrayType").toInt();
    ShowCurHoleInfo((GripType)gripType, (TrayType)trayType, row * trayCols + col);
}

void WidgetRecipeTray::on_doubleSpinBoxPosX_editingFinished()
{
    if (m_selectTray == nullptr)return;
    double posX=ui->doubleSpinBoxPosX->value();
    (*m_trayToPos[m_selectTray])[m_curHole].setX(posX);
}

void WidgetRecipeTray::on_doubleSpinBoxPosY_editingFinished()
{
    if (m_selectTray == nullptr)return;
    double posY=ui->doubleSpinBoxPosY->value();
    (*m_trayToPos[m_selectTray])[m_curHole].setY(posY);
}
