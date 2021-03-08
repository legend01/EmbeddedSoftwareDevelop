/*
 * @Description: 
 * @Autor: lihelin
 * @Date: 2021-02-24 09:20:34
 * @LastEditors: HLLI8
 * @LastEditTime: 2021-03-08 17:08:17
 */
/**
 * \addtogroup HvProcess 高压流程控制
 * \{
 */

#ifndef __HV_PROCESS__CHG_H__
#define __HV_PROCESS__CHG_H__

#include "HvProcess_Types.h"

typedef enum {
    CHARGE_POWERON, /* 上电 */
    CHARGE_SYSSTARTUP, /* 系统启动 */
}HvProcess_ChargeStatusType;

typedef enum{
    INVALID, 
    START_CHARGE, 
    STOP_CHARGE, 
    AUTO_SELECT,
}HvProcess_CommandType;
/**
 * \brief 充电高压流程控制状态类型
 */
typedef enum{
    HVPROCESS_CHG_SELFTEST = 0,  /**< 上电自检 */
    HVPROCESS_CHG_CONNECTCONFIRM = 1, /**< 连接确认状态，辅源给BMS供电 */
    HVPROCESS_CHG_ShakeHand_Identity = 2, /**< 充电辨识阶段 */
    HVPROCESS_CHG_INSULATION_TEST_GET = 3, /**< 绝缘校验 */
    HVPROCESS_CHG_CHARGING = 4, /**< 充电中 */
    HVPROCESS_CHG_CHARGING_SUSPEND = 5, /**< 充电暂停 */
    HVPROCESS_CHG_PRESTOP_CHARGING = 6,/*预停止充电*/
    HVPROCESS_CHG_STOP_CHARGING = 7, /*停止充电*/
    HVPROCESS_CHG_SysSleep = 8,/*系统休眠 */
    HVPROCESS_CHG_STATE_MAX = 9, /*充电控制流程状态最大值*/
}HvProcess_ChgStateType;

typedef __packed struct{
    u32 InsulationVoltageUpper;
}HvProcess_ChgTimeTick;

/**
 * \brief 充电高压流程控制内部数据类型
 */
typedef __packed struct{
    HvProcess_ChgStateType       State;         /**< 充电高压流程状态 */
    HvProcess_ChgTimeTick        TimeTick;
    HvProcess_ChargeStatusType    ChargeStatus; /** 充电状态 */
    bool  K5K6flag;
}HvProcess_ChgInnerDataType;

extern const HvProcess_StateConfigType HvProcess_ChgStateConfig[HVPROCESS_CHG_STATE_MAX];
/**
 * \brief 充电高压流程控制初始化
 */
void HvProcess_ChgInit(void);

/**
 * \brief 获取充电高压流程控制状态
 * \return 充电流程状态
 */
u16 HvProcess_GetChgState(void);

/**
 * \brief 设置充电高压流程控制状态
 *
 * \param state 充电流程状态
 */
void HvProcess_SetChgState(u16 state);

/**
 * \brief 充电高压流程控制周期函数
 * \details 用于需要周期性更新的策略
 */
void HvProcess_ChgPoll(void);

bool HvProcess_ChgStateStartCond(void);
bool HvProcess_ChgConnectCond(void);
void HvProcess_ChgConnectAction(void);
bool HvProcess_ChgDisconnectCond(void);
bool HvProcess_ParamsSuitableCond(void);
void HvProcess_ParamsSuitableAction(void);
bool HvProcess_ChgFaultCond(void);
void HvProcess_ChgFaultAction(void);
bool HvProcess_InsulationTestCond(void);
void HvProcess_InsulationTestAction(void);
bool HvProcess_ChargeForbiddenCond(void);
bool HvProcess_StopChargeCond(void);
void HvProcess_StopChargeActon(void);
bool HvProcess_ChargeSuspendTimeOutCond(void);
bool HvProcess_ChargeEnableCond(void);
bool HvProcess_BMSStatisticsCond(void);
bool HvProcess_CurrentBelove5ATimeoutCond(void);
bool HvProcess_CurrentBelow5A(void);
void HvProcess_OpenK5K6(void);
bool HvProcess_Wait1SCond(void);
void HvProcess_SysSleep(void);


void HvProcess_Start_Insulation_test_Init(void);
void HvProcess_ShakeHandIdentity_Init(void);
void HvProcess_InsulationTest_Init(void);
void HvProcess_Charging_Init(void);
void HvProcess_ChargingSuspend_Init(void);
void HvProcess_PreStopCharging_Init(void);
void HvProcess_StopCharging_Init(void);
void HvProcess_SYSSleep_Init(void);

/************************外部接口****************************************/
/**
 * \brief 获取当前充电状态
 */
HvProcess_ChargeStatusType HvProcess_GetChargeStatus(void);

/**
 * \brief 根据当前状态来执行启动或结束充电动作
 */
bool HvProcess_Chg_SetChargeCommand(HvProcess_CommandType Cmd);

/**
 * \brief 获取当前控制动作，返回值表示此动作的执行结果
 */
HvProcess_CommandType HvProcess_Chg_GetChargeCommand(void);

/**
 * \brief 获取当前继电器闭合状态
 */
bool HvProcess_GetK5K6Status(void);

#endif

/**
 * \}
 */
