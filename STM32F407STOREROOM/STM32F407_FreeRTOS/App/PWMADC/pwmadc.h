/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-24 15:00:01
 * @LastEditTime: 2021-01-24 15:14:05
 * @LastEditors: HLLI8
 */
#ifndef __PWMADC_H__
#define __PWMADC_H__
#include "tim.h"

void SetPWMCompare(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t pulse);

#endif // !__PWMADC_H__
