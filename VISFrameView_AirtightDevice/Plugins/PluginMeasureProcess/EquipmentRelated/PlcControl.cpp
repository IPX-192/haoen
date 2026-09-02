#include "PlcControl.h"
#include "VisMotorManager.h"
#include "VisMotorToolData.h"

PlcControl& PlcControl::Instance()
{
    static PlcControl instance;
    return instance;
}

PlcControl::PlcControl()
{
}

PlcControl::~PlcControl()
{
}

void PlcControl::SetHomeStatus(bool status)
{
    plcIsHomed=status;
}

bool PlcControl::GetIsHomed()
{
    return plcIsHomed;
}

bool PlcControl::WritePLCReset()
{
    QString strRegName = "M3000";
    if (false == WriteMlInterval(strRegName, 500))
    {
        return false;
    }
    return true;
}

bool PlcControl::WritePLCStart()
{
    QString strRegName = "M532";
    if (false == WriteMlInterval(strRegName, 500))
    {
        return false;
    }
    return true;
}

bool PlcControl::WritePLCStop()
{
    QString strRegName = "M534";
    if (false == WriteMlInterval(strRegName, 500))
    {
        return false;
    }
    return true;
}

bool PlcControl::WritePLCEmgStop()
{
    QString strRegName = "M545";
    if (false == WriteMlInterval(strRegName, 500))
    {
        return false;
    }
    return true;
}

bool PlcControl::WritePLCHandeMode(bool autoMode)
{
    return true;
}

bool PlcControl::ClearPLCWarning()
{
    QString strRegName = "M529";
    if (false == WriteMlInterval(strRegName, 500))
    {
        return false;
    }
    return true;
}

bool PlcControl::WriteMlInterval(QString strRegName, int nInterval)
{
    if (false == WriteML(strRegName, true))
    {
        return false;
    }

    QThread::msleep(nInterval);

    if (false == WriteML(strRegName, false))
    {
        return false;
    }
    return true;
}

bool PlcControl::ReadAutoMode(int& status)
{
    QString strRegName = "M315";
    bool bStatus = false;
    if (false == ReadML(strRegName, bStatus))
    {
        return false;
    }

    if (bStatus)
    {
        status = 1;
    }
    else
    {
        status = 0;
    }

    return true;
}

bool PlcControl::ReadResetStatus()
{
    QString strRegName = "M581";
    bool bStatus = false;
    if (false == ReadML(strRegName, bStatus))
    {
        return false;
    }
    return bStatus;
}

bool PlcControl::ReadAlarmStatus(int& status)
{
    QString strRegName = "M301";
    bool bStatus = false;
    if (false == ReadML(strRegName, bStatus))
    {
        status = 1;
        return false;
    }

    if (bStatus)
    {
        status = 1;
    }
    else
    {
        status = 0;
    }

    return true;
}

bool PlcControl::SetAlarm(bool on)
{
    QString reg =  QString("D40");
    if (false == WriteD(reg, on?1:0))
    {
        return false;
    }
    return true;
}


bool PlcControl::ReadD(QString& regName, qint32& data)
{
    if (VisMotorToolSpace::VisMotorInstance->IsConnected())
    {
        return VisMotorToolSpace::VisMotorInstance->ReadD(regName, data);
    }
    return false;
}

bool PlcControl::WriteD(QString& regName, qint32 data)
{
    return VisMotorToolSpace::VisMotorInstance->WriteD(regName, data);
}

bool PlcControl::WritePageD(QString& regName, int length, QVector<qint32>& data)
{
    return VisMotorToolSpace::VisMotorInstance->WriteRangeD(regName, length, data);
}

bool PlcControl::ReadPageD(QString& regName, int length, QVector<qint32>& data)
{
    return VisMotorToolSpace::VisMotorInstance->RealReadRangeD(regName, length, data);
}

bool PlcControl::ReadPageML(QString &regName, int length, QVector<bool> &data)
{
    return VisMotorToolSpace::VisMotorInstance->RealReadRangeML(regName, length, data);
}

bool PlcControl::ReadML(QString& regName, bool& state)
{
    if (VisMotorToolSpace::VisMotorInstance->IsConnected())
    {
        return VisMotorToolSpace::VisMotorInstance->ReadML(regName, state);
    }
    return false;
}

bool PlcControl::WriteML(QString& regName, bool state)
{
    if (VisMotorToolSpace::VisMotorInstance->IsConnected())
    {
        for (int i = 0; i != 10; i++)
        {
            bool bRet = VisMotorToolSpace::VisMotorInstance->WriteML(regName, state);
            if (bRet)
            {
                return true;
            }
            QThread::msleep(100);
        }
        return true;
    }
    return false;
}

double PlcControl::ConvertTiUnit(double dValue, QString axisName)
{
    return dValue*VisMotorToolSpace::VisMotorDataInstance->m_plcMotorMap[axisName].equiv;
}





