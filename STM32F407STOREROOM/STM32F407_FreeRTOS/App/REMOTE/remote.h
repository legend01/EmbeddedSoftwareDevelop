/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-02-21 14:58:45
 * @LastEditTime: 2021-02-21 15:31:25
 * @LastEditors: HLLI8
 */
#ifndef __REMOTE_H__
#define __REMOTE_H__
#include "stm32f4xx.h"

#define RDATA HAL_GPIO_ReadPin(Capture_light_GPIO_Port, Capture_light_Pin)

//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
#define REMOTE_ID 0    

extern volatile uint8_t RmtCnt;	        //按键按下的次数
extern volatile uint8_t RmtSta;
extern volatile uint32_t RmtRec;

uint8_t Remote_Scan(void);
#endif // !__REMOTE_H__
