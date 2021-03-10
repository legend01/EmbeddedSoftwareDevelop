/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-01 11:10:35
 * @LastEditTime: 2021-03-10 16:21:45
 * @LastEditors: HLLI8
 */
#ifndef __BMS_BMS_H__
#define __BMS_BMS_H__

#include "BMS_J1939x.h"
#include "pub_data.h"
#include "ChargeInf.h"

#define _SEND_BRM_VEHICLEVIN(x) Send_BRM.VehicleVIN_M##x
#define SEND_BRM_VEHICLEVIN(x) _SEND_BRM_VEHICLEVIN(x)

#define messageData_MAXnum 60

#define SendDataLen_Max 50


typedef struct BMS_Manage
{
	char* 	messageData;      //接收的PGN数据缓存
    char    msgSendData[SendDataLen_Max];  //发送的数据缓存  
    
}sBMS_Manage,*psBMS_Manage;
extern sBMS_Manage BMSmanager;

extern RCV_CHM Rcv_CHM;
extern SEND_BHM Send_BHM;
extern RCV_CRM Rcv_CRM;
extern SEND_BEM Send_BEM;
extern SEND_BRM Send_BRM;
extern SEND_BCP Send_BCP;
extern RECV_CTS Rcv_CTS;
extern RECV_CML Rcv_CML;
extern SEND_BRO Send_BRO;
extern RECV_CRO Rcv_CRO;
extern SEND_BCL Send_BCL;
extern SEND_BCS Send_BCS;
extern RECV_CCS Rcv_CCS;

int BMS_Get_message(PGNTypeRcv ePGNTypeRcv, char** messageData);
void BMS_Send_message(PGNTypeSend ePGNTypeSend, char *data);
void BMSmanager_Init(void);

bool BMS_Check_Valid(PGNTypeRcv ePGNTypeRcv);

SEND_BHM* Get_Send_BHM_Inf(void);
SEND_BRM* Get_Send_BRM_Inf(void);
SEND_BCP* Get_Send_BCP_Inf(void);
SEND_BCL* Get_Send_BCL_Inf(void);
SEND_BCS* Get_Send_BCS_Inf(void);
#endif // !__BMS_BMS_H__
