/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2020-12-08 20:21:42
 * @LastEditTime: 2020-12-08 20:25:02
 * @LastEditors: HLLI8
 */
#ifndef __KEY_H__
#define __KEY_H__
#include "main.h"

#define KEY0 HAL_GPIO_ReadPin(Key0_GPIO_Port, Key0_Pin)
#define KEY1 HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin)
#define KEY2 HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin)
#define KEY_UP HAL_GPIO_ReadPin(KeyUp_GPIO_Port, KeyUp_Pin)


#endif // !__KEY_H__
