/*
 * @Description: CAN过滤器filter配置
 * @Author: HLLI8
 * @Date: 2021-01-14 15:33:10
 * @LastEditTime: 2021-03-01 21:16:11
 * @LastEditors: HLLI8
 */
#ifndef __APP_CAN_H__
#define __APP_CAN_H__
#include "can.h" 

#if BMS_BMS
#define CAN_BASE_ID 0x56F4  ///< CAN标准ID，最大11位，也就是0x7FF
#else
#define CAN_BASE_ID 0xF456  ///< CAN标准ID，最大11位，也就是0x7FF
#endif
/**
 * @description:
 *  为了过滤出一组标识符，应该设置过滤器组工作在屏蔽位模式
 *  为了过滤出一个标识符，应该设置过滤器组工作在标识符列表模式
 */
#define CAN_FILTER_MODE_MASK_ENABLE 1		///< CAN过滤器模式选择：=0：列表模式  =1：屏蔽模式

#define CAN_ID_TYPE_STD_ENABLE      0       ///< CAN过滤ID类型选择：=1：标准ID，=0：扩展ID

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

void CAN_Filter_Config(void);

uint8_t CAN1_Send_Msg(CANSend tx,void *arg);
uint8_t CAN_Register(CANRecv rx, CANSend tx);
uint8_t CAN_Recv(CAN_RxPacketTypeDef *m);
#endif // !__APP_CAN_H__
