#ifndef __J1939X_h__
#define __J1939X_h__

#include "Ringbuff_manger.h"

#define BMS_Addr            244
#define Charger_Addr        86

#define  J1939_CYCLE_TIME   5  //ѭ������ 25 ms

typedef enum tranState {Idle=0,    connected,}tranState;
typedef enum tranError {noFail=0, connectFail, transmitFail, timeOut}tranError;
typedef enum connectType{singleFrame=0, multiFrame}connectType;
typedef enum tranDirection{receive=0, send}tranDirection;

typedef enum PGNTypeSend          //��
{
    CRM = 0, //������ʶ����  ��
    CHM, //�������ֱ���
    CTS, //��������ʱ��ͬ����Ϣ
    CML, //��������ʱ��ͬ����Ϣ
    CRO, //�������׼������״̬
    CCS, //�������״̬
    CST, //������ֹ���
    CSD, //����ͳ������
    CEM, //�����������

    TPCM_send,//������ݹ���
    TPDT_send,//������ݴ���
    PGN_MAX_Send,
}PGNTypeSend;

typedef enum PGNTypeRcv          //��
{
    BRM = 0, //BMS�ͳ�����ʶ����     �� 
    BHM,    
    BCP, //�������س�����
    BRO, //��س��׼������״̬
    BCL, //��س������
    BCS, //��س����״̬
    BSM, //��������״̬��Ϣ
    BMV, //���嶯�����ص�ѹ
    BMT, //���������¶�
    BSP, //��������Ԥ������
    BST, //BMS��ֹ���
    BSD, //BMSͳ������  
    BEM, //BMS������
    
    TPCM_rcv,//������ݹ���
    TPDT_rcv,//������ݴ���
    PGN_MAX_Rcv,//Ԥ�����ֵ
}PGNTypeRcv;


#define TP_CM_PF 			0xec00  	// ���ӹ���PF
#define TP_CM_RTS 			16	 	//������
#define TP_CM_CTS 			17		//׼������	
#define TP_CM_EndofMsgAck 	19		//��Ϣ����Ӧ��
#define TP_CM_Abort			255		//��������
#define TP_CM_BAM			32		//�㲥������Ϣ

#define TP_DT_PF 			0xeb00  	// ����ģʽ�µ����ݴ���

#define timeout_T1          75      //750MS
#define timeout_T2          125     //1250MS

#define PGN_MessageRcvBuff_Max 200  //PGN  

typedef struct PGNInfo
{
    unsigned int    PGNnum;
    unsigned int    priority;   // ���ȼ�
    unsigned int    dataLen;    // ���ݳ���
    unsigned int    period; //��������ms
}sPGNInfo,*psPGNInfo;


typedef struct sJ1939_multiTransfeManger
{
     unsigned int   data_num;                   //�������ݳ���
     unsigned char  num_packet;                 //���İ���
     unsigned char  cur_packet;                 //��ǰ����ı���
     unsigned int   destAddr;                   //Ŀ���ַ
     unsigned int   sourceAddr;                 //Դ��ַ
     unsigned int   PGNnum;                     //�����ӹ������PGN
     unsigned char  PGNindex;                   //PGN��ö��ֵ
     tranState      connectState;               //����״̬
     unsigned char data_position;               // ������BUFʲôλ��
}sJ1939_transfeManger,*psJ1939_transfeManger;

typedef struct J1939_message
{
    unsigned int    priority;       // ���ȼ�
    unsigned int    PGNnum;         //PGN��ʵֵ
    unsigned char   sourceAddr;     // Դ��ַ
    unsigned char   destAddr;       // Դ��ַ
    unsigned char   dataLen;        // ���ݳ���
    unsigned char   data[8];        // ��������
    
    unsigned char   Reserved;       //����λ
    
}J1939_message,*pJ1939_message;


typedef struct J1939_message_Rcv  //J1939���յ�������
{
    unsigned char   PDUformat;      // PDU��ʽPF
    unsigned char   PDUspecific;    // PS
    unsigned char   dataLen;        // ���ݳ���
    char*       data;           // ����ָ��
    unsigned char   valid;          // ������Ч
    
}J1939_message_Rcv,*pJ1939_message_Rcv;
extern J1939_message_Rcv PGN_MessageRcv[PGN_MAX_Rcv]; 

typedef struct J1939mg_interrupt  //J1939���ж��ж�ȡ��ԭʼ����
{
    unsigned int    PGNnum;
    unsigned char   data[8];
    unsigned char   valid;          // ������Ч
}J1939mg_interrupt,*pJ1939mg_interrupt;

extern sPGNInfo PGNInfoSend[PGN_MAX_Send+1];
extern sPGNInfo PGNInfoRcv[PGN_MAX_Rcv+1];



int PGN_MessageRcv_Init(void);
int J1939_SendOnePacket( J1939_message *msg);
int GetmsgconvertToSend(void);
int GtmgFrRcvbufToPGN(void);
char MultiTrans_Manage_SEND(psJ1939_transfeManger pJ1939_connect_arrys, unsigned char CMMD);
void J1939_connect_clear(void );
int GetmessageToRcvbuff(void* rcv);
void J1939_register(void);
char PGN_MessageRcv_clear(void);

#endif














