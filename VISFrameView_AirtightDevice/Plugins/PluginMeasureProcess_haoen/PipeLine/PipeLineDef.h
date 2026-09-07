#ifndef PIPELINEDEF_H
#define PIPELINEDEF_H

#include <QObject>

#define  TcpDevPort  8385
#define  UdpDevPort  7385

enum PipeLineStatus{
    PipeLine_Load,       //入料口到测试位
    PipeLine_Blank,      //测试位下料到出料口
    PipeLine_CurToNext,  //当前站到下一站
};


//Client:本站  Server:上游设备
enum PipeLineCommandType {
    RequestTray,      //请求托盘       本站->上游  
    PreExistTray,     //上站有盘       上游->本站
    TrayArrive,       //托盘到达       本站->上游
};

struct TcpCommand {
    uint16_t  head = 0x0050;				//报文头部
	uint16_t  nType;                //报文类型,PipeLineCommandType
    uint16_t  nLenth = 0;
};


#endif // PIPELINEDEF_H
