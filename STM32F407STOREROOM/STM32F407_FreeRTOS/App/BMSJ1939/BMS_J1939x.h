#ifndef __BMS_J1939X_h__
#define __BMS_J1939X_h__

#include "Ringbuff_manger.h"

#define BMS_Addr            244
#define Charger_Addr        86

#define J1939_Connectsed_Max  50 //多包发送的数据最大长度

typedef enum tranState {Idle=0,    connected,}tranState;
typedef enum tranError {noFail=0, connectFail, transmitFail, timeOut}tranError;
typedef enum connectType{singleFrame=0, multiFrame}connectType;
typedef enum tranDirection{receive=0, send}tranDirection;

typedef enum PGNTypeRcv          //收
{
    CRM = 0, //充电机辨识报文  收
    CHM, //充电机握手报文
    CTS, //充电机发送时间同步信息
    CML, //充电机发送时间同步信息
    CRO, //充电机输出准备就绪状态
    CCS, //充电机充电状态
    CST, //充电机中止充电
    CSD, //充电机统计数据
    CEM, //充电器错误报文

    TPCM_rcv,//多包数据管理
	TPDT_rcv,//多包数据传输
	PGN_MAX_Rcv,//预留最大值
}PGNTypeSend;

typedef enum PGNTypeSend          //发
{
    BRM = 0, //BMS和车辆辨识报文     发
    BHM,    
    BCP, //动力蓄电池充电参数
    BRO, //电池充电准备就绪状态
    BCL, //电池充电需求
    BCS, //电池充电总状态
    BSM, //动力蓄电池状态信息
    BMV, //单体动力蓄电池电压
    BMT, //动力蓄电池温度
    BSP, //动力蓄电池预留报文
    BST, //BMS中止充电
    BSD, //BMS统计数据  
    BEM, //BMS错误报文
    
    TPCM_send,//多包数据管理
	TPDT_send,//多包数据传输
	PGN_MAX_Send,
}PGNTypeRcv;

#define TP_CM_PF 			0xec00  	// 连接管理PF
#define TP_CM_RTS 			16	 	//请求发送
#define TP_CM_CTS 			0x11		//准备发送	
#define TP_CM_EndofMsgAck 	0x13		//消息结束应答
#define TP_CM_Abort			0xFF		//放弃连接
#define TP_CM_BAM			32		//广播公告消息

#define TP_DT_PF 			0xeb00  	// 连接模式下的数据传输

#define timeout_T1          75      //750MS
#define timeout_T2          125     //1250MS

#define PGN_MessageRcvBuff_Max 200  //PGN  

typedef struct PGNInfo
{
    unsigned int    PGNnum;
    unsigned int    priority;   // 优先级
    unsigned int    dataLen;    // 数据长度
    unsigned int    period; //报文周期ms
}sPGNInfo,*psPGNInfo;


typedef struct sJ1939_multiTransfeManger
{
     unsigned int   data_num;                   //报文数据长度
     unsigned char  num_packet;                 //报文包数
     unsigned char  cur_packet;                 //当前传输的报数
     unsigned int   destAddr;                   //目标地址
     unsigned int   sourceAddr;                 //源地址
     unsigned int   PGNnum;                     //此连接管理传输的PGN
     unsigned char  PGNindex;                   //PGN的枚举值
     tranState      connectState;               //连接状态
     unsigned char data_position;               // 拷贝到BUF什么位置
}sJ1939_transfeManger,*psJ1939_transfeManger;

typedef struct J1939_message
{
    unsigned int    priority;       // 优先级
    unsigned int    PGNnum;         //PGN真实值
    unsigned char   sourceAddr;     // 源地址
    unsigned char   destAddr;       // 目的地址
    unsigned char   dataLen;        // 数据长度
    unsigned char   data[8];        // 数据数组
    
    unsigned char   Reserved;       //保留位
    
}J1939_message,*pJ1939_message;


typedef struct J1939_message_Rcv  //J1939接收到的数据
{
    unsigned int   PDUformat;      // PDU格式PF
    unsigned char   PDUspecific;    // PS
    unsigned char   dataLen;        // 数据长度
    char*       data;           // 数据指针
    unsigned char   valid;          // 数据无效
    
}J1939_message_Rcv,*pJ1939_message_Rcv;
extern J1939_message_Rcv PGN_MessageRcv[PGN_MAX_Rcv]; 

typedef struct J1939mg_interrupt  //J1939在中断中读取的原始数据
{
    unsigned int    PGNnum;
    unsigned char   data[8];
    unsigned char   valid;          // 数据无效
}J1939mg_interrupt,*pJ1939mg_interrupt;

extern sPGNInfo PGNInfoSend[PGN_MAX_Send+1];
extern sPGNInfo PGNInfoRcv[PGN_MAX_Rcv+1];



int PGN_MessageRcv_Init(void);
int J1939_SendOnePacket( J1939_message *msg);
int GetmsgconvertToSend(void);
int GtmgFrRcvbufToPGN(void);
// uint8_t MultiTrans_Manage_SEND(psJ1939_transfeManger pJ1939_connect_arrys, unsigned char CMMD);
void J1939_connect_clear(void );
int GetmessageToRcvbuff(void* rcv);
void J1939_register(void);
uint8_t PGN_MessageRcv_clear(void);

void J1939_connect_send_clear(void);
void PushMsgToRBUFF(PGNTypeSend, char*);

#endif














