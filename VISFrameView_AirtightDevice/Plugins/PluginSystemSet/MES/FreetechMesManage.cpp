#include "FreetechMesManage.h"
#include <QTimer>
#include <qDebug>
#include <QApplication>
#include "ParamManager.h"

FreetechMes* FreetechMes::instance = nullptr;
QMutex FreetechMes::m_mutex;
QMutex FreetechMes::m_mutex2;
FreetechMes::FreetechMes()
{
    //获取当前的线程id
    m_currentId = std::this_thread::get_id();
}



FreetechMes::~FreetechMes()
{
}

FreetechMes* FreetechMes::getInstance()
{
    QMutexLocker locker(&m_mutex2);
    if (!instance) {
        instance = new FreetechMes();
        instance->initConnet();
    }
    return instance;
}

void FreetechMes::setUrlIp(QString urlIp)
{
    m_urlIp = urlIp;
}

QString FreetechMes::GetToken(TOKEN_IN info, TOKEN_OUT& outData)
{
    QJsonObject dataObject;
    dataObject.insert("ExCode", "GetToken");
    dataObject.insert("CommunityID", info.CommunityId);
    dataObject.insert("UserCode", info.UserCode);
    dataObject.insert("PlainPWD", info.PlainPWD);
    m_mutex.lock();
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObject, "GetToken")) {
            m_request.setRawHeader("Access_Token", m_token.toUtf8());
            outData = m_tokenOutData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }
    }
    else {
        bool result;
        QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObject), Q_ARG(QString, "GetToken"));
        if (result) {
            m_request.setRawHeader("Access_Token", m_token.toUtf8());
            outData = m_tokenOutData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }
    }

    return NULL;
}

//bool FreetechMes::GetEquipmentList(QString stationId)
//{
//	QJsonObject dataObject;
//	dataObject.insert("ExCode", "GetEquipmentList");
//	dataObject.insert("StationID", stationId);
//	m_messageType = "GetEquipmentList";
//	if (m_currentId == std::this_thread::get_id()) {
//		return postData(dataObject);
//	}
//	else {
//		bool result;
//		QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
//			Q_ARG(QJsonObject, dataObject));
//		return result;
//	}
//
//}

QString FreetechMes::GetMFGCtrlParameters(int station,GetMFGCtrl_IN inData, GetMFGCtrl_OUT& outData)
{
    QJsonObject dataObject;
    dataObject.insert("ExCode", "GetMFGCtrlParameters");
    dataObject.insert("EquipmentCode", inData.EquipmentCode);
    dataObject.insert("OperationInstanceCode", inData.OperationInstanceCode);
    dataObject.insert("WorkStationCode", inData.WorkStationCode);
    dataObject.insert("Model", inData.Model);
    dataObject.insert("ParameterName", inData.ParameterName);
    dataObject.insert("ParameterType", inData.ParameterType);
    m_mutex.lock();
    m_station = station;
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObject, "GetMFGCtrlParameters")) {
            outData = m_GetMFGCtrl_OUTData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }

    }
    else {
        bool result;
        QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObject), Q_ARG(QString, "GetMFGCtrlParameters"));
        if (result) {
            outData = m_GetMFGCtrl_OUTData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }

    }
}

QString FreetechMes::UploadMFGCtrlParameterResult(int station,UploadMFGCtrlParameterResult_IN inData, UploadMFGCtrlParameterResult_Out& outData)
{
    QJsonObject dataObject;
    dataObject.insert("ExCode", "UploadMFGCtrlParameterResult");
    dataObject.insert("EquipmentCode", inData.EquipmentCode);
    dataObject.insert("PWONo", inData.PWONo);
    dataObject.insert("ProductNo", inData.ProductNo);
    dataObject.insert("OperationInstanceCode", inData.OperationInstanceCode);
    dataObject.insert("WorkStationCode", inData.WorkStationCode);
    dataObject.insert("Model", inData.Model);
    dataObject.insert("Conclusion", inData.Conclusion);
    QJsonArray dataArray;
    for (int m = 0; m < inData.rowData.size(); m++) {
        QJsonObject oneTestData;
        oneTestData.insert("ParameterName", inData.rowData[m].ParameterName);
        oneTestData.insert("ParameterValue", inData.rowData[m].ParameterValue);
        oneTestData.insert("Lowerlimit", inData.rowData[m].Lowerlimit);
        oneTestData.insert("Upperlimit", inData.rowData[m].Upperlimit);
        oneTestData.insert("ParameterType", inData.rowData[m].ParameterType);
        oneTestData.insert("ActualValue", inData.rowData[m].ActualValue);
        oneTestData.insert("Result", inData.rowData[m].Result);
        oneTestData.insert("Remark", inData.rowData[m].Remark);
        dataArray.append(oneTestData);
    }
    dataObject.insert("Rows", dataArray);

    m_mutex.lock();
    m_station = station;
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObject, "UploadMFGCtrlParameterResult")) {
            outData = m_UploadMFGCtrlParameterResultData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }

    }
    else {
        bool result;
        QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObject), Q_ARG(QString, "UploadMFGCtrlParameterResult"));
        if (result) {
            outData = m_UploadMFGCtrlParameterResultData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }

    }
}

QString FreetechMes::GetPWOList(int station,PwoInfo_IN inData, PwoInfo_Out& outData)
{
    QJsonObject dataObject;
    QString ExCode;
    if(inData.isProduct){
        ExCode = "GetPWOPN";
        dataObject.insert("WIP_ID_Type_Code", inData.WIP_ID_Type_Code);
        dataObject.insert("WIP_ID_Code", inData.WIP_ID_Code);
    }
    else{
        ExCode = "GetPWOList";
        dataObject.insert("ContainerType", inData.ContainerType);
        dataObject.insert("ToolingSequenceNo", inData.toolingSeque);
    }
    dataObject.insert("ExCode", ExCode);
    dataObject.insert("EquipmentCode", inData.equipmentCode);
    dataObject.insert("OperationInstanceCode", inData.operationInstanceCode);
    m_mutex.lock();
    m_station = station;
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObject, ExCode)) {
            outData = m_PwoInfo;
            if (outData.Errcode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }
    }
    else {
        bool result = false;
        QMetaObject::invokeMethod(instance, "postData",
                                  Qt::BlockingQueuedConnection,
                                  Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObject),
                                  Q_ARG(QString, ExCode));

        if (result) {
            outData = m_PwoInfo;
            if (outData.Errcode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }
    }
}

//bool FreetechMes::GetProcessControlMatrix(QString StationID)
//{
//	//添加url(接口)
//	QString postUrl = "http://" + m_urlIp + "/api/DCS/GetPWOList";
//	m_request.setUrl(postUrl);
//	QJsonObject dataObject;
//	dataObject.insert("ExCode", "GetProcessControlMatrix");
//	dataObject.insert("StationID", StationID);
//	m_messageType = "GetProcessControlMatrix";
//	bool ifSend = postData(dataObject);
//	if (m_currentId == std::this_thread::get_id()) {
//		return postData(dataObject);
//	}
//	else {
//		bool result;
//		QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
//			Q_ARG(QJsonObject, dataObject));
//		return result;
//	}
//}

QString FreetechMes::PokaYoke(int station,ErrorList_IN inData, ErrorList_Out& outData)
{
    QJsonObject dataObject;
    dataObject.insert("ExCode", "PokaYoke");
    dataObject.insert("EquipmentCode", inData.EquipmentCode);
    dataObject.insert("ProductNo", inData.ProductNo);
    dataObject.insert("OperationInstanceCode", inData.OperationInstanceCode);
    dataObject.insert("WorkStationCode", inData.WorkStationCode);
    dataObject.insert("WIP_ID_Type_Code", inData.WIP_ID_Type_Code);
    dataObject.insert("WIP_ID_Code", inData.WIP_ID_Code);
    dataObject.insert("P", inData.pText);
    dataObject.insert("Product_Family", inData.Product_Family);
    m_mutex.lock();
    m_station = station;
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObject, "PokaYoke")) {
            outData = m_error;
            bool ok = false;
            int pValue = outData.pValue.toInt(&ok);
            if (inData.pText == "SensorID")
            {
                ok = true;
                pValue = 0;
            }
            if (outData.Errcode != 0 || !ok || (ok && pValue != 0)) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }
    }
    else {
        bool result;
        QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObject), Q_ARG(QString, "PokaYoke"));
        if (result) {
            outData = m_error;
            if (outData.Errcode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }
    }
}

QString FreetechMes::DC_TestData(int station,DC_TestData_In inData, DC_TestData_Out& outData)
{
    QJsonObject dataObject;
    //	dataObject.insert("ExCode", "DC_TestData");
    dataObject.insert("ExCode", inData.ExCode);
    dataObject.insert("EquipmentCode", inData.EquipmentCode);
    dataObject.insert("OperationInstanceCode", inData.OperationInstanceCode);
    dataObject.insert("ProductNo", inData.ProductNo);
    dataObject.insert("WIP_ID_Type_Code", inData.WIP_ID_Type_Code);
    dataObject.insert("WIP_ID_Code", inData.WIP_ID_Code);
    dataObject.insert("SensorID", inData.SensorID);
    dataObject.insert("PWONo", inData.PWONo);
    dataObject.insert("WIPStationCode", inData.WIPStationCode);
    dataObject.insert("NumSubWIPs", inData.NumSubWIPs);
    dataObject.insert("StartTime", inData.StartTime);
    dataObject.insert("EndTime", inData.EndTime);
    dataObject.insert("CntOfTests", inData.CntOfTests);
    dataObject.insert("TestConclusion", inData.TestConclusion);
    dataObject.insert("TestSteps", inData.TestSteps);
    dataObject.insert("NumOfMetrics", inData.NumOfMetrics);
    dataObject.insert("FailureCode", inData.FailureCode);
    dataObject.insert("FailureMessage", inData.FailureMessage);
    //TestData
    QJsonArray dataArray;
    for (int m = 0; m < inData.allTestData.size(); m++) {
        QJsonObject oneTestData;
        oneTestData.insert("Ordinal", inData.allTestData[m].Ordinal);
        oneTestData.insert("TestItemCode", inData.allTestData[m].TestItemCode);
        oneTestData.insert("MetricName", inData.allTestData[m].MetricName);
        oneTestData.insert("LowLimit", inData.allTestData[m].LowLimit);
        oneTestData.insert("Criterion", inData.allTestData[m].Criterion);
        oneTestData.insert("HighLimit", inData.allTestData[m].HighLimit);
        oneTestData.insert("Scale", inData.allTestData[m].Scale);
        oneTestData.insert("UnitOfMeasure", inData.allTestData[m].UnitOfMeasure);
        oneTestData.insert("Conclusion", inData.allTestData[m].Conclusion);
        oneTestData.insert("Remark", inData.allTestData[m].remark);
        oneTestData.insert("ImageBase64", inData.allTestData[m].ImageBase64);
        oneTestData.insert("Metric01", inData.allTestData[m].Metric01);
        dataArray.append(oneTestData);
    }
    dataObject.insert("TestData", dataArray);
    //FailureModes
    //QJsonArray modesArray;
    //for (int i = 0; i < inData.allModes.size(); i++) {
    //	QJsonObject oneModes;
    //	oneModes.insert("Ordinal", inData.allModes[i].Ordinal);
    //	oneModes.insert("CompLocCode", inData.allModes[i].CompLocCode);
    //	oneModes.insert("MaterialCode", inData.allModes[i].MaterialCode);
    //	oneModes.insert("DefectCode", inData.allModes[i].DefectCode);
    //	oneModes.insert("RootOperationInstanceCode", inData.allModes[i].RootOperationInstanceCode);
    //	oneModes.insert("DefectTypeCode", inData.allModes[i].DefectTypeCode);
    //	oneModes.insert("DefectOwner", inData.allModes[i].DefectOwner);
    //	oneModes.insert("CntDefect", inData.allModes[i].CntDefect);
    //	oneModes.insert("DefectImageBase64", inData.allModes[i].DefectImageBase64);
    //	modesArray.append(oneModes);
    //}
    //dataObject.insert("FailureModes", modesArray);
    ////Recipes
    //QJsonArray recipesArray;
    //for (int g = 0; g < inData.allRecipes.size(); g++) {
    //	QJsonObject oneRecipes;
    //	oneRecipes.insert("ParamCode", inData.allRecipes[g].ParamCode);
    //	oneRecipes.insert("ParamDesc", inData.allRecipes[g].ParamDesc);
    //	oneRecipes.insert("DataType",inData.allRecipes[g].DataType);
    //	oneRecipes.insert("Scale", inData.allRecipes[g].Scale);
    //	oneRecipes.insert("UnitOfMeasure", inData.allRecipes[g].UnitOfMeasure);
    //	oneRecipes.insert("ParamValue", inData.allRecipes[g].ParamValue);
    //	oneRecipes.insert("ParamStrValue", inData.allRecipes[g].ParamStrValue);
    //	recipesArray.append(oneRecipes);
    //}
    //dataObject.insert("Recipes", recipesArray);
    m_mutex.lock();
    m_station = station;
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObject, inData.ExCode)) {
            outData = m_dcTestData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }

    }
    else {
        bool result;
        //		QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_ARG(QJsonObject, dataObject),
        //			Q_ARG(QString, "DC_TestData"),
        //			Q_RETURN_ARG(bool, result));
        QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObject),
                                  Q_ARG(QString, inData.ExCode));
        if (result) {
            outData = m_dcTestData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }

    }
}

QString FreetechMes::IDBinding(int station,BindSensorID_In inData, BindSensorID_Out& outData)
{
    QJsonObject dataObj;
    dataObj.insert("ExCode", "IDBinding");
    dataObj.insert("OperationInstanceCode", inData.OperationInstanceCode);
    dataObj.insert("WorkStationCode", inData.WorkStationCode);
    dataObj.insert("EquipmentCode", inData.EquipmentCode);
    dataObj.insert("ProductNo", inData.ProductNo);
    dataObj.insert("WIP_ID_Type_Code", inData.WIP_ID_Type_Code);
    dataObj.insert("WIP_ID_Code", inData.WIP_ID_Code);

    QJsonArray dataArray;
    for (int m = 0; m < inData.allID.size(); m++) {
        QJsonObject oneWip;
        oneWip.insert("ID_Part_Type_Code", inData.allID[m].ID_Part_Type_Code);
        oneWip.insert("ID_Part_Name", inData.allID[m].ID_Part_Name);
        oneWip.insert("ID_Part_SN_Scanner_Code", inData.allID[m].ID_Part_SN_Scanner_Code);
        oneWip.insert("Sequence_Number", inData.allID[m].Sequence_Number);
        dataArray.append(oneWip);
    }
    dataObj.insert("Rows", dataArray);
    m_mutex.lock();
    m_station = station;
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObj, "IDBinding")) {
            outData = m_BindIdData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }

    }
    else {
        bool result;
        QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObj), Q_ARG(QString, "IDBinding"));
        if (result) {
            outData = m_BindIdData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }
    }
}

QString FreetechMes::GetGlueCtrlInfo(int station,GetGlueCtrlInfo_IN inData, GetGlueCtrlInfo_OUT& outData)
{
    QJsonObject dataObject;
    dataObject.insert("ExCode", "GetGlueCtrlInfo");
    dataObject.insert("EquipmentCode", inData.EquipmentCode);
    dataObject.insert("SKUID", inData.SKUID);
    dataObject.insert("OperationInstanceCode", inData.OperationInstanceCode);
    dataObject.insert("WorkStationCode", inData.WorkStationCode);
    m_mutex.lock();
    m_station = station;
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObject, "GetGlueCtrlInfo")) {
            outData = m_GetGlueCtrlInfoData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }

    }
    else {
        bool result;
        QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObject), Q_ARG(QString, "GetGlueCtrlInfo"));
        if (result) {
            outData = m_GetGlueCtrlInfoData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }

    }
}

//bool FreetechMes::DC_Inspect(DC_Inspect_In inData)
//{
//	QJsonObject dataObj;
//	dataObj.insert("ExCode", "DC_Inspect");
//	dataObj.insert("ProductNo", inData.ProductNo);
//	dataObj.insert("EquipmentCode", inData.EquipmentCode);
//	dataObj.insert("OperationInstanceCode", inData.OperationInstanceCode);
//	dataObj.insert("WorkStationCode", inData.WorkStationCode);
//	dataObj.insert("WIP_ID_Type_Code", inData.WIP_ID_Type_Code);
//	dataObj.insert("WIP_ID_Code", inData.WIP_ID_Code);
//	dataObj.insert("PWONo", inData.PWONo);
//	dataObj.insert("ContainerNo", inData.ContainerNo);
//	dataObj.insert("StartTime", inData.StartTime);
//	dataObj.insert("EndTime", inData.EndTime);
//	dataObj.insert("InspectionType", inData.InspectionType);
//	dataObj.insert("InspectionConclusion", inData.InspectionConclusion);
//
//	QJsonArray dataArray;
//	for (int m = 0; m < inData.allDetail.size(); m++) {
//		QJsonObject detail;
//		detail.insert("Ordinal", inData.allDetail[m].Ordinal);
//		detail.insert("CompLocCode", inData.allDetail[m].CompLocCode);
//		detail.insert("MaterialCode", inData.allDetail[m].MaterialCode);
//		detail.insert("DefectCode", inData.allDetail[m].DefectCode);
//		detail.insert("ConfirmedDefectCode", inData.allDetail[m].ConfirmedDefectCode);
//		detail.insert("StandardDefectCode", inData.allDetail[m].StandardDefectCode);
//		detail.insert("RootOperationInstanceCode", inData.allDetail[m].RootOperationInstanceCode);
//		detail.insert("DefectTypeCode", inData.allDetail[m].DefectTypeCode);
//		detail.insert("DefectOwner", inData.allDetail[m].DefectOwner);
//		detail.insert("CntDefect", inData.allDetail[m].CntDefect);
//		detail.insert("DefectImageBase64", inData.allDetail[m].DefectImageBase64);
//		dataArray.append(detail);
//	}
//	dataObj.insert("Detail", dataArray);
//	m_messageType = "DC_Inspect";
//	if (m_currentId == std::this_thread::get_id()) {
//		return postData(dataObj);
//	}
//	else {
//		bool result;
//		QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
//			Q_ARG(QJsonObject, dataObj));
//		return result;
//	}
//}

QString FreetechMes::ProductionEnd(int station,ProductionEnd_In inData, ProductionEnd_Out& outData)
{
    QJsonObject dataObj;
    dataObj.insert("ExCode", inData.ExCode);
    //    dataObj.insert("ExCode", "ProductionEnd");
    dataObj.insert("EquipmentCode", inData.EquipmentCode);
    dataObj.insert("ProductNo", inData.ProductNo);
    dataObj.insert("OperationInstanceCode", inData.OperationInstanceCode);
    dataObj.insert("WorkStationCode", inData.WorkStationCode);
    dataObj.insert("WIP_ID_Type_Code", inData.WIP_ID_Type_Code);
    dataObj.insert("WIP_ID_Code", inData.WIP_ID_Code);
    dataObj.insert("Qty", inData.Qty);
    dataObj.insert("Conclusion", inData.Conclusion);

    //QJsonArray dataArray;
    //for (int m = 0; m < inData.allPoint.size(); m++) {
    //	QJsonObject onePoint;
    //	onePoint.insert("TagName", inData.allPoint[m].TagName);
    //	onePoint.insert("Value", inData.allPoint[m].value);
    //	onePoint.insert("Test_Item_Number", inData.allPoint[m].Test_Item_Number);
    //	onePoint.insert("Conclusion", inData.allPoint[m].Conclusion);
    //	onePoint.insert("Remark", inData.allPoint[m].Remark);
    //	onePoint.insert("Metric01", inData.allPoint[m].Metric01);
    //	onePoint.insert("Low_Limit", inData.allPoint[m].Low_Limit);
    //	onePoint.insert("Criterion", inData.allPoint[m].Criterion);
    //	onePoint.insert("High_Limit", inData.allPoint[m].High_Limit);
    //	onePoint.insert("Tool_Code", inData.allPoint[m].Tool_Code);
    //	onePoint.insert("Tool_SN", inData.allPoint[m].Tool_SN);
    //	onePoint.insert("Tool_Use_Life_In_Times", inData.allPoint[m].Tool_Use_Life_In_Times);
    //	onePoint.insert("Tool_PM_Life_In_Times", inData.allPoint[m].Tool_PM_Life_In_Times);
    //	dataArray.append(onePoint);
    //}
    //dataObj.insert("Rows", dataArray);
    m_mutex.lock();
    m_station = station;
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObj, "ProductionEnd")) {
            outData = m_ProductionEndData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }

    }
    else {
        bool result;
        QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObj), Q_ARG(QString, "ProductionEnd"));
        if (result) {
            outData = m_ProductionEndData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }

    }

}

QString FreetechMes::WIPBindToContainer(int station,WIPBindToContainer_In inData, WIPBindToContainer_Out& outData)
{
    QJsonObject dataObj;
    dataObj.insert("ExCode", "WIPBindToContainer");
    dataObj.insert("OperationInstanceCode", inData.OperationInstanceCode);
    dataObj.insert("WorkStationCode", inData.WorkStationCode);
    dataObj.insert("EquipmentCode", inData.EquipmentCode);
    dataObj.insert("ProductNo", inData.ProductNo);
    dataObj.insert("ContainerNo", inData.ContainerNo);
    dataObj.insert("PWONo", inData.PWONo);
    dataObj.insert("Qty", inData.Qty);

    QJsonArray dataArray;
    for (int m = 0; m < inData.allWIP.size(); m++) {
        QJsonObject oneWip;
        oneWip.insert("WIP_ID_Type_Code", inData.allWIP[m].WIP_ID_Type_Code);
        oneWip.insert("WIP_ID_Code", inData.allWIP[m].WIP_ID_Code);
        oneWip.insert("SensorID", inData.allWIP[m].SensorID);
        oneWip.insert("Qty", inData.allWIP[m].Qty);
        oneWip.insert("QtyScale", inData.allWIP[m].QtyScale);
        oneWip.insert("ProductNo", inData.allWIP[m].ProductNo);
        dataArray.append(oneWip);
    }
    dataObj.insert("Rows", dataArray);
    m_mutex.lock();
    m_station = station;
    if (m_currentId == std::this_thread::get_id()) {
        if (postData(dataObj, "WIPBindToContainer")) {
            outData = m_WipBingdData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QString(u8"发送失败:%1").arg(m_errorInfo);
        }

    }
    else {
        bool result;
        QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
                                  Q_ARG(QJsonObject, dataObj), Q_ARG(QString, "WIPBindToContainer"));
        if (result) {
            outData = m_WipBingdData;
            if (outData.ErrCode != 0) {
                m_mutex.unlock();
                return outData.ErrText;
            }
            else {
                m_mutex.unlock();
                return NULL;
            }
        }
        else {
            m_mutex.unlock();
            return QStringLiteral("数据返回失败");
        }

    }
}

void FreetechMes::SetTimeout(int time)
{
    if(time <= 0) return;
    m_timeout = time;
}

//bool FreetechMes::STD_M01(QString EquipmentCode, QString ProductNo, QString Product_Family, QString OperationInstanceCode)
//{
//	QJsonObject dataObj;
//	dataObj.insert("ExCode", "STD_M01");
//	dataObj.insert("EquipmentCode", EquipmentCode);
//	dataObj.insert("ProductNo", ProductNo);
//	dataObj.insert("Product_Family", Product_Family);
//	dataObj.insert("OperationInstanceCode", OperationInstanceCode);
//	m_messageType = "STD_M01";
//	if (m_currentId == std::this_thread::get_id()) {
//		return postData(dataObj);
//	}
//	else {
//		bool result;
//		QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
//			Q_ARG(QJsonObject, dataObj));
//		return result;
//	}
//}

//bool FreetechMes::CollectionDataRequest(CollectionDataRequest_In parm)
//{
//	QJsonObject dataObj;
//	dataObj.insert("ExCode", "CollectionDataRequest");
//	dataObj.insert("EquipmentCode", parm.EquipmentCode);
//	dataObj.insert("ProductNo", parm.ProductNo);
//	dataObj.insert("OperationInstanceCode", parm.OperationInstanceCode);
//	dataObj.insert("WorkStationCode", parm.WorkStationCode);
//	dataObj.insert("PWONo", parm.PWONo);
//	dataObj.insert("WIP_ID_Type_Code", parm.WIP_ID_Type_Code);
//	dataObj.insert("WIP_ID_Code", parm.WIP_ID_Type_Code);
//	
//	QJsonArray dataArrays;
//	for (int m = 0; m < parm.allCollectParm.size(); m++) {
//		QJsonObject oneData;
//		oneData.insert("ItemCode", parm.allCollectParm[m].ItemCode);
//		oneData.insert("ItemName", parm.allCollectParm[m].ItemName);
//		oneData.insert("SrcOperationInstanceCode", parm.allCollectParm[m].SrcOperationInstanceCode);
//		oneData.insert("ItemType", parm.allCollectParm[m].ItemType);
//		dataArrays.append(oneData);
//	}
//	dataObj.insert("Rows", dataArrays);
//	
//	if (m_currentId == std::this_thread::get_id()) {
//		return postData(dataObj);
//	}
//	else {
//		bool result;
//		QMetaObject::invokeMethod(this, "postData", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, result),
//			Q_ARG(QJsonObject, dataObj));
//		return result;
//	}
//}


bool FreetechMes::postData(QJsonObject dataJson, QString messageType)
{
    qDebug() << QThread::currentThreadId() << dataJson << "\n";
    ////只有与构造线程的id相同才能发送
    //if (m_currentId != std::this_thread::get_id())
    //	return false;
    //上锁一次发送一个请求
    //添加url(接口)
    QString postUrl = "http://" + m_urlIp + "/api/DCS/" + messageType;
    m_request.setUrl(QUrl(postUrl));
    m_messageType = messageType;
    QJsonDocument doc;
    doc.setObject(dataJson);
    QByteArray dataArray = doc.toJson();
    QString value = QString(dataArray);
    ShowLog(Log_Station, m_station, Log_Debug, QString::fromLocal8Bit("%1_PostJson:%2").arg(messageType).arg(value));
    //(Log_Info, QString::fromLocal8Bit("Json:%1").arg(value));
    //QNetworkReply* reply = m_accessManger.get(m_request);
    //connect(reply,&QNetworkReply::readyRead, this, [=]() {
    //	QByteArray datas = reply->readAll();
    //	QJsonParseError parseError;
    //	QJsonDocument parseDoc = QJsonDocument::fromJson(QString::fromUtf8(datas).toUtf8(), &parseError);
    //	QJsonObject datass = parseDoc.object();
    //	});
    m_accessManger.post(m_request, value.toUtf8());
    m_waitingFlag = true;
    //等待回传信息
    m_errorInfo = "";
    QTime time;
    time.start();
    while (m_waitingFlag)
    {
        if (time.elapsed() < m_timeout)
        {
            QApplication::processEvents();
        }
        else
        {

            m_errorInfo = QString::fromLocal8Bit("等待回复信息超时");
            return false;
        }
    }
    if (m_operationStatus)
        return true;
    else
        return false;
}
void FreetechMes::processReply(const QJsonObject dataObj)
{
    m_errorInfo = u8"解析失败";
    //获取token
    if (m_messageType == "GetToken") {
        m_token = dataObj.value("Access_Token").toString();
        m_tokenOutData.Access_Token = m_token;
        m_tokenOutData.ErrCode = dataObj.value("ErrCode").toInt();
        m_tokenOutData.ErrSeverity = dataObj.value("ErrSeverity").toInt();
        m_tokenOutData.ErrText = dataObj.value("ErrText").toString();
        m_operationStatus = true;
    }
    ////获取设备清单
    //else if (m_messageType  == "GetEquipmentList") {
    //	if (dataObj.value("ErrCode").toInt() != 0) {
    //		m_errorInfo = dataObj.value("ErrText").toString();
    //		return;
    //	}
    //	m_operationStatus = true;
    //	//不需要赋值？
    //	/*EquipmentList_Out outData;
    //	outData.equipmentList.clear();



    //	QJsonArray dataArray = dataObj.value("Rows").toArray();
    //	for (auto g : dataArray) {
    //		QJsonObject oneValue = g.toObject();
    //		EquipmentList_Out::EquipmentItem item;
    //		item.EquipmentCode = oneValue.value("EquipmentCode").toInt();
    //		item.lineCode = oneValue.value("LineCode").toString();
    //		outData.equipmentList.append(item);
    //	}
    //	m_equipmentList = outData;*/
    //}
    else if (m_messageType == "UploadMFGCtrlParameterResult") {
        UploadMFGCtrlParameterResult_Out outData;
        outData.ErrCode = dataObj.value("ErrCode").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toInt();
        m_UploadMFGCtrlParameterResultData = outData;
        m_operationStatus = true;
    }
    //获取胶水信息
    else if (m_messageType == "GetGlueCtrlInfo") {
        GetGlueCtrlInfo_OUT outData;
        outData.ErrCode = dataObj.value("ErrCode").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toInt();
        QJsonArray dataArray = dataObj.value("Data").toArray();
        for (auto g : dataArray) {
            QJsonObject oneValue = g.toObject();
            GetGlueCtrlInfo_OUT::GetGlueCtrlInfoRowData oneData;
            oneData.MaterialCode = oneValue.value("MaterialCode").toString();
            oneData.SKUID = oneValue.value("SKUID").toString();
            oneData.MoveOutTime = oneValue.value("MoveOutTime").toString();
            oneData.UpperExposeTimes = oneValue.value("UpperExposeTimes").toInt();
            oneData.ScrapTime = oneValue.value("ScrapTime").toString();
            outData.m_rowData.append(oneData);
        }
        m_GetGlueCtrlInfoData = outData;
        m_operationStatus = true;
    }
    //获取工单列表
    else if (m_messageType == "GetPWOList"|| m_messageType == "GetPWOPN") {
        PwoInfo_Out outData;
        outData.allProduct.clear();

        outData.Errcode = dataObj.value("ErrCode").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toInt();

        QJsonArray dataArray = dataObj.value("Rows").toArray();
        for (auto g : dataArray) {
            QJsonObject oneValue = g.toObject();
            PwoInfo_Out::PwoProduct item;
            item.PWOCode = oneValue.value("PWOCode").toString();
            item.ProductID = oneValue.value("ProductID").toInt();
            item.ProductNo = oneValue.value("ProductNo").toString();
            item.Qty = oneValue.value("Qty").toInt();
            item.AvailableQty = oneValue.value("AvailableQty").toInt();
            item.PWOPriority = oneValue.value("PWOPriority").toInt();
            item.ProductFamily = oneValue.value("ProductFamily").toString();
            item.ContainerBindingQty = oneValue.value("ContainerBindingQty").toInt();
            item.LotNumber = oneValue.value("LotNumber").toString();

            QString start = oneValue.value("PlannedStartTime").toString();
            QDateTime timeStart = QDateTime::fromString(start, "yyyy-MM-dd HH:mm:ss");
            item.PlannedStartTime = timeStart;


            QString end = oneValue.value("PlannedEndTime").toString();
            QDateTime timeEnd = QDateTime::fromString(end, "yyyy-MM-dd HH:mm:ss");
            item.PlannedEndTime = timeEnd;

            outData.allProduct.append(item);
        }
        m_PwoInfo = outData;
        m_operationStatus = true;
    }
    //下放管控参数
    else if (m_messageType == "GetMFGCtrlParameters") {
        GetMFGCtrl_OUT outData;
        outData.ErrCode = dataObj.value("ErrCode").toInt();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        QJsonArray dataArray = dataObj.value("Data").toArray();
        for (auto m : dataArray) {
            GetMFGCtrl_OUT::GetMFGCtrl_OUTRowData oneData;
            QJsonObject oneValue = m.toObject();

            oneData.ParameterName = oneValue.value("ParameterName").toString();
            oneData.ParameterValue = oneValue.value("ParameterValue").toString();
            oneData.Lowerlimit = oneValue.value("Lowerlimit").toString();
            oneData.Upperlimit = oneValue.value("Upperlimit").toString();
            oneData.Remark1 = oneValue.value("Remark1").toString();
            oneData.Remark2 = oneValue.value("Remark2").toString();
            oneData.Remark3 = oneValue.value("Remark3").toString();
            outData.rowData.append(oneData);
        }
        m_GetMFGCtrl_OUTData = outData;
        m_operationStatus = true;
    }
    //获取工艺矩阵
    //else if (m_messageType  == "GetProcessControlMatrix") {
    //不需要赋值？
    //ProcessMatrix_Out outData;
    //outData.allMatrix.clear();

    //outData.Errcode = dataObj.value("ErrCode").toInt();
    //outData.ErrText = dataObj.value("ErrText").toString();
    //outData.ErrSeverity = dataObj.value("ErrSeverity").toString();

    //QJsonArray dataArray = dataObj.value("Rows").toArray();
    //for (auto g : dataArray) {
    //	QJsonObject oneValue = g.toObject();
    //	ProcessMatrix_Out::oneMatrix item;
    //	item.LineCode = oneValue.value("LineCode").toString();
    //	//读取状态
    //	QJsonArray stateArray = oneValue.value("ControlMatrix").toArray();
    //	for (auto m : dataArray) {
    //		QJsonObject stateObj = m.toObject();
    //		ProcessMatrix_Out::curState oneState;
    //		oneState.OperationInstanceCode = stateObj.value("OperationInstanceCode").toString();
    //		oneState.B01 = stateObj.value("B01").toBool();
    //		oneState.B02 = stateObj.value("B02").toBool();
    //		oneState.B03 = stateObj.value("B03").toBool();
    //		oneState.B04 = stateObj.value("B04").toBool();
    //		oneState.B05 = stateObj.value("B05").toBool();
    //		oneState.B06 = stateObj.value("B06").toBool();
    //		oneState.B07 = stateObj.value("B07").toBool();
    //		oneState.B08 = stateObj.value("B08").toBool();
    //		oneState.B09 = stateObj.value("B09").toBool();
    //		oneState.B10 = stateObj.value("B10").toBool();
    //		oneState.B11 = stateObj.value("B11").toBool();
    //		oneState.B12 = stateObj.value("B12").toBool();
    //		oneState.B13 = stateObj.value("B13").toBool();
    //		oneState.B14 = stateObj.value("B14").toBool();
    //		oneState.B15 = stateObj.value("B15").toBool();
    //		oneState.B16 = stateObj.value("B16").toBool();
    //		oneState.B17 = stateObj.value("B17").toBool();
    //		oneState.B18 = stateObj.value("B18").toBool();
    //		oneState.B19 = stateObj.value("B19").toBool();
    //		oneState.B20 = stateObj.value("B20").toBool();
    //		oneState.B21 = stateObj.value("B21").toBool();
    //		oneState.B22 = stateObj.value("B22").toBool();
    //		oneState.B23 = stateObj.value("B23").toBool();
    //		oneState.B24 = stateObj.value("B24").toBool();
    //		oneState.B25 = stateObj.value("B25").toBool();
    //		oneState.B26 = stateObj.value("B26").toBool();
    //		oneState.B27 = stateObj.value("B27").toBool();
    //		oneState.B28 = stateObj.value("B28").toBool();
    //		oneState.B29 = stateObj.value("B29").toBool();
    //		oneState.B30 = stateObj.value("B30").toBool();
    //		oneState.B31 = stateObj.value("B31").toBool();
    //		oneState.B32 = stateObj.value("B32").toBool();
    //		oneState.B56 = stateObj.value("B56").toBool();
    //		item.state.append(oneState);
    //	}


    //	outData.allMatrix.append(item);

    //}
    //	//m_matrix = outData;
    //}
    //获取防错验证
    else if (m_messageType == "PokaYoke") {
        ErrorList_Out outData;
        outData.ObjectType = dataObj.value("ObjectType").toInt();
        outData.Errcode = dataObj.value("ErrCode").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toInt();
        outData.ContainerBindingQty = dataObj.value("ContainerBindingQty").toInt();
        outData.pValue = dataObj.value("P").toString().split("=").last();
        m_error = outData;
        m_operationStatus = true;
    }
    //获取DC_TestData数据
    else if (m_messageType == "DC_TestData" || m_messageType == "DC_Test") {
        DC_TestData_Out outData;
        outData.ErrCode = dataObj.value("ErrCode").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toInt();
        m_dcTestData = outData;
        m_operationStatus = true;
    }
    //获取DC_Inspect数据
    //else if (m_messageType  == "DC_Inspect")
    //{
    //	if (dataObj.value("ErrCode").toInt() != 0) {
    //		m_errorInfo = dataObj.value("ErrText").toString();
    //		return;
    //	}
    //不需要返回值？
    /*DC_Inspect_Out outData;
        outData.ExCode = dataObj.value("ObjectType").toString();
        outData.ErrCode = dataObj.value("ErrCode").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toString();
        m_dcInspectData = outData;*/
    //m_operationStatus = true;
    //}
    //获取ProductionEnd 数据
    else if (m_messageType == "ProductionEnd") {
        ProductionEnd_Out outData;
        outData.ExCode = dataObj.value("ObjectType").toString();
        outData.ErrCode = dataObj.value("ErrCode").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toInt();
        m_ProductionEndData = outData;
        m_operationStatus = true;
    }
    //SensorID和产品绑定
    else if (m_messageType == "IDBinding") {
        BindSensorID_Out outData;
        outData.ExCode = dataObj.value("ExCode").toString();
        outData.ErrCode = dataObj.value("ErrCode").toInt();
        outData.Part_Number_Feedback = dataObj.value("Part_Number_Feedback").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toInt();
        m_BindIdData = outData;
        m_operationStatus = true;
    }
    //制品和容器绑定
    else if (m_messageType == "WIPBindToContainer") {
        WIPBindToContainer_Out outData;
        outData.ErrCode = dataObj.value("ErrCode").toInt();
        outData.ErrText = dataObj.value("ErrText").toString();
        outData.ErrSeverity = dataObj.value("ErrSeverity").toInt();
        m_WipBingdData = outData;
        m_operationStatus = true;
    }
}

void FreetechMes::slotPostFinished(QNetworkReply* reply) {
    m_operationStatus = false;
    if (reply->error() != QNetworkReply::NoError)
    {
        QNetworkReply::NetworkError err = reply->error();
    }
    else
    {
        m_waitingFlag = false;
        QByteArray datas = reply->readAll();
        QJsonParseError parseError;
        ShowLog(Log_Station,m_station, Log_Debug, QString::fromLocal8Bit("ReplyJson:%1").arg(QString::fromUtf8(datas)));
        QJsonDocument parseDoc = QJsonDocument::fromJson(QString::fromUtf8(datas).toUtf8(), &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            m_errorInfo = "json err:" + QString::number(parseError.error);
        }
        else {

            //数据处理
            QJsonObject dataObj = parseDoc.object();

            processReply(dataObj);

        }
    }
}
void FreetechMes::initConnet()
{
    bool m = connect(&m_accessManger, &QNetworkAccessManager::finished,
                     instance, &FreetechMes::slotPostFinished);
    m_request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
}
