#include "HvProcess_BmsCom.h"


const HvProcess_StateInfoType HvProcess_BmsComStart_InfoConfig[] = { //0
    {
        HvProcess_LowPowerOnCond, /**< Cond: 条件函数指针 *//*低压辅助上电，获取系统启动的flag*/
        HvProcess_LowPowerOnAction, /**< Action: 动作函数指针 */ /**/
        (u16)HVPROCESS_BMSCOM_HANDSHAKE_START, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_BmsComHandshakeStart_InfoConfig[] = { //1
    {
        HvProcess_ReceiveCHMCond, /**< Cond: 条件函数指针 *//*接收到CHM报文*/
        HvProcess_RecvCHMAction, /**< Action: 动作函数指针 */ 
        (u16)HVPROCESS_BMSCOM_HANDSHAKE_START, /**< Next: 下一状态 */
    },
    {
        HvProcess_SendBHMCond, /**< Cond: 条件函数指针 *//*达到发送BHM报文的周期*/
        HvProcess_SendBHM, /**< Action: 动作函数指针 */ /*发送BHM报文*/
        (u16)HVPROCESS_BMSCOM_HANDSHAKE_START, /**< Next: 下一状态 */
    },
    {
        HvProcess_RecvCRM0x00Cond, /**< Cond: 条件函数指针 *//*接收到CRM，停止发送BHM，进入握手辨识阶段*/
        HvProcess_ReceiveCRM0x00Action, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_HANDSHAKE_IDENTIFY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ReceiveCHMTimeOut, /**< Cond: 条件函数指针 *//*5S未收到CHM报文*/
        NULL, //HvProcess_RecvCHMTimeOutAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_HANDSHAKE_START, /**< Next: 下一状态 */
    },
    {
        HvProcess_ReceCRM0x00TimeoutCond, /**< Cond: 条件函数指针 */
        HvProcess_SetCRMError, /**< Action: 动作函数指针 *//*充电时序结束 故障级别3（error）*/
        (u16)HVPROCESS_BMSCOM_TIMEOUT, /**< Next: 下一状态 */
    },
};


const HvProcess_StateInfoType HvProcess_BmsComHandshakeIdentify_InfoConfig[] = { //2
    {
        HvProcess_SendBRM_Cond, /**< Cond: 条件函数指针 *//*达到发送BRM报文的周期*/
        HvProcess_SendBRMAction, /**< Action: 动作函数指针 */ /*发送BRM报文*/
        (u16)HVPROCESS_BMSCOM_HANDSHAKE_IDENTIFY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ReceiveCRM0xAA_Cond, /**< Cond: 条件函数指针 *//*接收到CRM报文*/
        HvProcess_ReceiveCRM0xAAAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_SEND_BCP, /**< Next: 下一状态 */
    },
    {
        HvProcess_RecvCRM0xAATimeoutCond, /**< Cond: 条件函数指针 *//*当发送BRM后，接收CRM 0xAA报文超时*/
        HvProcess_RecvCRM0xAATimeoutAction, /**< Action: 动作函数指针 *//*充电时序结束 故障级别3（error）*/
        (u16)HVPROCESS_BMSCOM_TIMEOUT, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_BmsComSendBCP_InfoConfig[] = { //3
    {
        HvProcess_SendBCPCond, /**< Cond: 条件函数指针 */ /* 达到发送BCP报文周期 */
        HvProcess_SendBCPAction, /**< Action: 动作函数指针 */ /* 发送一帧BCP报文 */
        (u16)HVPROCESS_BMSCOM_SEND_BCP, /**< Next: 下一状态 */
    },
    {
        HvProcess_RecvCTSCond, /**< Cond: 条件函数指针 */ 
        HvProcess_RecvCTSAction, /**< Action: 动作函数指针 */ 
        (u16)HVPROCESS_BMSCOM_SEND_BCP, /**< Next: 下一状态 */
    },
    {
        HvProcess_RecvCMLCond, /**< Cond: 条件函数指针 */
        HvProcess_RecvCMLAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_CONFIG, /**< Next: 下一状态 */   
    },
};

const HvProcess_StateInfoType HvProcess_BmsComConfig_InfoConfig[] = {/*4*/
    {
        HvProcess_SendBROCond, /**< Cond: 条件函数指针 */ /*发送BRO报文*/
        HvProcess_SendBROAction, /**< Action: 动作函数指针 *//*发送报文*/
        (u16)HVPROCESS_BMSCOM_CONFIG, /**< Next: 下一状态 */
    },
    {
        HvProcess_TimeSynCond, /**< Cond: 条件函数指针 */ /* 处理CTS报文 进行时间同步处理 */
        HvProcess_TimeSynAction, /**< Action: 动作函数指针 */ 
        (u16)HVPROCESS_BMSCOM_CONFIG, /**< Next: 下一状态 */
    },
    {
        HvProcess_CMLIsWrongCond, /**< Cond: 条件函数指针 */ /*CML参数不合适*/
        HvProcess_CMLIsWrongAction, /**< Action: 动作函数指针 *//*故障级别3 ，todo*/
        (u16)NULL, /**< Next: 下一状态 */   /*这里还不知道该如何跳转*/
    },
    {
        HvProcess_RecvCROCond, /**< Cond: 条件函数指针 */
        HvProcess_RecvCROAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_CONFIG, /**< Next: 下一状态 */
    },
    {
        HvProcess_RecvCRO0xAACond, /**< Cond: 条件函数指针 */
        NULL, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_CHARGE, /**< Next: 下一状态 */ /* 进入充电阶段 */
    },
    {
        HvProcess_RecvCRO0xAATimeout, /**< Cond: 条件函数指针 */
        HvProcess_RecvCRO0xAATimeoutAction, /**< Action: 动作函数指针 */ /* 充电时序结束 故障级别3 error */
        (u16)HVPROCESS_BMSCOM_TIMEOUT, /**< Next: 下一状态 */
    },
    {
        HvProcess_60STimeoutCond, /**< Cond: 条件函数指针 *//*60s未收到CRO(0xAA)报文，即在此状态停留超过60S*/
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 *//*充电时序结束 故障级别3 error */
        (u16)HVPROCESS_BMSCOM_TIMEOUT, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_BmsComCharge_InfoConfig[] = {/*5*/
    {
        HvProcess_SendBCLCond, /**< Cond: 条件函数指针 */ /*发送BCL,达到发送的周期了:50ms*/
        HvProcess_SendBCLAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_CHARGE, /**< Next: 下一状态 */
    },
    {
        HvProcess_SendBCSCond, /**< Cond: 条件函数指针 */ /*发送BCS,达到发送的周期了:250ms*/
        HvProcess_SendBCSAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_CHARGE, /**< Next: 下一状态 */
    },
    {
        HvProcess_ReceiveCCSCond, /**< Cond: 条件函数指针 */ 
        HvProcess_RecvCCSAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_CHARGE, /**< Next: 下一状态 */
    },
    {
        HvProcess_RecvCCSTimeoutCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 *//* 充电时序结束 故障级别3 error */
        (u16)HVPROCESS_BMSCOM_TIMEOUT, /**< Next: 下一状态 */
    },
    {
        HvProcess_SendBSMCond, /**< Cond: 条件函数指针 */ 
        HvProcess_SendBSMAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_CHARGE, /**< Next: 下一状态 */
    },
    {
        HvProcess_BmsStopChargeCond, /**< Cond: 条件函数指针 *//*充电结束条件成立，可以是正常停止充电，也可以是故障停止*/
        HvProcess_BmsStopChargeAction, /**< Action: 动作函数指针 *//*暂时没想好该做什么*/
        (u16)HVPROCESS_BMSCOM_STOPCHARGE, /**< Next: 下一状态 */
    },
    {
        HvProcess_ReceiveCSTCond, /**< Cond: 条件函数指针 */ /*查询是否收到CST*/
        HvProcess_RefrushRecvCSTTime, /**< Action: 动作函数指针 *//**/
        (u16)HVPROCESS_BMSCOM_CHARGE, /**< Next: 下一状态 *//*这里不直接进行状态跳转*/
    },
};

const HvProcess_StateInfoType HvProcess_BmsComStopCharge_InfoConfig[] = { /*6 停止充电，发送停止充电报文*/
   {
        HvProcess_SendBSTCond, /**< Cond: 条件函数指针 */ /*发送BST,达到发送的周期了:10ms*/
        HvProcess_SendBSTAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_STOPCHARGE, /**< Next: 下一状态 */
    },
    {
        HvProcess_ReceCSTTimeoutCond, /**< Cond: 条件函数指针 *//*接收CST报文超时5S*/
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 *//*故障级别3，error 超时次数加一*/
        (u16)HVPROCESS_BMSCOM_TIMEOUT, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChargeStatisticCond, /**< Cond: 条件函数指针 */
        NULL, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_STATISTICS, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_BmsComStatistics_InfoConfig[] = { /*7 充电统计*/
    {
        HvProcess_SendBSDCond, /**< Cond: 条件函数指针 */ /*发送BSD,达到发送的周期:250ms*/
        HvProcess_SendBSDAction, /**< Action: 动作函数指针 */
        (u16)HVPROCESS_BMSCOM_STATISTICS, /**< Next: 下一状态 */
    },
    {
        HvProcess_ReceiveCSDCond, /**< Cond: 条件函数指针 */ /*查询是否收到CSD*/
        HvProcess_ReceiveCSDAction, /**< Action: 动作函数指针 *//*还没想好该做什么*/
        (u16)HVPROCESS_BMSCOM_STATISTICS, /**< Next: 下一状态 *//**/
    },
    {
        HvProcess_ReceiveCSDTimeoutCond, /**< Cond: 条件函数指针 */ 
        HvProcess_ReceiveCSDTimeoutAction, /**< Action: 动作函数指针 */ /* 充电时序结束 充电故障级别1 */
        (u16)HVPROCESS_BMSCOM_TIMEOUT, /**< Next: 下一状态 *//**/
    },
    {
        HvProcess_K5K6OpenCond, /**< Cond: 条件函数指针 */ /*辅助电源已经关闭了*/
        HvProcess_K5K6OpenAction, /**< Action: 动作函数指针 *//*还没想好可以做什么*/
        (u16)HVPROCESS_BMSCOM_START, /**< Next: 下一状态 *//*回到最开头的部分*/
    },
};

const HvProcess_StateInfoType HvProcess_BmsComTimeOut_InfoConfig[] = { /*8  BMS通信超时的处理*/
    {
        HvProcess_SendBEMCond, /**< Cond: 条件函数指针 */ /*发送BEM,达到发送的周期:250ms*/
        HvProcess_SendBEMAction, /**< Action: 动作函数指针 *//*还没想好该做什么*/
        (u16)HVPROCESS_BMSCOM_STOPCHARGE, /**< Next: 下一状态 */
    },
};


const HvProcess_StateConfigType HvProcess_BmsComStateConfig[HVPROCESS_CHG_STATE_MAX] = {
    {
        (u8)ARRAY_SIZE(HvProcess_BmsComStart_InfoConfig), /**< Num: 状态配置个数 *///0
        HvProcess_BmsComStart_InfoConfig, /**< State: 状态信息指针 */
        NULL,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_BmsComHandshakeStart_InfoConfig), /**< Num: 状态配置个数 *///1
        HvProcess_BmsComHandshakeStart_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_BmsComHandshakeStart_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_BmsComHandshakeIdentify_InfoConfig), /**< Num: 状态配置个数 *///2
        HvProcess_BmsComHandshakeIdentify_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_BmsComHandshakeIdentify_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_BmsComSendBCP_InfoConfig), /**< Num: 状态配置个数 *///3
        HvProcess_BmsComSendBCP_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_BmsComSendBCP_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_BmsComConfig_InfoConfig), /**< Num: 状态配置个数 *///4
        HvProcess_BmsComConfig_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_BmsComConfig_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_BmsComCharge_InfoConfig), /**< Num: 状态配置个数 *///5
        HvProcess_BmsComCharge_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_BmsComCharge_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_BmsComStopCharge_InfoConfig), /**< Num: 状态配置个数 *///6
        HvProcess_BmsComStopCharge_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_BmsComStopCharge_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_BmsComStatistics_InfoConfig), /**< Num: 状态配置个数 *///7
        HvProcess_BmsComStatistics_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_BmsComStatistics_Init,
    },
    {
        (u8)ARRAY_SIZE(HvProcess_BmsComTimeOut_InfoConfig), /**< Num: 状态配置个数 *///8
        HvProcess_BmsComTimeOut_InfoConfig, /**< State: 状态信息指针 */
        HvProcess_BmsComTimeOut_Init,
    },
};
