/*
 * @Description: CAN过滤器filter配置
 * @Author: HLLI8
 * @Date: 2021-01-14 15:33:10
 * @LastEditTime: 2021-01-15 09:52:46
 * @LastEditors: HLLI8
 */
#ifndef __APP_CAN_H__
#define __APP_CAN_H__
#include "can.h" 
#define CAN_BASE_ID 0x56F4  ///< CAN标准ID，最大11位，也就是0x7FF

/**
 * @description:
 *  为了过滤出一组标识符，应该设置过滤器组工作在屏蔽位模式
 *  为了过滤出一个标识符，应该设置过滤器组工作在标识符列表模式
 */
#define CAN_FILTER_MODE_MASK_ENABLE 1		///< CAN过滤器模式选择：=0：列表模式  =1：屏蔽模式

#define CAN_ID_TYPE_STD_ENABLE      0       ///< CAN过滤ID类型选择：=1：标准ID，=0：扩展ID

typedef enum {
	SEND_CAN_ERROR = -4,
	DATA_LENGTH_FALSE = -3,
	FUNCTION_NULL = -2,
	FALSE = 0,
	SUCCESS = 1,
}RETURN_ENU;

typedef struct
{
	uint32_t mailbox;
	CAN_TxHeaderTypeDef hdr;
	uint8_t message[8];
}CAN_TxPacketTypeDef;

typedef struct
{
	CAN_RxHeaderTypeDef hdr;
	uint8_t message[8];
}CAN_RxPacketTypeDef;

typedef int (*CANRecv)(void *r);
typedef int (*CANSend)(void *arg, uint32_t* id, uint8_t *dat, uint8_t* len);

typedef struct{
	CANRecv rx;
	CANSend tx;
	uint8_t buffer[16];
}CAN_param_STR;

uint8_t CAN1_Send_Msg(CANSend tx,void *arg);
#endif // !__APP_CAN_H__
