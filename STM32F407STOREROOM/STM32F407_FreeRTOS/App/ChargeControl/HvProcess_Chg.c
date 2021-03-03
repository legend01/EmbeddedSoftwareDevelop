/*
 * @Description: 
 * @Autor: lihelin
 * @Date: 2021-02-24 09:20:34
 * @LastEditors: HLLI8
 * @LastEditTime: 2021-03-02 17:33:58
 */
#include "HvProcess_Chg.h"

HvProcess_ChgInnerDataType HvProcess_ChgInnerData;

void HvProcess_ChgInit(void){
    HvProcess_ChgInnerData.State = HVPROCESS_CHG_SELFTEST;
    HvProcess_ChgInnerData.ChargeStatus = CHARGE_POWERON;
}

HvProcess_ChargeStatusType HvProcess_GetChargeStatus(void){
    return HvProcess_ChgInnerData.ChargeStatus;
}

bool HvProcess_ChgStateStartCond(void){
    bool res = false;
    if (true/* TODO:上电检查系统是否存在错误 自检 */)
    {
        res = true;
    }
    return res;
}

bool HvProcess_ChgConnectCond(void) 
{
    bool res = false;

    if(true/*检测点2电压为6V*/) //检查物理连接是否完成，上电，辅助电压打开 检测CAN线是否连接完成
    {
        res = true;
    }

    return res;
}

void HvProcess_ChgConnectAction(void)
{
    /*初始化清零*/
    HvProcess_ChgInnerData.ChargeStatus = CHARGE_SYSSTARTUP;
}

bool HvProcess_ChgDisconnectCond(void){
    bool res = false;
    if (true/* 监测点2处电压不为6V */)
    {
        res = true;
    }
    return res;
}

bool HvProcess_ChgStartChargeCond(void)
{
    bool res = false;

    if(true/*开始充电*/)
    {
        res = true;
    }

    return res;
}

bool HvProcess_InsulationVoltageIsOKCond(void)
{
    bool res = false;

    if(true/*绝缘检测电压正常*/)
    {
        res = true;
    }

    return res;
}

bool HvProcess_ChargeForbiddenCond(void)
{
    bool res = false;
    if (true/* 充电桩是否发送禁止充电报文 */)
    {
        /* TODO：响应操作 */
        res = true;
    }
    return res;
    //BSM报文中的内容
    //接收到充电桩发来的停止充电标志位
}

bool HvProcess_StopChargeCond(void){
    bool res = false;
    if (true/* 是否达到充电结束条件 */)
    {
        /* TODO:停止充电操作 */
        res = true;
    }
    return res;
    //达到充电结束条件成立 停止充电
}

void HvProcess_StopChargeActon(void){
    /* TODO:停止充电响应操作 */
}
bool HvProcess_ChargeSuspendTimeOutCond(void)
{
    bool res = false;
    if (true/* TODO:判断暂停充电时间是否超时 */)
    {
        /* TODO:暂停充电操作 */
        res = true;
    }
    return res;
    /*暂停充电超时10min*/
}

bool HvProcess_ParamsSuitableCond(void){
    bool res = false;
    if (HvProcess_BmsComChargeState())
    {
        /* code */
        res = true;
    }
    return res;
}

void HvProcess_ParamsSuitableAction(void){
    /* code */
    /* TODO:充电桩充电参数合适动作 闭合K5 K6*/
}

bool HvProcess_InsulationTestCond(void){
    bool res = false;
    if (true/* 绝缘测试 */)
    {
        /* code */
        /* TODO:操作 */
        res = true;
    }
    return res;
}

void HvProcess_InsulationTestAction(void){
    /* TODO:绝缘测试响应操作 */
}

bool HvProcess_ChgFaultCond(void){
    bool res = false;
    if (true/* 故障 */)
    {
        /* code */
        /* TODO:故障状态下对应操作 */
        res = true;
    }
    return res;
}

bool HvProcess_ChargeEnableCond(void){
    bool res = false;
    if (true/* TODO:判断是否可以恢复充电条件 */)
    {
        /* TODO：响应操作 */
        res = true;
    }
    return res;
}

bool HvProcess_BMSStatisticsCond(void){
    bool res = false;
    if (true/* TODO:判断是否进入到统计阶段 */)
    {
        res = true;
        /* TODO：响应操作 */
    }
    return res;
}

bool HvProcess_CurrentBelove5ATimeoutCond(void){
    bool res = false;
    if (true/* TODO:发送BST开始是否Timeout */)
    {
        /* code */
        /* TODO:进行相关操作 */
        res = true;
    }
    return res;
}

bool HvProcess_CurrentBelow5A(void){
    bool res = false;
    if (true/* TODO:电流小于5A */)
    {
        /* TODO:响应操作 */
        res = true;
    }
    return res;
}

void HvProcess_OpenK5K6(void){
    /* 断开K5 K6继电器 */
}

bool HvProcess_Wait1SCond(void){
    bool res = false;
    if (true /* TODO:系统该进入到系统休眠状态 */)
    {
        /* 相应操作 */
        res = true;
    }
    return res;
}

void HvProcess_SysSleep(void){
    /* TODO:系统进入休眠状态 */
}

void HvProcess_ChgFaultAction(void)
{
    //充电控制设置为stopcharge
    //并且清除一些标志位
}

/***************************各状态点的初始化函数****************************/
void HvProcess_Start_Insulation_test_Init(void)
{

}

void HvProcess_ShakeHandIdentity_Init(void)
{

}

void HvProcess_InsulationTest_Init(void)
{
    
}

void HvProcess_Charging_Init(void)
{

}

void HvProcess_ChargingSuspend_Init(void)
{

}

void HvProcess_PreStopCharging_Init(void)
{
    
}

void HvProcess_StopCharging_Init(void)
{

}

void HvProcess_SYSSleep_Init(void)
{
    
}