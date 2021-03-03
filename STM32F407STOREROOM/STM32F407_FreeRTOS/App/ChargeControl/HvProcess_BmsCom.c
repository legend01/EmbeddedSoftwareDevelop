#include "HvProcess_BmsCom.h"

HvProcess_BmsComInnerDataType HvProcess_BmsComInnerData;
extern BAT_INFOR BAT_inf;
extern RCV_CHM Rcv_CHM;

void HvProcess_BmsComInit(void){
    HvProcess_BmsComInnerData.State = HVPROCESS_BMSCOM_START;
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
            Rcv_CHM.chargeComVersion_L = BMSmanager.messageData[1];
            Rcv_CHM.chargeComVersion_M = BMSmanager.messageData[2];
            Rcv_CHM.chargeComVersion_H = BMSmanager.messageData[3];
            Getmsglen = 0; /* 这里对CHM报文不做记录 */
            ret = true;
        }
    }
    return false;
}

void HvProcess_RecvCHMAction(void){
    /* TODO:接收CHM报文后 动作 */
    HvProcess_BmsComInnerData.Flag.RecvCHM = true;
    HvProcess_BmsComInnerData.TimeTick.RecvCHM = GetTimeMs();
}

bool HvProcess_SendBHMCond(void)
{
    static u32 lastime = 1;
    bool res = false;

    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }
    else
    {

        if(TimeAfterMs(lastime) >= 250/* BHM的发送周期 250ms */&& HvProcess_BmsComInnerData.Flag.RecvCHM == true)
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
        /* TODO:发送BMS最高允许充电总电压 */
        HvProcess_BmsComInnerData.ChargeFlag.SendBHM = true;
    }

}

bool HvProcess_SendBRM_Cond(void)
{
    static u32 lastime = 1;
    bool res = false;

    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }
    else
    {
        if(TimeAfterMs(lastime) >= 250/* BRM的发送周期250ms */)
        {
            lastime = 0;
            res = true;
        }
    }

    return res;
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
        if(TimeAfterMs(lastime) >= 250/* BRO的发送周期 250ms*/)
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
        /* code */
        if (true/* BMS绝缘校验是否完成 从充电控制部分获取绝缘校验状态 */)
        {
            /* TODO:发送BRO 0xAA报文 */
            HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0xAA = true;
            HvProcess_BmsComInnerData.TimeTick.SendBRO_0xAA = GetTimeMs();
        }else{
            /* TODO:发送BRO 0x00报文 */
            HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0x00 = true;
        }
    }
    
}

bool HvProcess_TimeSynCond(void){
    bool res = true;
    if (HvProcess_BmsComInnerData.Flag.RecvCTS == true)
    {
        /* code */
        res = true;
    }
    return res;
}

void HvProcess_TimeSynAction(void){
    /* TODO:时间同步处理函数 */
}

bool HvProcess_CMLIsWrongCond(void){
    bool res = false;
    if (true/* CML充电参数不合适 */)
    {
        /* code */
        /* TODO: */
        res = true;
    }
    return res;
}

void HvProcess_CMLIsWrongAction(void){
    /* TODO:解析CML报文充电参数不适合BMS */
    HvProcess_BmsComInnerData.ChargeFlag.CMLIsWrong = true;
}

bool HvProcess_RecvCROCond(void){
    bool res = false;
    if (true/* 从底层检测是否接受到CRO报文  */&& HvProcess_BmsComInnerData.ChargeFlag.SendBRO_0xAA == true)
    {
        /* code */
        res = true;
    }
    return res;
}

void HvProcess_RecvCROAction(void){
    /* code */ 
    /* TODO:解析CRO报文 */
    if (true/* CRO报文发送的是0xAA 充电机准备就绪 */)
    {
        /* code */
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
    if (HvProcess_BmsComInnerData.Flag.RecvCRO_0xAA == false && TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.SendBRO_0xAA)>= 5000)
    {
        res = true;
    }
    return res;
}

void HvProcess_RecvCRO0xAATimeoutAction(void){
    /* code */
    /* TODO:设置故障级别3 */
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
        if(TimeAfterMs(lastime) >= 50/* BCL的发送周期 50ms */)
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBCLAction(void){
    /* code */
    /* TODO:发送电压需求 电流需求 充电模式 */
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
        if(TimeAfterMs(lastime) >= 250/* BCS的发送周期 250ms */)
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
}

void HvProcess_SendBCSAction(void){
    /* code */
    /* TODO:发送电池组充电电压 充电电流等充电状态 */
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
        if(TimeAfterMs(lastime) >= 500/* BCP发送周期 500ms */)
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
        /* code */
        /* 发送BCP报文 */
    }
}

bool HvProcess_RecvCMLCond(void){
    bool res = false;
    if (true/* 判断已经接收到CML报文 */)
    {
        /* code */
        res = true;
    }
    return res;
}

void HvProcess_RecvCMLAction(void){
    HvProcess_BmsComInnerData.Flag.RecvCML = true;
    /* code */
    /* TODO:处理CML报文 充电机最大输出能力 最高输出电压 最低输出电压 最大输出电流 最小输出电流 */
}

bool HvProcess_RecvCTSCond(void){
    bool res = false;
    if (true/* 判断已经接收到CTS报文 */)
    {
        /* code */
        res = true;
    }
    return res;
}

void HvProcess_RecvCTSAction(void){
    /* code  */
    /* TODO:处理CTS报文 时间同步信息 */
    HvProcess_BmsComInnerData.Flag.RecvCTS = true;
}

bool HvProcess_RecvCRM0x00Cond(void)
{
    bool res = false;
    if (true/*判断已经接收到CRM报文*/&& HvProcess_BmsComInnerData.ChargeFlag.SendBHM == true)
    {
        /* code */
        res = true;
    }
    return res;
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

void HvProcess_RecvCHMTimeOutAction(void){
    /* TODO:接收CHM超时 动作 */
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
    
}

bool HvProcess_RecvCRM0xAATimeoutCond(void){
    bool res = false;
    if (HvProcess_BmsComInnerData.Flag.RecvCRM_0xAA == false)
    {
        /* code */  /* 自初次发送BRM起5S */
        if (TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.SendBRM) >= 5000)
        {
            res = true;
        }
    }
    return res;
}

void HvProcess_RecvCRM0xAATimeoutAction(void){
    /* TODO: 接收CRM0xAA超时 动作 */
}

bool HvProcess_60STimeoutCond(void)
{
    static u32 lastime = 0;
    bool res = false;

    if(lastime == 0)
    {
        lastime = GetTimeMs();
    }
    else
    {

        if(TimeAfterMs(lastime) >= 60000U)
        {
            lastime = 0;
            res = true;
        }
    }
    return res;
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
void HvProcess_SendBRMAction(void)
{
    /* 发送一次BRM报文 */
    if (HvProcess_BmsComInnerData.Flag.RecvCRM_0x00 == true)
    {
        /* code */
        HvProcess_BmsComInnerData.TimeTick.SendBRM = GetTimeMs();
        /* TODO:车辆辨识(协议版本 电池类型 容量 电池电压 VIN码等) */
    }
    
}

void HvProcess_ReceiveCRM0x00Action(void)
{
    HvProcess_BmsComInnerData.Flag.RecvCRM_0x00 = true;
}

bool HvProcess_ReceiveCRM0xAA_Cond(void)
{
    bool res = false;
    if(true/*从底层中读取是否有CRM0xAA报文*/)
    {
        /* code */
        res = true;
    }
    return res;
}

void HvProcess_ReceiveCRM0xAAAction(void){

    HvProcess_BmsComInnerData.Flag.RecvCRM_0xAA = true;

}


bool HvProcess_ReceiveCCSCond(void)
{
    bool res = false;
    if(true/*从底层中读取是否有CCS报文*/)
    {
        //处理CCS报文中的充电机电压 电流 累计充电时间 充电允许
        res = true;
    }
    return res;
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
            if(TimeAfterMs(lastime) >= 250/* BSM的发送周期 250ms*/)
            {
                lastime = 0;
                res = true;
            }
        }
    }
    return res;
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
void HvProcess_SendBSMAction(void){
    /* code */
    /* TODO:发送BSM报文 发送动力蓄电池状态信息 */
    /* //!_充电允许< 0:禁止 1:允许> */
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

bool HvProcess_RecvCCSTimeoutCond(void)
{
    bool res = false;

    if(HvProcess_BmsComInnerData.TimeTick.ReceCCS != 0U)
    {
        if(TimeAfterMs(HvProcess_BmsComInnerData.TimeTick.ReceCCS) >= 1000)
        {
            res = true;
        }
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

void HvProcess_RecvCCSAction(void)
{
    /* code */
    /* TODO:处理CCS报文 获取电压输出值 电流输出值 累计充电时间 //!_充电允许< 0:暂停 1:允许 */
    //!_充电开始
    HvProcess_BmsComInnerData.TimeTick.ReceCCS = GetTimeMs();
    HvProcess_BmsComInnerData.Flag.RecvCCS = true;
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

bool HvProcess_SendBEMCond(void){
    bool res = false;
    if (true/* 发送BEM */)
    {
        /* 发送BEM */
        res = true;
    }
    return res;
}

void HvProcess_SendBEMAction(void){
    /* 发送BEM进行的操作 */
}
/***************************各状态点的初始化函数****************************/
void HvProcess_BmsComHandshakeStart_Init(void){
    HvProcess_BmsComInnerData.Flag.RecvCHM = false;
}

void HvProcess_BmsComHandshakeIdentify_Init(void){

}

void HvProcess_BmsComSendBCP_Init(void)
{

}

void HvProcess_BmsComConfig_Init(void)
{
    HvProcess_BmsComInnerData.Flag.RecvCRO_0xAA = false;
    HvProcess_BmsComInnerData.TimeTick.SendBRO_0xAA = GetTimeMs();
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


