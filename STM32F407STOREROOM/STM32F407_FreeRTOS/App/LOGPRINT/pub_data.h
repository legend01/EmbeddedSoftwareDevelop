/*
 * @Description: 公共数据 返回值等
 * @Author: HLLI8
 * @Date: 2021-01-15 13:19:17
 * @LastEditTime: 2021-01-15 13:41:43
 * @LastEditors: HLLI8
 */
#ifndef __PUB_DATA_H__
#define __PUB_DATA_H__

typedef enum {
    FALSE_ADDRESS = -5,
	SEND_CAN_ERROR = -4,
	DATA_LENGTH_FALSE = -3,
	FUNCTION_NULL = -2,
    RET_ERROR = -1,
	FALSE = 0,
	SUCCESS_RET = 1,
}RETTypeDef;
#endif // !__PUB_DATA_H__
