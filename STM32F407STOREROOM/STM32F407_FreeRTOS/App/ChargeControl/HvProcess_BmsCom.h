/**
 * \addtogroup HvProcess CAN通信控制
 * \{
 */

#ifndef HV_PROCESS__CHG_H_
#define HV_PROCESS__CHG_H_

#include "HvProcess_Types.h"
#include "HvProcess_Chg.h"
#include "BMS_BMS.h"
#include "ChargeInf.h"

/**
 * \brief 通信流程控制状态类型
 */
typedef enum{
    HVPROCESS_BMSCOM_START = 0,  /**< 启动状态 */
    HVPROCESS_BMSCOM_HANDSHAKE_START = 1, /**< 握手启动状态 */
    HVPROCESS_BMSCOM_HANDSHAKE_IDENTIFY = 2, /**< 握手辨识状态 */
    HVPROCESS_BMSCOM_SEND_BCP = 3, /**< 发送BCP状态 */
    HVPROCESS_BMSCOM_CONFIG = 4, /**< 配置阶段 */
    HVPROCESS_BMSCOM_CHARGE = 5, /**< 充电阶段 */
    HVPROCESS_BMSCOM_STOPCHARGE = 6, /**< 停止充电 */
    HVPROCESS_BMSCOM_STATISTICS = 7,   /**< 充电统计 */
    HVPROCESS_BMSCOM_TIMEOUT = 8,   /**< 通信超时 */
    HVPROCESS_BMSCOM_STATE_MAX = 9, /**< BMS通信流程控制状态最大值 */
}HvProcess_BmsComStateType;

typedef enum{
    HVPROCESS_NOERROR, /* 无错误 */
    HVPROCESS_CRM_TIMEOUT, /* 接收CRM超时 */
    HVPROCESS_CTS_TIMEOUT, /* 接收CTS超时 */
    HVPROCESS_CRO_TIMEOUT, /* 接收CRO超时 */
    HVPROCESS_CCS_TIMEOUT, /* 接收CCS超时 */
    HVPROCESS_CST_TIMEOUT, /* 接收CST超时 */
    HVPROCESS_CSD_TIMEOUT, /* 接收CSD超时 */
    HVPROCESS_ERROR_MAX,
}HvProcess_BmsComErrorType;

typedef __packed struct{
    u32 SysStart;
    u32 RecvCHM;
    u32 SendBRM;
    u32 SendBRO_0xAA;
    u32 ReceCCS;
    u32 SendBST;
}HvProcess_BmsComTimeTick;

typedef __packed struct{
    bool RecvCHM;
    bool RecvCRM_0x00;
    bool RecvCRM_0xAA;
    bool RecvCML;
    bool RecvCTS;
    bool RecvCRO_0xAA;
    bool RecvCRO_0xFF;
    bool RecvCCS;
    bool RecvCST;
    bool RecvCSTConfirm;
    bool RecvCSD;
}ReceiveFlag;

typedef __packed struct{
    bool SysStart;
    bool SendBHM;
    bool SendBRO_0x00;
    bool SendBRO_0xAA;
    bool CMLIsWrong;
    bool SendBRM;
    bool SendBCP;
}ChargeFlag;
/**
 * \brief 充电高压流程控制内部数据类型
 */
typedef __packed struct{
    HvProcess_BmsComStateType       State;         /**< 充电高压流程状态 */
    HvProcess_BmsComTimeTick        TimeTick;
    ReceiveFlag                  Flag;
    ChargeFlag                   ChargeFlag;
    HvProcess_BmsComErrorType    ErrorType;
}HvProcess_BmsComInnerDataType;

extern const HvProcess_StateConfigType HvProcess_BmsComStateConfig[HVPROCESS_BMSCOM_STATE_MAX];

/**
 * \brief 充电高压流程控制初始化
 */
void HvProcess_BmsComInit(void);

/**
 * \brief 获取充电高压流程控制状态
 * \return 充电流程状态
 */
u16 HvProcess_GetCanComState(void);

/**
 * \brief 设置充电高压流程控制状态
 *
 * \param state 充电流程状态
 */
void HvProcess_SetCanComState(u16 state);

/**
 * \brief 充电高压流程控制周期函数
 * \details 用于需要周期性更新的策略
 */
void HvProcess_BmsComPoll(void);

bool HvProcess_BmsComStateStartCond(void);

/* ************************************************************************ */
bool HvProcess_LowPowerOnCond(void);
bool HvProcess_SendBHMCond(void);
void HvProcess_SendBHM(void);
bool HvProcess_SendBRM_Cond(void);
bool HvProcess_SendBROCond(void);
void HvProcess_SendBROAction(void);
bool HvProcess_TimeSynCond(void);
bool HvProcess_CMLIsWrongCond(void);
void HvProcess_CMLIsWrongAction(void);
bool HvProcess_RecvCROCond(void);
void HvProcess_RecvCROAction(void);
bool HvProcess_RecvCRO0xAACond(void);

bool HvProcess_RecvCRO0xFFCond(void);
void HvProcess_RecvCRO0xFFAction(void);

bool HvProcess_RecvCRO0xAATimeout(void);
void HvProcess_RecvCRO0xAATimeoutAction(void);
bool HvProcess_SendBCLCond(void);
void HvProcess_SendBCLAction(void);
bool HvProcess_SendBCSCond(void);
void HvProcess_SendBCSAction(void);
bool HvProcess_SendBCPCond(void);
void HvProcess_SendBCPAction(void);
bool HvProcess_RecvCMLCond(void);
void HvProcess_RecvCMLAction(void);

bool HvProcess_RcvCTSCMLTransitionCond(void);
void HvProcess_RcvCTSCMLTransitionAction(void);

bool HvProcess_RecvCTSCond(void);
void HvProcess_RecvCTSAction(void);
bool HvProcess_RecvCRM0x00Cond(void);
bool HvProcess_ReceiveCHMTimeOut(void);
void  HvProcess_RecvCHMTimeOutAction(void);
bool HvProcess_RecvCRM0xAATimeoutCond(void);
void HvProcess_RecvCRM0xAATimeoutAction(void);
void HvProcess_LowPowerOnAction(void);
bool HvProcess_ReceiveCSDTimeoutCond(void);
void HvProcess_SendBRMAction(void);
void HvProcess_ReceiveCRM0x00Action(void);
bool HvProcess_ReceiveCHMCond(void);
void HvProcess_RecvCHMAction(void);
bool HvProcess_ReceCRM0x00TimeoutCond(void);
void HvProcess_SetCRMError(void);
bool HvProcess_ReceiveCRM0xAA_Cond(void);
void HvProcess_ReceiveCRM0xAAAction(void);
void HvProcess_TimeSynAction(void);
bool HvProcess_ReceiveCCSCond(void);
void HvProcess_RecvCCSAction(void);
bool HvProcess_RecvCCSTimeoutCond(void);
bool HvProcess_SendBSMCond(void);
void HvProcess_SendBSMAction(void);
bool HvProcess_BmsStopChargeCond(void);
void HvProcess_BmsStopChargeAction(void);
bool HvProcess_ReceiveCSTCond(void);
void HvProcess_ReceiveCSTAction(void);
bool HvProcess_SendBSTCond(void);
void HvProcess_SendBSTAction(void);

bool HvProcess_ReceiveCSTConfirmCond(void);
void HvProcess_ReceiveCSTConfirmAction(void);

bool HvProcess_ReceCSTTimeoutCond(void);
void HvProcess_ReceCSTTimeoutAction(void);

bool HvProcess_ChargeStatisticCond(void);
bool HvProcess_SendBSDCond(void);
void HvProcess_SendBSDAction(void);
bool HvProcess_ReceiveCSDCond(void);
void HvProcess_ReceiveCSDAction(void);
void HvProcess_ReceiveCSDTimeoutAction(void);
bool HvProcess_K5K6OpenCond(void);
void HvProcess_K5K6OpenAction(void);
bool HvProcess_SendBEMCond(void);
void HvProcess_SendBEMAction(void);
bool HvProcess_RecoveryCond(void);
void HvProcess_RecoveryAction(void);
void HvProcess_RecvCCSTimeoutAction(void);
/* ************************************************************************ */
void HvProcess_BmsComHandshakeStart_Init(void);
void HvProcess_BmsComHandshakeIdentify_Init(void);
void HvProcess_BmsComSendBCP_Init(void);
void HvProcess_BmsComConfig_Init(void);
void HvProcess_BmsComCharge_Init(void);
void HvProcess_BmsComStopCharge_Init(void);
void HvProcess_BmsComStatistics_Init(void);
void HvProcess_BmsComTimeOut_Init(void);
/* **********************************外部调用************************************** */
bool HvProcess_BmsComChargeState(void);
HvProcess_BmsComErrorType HvProcess_GetBmsComErrorState(void);
bool HvProcess_BmsComChargeAllowStatus(void);
HvProcess_BmsComStateType HvProcess_BmsComState(void);
bool HvProcess_BmsComChargeNotReadyStatus(void);
bool HvProcess_BmsComChargePrepareStatus(void);
/* ************************************************************************ */
#endif

/**
 * \}
 */



/*需要用到j1939中的内容*/


