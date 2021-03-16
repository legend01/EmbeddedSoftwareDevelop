/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-01 11:10:29
 * @LastEditTime: 2021-03-16 09:30:12
 * @LastEditors: HLLI8
 */
#include "BMS_BMS.h"



sBMS_Manage BMSmanager;
RCV_CHM Rcv_CHM;
SEND_BHM Send_BHM;
RCV_CRM Rcv_CRM;
SEND_BEM Send_BEM;
SEND_BRM Send_BRM;
SEND_BCP Send_BCP;
RECV_CTS Rcv_CTS;
RECV_CML Rcv_CML;
SEND_BRO Send_BRO;
SEND_BCL Send_BCL;
SEND_BCS Send_BCS;
RECV_CCS Rcv_CCS;
SEND_BSM Send_BSM;
RECV_CST Rcv_CST;
SEND_BST Send_BST;
SEND_BSD Send_BSD;

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

    for (u8 m = 0; m < 4; m++)
    {
        Send_BRM.BatteryManufacturers[m] = Get_Vehicle_ParamInf()->BatteryManufacturers[m];
    }
    
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

SEND_BCP* Get_Send_BCP_Inf(void){
    Send_BCP.SingleBatMaxAllowVoltage_L = Get_Vehicle_ParamInf()->SingleBatMaxAllowVoltage & 0xff;
    Send_BCP.SingleBatMaxAllowVoltage_H = Get_Vehicle_ParamInf()->SingleBatMaxAllowVoltage >> 8 & 0xff;

    Send_BCP.MaxAllowChargeCurrent_L = Get_Vehicle_ParamInf()->MaxAllowChargeCurrent & 0xff;
    Send_BCP.MaxAllowChargeCurrent_H = Get_Vehicle_ParamInf()->MaxAllowChargeCurrent >> 8 & 0xff;

    Send_BCP.BatNominalTotalEnergy_L = Get_Vehicle_ParamInf()->BatNominalTotalEnergy & 0xff;
    Send_BCP.BatNominalTotalEnergy_H = Get_Vehicle_ParamInf()->BatNominalTotalEnergy >> 8 & 0xff;
    
    Send_BCP.MaxAllowTotalChargeVol_L = Get_Vehicle_ParamInf()->MaxAllowTotalChargeVol & 0xff;
    Send_BCP.MaxAllowTotalChargeVol_H = Get_Vehicle_ParamInf()->MaxAllowTotalChargeVol >> 8 & 0xff;

    Send_BCP.MaxAllowTempe = Get_Vehicle_ParamInf()->MaxAllowTempe & 0xff;

    Send_BCP.VehicleBatChargeState_L = Get_Vehicle_ParamInf()->VehicleBatChargeState & 0xff;
    Send_BCP.VehicleBatChargeState_H = Get_Vehicle_ParamInf()->VehicleBatChargeState >> 8 & 0xff;

    Send_BCP.CurrentVehicleBatVol_L = Get_Vehicle_ParamInf()->CurrentVehicleBatVol & 0xff;
    Send_BCP.CurrentVehicleBatVol_H = Get_Vehicle_ParamInf()->CurrentVehicleBatVol >> 8 & 0xff;
    return &Send_BCP;
}

SEND_BCL* Get_Send_BCL_Inf(void){
    Send_BCL.NeedV_L = Get_Vehicle_ParamInf()->NeedV & 0xff;
    Send_BCL.NeedV_H = Get_Vehicle_ParamInf()->NeedV >> 8 & 0xff;

    Send_BCL.NeedI_L = (4000 - Get_Vehicle_ParamInf()->NeedI) & 0xff;
    Send_BCL.NeedI_H = (4000 - Get_Vehicle_ParamInf()->NeedI >> 8) & 0xff;

    Send_BCL.ChargeMode = Get_Vehicle_ParamInf()->ChargeMode & 0xff;
    return &Send_BCL;
}

SEND_BCS* Get_Send_BCS_Inf(void){
    Send_BCS.MeasureChargeVol_L = Get_Vehicle_ParamInf()->BCS_MeasureVol & 0xff;
    Send_BCS.MeasureChargeVol_H = Get_Vehicle_ParamInf()->BCS_MeasureVol >> 8 & 0xff;

    Send_BCS.MeasureChargeI_L = (4000 - Get_Vehicle_ParamInf()->BCS_MeasureI) & 0xff;
    Send_BCS.MeasureChargeI_H = (4000 - Get_Vehicle_ParamInf()->BCS_MeasureI) >> 8 & 0xff;

    Send_BCS.MaxSinBatVol_L = Get_Vehicle_ParamInf()->BCS_SinMaxBatVol & 0xff;
    Send_BCS.MaxSinBatVol_H = Get_Vehicle_ParamInf()->BCS_SinMaxBatVol >> 8 & 0xff;
    Send_BCS.MaxSinBatVolNum = Get_Vehicle_ParamInf()->BCS_SinMaxBATVol_Num & 0xff;

    Send_BCS.NowSOC = Get_Vehicle_ParamInf()->BCS_NowSOC & 0xff;

    Send_BCS.LeftChargeTime_L = Get_Vehicle_ParamInf()->BCS_LeftChargeTIM & 0xff;
    Send_BCS.LeftChargeTime_H = Get_Vehicle_ParamInf()->BCS_LeftChargeTIM >> 8 & 0xff;
    return &Send_BCS;
}

SEND_BSM *Get_Send_BSM_Inf(void){
    Send_BSM.SinBatMaxVolNum = Get_Vehicle_ParamInf()->BSM_SinBatMaxVolNum;
    Send_BSM.BatMaxTemp = Get_Vehicle_ParamInf()->BSM_BatMaxTemp;
    Send_BSM.BatMinTemp = Get_Vehicle_ParamInf()->BSM_BatMaxTempCheckNum;
    Send_BSM.BatMinTempCheckNum = Get_Vehicle_ParamInf()->BSM_BatMinTemp;
    Send_BSM.SinBatVolHighOrLow = Get_Vehicle_ParamInf()->BSM_BatMinTempCheckNum;
    Send_BSM.VehicleBatSOCHightOrLow = Get_Vehicle_ParamInf()->BSM_SinBatVolHighOrLow;
    Send_BSM.VehicleBatChargeOvercurrent = Get_Vehicle_ParamInf()->BSM_VehicleBatChargeOvercurrent;
    Send_BSM.BatTempTooHight = Get_Vehicle_ParamInf()->BSM_BatTempTooHight;
    Send_BSM.BatInsulationStatus = Get_Vehicle_ParamInf()->BSM_BatInsulationStatus;
    Send_BSM.BatPackOutputConnectStatus = Get_Vehicle_ParamInf()->BSM_BatPackOutputConnectStatus;
    Send_BSM.VehicleChargePermission = Get_Vehicle_ParamInf()->BSM_VehicleChargePermission;

    return &Send_BSM;
}

SEND_BST *Get_Send_BST_Inf(void){
    Send_BST.ReachSOCTargetAbort = Get_Vehicle_ParamInf()->BST_ReachSOCTargetAbort;
    Send_BST.ReachVolTargetAbort = Get_Vehicle_ParamInf()->BST_ReachVolTargetAbort;
    Send_BST.ReachSinVolTargetAbort = Get_Vehicle_ParamInf()->BST_ReachSinVolTargetAbort;
    Send_BST.ChargeActiveAbort = Get_Vehicle_ParamInf()->BST_ChargeActiveAbort;

    Send_BST.InsulationFault = Get_Vehicle_ParamInf()->BST_InsulationFault;
    Send_BST.OutputLinkerOverTempFault = Get_Vehicle_ParamInf()->BST_OutputLinkerOverTempFault;
    Send_BST.BMSComponentOverTempFault = Get_Vehicle_ParamInf()->BST_BMSComponentOverTempFault;
    Send_BST.ChargeLinkerFault = Get_Vehicle_ParamInf()->BST_ChargeLinkerFault;

    Send_BST.BatPackOverTempFault = Get_Vehicle_ParamInf()->BST_BatPackOverTempFault;
    Send_BST.HighVolRelayFault = Get_Vehicle_ParamInf()->BST_HighVolRelayFault;
    Send_BST.CheckPoint2VolFault = Get_Vehicle_ParamInf()->BST_CheckPoint2VolFault;
    Send_BST.ElseFault = Get_Vehicle_ParamInf()->BST_ElseFault;

    Send_BST.IOverHighError = Get_Vehicle_ParamInf()->BST_IOverHighError;
    Send_BST.VolAbnormalError = Get_Vehicle_ParamInf()->BST_VolAbnormalError;
    
    return &Send_BST;
}

SEND_BSD * Get_Send_BSD_Inf(void){
    Send_BSD.SinBatMinVol_L = Get_Vehicle_ParamInf()->BSD_SinBatMinVol & 0xff;
    Send_BSD.SinBatMinVol_H = Get_Vehicle_ParamInf()->BSD_SinBatMinVol >> 8 & 0xff;

    Send_BSD.SinBatMaxVol_L = Get_Vehicle_ParamInf()->BSD_SinBatMaxVol & 0xff;
    Send_BSD.SinBatMaxVol_H = Get_Vehicle_ParamInf()->BSD_SinBatMaxVol >> 8 & 0xff;

    Send_BSD.BatMinTemp = Get_Vehicle_ParamInf()->BSD_BatMinTemp & 0xff;
    Send_BSD.BatMaxTemp = Get_Vehicle_ParamInf()->BSD_BatMaxTemp & 0xff;

    return &Send_BSD;
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
    memset(&Send_BRM, 0, sizeof(Send_BRM));
    memset(&Send_BCP, 0, sizeof(Send_BCP));
    memset(&Rcv_CTS, 0, sizeof(Rcv_CTS));
    memset(&Rcv_CML, 0, sizeof(Rcv_CML));
    memset(&Send_BRO, 0, sizeof(Send_BRO));
    memset(&Send_BCL, 0, sizeof(Send_BCL));
    memset(&Send_BCS, 0, sizeof(Send_BCS));
    memset(&Rcv_CCS, 0, sizeof(Rcv_CCS));
    memset(&Send_BSM, 0, sizeof(Send_BSM));
    memset(&Send_BST, 0, sizeof(Send_BST));
    memset(&Send_BSD, 0, sizeof(Send_BSD));
}

void BMS_Send_message(PGNTypeSend ePGNTypeSend, char *data){
    if(data != NULL)
    {
        PushMsgToRBUFF(ePGNTypeSend, data);
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