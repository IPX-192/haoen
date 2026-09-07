#include "DirtyNode.h"
#include "VisAppBus.h"
#include "VisMotorManager.h"
#include "VisAppThreadPool.h"
#include "VisCameraTool.h"
#include "ParamManager.h"
#include "../../PipeLine/UdpServer.h"   // DataDetail 定义(上传MES结果用)

#include <QDir>
#include <QDateTime>
#include <QCoreApplication>
#include <thread>
#include <chrono>
#include <mutex>
#include <opencv2/imgcodecs.hpp>

static const int kDirtyGrabStableMs = 500;

static std::mutex g_dirtyCaptureMutex;

static void SaveDirtyCapture(const cv::Mat& img, int station)
{
    if (img.empty()) return;
    std::lock_guard<std::mutex> lock(g_dirtyCaptureMutex);   //串行化存图,消除并发竞争
    QString dir = QCoreApplication::applicationDirPath() + "/DirtyImage";
    QDir().mkpath(dir);
    QString name = QString("dirty_%1_s%2.png")
                       .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz"))
                       .arg(station + 1);
    QString path = dir + "/" + name;
    bool ok = cv::imwrite(path.toStdString(), img);
    if (ok) {
        ShowLog(Log_Fixture, station, Log_Info, QString(u8"脏污采图已保存:%1").arg(path));
    } else {
        ShowLog(Log_Fixture, station, Log_Error, QString(u8"脏污采图保存失败:%1").arg(path));
    }
    // 数量控制:最多保留 200 张。QDir::Time 排序旧在前,Reversed 后最新在前 → takeLast 删最旧。
    QDir d(dir);
    QStringList files = d.entryList(QStringList() << "dirty_*.png", QDir::Files, QDir::Time | QDir::Reversed);
    const int kMaxFiles = 200;
    while (files.size() > kMaxFiles) {
        if (!d.remove(files.takeLast())) break;   //删除失败(如被占用)立即退出,防死循环
    }
}

EAD_Handle DirtyNode::s_detector = nullptr;
bool       DirtyNode::s_initialized = false;
bool       DirtyNode::s_initAttempted = false;
Q_DECLARE_METATYPE(DataDetail)
Q_DECLARE_METATYPE(QList<DataDetail>)

DirtyNode::DirtyNode(ModuleInfo* item,int station)
{
    m_item = item;
    m_station = station;

    InitDetector();
}


int DirtyNode::InitDetector()
{	
    if (s_initialized) return 0;
    if (s_initAttempted) return -1;   //已尝试失败过,不再每个料重复重试(改配置后重启软件生效)
    RecipeDirty& cfg = GlobalParam->recipeDirty;
    if (cfg.modelPath.isEmpty() || cfg.modelPathMtr.isEmpty()) {
        ShowSystemLog(Log_Error, QString(u8"脏污模型路径未配置,跳过初始化"));
        s_initAttempted = true;
        return -1;
    }
    s_detector = EAD_Create();
    if (!s_detector) {
        ShowSystemLog(Log_Error, QString(u8"EAD_Create 失败"));
        s_initAttempted = true;
        return -1;
    }
    EAD_Config config;
    config.model_path      = cfg.modelPath.toLocal8Bit().constData();
    config.threshold       = static_cast<float>(cfg.threshold);
    config.mask_threshold  = static_cast<float>(cfg.maskThreshold);
    config.enable_openvino = cfg.enableOpenvino;
    config.intra_threads   = cfg.intraThreads;
    int ret = EAD_Init(s_detector, &config);
    if (ret != EAD_OK) {
        ShowSystemLog(Log_Error, QString(u8"EAD_Init 失败:%1").arg(ret));
        EAD_Destroy(s_detector);
        s_detector = nullptr;
        s_initAttempted = true;
        return -1;
    }
    std::string mtrPath = cfg.modelPathMtr.toStdString();
    ret = InitModel(mtrPath);
    if (ret != 0) {
        ShowSystemLog(Log_Error, QString(u8"InitModel(MTR) 失败:%1").arg(ret));
        EAD_Destroy(s_detector);
        s_detector = nullptr;
        s_initAttempted = true;
        return -1;
    }
    s_initialized = true;
    ShowSystemLog(Log_Info, QString(u8"脏污检测器初始化成功"));
    return 0;
}

void DirtyNode::ReleaseDetector()
{
    if (s_detector) { EAD_Destroy(s_detector); s_detector = nullptr; }
    s_initialized = false;
    s_initAttempted = false;   //重置后可重新尝试初始化
}

int DirtyNode::event_TestDetect(int station)
{
    if (!s_initialized && InitDetector() != 0)
    {
        VisAppBus::sendEvent("ShowResultImage", station, QString(u8"脏污模型加载失败"));
        return -1;
    }

    cv::Mat img;
    QImage imgShow;
    std::this_thread::sleep_for(std::chrono::milliseconds(kDirtyGrabStableMs));   //等机构稳定再拍照
    int nRes = VisCameraTool::instance()->GrabImgFrame(station, DirtyDetectCam, img, &imgShow);
    if (nRes != 0)
    {
        VisAppBus::sendEvent("ShowResultImage", station, QString(u8"脏污相机抓图失败:%1").arg(nRes));
        return -1;
    }

    cv::Mat processed;
    int ret = PreprocessImg(img, processed);
    if (ret != 0)
    {
        VisAppBus::sendEvent("ShowResultImage", station, QString(u8"预处理失败:%1").arg(ret));
        return -1;
    }

    cv::Mat binary_mask;
    EAD_Result result;
    ret = EAD_DetectMat(s_detector, processed, binary_mask, &result);
    if (ret != EAD_OK)
    {
        VisAppBus::sendEvent("ShowResultImage", station, QString(u8"检测失败:%1").arg(ret));
        return -1;
    }

    QString ngInfo = (result.is_ng == 0) ? "OK" : QString(u8"NG(%1)").arg(result.score);
    VisAppBus::sendEvent("ShowResultImage", station, ngInfo);

    if (result.is_ng != 0 && !binary_mask.empty())
    {
        /* ...mask叠加... */
    }
    else
    {
        VisAppBus::sendEvent("ShowDirtyImg", station, imgShow);
    }

    return 0;
}


int DirtyNode::Process()
{
    ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"开始脏污检测%1").arg(m_station + 1));
    cv::Mat img;
    QImage imgShow;
    std::this_thread::sleep_for(std::chrono::milliseconds(kDirtyGrabStableMs));   //转盘到位后等机构稳定再拍照,避免模糊
    int nRes = VisCameraTool::instance()->GrabImgFrame(m_station, DirtyDetectCam, img, &imgShow);
    if (nRes != 0) {
        QString errInfo = QString(u8"脏污相机抓图失败:%1").arg(nRes);
        ShowLog(Log_Fixture, m_station, Log_Error, errInfo);
        if (!GlobalParam->ng_2) {
            //空跑/离线:检测流程照做,仅结果置通过不NG
            m_item->result = true;
        }
        else {
            //脏污NG:抓图失败标记result=false
            m_item->result = false;
            m_item->ngReason = u8"脏污NG_1_3";   //后缀:固定_1 + PCB码_1
            m_item->errorMsg = errInfo;
        }
    }
    sigShowDirtyImg(m_station, imgShow);

    // 脏污检测(流程):预处理 + 模型检测,NG判定参照抓图失败逻辑(ng_2=true才标NG,空跑照做不NG)
    if (nRes == 0 && !img.empty()) {
        if (!s_initialized) {
            ShowLog(Log_Fixture, m_station, Log_Error, QString(u8"脏污检测器未初始化,跳过脏污检测"));
        }
        else {
            cv::Mat processed;
            int ret = PreprocessImg(img, processed);
            if (ret != 0) {
                QString err = QString(u8"脏污预处理失败:%1").arg(ret);
                ShowLog(Log_Fixture, m_station, Log_Error, err);
                if (GlobalParam->ng_2) {
                    m_item->result = false;
                    m_item->ngReason = u8"脏污NG_1_3";   //后缀:固定_1 + 壳体与PCB都有_3
                    m_item->errorMsg = err;
                }
            }
            else {
                cv::Mat binary_mask;
                EAD_Result result;
                ret = EAD_DetectMat(s_detector, processed, binary_mask, &result);
                if (ret != EAD_OK) {
                    QString err = QString(u8"脏污检测失败:%1").arg(ret);
                    ShowLog(Log_Fixture, m_station, Log_Error, err);
                    if (GlobalParam->ng_2) {
                        m_item->result = false;
                        m_item->ngReason = u8"脏污NG_1_3";   //后缀:固定_1 + 壳体与PCB都有_3
                        m_item->errorMsg = err;
                    }
                }
                else if (result.is_ng != 0) {
                    QString err = QString(u8"脏污NG(score=%1)").arg(result.score);
                    ShowLog(Log_Fixture, m_station, Log_Error, err);
                    VisAppBus::sendEvent("ShowResultImage", m_station, err);
                    if (GlobalParam->ng_2) {
                        m_item->result = false;
                        m_item->ngReason = u8"脏污NG_1_3";   //后缀:固定_1 + 壳体与PCB都有_3
                        m_item->errorMsg = err;
                    }
                }
                else {
                    ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"脏污OK(score=%1)").arg(result.score));
                    VisAppBus::sendEvent("ShowResultImage", m_station, QString(u8"脏污OK(%1)").arg(result.score));
                }
            }
        }
    }

    // 采图给算法建模
    // 每流程只存前2个治具(station 0/1),减少IO与卡顿风险
    if (nRes == 0 && !img.empty() && GlobalParam->recipeDirty.captureEnable
        && !GlobalParam->systemParam.shieldParam.dirtyCapture && m_station < 2) {
        SaveDirtyCapture(img, m_station);
    }

    // 脏污检测完成:上传脏污测量结果和当前治具通道号到MES
    DataDetail dirtyVal;
    dirtyVal.name = u8"脏污测量";
    dirtyVal.message = "Dirty";
    dirtyVal.result = "1";
    dirtyVal.value = "0.2";   // 脏污值先填假数据

    DataDetail dirtyCha;
    dirtyCha.name = u8"治具通道";
    dirtyCha.message = "Channel";
    dirtyCha.result = "1";
    dirtyCha.value = QString::number(m_station);   // 真实治具通道号

    QList<DataDetail> listTest;
    listTest.append(dirtyVal);
    listTest.append(dirtyCha);

    QString src;
    bool mesOk = true;   //MES上传成败由输出参数判定(插件内按errMsg设置),不依赖总线返回值
    VisAppBus::sendEvent("MesSaveProcessOpResult", m_item->pcbCode, 1, listTest, src, mesOk);
    if (!mesOk) {
        //MES保存工序结果失败:标记NG
        ShowLog(Log_Fixture, m_station, Log_Error, QString(u8"脏污MES上传失败,标记NG"));
        if (GlobalParam->ng_2) {
            m_item->result = false;
            m_item->ngReason = u8"脏污MES上传NG_1_3";   //后缀:固定_1 + 壳体与PCB都有_3
            m_item->errorMsg = QString(u8"脏污MES上传失败");
        }
    }

    ShowLog(Log_Fixture, m_station, Log_Info, QString(u8"脏污检测结束%1").arg(m_station + 1));
    return 0;
}

void DirtyNode::sigShowDirtyImg(int station, QImage imgShow)
{
    VisAppBus::sendEvent("ShowDirtyImg", station, imgShow);
}
