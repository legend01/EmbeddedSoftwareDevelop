/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-01 11:10:35
 * @LastEditTime: 2021-03-02 14:22:55
 * @LastEditors: HLLI8
 */
#ifndef __BMS_BMS_H__
#define __BMS_BMS_H__

#include "BMS_J1939x.h"
#include "pub_data.h"

#define messageData_MAXnum 60


typedef enum BMS_stage           //发
{
	Handshake_rcvCRM_norec = 0, //未识别                 
	Handshake_sendBRM,                 //               
	Handshake_rcvCRM_rec, //识别                         
    Handshake_rcvCHM,                                     
	Handshake_sendBHM,
    
	ParConfig_sendBCP,
	ParConfig_rcvCTS_CML,
	ParConfig_sendBRO,
	ParConfig_rcvCRO,

	charging_sendBCS,
	charging_sendBCL,
	charging_rcvCCS,
	charging_sendBSM,
	charging_sendBMV,
	charging_sendBMT,
	charging_sendBSP,
	
	chargend_rcvCST,
	chargend_sendBST,
	chargend_sendBSD,
	chargend_rcvCSD,
}BMS_stage;

typedef enum BMS_flag         //充电阶段标志
{
	BMS_ready 	= 0, 	//充电前的准备	
	BMS_Handshake,		//充电握手
	BMS_Config,			//配置
	BMS_Charging,		//充电
	BMS_END,			//结束
	BMS_Finish,			//关闭
	BMS_ErrorTimeout,	//错误或者超时状态
	
	BMS_StateMax,       
}BMS_flag;

typedef enum
{
    BMS_NOREC = 0,
    BMS_2011,
    BMS_2015,
}PROTOL;

#define SendDataLen_Max 50
typedef struct BMS_Manage
{
	u16 	BMS_needV;        //BMS实时要求充电机的电压
	u16 	BMS_needI;        //BMS实时要求充电机的电流 
	u8      BMS_needM;        //BMS要求充电机的充电模式
	u8      Stop_Mod:2;       //充电连接停止类型 0正常通信  1被动停止  2为主动停止  
    u8      BMS_relink:3;     // BMS超时重新连接次数
	u16 	BMSTime_rcv;      //定时器计时ms
	u16     MsgSend_cycle;    //一个信息每隔多少MS发送一次
	u16     MsgSend_waitTime; //定时器计数等待时间MS
	u16     charg_Curmeasure; //BMS测量电流值
	u16	charg_Volmeasure; //BMS测量电压值
        u8      charg_NowSOC;     //当前SOC实际值
        u16     BAT_TMleft;       //估算剩余充电时间
        u8      BMS_readyStat;    //准备是否就绪
        u8      ChargReady_YESNO; //充电桩准备是否就绪
        u8      BMS_Stop;     //  0:正常运行 1:BMS主动停止
	u8      MsgSend_Count;    
	char* 	messageData;      //接收的PGN数据缓存
    char    msgSendData[SendDataLen_Max];  //发送的数据缓存  
    bool        BMS_STOP;
    
	PROTOL	BMS_V;		// 0为2015年薪国标 1 为2011年老国标
    u16     BMS_PREV;
	BMS_flag  Chargeworkflg;
	BMS_stage Charge_sata ;    //充电过程中 充电状态
}sBMS_Manage,*psBMS_Manage;
extern sBMS_Manage BMSmanager;

int BMS_Get_message(PGNTypeRcv ePGNTypeRcv, char** messageData);
void BMS_Send_message(PGNTypeSend ePGNTypeSend, char *data);
void BMSmanager_Init(void);

bool BMS_Check_Valid(PGNTypeRcv ePGNTypeRcv);
#endif // !__BMS_BMS_H__
