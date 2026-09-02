#include "MesManager.h"
#include "FreetechMesManage.h"
#include "ParamManager.h"
#include "VisAppBus.h"

MesManager::MesManager(QObject *parent)
{
    VisAppBus::subscibeEvent(this, "getWorkPwoList");
    VisAppBus::subscibeEvent(this, "checkLensCode");
    VisAppBus::subscibeEvent(this, "passStation");
    VisAppBus::subscibeEvent(this, "productionEnded");
    VisAppBus::subscibeEvent(this, "sensorIdBind");
    VisAppBus::subscibeEvent(this, "getCtrIParameters");
}

int MesManager::event_getWorkPwoList(QString trayCode,QString &workPwoNo,int &res, QString &msg)
{
    for (int i = 0; i < 2; i++)
    {
        msg.clear();
        workPwoNo.clear();
        PwoInfo_IN inData;
        inData.equipmentCode = GlobalParam->systemParam.mesParam.sEquipmentCode;
        inData.operationInstanceCode = GlobalParam->systemParam.mesParam.sOperationInstance;
        inData.ContainerType = "H";
        inData.toolingSeque = trayCode;
        inData.WIP_ID_Type_Code = "25";
        inData.WIP_ID_Code = trayCode;
        inData.isProduct = false;

        PwoInfo_Out outData;
        QString flag = FreetechMes::getInstance()->GetPWOList(0,inData, outData);
        if (flag.isEmpty())
        {
            ShowSystemLog(Log_Debug, QString(u8"MES工单列表获取成功"));
            if (outData.allProduct.size() == 0)
            {
                msg = QString(u8"MES工单列表为空");
                ShowSystemLog(Log_Error, msg);
                res = -1;
                return -1;
            }
            if (outData.allProduct[0].ProductNo != GlobalParam->recipeProduct.curMatrix->productNo)
            {
                msg = QString(u8"MES获取工单为:%1, 本地作业工单为:%2").arg(outData.allProduct[0].ProductNo).arg(GlobalParam->recipeProduct.curMatrix->productNo);
                ShowSystemLog(Log_Error, msg);
                res = -1;
                return -1;
            }
            res = 0;
            workPwoNo = outData.allProduct[0].PWOCode;
            ShowSystemLog(Log_Debug, QString(u8"MES工单与本地工单匹配"));
            return 0;
        }
        else
        {
            msg =QString(u8"MES工单列表获取失败:%1").arg(flag);
            if(msg.contains(QString(u8"等待回复信息超时")))
            {
                QString log = msg + QString(u8"%1").arg(i == 0 ? u8" \n 是否重新执行此接口?" : "");
                int nRes = VisAppBus::sendEvent("ShowMessageBox", log);
                if (nRes != 0){
                    res = -1;
                    return -1;
                }
            }
            else
            {
                res = -1;
                return -1;
            }
        }
    }
    res = -1;
    return -1;
}

int MesManager::event_checkLensCode(int station,QString lensCode,int checkType,QString &sensorId,int &res,QString &msg)
{
    for (int i = 0; i < 2; i++)
    {
        msg.clear();
        ErrorList_IN inData;

        QStringList stationCodeList;
        stationCodeList<<GlobalParam->systemParam.mesParam.sWorkStationCodeOne
                      <<GlobalParam->systemParam.mesParam.sWorkStationCodeTwo;

        inData.EquipmentCode = GlobalParam->systemParam.mesParam.sEquipmentCode;
        inData.OperationInstanceCode = GlobalParam->systemParam.mesParam.sOperationInstance;
        inData.ProductNo = GlobalParam->recipeProduct.curMatrix->productNo;
        inData.WorkStationCode = stationCodeList.at(station);
        inData.WIP_ID_Type_Code = "31";
        inData.WIP_ID_Code = lensCode;
        inData.Product_Family = "H";//"P"有模组就不管这个值
        if (checkType == 1)
            inData.pText = "SensorID";
        else
            inData.pText = "CheckJumpStation";

        ErrorList_Out outData;
        QString flag = FreetechMes::getInstance()->PokaYoke(station,inData, outData);
        msg = flag;
        if (msg.isEmpty())
        {
            ShowLog(Log_Station, station, Log_Debug, QString(u8"MES防错验证通过"));
            if (checkType == 1)
                sensorId = outData.pValue;
            res = 0;
            return 0;
        }
        else
        {
            QString log = QString(u8"MES防错验证未通过:%1").arg(msg);
            ShowLog(Log_Station, station, Log_Error, log);
            if (checkType == 0)
            {
                //调过当前站
                if (outData.pValue.toInt() == 2 && outData.Errcode == 15)
                {
                    ShowLog(Log_Station, station, Log_Error, QString(u8"模组跳站"));
                    res = -2;
                    return -2;
                }
            }
            if(msg.contains(QString(u8"等待回复信息超时")))
            {
                log = log + QString(u8"%1").arg(i == 0 ? u8" \n 是否重新执行此接口?" : "");
                int nRes = VisAppBus::sendEvent("ShowMessageBox", log);
                if (nRes != 0){
                    res = -1;
                    return -1;
                }
            }
            else
            {
                res = -1;
                return -1;
            }
        }
    }
    res = -1;
    return -1;
}

int MesManager::event_passStation(AritightTask item,int &res,QString &msg)
{
    for (int i = 0; i < 2; i++)
    {
        msg.clear();
        DC_TestData_In inData;
        inData.ExCode = "DC_TestData";
        inData.EquipmentCode = GlobalParam->systemParam.mesParam.sEquipmentCode;
        inData.OperationInstanceCode = GlobalParam->systemParam.mesParam.sOperationInstance;
        inData.ProductNo = GlobalParam->recipeProduct.curMatrix->productNo;
        inData.WIP_ID_Type_Code = "31";
        inData.WIP_ID_Code = item.barCode;
        inData.SerialNumber = item.barCode;//产品序列号
        //inData.SensorID = item.sensorId;
        inData.PWONo = item.workPwoNo;//工单号
        inData.ContainerNo = "";//在制品容器编号
        switch (item.station)
        {
        case 0: inData.WIPStationCode = GlobalParam->systemParam.mesParam.sWorkStationCodeOne; break;
        case 1: inData.WIPStationCode = GlobalParam->systemParam.mesParam.sWorkStationCodeTwo; break;
        }
        inData.RecipeID= "";//工艺配方标识
        inData.NumSubWIPs= "1";//子在制品数(int)
        inData.StartTime= item.startDateTime.toString("yyyy-MM-dd HH:mm:ss");//开始测试时间
        inData.EndTime= item.endDateTime.toString("yyyy-MM-dd HH:mm:ss");//截止测试时间
        inData.CntOfTests= "1";//测试次数（int）
        inData.FailureCode= item.result?"":QString::number(item.errorCode);//失效代码
        inData.FailureMessage= item.errorMsg;//失效名称
        inData.TestConclusion= item.result?1:2;//测试结论
        inData.TestSteps= "1";//测试步数(int)
        //测量值数
        inData.NumOfMetrics = GlobalParam->recipeProduct.curMatrix->barCodeStartPos1.toInt();

        //组织测试数据
        //AssemblyData(item,inData.allTestData);

        DC_TestData_Out outData;
        QString flag = FreetechMes::getInstance()->DC_TestData(item.station,inData, outData);
        msg = flag;
        if (flag.isEmpty())
        {
            ShowLog(Log_Station,item.station,Log_Info,QString(u8"气密测试数据上传MES成功"));
            //数据OK进行模组过站操作
            ShowLog(Log_Station,item.station,Log_Info,QString(u8"模组过站"));
            res = ModulePassStation(item, msg);
            if(res == 0){
                return res;
            }
        }
        else
        {
            QString log = QString(u8"气密测试数据上传MES失败:%1").arg(msg);
            ShowLog(Log_Station,item.station,Log_Error,log);
            if(msg.contains(QString(u8"等待回复信息超时")))
            {
                QString log = msg + QString(u8"%1").arg(i == 0 ? u8" \n 是否重新执行此接口?" : "");
                int nRes = VisAppBus::sendEvent("ShowMessageBox", log);
                if (nRes != 0){
                    res = -1;
                    return -1;
                }
            }
            else
            {
                res = -1;
                return -1;
            }
        }
    }
    res = -1;
    return -1;
}

int MesManager::ModulePassStation(AritightTask item, QString &describe)
{
    for (int i = 0; i < 2; i++)
    {
        DC_TestData_In inData;
        inData.ExCode = "DC_Test";
        inData.EquipmentCode = GlobalParam->systemParam.mesParam.sEquipmentCode;
        inData.OperationInstanceCode = GlobalParam->systemParam.mesParam.sOperationInstance;
        inData.ProductNo = GlobalParam->recipeProduct.curMatrix->productNo;
        inData.WIP_ID_Type_Code = "31";
        inData.WIP_ID_Code = item.barCode;
        inData.SerialNumber = item.barCode;//产品序列号
//        inData.SensorID = item.sensorId;
        inData.PWONo = item.workPwoNo;//工单号
        inData.ContainerNo = "";//在制品容器编号
        switch (item.station)
        {
        case 0: inData.WIPStationCode = GlobalParam->systemParam.mesParam.sWorkStationCodeOne; break;
        case 1: inData.WIPStationCode = GlobalParam->systemParam.mesParam.sWorkStationCodeTwo; break;
        }
        inData.RecipeID = "";//工艺配方标识
        inData.NumSubWIPs = "1";//子在制品数(int)
        inData.StartTime = item.startDateTime.toString("yyyy-MM-dd HH:mm:ss");//开始测试时间
        inData.EndTime = item.endDateTime.toString("yyyy-MM-dd HH:mm:ss");//截止测试时间
        inData.CntOfTests = "1";//测试次数（int）
        inData.FailureCode = item.result ? "" : QString::number(item.errorCode);//失效代码
        inData.FailureMessage = item.errorMsg;//失效名称
        inData.TestConclusion = item.result ? 1 : 2;//测试结论
        inData.TestSteps = "1";//测试步数(int)
        //测量值数
        inData.NumOfMetrics = GlobalParam->recipeProduct.curMatrix->barCodeStartPos1.toInt();
        DC_TestData_Out outData;
        QString msg = FreetechMes::getInstance()->DC_TestData(item.station,inData, outData);
        describe = msg;
        if (msg.isEmpty())
        {
            ShowLog(Log_Station,item.station,Log_Info,QString(u8"气密测试数据MES过站成功"));
            return 0;
        }
        else
        {
            QString log = QString(u8"气密测试数据MES过站失败:%1").arg(msg);
            ShowLog(Log_Station,item.station,Log_Error,log);
            if(msg.contains(QString(u8"等待回复信息超时")))
            {
                QString log = msg + QString(u8"%1").arg(i == 0 ? u8" \n 是否重新执行此接口?" : "");
                int nRes = VisAppBus::sendEvent("ShowMessageBox", log);
                if (nRes != 0){
                    return -1;
                }
            }
            else
            {
                return -1;
            }
        }
    }
    return -1;
}

int MesManager::event_productionEnded(int station, QString lensCode,bool result, int &res, QString &msg)
{
    msg.clear();
    ProductionEnd_In inData;

    inData.ExCode = "DC_Test";
    inData.EquipmentCode = GlobalParam->systemParam.mesParam.sEquipmentCode;
    inData.OperationInstanceCode = GlobalParam->systemParam.mesParam.sOperationInstance;
    inData.ProductNo = GlobalParam->recipeProduct.curMatrix->productNo;
    switch (station)
    {
    case 0: inData.WorkStationCode = GlobalParam->systemParam.mesParam.sWorkStationCodeOne; break;
    case 1: inData.WorkStationCode = GlobalParam->systemParam.mesParam.sWorkStationCodeTwo; break;
    }
    inData.WIP_ID_Type_Code = "25";
    inData.WIP_ID_Code = lensCode;
    inData.Qty = 1;
    inData.Conclusion = static_cast<int>(result);

    ProductionEnd_Out outData;
    QString flag = FreetechMes::getInstance()->ProductionEnd(station,inData, outData);
    if(!flag.isEmpty())
    {
        res = -1;
        msg = QStringLiteral("生产结束上传失败:原因%1").arg(flag);
        return res;
    }
    res = outData.ErrCode;
    msg = outData.ErrText;
    return res;
}

//int MesManager::event_getCtrIParameters(int station,QString &msg)
//{
//    GetMFGCtrl_IN inData;
//    inData.EquipmentCode = GlobalParam->systemParam.deviceCode;
//    inData.ProductNo = GlobalParam->recipeProduct.curMatrix->productNo;
//    inData.OperationInstanceCode = GlobalParam->systemParam.procedureCode;
//    QStringList stationCodeList;
//    stationCodeList<<GlobalParam->systemParam.stationCode1<<GlobalParam->systemParam.stationCode2<<GlobalParam->systemParam.stationCode3<<GlobalParam->systemParam.stationCode4;
//    inData.WorkStationCode = stationCodeList.at(station);
//    inData.Model = GlobalParam->recipeProduct.curMatrix->productName;
//    inData.ParameterName = "";
//    inData.ParameterType = "0";

//    GetMFGCtrl_OUT outData;
//    QString flag = FreetechMes::getInstance()->GetMFGCtrlParameters(station,inData,outData);
//    if(!flag.isEmpty())
//    {
//        msg = QStringLiteral("请求参数管控失败:原因%1").arg(flag);
//        return -1;
//    }
//    msg = outData.ErrText;
//    return outData.ErrCode;
//}

//int MesManager::event_uploadCtrIParameters(FinalItemTask item, QString &msg)
//{
//    UploadMFGCtrlParameterResult_IN inData;
//    inData.EquipmentCode = GlobalParam->systemParam.deviceCode;
//    inData.PWONo = item.workPwoNo;
//    inData.ProductNo = GlobalParam->recipeProduct.curMatrix->productNo;
//    inData.OperationInstanceCode = GlobalParam->systemParam.procedureCode;
//    QStringList stationCodeList;
//    stationCodeList<<GlobalParam->systemParam.stationCode1<<GlobalParam->systemParam.stationCode2<<GlobalParam->systemParam.stationCode3<<GlobalParam->systemParam.stationCode4;
//    inData.WorkStationCode = stationCodeList.at(item.holeNum);
//    inData.Model = GlobalParam->recipeProduct.curMatrix->productName;
//    inData.Conclusion = "";

//    UploadMFGCtrlParameterResult_IN::UploadMFGCtrlParameterResultRowData data;
//    data.ParameterName = "";
//    data.ParameterValue = "";
//    data.Lowerlimit = "";
//    data.Upperlimit = "";
//    data.ParameterType = "";
//    data.ActualValue = "";
//    data.Result = "";
//    data.Remark = "";

//    inData.rowData.append(data);
//    UploadMFGCtrlParameterResult_Out outData;
//    QString flag = FreetechMes::getInstance()->UploadMFGCtrlParameterResult(item.holeNum,inData,outData);
//    if(!flag.isEmpty())
//    {
//        msg = QStringLiteral("请求参数管控回传失败:原因%1").arg(flag);
//        return -1;
//    }
//    msg = outData.ErrText;
//    return outData.ErrCode;
//}

//void MesManager::AssemblyData(FinalItemTask item,QVector<DC_TestData_In::testData> &allTestData)
//{
//    DC_TestData_In::testData data;
//    int index = 1;

//    m_scale =static_cast<int>(pow(10,GlobalParam->systemParam.decimals));

//    AssemblyVppVccData(item,index,allTestData);

//    //GELE 大于等于X小于等于
//    //GTLE 大于X 小于等于
//    //GELT 大于等于 X 小于
//    //GTLT 大于 X 小于
//    //EQ 等于
//    //NE 不等于
//    //GT 大于
//    //GE 大于等于
//    //LT 小于
//    //LE 小于等于、BOOL 布尔是否
//}

//void MesManager::AssemblyVppVccData(FinalItemTask item,int &index,QVector<DC_TestData_In::testData> &allTestData)
//{
//    DC_TestData_In::testData data;

//    data = GetTestData(index,(GlobalParam->recipeFinal.electriParamVec[item.holeNum].currentRefer-GlobalParam->recipeFinal.electriParamVec[item.holeNum].currentSpec),
//            (GlobalParam->recipeFinal.electriParamVec[item.holeNum].currentRefer+GlobalParam->recipeFinal.electriParamVec[item.holeNum].currentSpec),
//            item.finalTestResult.pmuA,"WorkingCurrent_Ivpp","GTLT","mA");
//    allTestData.append(data);

//    data = GetTestData(index,(GlobalParam->recipeFinal.electriParamVec[item.holeNum].voltageRefer-GlobalParam->recipeFinal.electriParamVec[item.holeNum].voltageSpec),
//            (GlobalParam->recipeFinal.electriParamVec[item.holeNum].voltageRefer+GlobalParam->recipeFinal.electriParamVec[item.holeNum].voltageSpec),
//            item.finalTestResult.pmuV,"WorkingVoltage_Vpp","GTLT","V");
//    allTestData.append(data);

//    data = GetTestData(index,(GlobalParam->recipeFinal.electriParamVec[item.holeNum].fpsRefer - GlobalParam->recipeFinal.electriParamVec[item.holeNum].fpsSpec),
//            (GlobalParam->recipeFinal.electriParamVec[item.holeNum].fpsRefer + GlobalParam->recipeFinal.electriParamVec[item.holeNum].fpsSpec),
//            item.finalTestResult.fps,"Fps","GTLT","");
//    allTestData.append(data);
//}


DC_TestData_In::testData MesManager::GetTestData(int &index,double low,double upper,double value,QString name,QString compareType,QString unit)
{
    DC_TestData_In::testData data;
    data.Ordinal = index;//序号
    data.TestItemCode = "";//测试项代码
    data.MetricName = name;//测试项名称
    data.HighLimit = upper*m_scale;//低限值
    data.Criterion = compareType;//测试通过标准
    data.LowLimit = low*m_scale;//高限值
    data.Scale = GlobalParam->systemParam.dataStorage.decimals;//放大数量级
    data.UnitOfMeasure = unit;//计量单位
    data.remark = "";//备注
    data.ImageBase64="";//影像Base64转换
    data.Metric01 = static_cast<int64_t>(value*m_scale);//第01个测量值
    data.Conclusion = getResult(data.Criterion,data.Metric01,data.LowLimit,data.HighLimit);//单项测试结论
    index++;
    return data;
}

QString MesManager::getResult(QString type,double value, double low, double upper)
{
    QString str = "";
    if(type == "GELE")//大于等于X小于等于
    {
        if( low <= value && value <= upper)
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    else if(type == "GTLE")//大于X 小于等于
    {
        if( low < value && value <= upper)
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    else if(type == "GELT")//大于等于 X 小于
    {
        if( low <= value && value < upper)
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    else if(type == "GTLT")//大于 X 小于
    {
        if( low < value && value < upper)
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    else if(type == "EQ")//等于
    {
        if( low == value)
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    else if(type == "NE")//不等于
    {
        if( low != value)
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    else if(type == "GT")//大于
    {
        if( low < value)
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    else if(type == "GE")// 大于等于
    {
        if( low <= value)
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    else if(type == "LT")// 小于
    {
        if( value < upper )
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    else if(type == "LE")// 小于等于
    {
        if( value <= upper )
        {
            str = "P";
        }
        else
        {
            str = "F";
        }
    }
    return str;
}
