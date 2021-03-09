/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-01 11:10:29
 * @LastEditTime: 2021-03-09 15:47:25
 * @LastEditors: HLLI8
 */
#include "BMS_BMS.h"



sBMS_Manage BMSmanager;
RCV_CHM Rcv_CHM;
SEND_BHM Send_BHM;
RCV_CRM Rcv_CRM;
SEND_BEM Send_BEM;
SEND_BRM Send_BRM;

SEND_BHM* Get_Send_BHM_Inf(void){
    Send_BHM.vehicleAllowMaxV_L = Get_Vehicle_ParamInf()->vehicle_maxallow_v & 0xff;
    Send_BHM.vehicleAllowMaxV_H = Get_Vehicle_ParamInf()->vehicle_maxallow_v >> 8 & 0xff;
    return &Send_BHM;
}

SEND_BRM* Get_Send_BRM_Inf(void){
    Send_BRM.BMSComProtocolVersion_L = Get_Vehicle_ParamInf()->BMSComProtocolVersion & 0xff;
    Send_BRM.BMSComProtocolVersion_M = Get_Vehicle_ParamInf()->BMSComProtocolVersion >> 8 & 0xff;
    Send_BRM.BMSComProtocolVersion_H = Get_Vehicle_ParamInf()->BMSComProtocolVersion >> 16 & 0xff;

    Send_BRM.BatteryType = Get_Vehicle_ParamInf()->BatteryType & 0xff;

    Send_BRM.VehicleBatterySysRatedCapacity_L = Get_Vehicle_ParamInf()->VehicleBatterySysRatedCapacity & 0xff;
    Send_BRM.VehicleBatterySysRatedCapacity_H = Get_Vehicle_ParamInf()->VehicleBatterySysRatedCapacity >> 8 & 0xff;

    Send_BRM.VehicleBatterySysRatedVoltage_L = Get_Vehicle_ParamInf()->VehicleBatterySysRatedVoltage & 0xff;
    Send_BRM.VehicleBatterySysRatedVoltage_H = Get_Vehicle_ParamInf()->VehicleBatterySysRatedVoltage >> 8 & 0xff; 

    Send_BRM.BatteryManufacturers_L = Get_Vehicle_ParamInf()->BatteryManufacturers[0];
    Send_BRM.BatteryManufacturers_M1 = Get_Vehicle_ParamInf()->BatteryManufacturers[1];
    Send_BRM.BatteryManufacturers_M2 = Get_Vehicle_ParamInf()->BatteryManufacturers[2];
    Send_BRM.BatteryManufacturers_H = Get_Vehicle_ParamInf()->BatteryManufacturers[3];
    
    Send_BRM.BatterySerialNum_L = Get_Vehicle_ParamInf()->BatterySerialNum & 0xff;
    Send_BRM.BatterySerialNum_M1 = Get_Vehicle_ParamInf()->BatterySerialNum >> 8 & 0xff;
    Send_BRM.BatterySerialNum_M2 = Get_Vehicle_ParamInf()->BatterySerialNum >> 16 & 0xff;
    Send_BRM.BatterySerialNum_H = Get_Vehicle_ParamInf()->BatterySerialNum >> 24 & 0xff;

    Send_BRM.BatteryManufactureData_Year = Get_Vehicle_ParamInf()->BatteryManufactureData[0];
    Send_BRM.BatteryManufactureData_Month = Get_Vehicle_ParamInf()->BatteryManufactureData[1];
    Send_BRM.BatteryManufactureData_Day = Get_Vehicle_ParamInf()->BatteryManufactureData[2];

    Send_BRM.ChargeTime_L = Get_Vehicle_ParamInf()->ChargeTime & 0xff;
    Send_BRM.ChargeTime_M = Get_Vehicle_ParamInf()->ChargeTime >> 8 & 0xff;
    Send_BRM.ChargeTime_H = Get_Vehicle_ParamInf()->ChargeTime >> 16 & 0xff;

    Send_BRM.BatteryPropertyIden = Get_Vehicle_ParamInf()->BatteryPropertyIden & 0xff;

    for (u8 i = 0; i < 16; i++)
    {
        Send_BRM.VehicleVIN[i] = Get_Vehicle_ParamInf()->VehicleVIN[i];
    }
    
    for (u8 j = 0; j < 8; j++)
    {
        Send_BRM.BMSSoftVersion[j] = Get_Vehicle_ParamInf()->BMSSoftVersion[j];
    }
    return &Send_BRM;
}

void BMSmanager_Init(void){
    Ringbuff_setEmpty();
    J1939_connect_send_clear();
    J1939_connect_clear();
    PGN_MessageRcv_Init();
    PGN_MessageRcv_clear();
    memset(&BMSmanager.msgSendData, 0, sizeof(BMSmanager.msgSendData));
    memset(&Rcv_CHM, 0, sizeof(Rcv_CHM));
    memset(&Send_BHM, 0, sizeof(Send_BHM));
    memset(&Rcv_CRM, 0, sizeof(Rcv_CRM));
    memset(&Send_BEM, 0, sizeof(Send_BEM));
}

void BMS_Send_message(PGNTypeSend ePGNTypeSend, char *data){
    if(data != NULL)
    {
        for (int i = 0; i < PGNInfoSend[ePGNTypeSend].dataLen; i++)
        {
            BMSmanager.msgSendData[i] = data[i] & 0xff;
        }
        PushMsgToRBUFF(ePGNTypeSend, BMSmanager.msgSendData);
    }
    
}
/***************************************************************************
功能: 通过PGN的索引号读取得到的实际值PGN的数据部分
参数: 1 PGN 索引    2 读取到的数据指针
返回: -1为错误 1 为数据无效 messageData_len 为数据正常更新并且被读取
*****************************************************************************/
int BMS_Get_message(PGNTypeRcv ePGNTypeRcv, char** messageData)
{
    uint8_t messageData_len = 0;

    if(ePGNTypeRcv >= TPCM_rcv)//判断PGN范围
    {
        return -1;
    }

    messageData_len = PGNInfoRcv[ePGNTypeRcv].dataLen;
    
    if(messageData_len > messageData_MAXnum)        //判断PGN的最大长度是否大于最大数据
    {
        return -1;
    }
    
    if(PGN_MessageRcv[ePGNTypeRcv].valid == 0) //所查看的PGN的数据的有效性,如果无效则直接返回0
    {
        return 0;
    }
    
    *messageData = PGN_MessageRcv[ePGNTypeRcv].data;  //直接赋值指针
    
    PGN_MessageRcv[ePGNTypeRcv].valid = 0; //数据读取成功后无效
    
    return PGNInfoRcv[ePGNTypeRcv].dataLen;
}

bool BMS_Check_Valid(PGNTypeRcv ePGNTypeRcv){
    if (PGN_MessageRcv[ePGNTypeRcv].valid == 1)
    {
        return true;
    }else{
        return false;
    }
}