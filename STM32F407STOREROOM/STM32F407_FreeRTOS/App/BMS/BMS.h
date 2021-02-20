#ifndef __BMS_H__
#define __BMS_H__

#include "J1939x.h"

#define messageData_MAXnum 60

typedef enum Charge_flag         //充电阶段标志
{
    Charge_ready    = 0,    //充电前的准备  
    Charge_Handshake,       //充电握手
    Charge_Config,          //配置
    Charge_Charging,        //充电
    Charge_End,             //结束
    Charge_Finish,          //关闭
    Charge_ErrorTimeout,    //错误或者超时状态
    Charge_StateMax,       
}Charge_flag;

typedef enum Charge_stage           //发
{
    Handshake_sendCRM_norec = 0, //未识别
    Handshake_rcvBRM,
    Handshake_sendCRM_rec, //识别
    Handshake_sendCHM,
    Handshake_rcvBHM,

    ParConfig_rcvBCP,
    ParConfig_sendCTS,
    ParConfig_sendCML,
    ParConfig_rcvBRO,
    ParConfig_sendCRO,

    charging_rcvBCS,
    charging_rcvBCL,
    charging_sendCCS,
    charging_rcvBSM,
    charging_rcvBMV,
    charging_rcvBMT,
    charging_rcvBSP,
    
    chargend_sendCST,
    chargend_rcvBST,
    chargend_rcvBSD,
    chargend_sendCSD,
}Charge_stage;

typedef enum
{
    BMS_NOREC = 0,
    BMS_2011,
    BMS_2015,
}PROTOL;

typedef struct BMS_Manage
{
    uint16_t     BMS_needV;        //BMS实时要求充电机的电压
    uint16_t     BMS_needI;        //BMS实时要求充电机的电流 
    uint8_t  BMS_needM;      //BMS要求充电机的充电模式 1恒压 2横流

    uint16_t     CH_setV;         //CH 设置给BMS的电压
    uint16_t     CH_setI;         //CH 设置给BMS的电流
    uint8_t      Stop_Mod:2;       //充电连接停止类型 0正常通信  1被动停止  2为主动停止  
    uint8_t      BMS_relink:3;     // BMS超时重新连接次数
    uint16_t     BMSTime_rcv;      //定时器计时ms
    uint16_t     MsgSend_cycle;    //一个信息每隔多少MS发送一次
    uint16_t     MsgSend_waitTime; //定时器计数等待时间MS
    uint16_t     MsgSend_Count;    //发送的次数
    char*   messageData;      //接收的PGN数据缓存
    bool        BMS_STOP;
    PGNTypeRcv  rcvTimout_PGN;  //接收超时的PGN
    PGNTypeSend sedTimout_PGN;
    PROTOL  BMS_V;      // 0为2015年薪国标 1 为2011年老国标
    uint16_t     BMS_PREV;

    Charge_flag  Chargeworkflg;
    Charge_stage Charge_sata ;    //充电过程中 充电状态
    uint32_t StopCode;
	bool	VinCodValid;
    bool     Tout_finish;
}sBMS_Manage,*psBMS_Manage;
extern  sBMS_Manage BMSmanager;

void BMS_Send_message(PGNTypeSend ePGNTypeSend, char *data, int dataLen);
int BMS_Get_message(PGNTypeRcv ePGNTypeRcv, char** messageData);
#endif // !__BMS_H__
