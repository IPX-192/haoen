#pragma once
#include <QObject>
#include <QSerialPort>
#include <QMutex>
#include <QStringList>
#include <vector>
#include <thread>

struct DisplacementPortSettings
{
    QString PortName = "";
    int BaudRate = 9600;
    int DataBits = 8;
    int Parity = 0;    // 0无校验
    int StopBits = 1;
    int TimeoutMs = 3000;
};

class DisplacementSensorClient : public QObject
{
    Q_OBJECT
public:
    explicit DisplacementSensorClient(QObject *parent = nullptr);
    ~DisplacementSensorClient() override;

    void SetName(QString name);
    QStringList ScanAllSerialPort();
    int Connect(const DisplacementPortSettings& settings);
    int DisconnectPort();
    bool IsPortOpened();

    int ReadSensorHeight(const QString& sensorId, double& outHeight);
    int ResetSensorZero();
    int GetLatestRawHeight(double& outVal);

signals:

private slots:
    bool slotSendAsciiCmd(const QString& cmd);
    void slotReadSerialData();
    void slotSerialPortError(QSerialPort::SerialPortError err);

private:
    static constexpr int MAX_CURVE_POINTS = 200;

    QSerialPort* m_serial = nullptr;
    QString m_name;
    mutable QMutex m_opMutex;

    std::thread::id m_initThreadID;

    QByteArray m_recvBuffer;
    bool m_replyFlag = false;

    double m_currentHeight = 0.0;
    std::vector<double> m_curveHistory;

    QString m_lastErrMsg;

    QString m_expectSensorId; // 期待应答的传感器ID

    bool WaitReply(int timeoutMs);
};
