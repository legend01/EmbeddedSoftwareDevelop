#include "HvProcess_Chg.h"


const HvProcess_StateInfoType HvProcess_ChgSelfTest_InfoConfig[] = { //此阶段可以做一些硬件自检  0
    {
        HvProcess_ChgStateStartCond, /**< Cond: 条件函数指针 *//*等待上电自检通过*/
        NULL, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_CHG_CONNECTCONFIRM, /**< Next: 下一状态 检查连接 */
    },
};

const HvProcess_StateInfoType HvProcess_ConnectComfirm_InfoConfig[] = {   //1
    {
        HvProcess_ChgConnectCond, /**< Cond: 条件函数指针 */ /* 检测点2是否为U2的1/2 检测CAN线是否连接完成 */
        HvProcess_ChgConnectAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_CHG_ShakeHand_Identity, /**< Next: 下一状态 充电辨识阶段 */
    },
};

const HvProcess_StateInfoType HvProcess_ShakeHandIdentity_InfoConfig[] = {  //2
    {
        HvProcess_ChgFaultCond, /**< Cond: 条件函数指针 */ //故障状态
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_CHG_CONNECTCONFIRM, /**< Next: 下一状态 *//**/
    },
    /*******************************分割线************************************/
    {
        HvProcess_ChgDisconnectCond, /**< Cond: 条件函数指针 */ /* 检测点2电压不为6V */
        NULL, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_CHG_CONNECTCONFIRM, /**< Next: 下一状态 */
    },
    {
        HvProcess_ParamsSuitableCond, /**< Cond: 条件函数指针 */ /* 充电参数适合 */
        HvProcess_ParamsSuitableAction, /**<Action: 动作函数指针 */ /* 闭合K5 K6 */
        (u16)HVPROCESS_CHG_INSULATION_TEST_GET, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_InsulationTest_InfoConfig[] = {  //3
    {
        HvProcess_ChgFaultCond, /**< Cond: 条件函数指针 */ //故障状态
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_CHG_STOP_CHARGING, /**< Next: 下一状态 *//**/
    },
    /*******************************分割线************************************/
    {
        HvProcess_InsulationTestCond, /**< Cond: 条件函数指针 */ 
        HvProcess_InsulationTestAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_CHG_CHARGING, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_Charging_InfoConfig[] = {  //4
    {
        HvProcess_ChgFaultCond, /**< Cond: 条件函数指针 */ //故障状态
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_CHG_STOP_CHARGING, /**< Next: 下一状态 *//**/
    },
    /*******************************分割线************************************/
    {
        HvProcess_ChargeForbiddenCond, /**< Cond: 条件函数指针 */ /*充电禁止标志位ON*/
        NULL, /**< Action: 动作函数指针 */   
        (u16)HVPROCESS_CHG_CHARGING_SUSPEND, /**< Next: 下一状态 */
    },
    {
        HvProcess_StopChargeCond, /**< Cond: 条件函数指针 */ /*充电command为stop*/
        HvProcess_StopChargeActon, /**< Action: 动作函数指针 */   
        (u16)HVPROCESS_CHG_PRESTOP_CHARGING, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChargingSuspend_InfoConfig[] = {  //5
    {
        HvProcess_ChargeSuspendTimeOutCond, /**< Cond: 条件函数指针 */ /*暂停充电超时（10min）*/
        NULL, /**< Action: 动作函数指针 */   /*todo*/
        (u16)HVPROCESS_CHG_STOP_CHARGING, /**< Next: 下一状态 *//*todo不确定超时是不是就是停止充电*/
    },
    {
        HvProcess_ChargeEnableCond, /**< Cond: 条件函数指针 */
        NULL, /**< Action: 动作函数指针 */   
        (u16)HVPROCESS_CHG_CHARGING, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_PreStopCharging_InfoConfig[] = {  //6
    {
        HvProcess_BMSStatisticsCond, /**< Cond: 条件函数指针 */ /*接收到充电桩发来的CST报文,也就是进入了统计阶段*/
        NULL, /**< Action: 动作函数指针 */  
        (u16)HVPROCESS_CHG_STOP_CHARGING, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_StopCharging_InfoConfig[] = {  //7
    {
        HvProcess_CurrentBelove5ATimeoutCond, /**< Cond: 条件函数指针 */ /* 自发送BST起Timeout时间10S */
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */   /* 充电时序结束 充电故障级别1 */
        (u16) HVPROCESS_CHG_STOP_CHARGING, /**< Next: 下一状态 */ 
    },
    {
        HvProcess_CurrentBelow5A, /**< Cond: 条件函数指针 */
        HvProcess_OpenK5K6, /**< Action: 动作函数指针 */   /* 车辆DC继电器断开(K5, K6)*/
        (u16)HVPROCESS_CHG_SysSleep, /**< Next: 下一状态 */
    },
};


const HvProcess_StateInfoType HvProcess_SYSSleep_InfoConfig[] = {  //8 
    {
        HvProcess_Wait1SCond, /**< Cond: 条件函数指针 */ 
        HvProcess_SysSleep, /**< Action: 动作函数指针 *//*系统休眠*/
        (u16)HVPROCESS_CHG_CONNECTCONFIRM, /**< Next: 下一状态 */
    },
};

const HvProcess_StateConfigType HvProcess_ChgStateConfig[HVPROCESS_CHG_STATE_MAX] = {
    {
        (u8)ARRAY_SIZE(HvProcess_ChgSelfTest_InfoConfig), /**< Num: 状态配置个数 *///0
        HvProcess_ChgSelfTest_InfoConfig, /**< State: 状态信息指针 */
        NULL,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_ConnectComfirm_InfoConfig), /**< Num: 状态配置个数 *///1
        HvProcess_ConnectComfirm_InfoConfig, /**< State: 状态信息指针 */
        NULL,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_ShakeHandIdentity_InfoConfig), /**< Num: 状态配置个数 *///2
        HvProcess_ShakeHandIdentity_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_ShakeHandIdentity_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_InsulationTest_InfoConfig), /**< Num: 状态配置个数 *///3
        HvProcess_InsulationTest_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_InsulationTest_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_Charging_InfoConfig), /**< Num: 状态配置个数 *///4
        HvProcess_Charging_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_Charging_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_ChargingSuspend_InfoConfig), /**< Num: 状态配置个数 *///5
        HvProcess_ChargingSuspend_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_ChargingSuspend_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_PreStopCharging_InfoConfig), /**< Num: 状态配置个数 *///6
        HvProcess_PreStopCharging_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_PreStopCharging_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_StopCharging_InfoConfig), /**< Num: 状态配置个数 */ //7
        HvProcess_StopCharging_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_StopCharging_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_SYSSleep_InfoConfig), /**< Num: 状态配置个数 */ //8
        HvProcess_SYSSleep_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_SYSSleep_Init,
    },
};