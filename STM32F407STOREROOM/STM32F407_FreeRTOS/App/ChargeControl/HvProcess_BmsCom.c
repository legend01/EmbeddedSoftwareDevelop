#include "HvProcess_BmsCom.h"

HvProcess_BmsComInnerDataType HvProcess_BmsComInnerData;

void HvProcess_BmsComInit(void){
    HvProcess_BmsComInnerData.State = HVPROCESS_BMSCOM_START;
    HvProcess_BmsComInnerData.ErrorType = HVPROCESS_NOERROR;
}

bool HvProcess_LowPowerOnCond(void){
    bool res = false;
    if (HvProcess_GetChargeStatus()==CHARGE_SYSSTARTUP)
    {
        res = true;
    }
    return res;
}

void HvProcess_LowPowerOnAction(void){
    HvProcess_BmsComInnerData.ChargeFlag.SysStart = true; 
    HvProcess_BmsComInnerData.TimeTick.SysStart = GetTimeMs();
}

bool HvProcess_ReceiveCHMCond(void)
{
    bool ret = false;
    u8 Getmsglen = 0;  
    if(BMS_Check_Valid(CHM))
    {
        //处理CHM报文中的通信协议版本号
        Getmsglen = BMS_Get_message(CHM, &BMSmanager.messageData); //获取PGN的数值长度
        if (Getmsglen == PGNInfoRcv[CHM].dataLen)
        {
            Rcv_CHM.chargeComVersion_L = BMSmanager.messageData[1]; /* 充电机通信协议版本号存放于CHM结构体中 */
            Rcv_CHM.chargeComVersion_M = BMSmanager.messageData[2];
            Rcv_CHM.chargeComVersion_H = BMSmanager.messageData[3];
            Getmsglen = 0; 
            ret = true;
        }
    }
    return false;
}

void HvProcess_RecvCHMAction(void){
    HvProcess_BmsComInnerData.Flag.RecvCHM = true;
    HvProcess_BmsComInnerData.TimeTick.RecvCHM = GetTimeMs();
}

bool HvProcess_ReceiveCHMTimeOut(void){
    bool res = false;
    if (HvProcess_BmsComInnerData.Flag.RecvCHM == false)
    { /* 自启动起经过5S或收到CRM */
        if (TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.SysStart) >= 5000)
        {
            res = true;
        }
    }
    return res;    
}

bool HvProcess_SendBHMCond(void)
{
    static u32 lastime = 1;
    bool res = false;

    if(lastime == 0){
        lastime = GetTimeMs();
    }else{
        if(TimeAfterMs(lastime) >= PGNInfoSend[BHM].period/* BHM的发送周期 250ms */&& HvProcess_BmsComInnerData.Flag.RecvCHM == true)
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBHM(void)
{
    /*发送一次BHM的内容*/
    if(HvProcess_BmsComInnerData.Flag.RecvCHM == true)
    {
        BMSmanager.msgSendData[0] = Get_Send_BHM_Inf()->vehicleAllowMaxV_L;
        BMSmanager.msgSendData[1] = Get_Send_BHM_Inf()->vehicleAllowMaxV_H;
        BMS_Send_message(BHM, BMSmanager.msgSendData);
        HvProcess_BmsComInnerData.ChargeFlag.SendBHM = true;
    }

}

bool HvProcess_RecvCRM0x00Cond(void)
{
    bool res = false;
    u8 Getmsglen = 0;
    if (BMS_Check_Valid(CRM)&& HvProcess_BmsComInnerData.ChargeFlag.SendBHM == true)
    {
        //处理CRM报文中内容
        Getmsglen = BMS_Get_message(CRM, &BMSmanager.messageData);
        if (Getmsglen == PGNInfoRcv[CRM].dataLen)
        {
            Rcv_CRM.identifyResult = BMSmanager.messageData[0];

            Rcv_CRM.chargeNumber_L = BMSmanager.messageData[1];
            Rcv_CRM.chargeNumber_M = BMSmanager.messageData[2];
            Rcv_CRM.chargeNumber_M1 = BMSmanager.messageData[3];
            Rcv_CRM.chargeNumber_H = BMSmanager.messageData[4];
            
            Rcv_CRM.chargeRegin_L = BMSmanager.messageData[5];
            Rcv_CRM.chargeRegin_M = BMSmanager.messageData[6];
            Rcv_CRM.chargeRegin_H = BMSmanager.messageData[7];

            Getmsglen = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_ReceiveCRM0x00Action(void)
{
    HvProcess_BmsComInnerData.Flag.RecvCRM_0x00 = true;
}

bool HvProcess_ReceCRM0x00TimeoutCond(void){
    bool res = false;
    if (HvProcess_BmsComInnerData.Flag.RecvCRM_0x00 == false)
    { /* 自收到CHM起30S或者系统启动(必须)60S */
        if(TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.RecvCHM) >= 30000 || TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.SysStart) >=60000)
        {
            res = true;
        }
    }
    return res;
}

void HvProcess_SetCRMError(void){
    HvProcess_BmsComInnerData.ErrorType = HVPROCESS_CRM_TIMEOUT;
}

bool HvProcess_SendBRM_Cond(void)
{
    static u32 lastime = 1;
    bool res = false;

    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }else{
        if(TimeAfterMs(lastime) >= PGNInfoSend[BRM].period) /* BRM的发送周期250ms */
        {
            lastime = 0;
            res = true;
        }
    }

    return res;
}

void HvProcess_SendBRMAction(void)
{
    /* 发送一次BRM报文 */
    if (HvProcess_BmsComInnerData.Flag.RecvCRM_0x00 == true)
    {
        HvProcess_BmsComInnerData.TimeTick.SendBRM = GetTimeMs();
        memcpy(BMSmanager.msgSendData, Get_Send_BRM_Inf(), PGNInfoSend[BRM].dataLen);
        BMS_Send_message(BRM, BMSmanager.msgSendData);
        HvProcess_BmsComInnerData.ChargeFlag.SendBRM = true;
    }
}

bool HvProcess_ReceiveCRM0xAA_Cond(void)
{
    bool res = false;
    u8 Getmsglen = 0;
    if(BMS_Check_Valid(CRM)&& HvProcess_BmsComInnerData.ChargeFlag.SendBRM == true) /*从底层中读取是否有CRM0xAA报文*/
    {
        Getmsglen = BMS_Get_message(CRM, &BMSmanager.messageData);
        if (Getmsglen = PGNInfoRcv[CRM].dataLen&&BMSmanager.messageData[0] == 0xAA)
        {
            
            Getmsglen = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_ReceiveCRM0xAAAction(void){

    HvProcess_BmsComInnerData.Flag.RecvCRM_0xAA = true;

}

bool HvProcess_RecvCRM0xAATimeoutCond(void){
    bool res = false;
    if (HvProcess_BmsComInnerData.Flag.RecvCRM_0xAA == false)
    {
        /* 自初次发送BRM起5S */
        if (TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.SendBRM) >= 5000)
        {
            res = true;
        }
    }
    return res;
}

void HvProcess_RecvCRM0xAATimeoutAction(void){
    /* 接收CRM0xAA超时 动作 */
    HvProcess_BmsComInnerData.ErrorType = HVPROCESS_CRM_TIMEOUT;
}

bool HvProcess_SendBCPCond(void){
    static u32 lastime = 1;
    bool res = false;

    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }
    else
    {
        if(TimeAfterMs(lastime) >= PGNInfoSend[BCP].period) /* BCP发送周期 500ms */
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBCPAction(void){
    /* 发送一次BCP报文 */
    if (HvProcess_BmsComInnerData.Flag.RecvCRM_0xAA == true)
    {
        memcpy(BMSmanager.msgSendData, Get_Send_BCP_Inf(), PGNInfoSend[BCP].dataLen);
        BMS_Send_message(BCP, BMSmanager.msgSendData);
        HvProcess_BmsComInnerData.ChargeFlag.SendBCP = true;
    }
}

bool HvProcess_RecvCTSCond(void){
    bool res = false;
    u8 Getmsglen = 0;
    if (BMS_Check_Valid(CTS) && HvProcess_BmsComInnerData.ChargeFlag.SendBCP == true) /* 判断已经接收到CTS报文 */
    {
        Getmsglen = BMS_Get_message(CTS, &BMSmanager.messageData);
        if (Getmsglen == PGNInfoRcv[CTS].dataLen)
        {
            Rcv_CTS.TimeSyncSecond = BMSmanager.messageData[0];
            Rcv_CTS.TimeSyncMinute = BMSmanager.messageData[1];
            Rcv_CTS.TimeSyncHour = BMSmanager.messageData[2];
            Rcv_CTS.TimeSyncDay = BMSmanager.messageData[3];
            Rcv_CTS.TimeSyncMonth = BMSmanager.messageData[4];
            Rcv_CTS.TimeSyncYear_L = BMSmanager.messageData[5];
            Rcv_CTS.TimeSyncYear_H = BMSmanager.messageData[6];

            Getmsglen = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_RecvCTSAction(void){
    HvProcess_BmsComInnerData.Flag.RecvCTS = true;
}

bool HvProcess_RecvCMLCond(void){
    bool res = false;
    u8 Getmsglen = 0;
    if (BMS_Check_Valid(CML) && HvProcess_BmsComInnerData.ChargeFlag.SendBCP == true) /* 判断已经接收到CML报文 */ 
    {
        Getmsglen = BMS_Get_message(CML, &BMSmanager.messageData);
        if (Getmsglen == PGNInfoRcv[CML].dataLen)
        {
            Rcv_CML.MaxOutputVol_L = BMSmanager.messageData[0];
            Rcv_CML.MaxOutputVol_H = BMSmanager.messageData[1];
            Rcv_CML.MinOutputVol_L = BMSmanager.messageData[2];
            Rcv_CML.MinOutputVol_H = BMSmanager.messageData[3];
            Rcv_CML.MaxOutputI_L = BMSmanager.messageData[4];
            Rcv_CML.MaxOutputI_H = BMSmanager.messageData[5];
            Rcv_CML.MinOutputI_L = BMSmanager.messageData[6];
            Rcv_CML.MinOutputI_H = BMSmanager.messageData[7];

            Getmsglen = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_RecvCMLAction(void){
    HvProcess_BmsComInnerData.Flag.RecvCML = true;
}

bool HvProcess_SendBROCond(void)
{
    static u32 lastime = 1;
    bool res = false;

    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }
    else
    {
        if(TimeAfterMs(lastime) >= PGNInfoSend[BRO].period) /* BRO的发送周期 250ms*/
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBROAction(void){
    if (HvProcess_BmsComInnerData.Flag.RecvCML == true)
    {
        if (HvProcess_GetK5K6Status()) /* BMS绝缘校验是否完成 从充电控制部分获取绝缘校验状态 */
        {
            HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0xAA = true; /* 发送BRO 0xAA报文 */
            BMSmanager.msgSendData[0] = 0xAA;
            BMS_Send_message(BRO, BMSmanager.msgSendData);
        }else{
            BMSmanager.msgSendData[0] = 0x00; /* 发送BRO 0x00报文 */
            BMS_Send_message(BRO, BMSmanager.msgSendData);
            HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0x00 = true;
        }
    }
}

bool HvProcess_TimeSynCond(void){
    bool res = true;
    if (HvProcess_BmsComInnerData.Flag.RecvCTS == true)
    {
        res = true;
    }
    return res;
}

void HvProcess_TimeSynAction(void){
    /* 时间同步处理函数 */
}

bool HvProcess_CMLIsWrongCond(void){
    // bool res = false;
    // u16 CH_MaxV = 0;
    // u16 CH_MaxI = 0;

    // u16 VE_MaxAllowV = 0;
    // u16 VE_MaxAllowI = 0;
    // if (HvProcess_BmsComInnerData.Flag.RecvCML == true) /* CML充电参数不合适 */
    // {
    //     CH_MaxV = Rcv_CML.MinOutputVol_L | (Rcv_CML.MinOutputVol_H << 8);
    //     CH_MaxI = Rcv_CML.MinOutputI_L | (Rcv_CML.MinOutputI_H << 8);

    //     VE_MaxAllowV = Send_BCP.MaxAllowTotalChargeVol_L | (Send_BCP.MaxAllowTotalChargeVol_H << 8);
    //     VE_MaxAllowI = Send_BCP.MaxAllowChargeCurrent_L | (Send_BCP.MaxAllowChargeCurrent_H << 8);
    //     if (CH_MaxV > VE_MaxAllowV || CH_MaxI > VE_MaxAllowI)
    //     {
    //          res = true;
    //     }
    // }
    // return res;
}

void HvProcess_CMLIsWrongAction(void){
    // /* 解析CML报文充电参数不适合BMS */
    // HvProcess_BmsComInnerData.ErrorType = HVPROCESS_CMLPARAM_ERROR;
    // HvProcess_BmsComInnerData.ChargeFlag.CMLIsWrong = true;
}

bool HvProcess_RecvCROCond(void){
    bool res = false;
    u8 Getmsglen = 0;
    if (BMS_Check_Valid(CRO) && HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0xAA == true) /* 从底层检测是否接受到CRO报文 */
    {
        Getmsglen = BMS_Get_message(CRO, &BMSmanager.messageData);
        if (Getmsglen == PGNInfoRcv[CRO].dataLen)
        {
            Rcv_CRO.ChargeReadyOrNot = BMSmanager.messageData[0];

            Getmsglen = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_RecvCROAction(void){
    if (Rcv_CRO.ChargeReadyOrNot == 0xAA) /* CRO报文发送的是0xAA 充电机准备就绪 */
    {
        HvProcess_BmsComInnerData.Flag.RecvCRO_0xAA = true;
    }
}

bool HvProcess_RecvCRO0xAACond(void){
    bool res = false;
    if (HvProcess_BmsComInnerData.Flag.RecvCRO_0xAA == true && HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0xAA == true)
    {
        res = true;
    }
    return res;
}

bool HvProcess_RecvCRO0xAATimeout(void){
    bool res = false;
    static u32 lastime = 0;
    if (lastime == 0)
    {
        lastime = GetTimeMs();
    }
    
    if ((HvProcess_BmsComInnerData.Flag.RecvCRO_0xAA == false && TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.SendBRO_0xAA)>= 5000) || (TimeAfterMs(lastime) >= 60000U && HvProcess_BmsComInnerData.Flag.RecvCRO_0xAA == false))
    {
        lastime = 0;
        res = true;
    }
    return res;
}

void HvProcess_RecvCRO0xAATimeoutAction(void){
    HvProcess_BmsComInnerData.ErrorType = HVPROCESS_CRO_TIMEOUT;
}

bool HvProcess_SendBCLCond(void)
{
    static u32 lastime = 1;
    bool res = false;

    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }
    else
    {
        if(TimeAfterMs(lastime) >= PGNInfoSend[BCL].period) /* BCL的发送周期 50ms */
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBCLAction(void){
    /* 发送电压需求 电流需求 充电模式 */
    if (HvProcess_BmsComInnerData.Flag.RecvCRO_0xAA == true)
    {
        memcpy(BMSmanager.msgSendData, Get_Send_BCL_Inf(), PGNInfoRcv[BCL].dataLen);
        BMS_Send_message(BCL, BMSmanager.msgSendData);
    }
}

bool HvProcess_SendBCSCond(void){
    static u32 lastime = 1;
    bool res = false;

    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }
    else
    {
        if(TimeAfterMs(lastime) >= PGNInfoSend[BCS].period) /* BCS的发送周期 250ms */
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBCSAction(void){
    /* 发送电池组充电电压 充电电流等充电状态 */
    if (HvProcess_BmsComInnerData.Flag.RecvCRO_0xAA == true)
    {
        if(HvProcess_BmsComInnerData.Flag.RecvCCS == false){
            memcpy(BMSmanager.msgSendData, Get_Send_BCS_Inf(), PGNInfoRcv[BCS].dataLen);
            BMS_Send_message(BCS, BMSmanager.msgSendData);
        }else{
            BMSmanager.msgSendData[0] = Rcv_CCS.CH_VolOutput_L;
            BMSmanager.msgSendData[1] = Rcv_CCS.CH_VolOutput_H;
            BMSmanager.msgSendData[2] = 4000 - Rcv_CCS.CH_IOutput_L;
            BMSmanager.msgSendData[3] = 4000 - Rcv_CCS.CH_IOutput_H;
            BMSmanager.msgSendData[4] = Get_Send_BCS_Inf()->MaxSinBatVol_L;
            BMSmanager.msgSendData[5] = (Get_Send_BCS_Inf()->MaxSinBatVolNum << 4 & 0xf0) | (Get_Send_BCS_Inf()->MaxSinBatVol_H & 0x0f);
            BMSmanager.msgSendData[6] = Get_Send_BCS_Inf()->NowSOC + ((Get_ChargeCalculation_ParamInf()->BCS_SocGoal - Get_Send_BCS_Inf()->NowSOC)/Get_ChargeCalculation_ParamInf()->BCS_ChargeTime)*Get_ChargeCalculation_ParamInf()->BCS_DurTime;
            BMSmanager.msgSendData[7] = Get_ChargeCalculation_ParamInf()->BCS_ChargeTime - Get_ChargeCalculation_ParamInf()->BCS_DurTime & 0xff;
            BMSmanager.msgSendData[8] = Get_ChargeCalculation_ParamInf()->BCS_ChargeTime - Get_ChargeCalculation_ParamInf()->BCS_DurTime >> 8 & 0xff;
            BMS_Send_message(BCS, BMSmanager.msgSendData);
        }
       
    }
}

bool HvProcess_ReceiveCCSCond(void)
{
    bool res = false;
    u8 Getmsglen = 0;
    if(BMS_Check_Valid(CCS)) /*从底层中读取是否有CCS报文*/
    {
        //处理CCS报文中的充电机电压 电流 累计充电时间 充电允许
        Getmsglen = BMS_Get_message(CCS, &BMSmanager.messageData);
        if (Getmsglen = PGNInfoRcv[CCS].dataLen)
        {
            Rcv_CCS.CH_VolOutput_L = BMSmanager.messageData[0];
            Rcv_CCS.CH_VolOutput_H = BMSmanager.messageData[1];
            Rcv_CCS.CH_IOutput_L = BMSmanager.messageData[2];
            Rcv_CCS.CH_IOutput_H = BMSmanager.messageData[3];
            Rcv_CCS.CH_ChargeTime_L = BMSmanager.messageData[4];
            Rcv_CCS.CH_ChargeTime_H = BMSmanager.messageData[5];
            Rcv_CCS.CH_ChargePermision = BMSmanager.messageData[6] & 0x03;

            Getmsglen = 0;
            res = true;

            if (Get_ChargeCalculation_ParamInf()->BCS_StartTime == 0xFFFF)
            {
                Set_ChargeCalculation_ParamInf()->BCS_StartTime = Rcv_CCS.CH_ChargeTime_L | Rcv_CCS.CH_ChargeTime_H << 8;
            }else{
                Set_ChargeCalculation_ParamInf()->BCS_CurrentTime = Rcv_CCS.CH_ChargeTime_L | Rcv_CCS.CH_ChargeTime_H << 8;
            }
            if (Get_ChargeCalculation_ParamInf()->BCS_CurrentTime > Get_ChargeCalculation_ParamInf()->BCS_StartTime)
            {
                Set_ChargeCalculation_ParamInf()->BCS_DurTime = Set_ChargeCalculation_ParamInf()->BCS_CurrentTime - Set_ChargeCalculation_ParamInf()->BCS_StartTime;
            }
            
        }
    }
    return res;
}

void HvProcess_RecvCCSAction(void)
{
    /*  //!_充电允许< 0:暂停 1:允许 */
    //!_充电开始
    HvProcess_BmsComInnerData.TimeTick.ReceCCS = GetTimeMs();
    HvProcess_BmsComInnerData.Flag.RecvCCS = true;
}

bool HvProcess_RecvCCSTimeoutCond(void)
{
    bool res = false;

    if(HvProcess_BmsComInnerData.Flag.RecvCCS == false)
    {
        if(TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.ReceCCS) >= 1000)
        {
            res = true;
        }
    }

    return res;
}

void HvProcess_RecvCCSTimeoutAction(void){
    HvProcess_BmsComInnerData.ErrorType = HVPROCESS_CCS_TIMEOUT;
}

bool HvProcess_SendBSMCond(void)
{
    static u32 lastime = 1;
    bool res = false;
    if (HvProcess_BmsComInnerData.Flag.RecvCCS == true)
    {
        if(lastime == 0)
        {
            lastime = GetTimeMs();
        }else{
            if(TimeAfterMs(lastime) >= PGNInfoSend[BSM].period) /* BSM的发送周期 250ms*/
            {
                lastime = 0;
                res = true;
            }
        }
    }
    return res;
}

void HvProcess_SendBSMAction(void){
    /* //!_充电允许< 0:禁止 1:允许> */
    if (HvProcess_BmsComInnerData.Flag.RecvCCS == true)
    {
        memcpy(BMSmanager.msgSendData, Get_Send_BSM_Inf(), PGNInfoRcv[BSM].dataLen);
        BMS_Send_message(BSM, BMSmanager.msgSendData);
    }
}

void HvProcess_RecvCHMTimeOutAction(void){
    /* TODO:接收CHM超时 动作 */
}

bool HvProcess_ReceiveCSDTimeoutCond(void){
    bool res = false;
    if (TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.SendBST)>=10000)
    {
        res = true;
    }
    return res;
}

void HvProcess_ReceiveCSDTimeoutAction(void){
    /* TODO:充电时序结束， 充电故障级别1 */
}



bool HvProcess_BmsStopChargeCond(void){
    bool res = false;
    if(true/* 达到充电结束条件 */){
        res = true;
        /* TODO:达到结束充电时间操作 */
    }
    return res;
}

void HvProcess_BmsStopChargeAction(void){
    /* TODO:停止充电操作 */
}


bool HvProcess_SendBSTCond(void){
    static u32 lastime = 1;
    bool res = false;
    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }else{
        if(TimeAfterMs(lastime) >= 10/* BST的发送周期 10ms*/)
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

bool HvProcess_SendBSDCond(void){
    static u32 lastime = 1;
    bool res = false;
    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }else{
        if(TimeAfterMs(lastime) >= 250/* BSD的发送周期 250ms*/)
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBSDAction(void){
    /* code */
    /* TODO:BMS发送本次充电过程的充电统计数据 */
}

bool HvProcess_ReceiveCSTCond(void)
{
    bool res = false;
    if(true/*从底层中读取是否有CST报文*/)
    {
        //处理CST中的停止充电信息
        //这里最好不要直接置stopcharge，统一由充电管理模块来设置停止充电
        res = true;
    }
    return res;
}

void HvProcess_RefrushRecvCSTTime(void){
    HvProcess_BmsComInnerData.Flag.RecvCST = true;
}

bool ChargeStatisticCond(void){
    bool res = false;
    if (HvProcess_BmsComInnerData.Flag.RecvCST == true)
    {
        res = true;
    }
    return res;
}

bool HvProcess_ReceiveCSDCond(void){
    bool res = false;
    if (true/* 从底层读取是否有CSD报文 */)
    {
        //处理CSD中的充电统计数据
        res = true;
    }
    return res;
}

void HvProcess_ReceiveCSDAction(void){
    /* 处理CSD报文统计数据 累计充电时间 输出能量 充电机编号 */
}

bool HvProcess_ChargeStatisticCond(void){
    bool res = false;
    if (HvProcess_BmsComInnerData.Flag.RecvCTS == true/* 接收到CST报文 */)
    {
        res = true;
    }
    return res;
}



bool HvProcess_ReceCSTTimeoutCond(void)
{
    bool res = false;

    if(HvProcess_BmsComInnerData.TimeTick.SendBST != 0U)
    {
        if(TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.SendBST) >= 5000U && HvProcess_BmsComInnerData.Flag.RecvCST != true)
        {
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBSTAction(void)
{
    /* code */
    /* TODO:发送BST报文 发送BMS中止充电原因 BMS中止充电故障原因 BMS中止充电错误原因 */
    if(HvProcess_BmsComInnerData.TimeTick.SendBST == 0)
    {
        HvProcess_BmsComInnerData.TimeTick.SendBST = GetTimeMs();
    }
}



bool HvProcess_K5K6OpenCond(void){
    bool res = false;
    if (true/* 判断辅助电源K5 K6是否关闭 */)
    {
        /* TODO: 辅助电源关闭后操作 */
        res = true;
    }
    return res;
}

void HvProcess_K5K6OpenAction(void){
    /* TODO:关闭辅源后操作 */
}


static void TimeOut_Handler(HvProcess_BmsComErrorType ErrorType){
    if (ErrorType >= HVPROCESS_ERROR_MAX)
    {
        return;
    }
    switch (ErrorType)
    {
    case HVPROCESS_CRM_TIMEOUT:
        Send_BEM.CRM0x00ErrorState = ErrorType_Timeout;
        break;
    case HVPROCESS_CTS_TIMEOUT:
        Send_BEM.CTSErrorState = ErrorType_Timeout;
        break;
    case HVPROCESS_CRO_TIMEOUT: 
        Send_BEM.CROErrorState = ErrorType_Timeout;
        break;
    case HVPROCESS_CCS_TIMEOUT:
        Send_BEM.CCSErrorState = ErrorType_Timeout;
        break;
    case HVPROCESS_CST_TIMEOUT:
        Send_BEM.CSTErrorState = ErrorType_Timeout;
        break;
    case HVPROCESS_CSD_TIMEOUT:
        Send_BEM.CSDErrorState = ErrorType_Timeout;
        break;
    default:
        break;
    }
    BMSmanager.msgSendData[0] = Send_BEM.CRM0x00ErrorState & 0xff;
    BMSmanager.msgSendData[1] = (Send_BEM.CTSErrorState | Send_BEM.CROErrorState) & 0xff;
    BMSmanager.msgSendData[2] = (Send_BEM.CCSErrorState | Send_BEM.CSTErrorState) & 0xff;
    BMSmanager.msgSendData[3] = (Send_BEM.CSDErrorState) & 0xff;

    BMS_Send_message(BEM, BMSmanager.msgSendData);
}

bool HvProcess_SendBEMCond(void){
    static u32 lastime = 1;
    bool res = false;

    if(lastime == 0){
        lastime = GetTimeMs();
    }else{
        if(TimeAfterMs(lastime) >= PGNInfoSend[BEM].period && HvProcess_BmsComInnerData.ErrorType != NULL) /* BEM的发送周期 250ms */
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBEMAction(void){
    /* 发送BEM进行的操作 */
    TimeOut_Handler(HvProcess_BmsComInnerData.ErrorType);
}

bool HvProcess_RecoveryCond(void){
    bool res = false;
    if (HvProcess_GetK5K6Status() == false)
    {
        res = true;
    }
    return res;
}

void HvProcess_RecoveryAction(void){
    /* TODO:code */
}

/***************************各状态点的初始化函数****************************/
void HvProcess_BmsComHandshakeStart_Init(void){
    HvProcess_BmsComInnerData.Flag.RecvCHM = false;
    HvProcess_BmsComInnerData.ChargeFlag.SendBHM = false;
    HvProcess_BmsComInnerData.Flag.RecvCRM_0x00 = false;
    HvProcess_BmsComInnerData.ErrorType = HVPROCESS_NOERROR;
}

void HvProcess_BmsComHandshakeIdentify_Init(void){
    HvProcess_BmsComInnerData.ChargeFlag.SendBRM = false;
    HvProcess_BmsComInnerData.Flag.RecvCRM_0xAA = false;
    HvProcess_BmsComInnerData.ErrorType = HVPROCESS_NOERROR;
}

void HvProcess_BmsComSendBCP_Init(void)
{
    HvProcess_BmsComInnerData.ChargeFlag.SendBCP = false;
    HvProcess_BmsComInnerData.Flag.RecvCTS = false;
    HvProcess_BmsComInnerData.Flag.RecvCML = false;
    HvProcess_BmsComInnerData.ErrorType = HVPROCESS_NOERROR;
}

void HvProcess_BmsComConfig_Init(void)
{
    HvProcess_BmsComInnerData.Flag.RecvCRO_0xAA = false;
    HvProcess_BmsComInnerData.TimeTick.SendBRO_0xAA = GetTimeMs();
    HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0xAA = false;
    HvProcess_BmsComInnerData.ChargeFlag.CMLIsWrong = false;
    HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0x00 = false;
    HvProcess_BmsComInnerData.ErrorType = HVPROCESS_NOERROR;
}

void HvProcess_BmsComCharge_Init(void){

}

void HvProcess_BmsComStopCharge_Init(void){

}

void HvProcess_BmsComStatistics_Init(void){

}

void HvProcess_BmsComTimeOut_Init(void){

}
/* ***************************************************外部调用********************************************* */
bool HvProcess_BmsComChargeState(void){
    bool res = false;
    if (HvProcess_BmsComInnerData.ChargeFlag.CMLIsWrong == false && HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0x00 == true)
    {
        res = true;
    }
    return res;
}

HvProcess_BmsComErrorType HvProcess_GetBmsComErrorState(void){
    return HvProcess_BmsComInnerData.ErrorType;
}
