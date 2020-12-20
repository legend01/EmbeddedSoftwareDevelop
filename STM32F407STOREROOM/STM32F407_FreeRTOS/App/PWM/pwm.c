/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2020-12-20 11:55:24
 * @LastEditTime: 2020-12-20 11:57:32
 * @LastEditors: HLLI8
 */
#include "pwm.h"
#include "tim.h"

void SetLight0Pwm(void){
  uint16_t pwmVal=0;   //PWM占空比
  while (pwmVal< 500)
  {
    pwmVal++;
    __HAL_TIM_SetCompare(&htim14, TIM_CHANNEL_1, pwmVal);    //修改比较值，修改占空比
    delay_ms(1000);
  }
  while (pwmVal)
  {
    pwmVal--;
    __HAL_TIM_SetCompare(&htim14, TIM_CHANNEL_1, pwmVal);    //修改比较值，修改占空比 
    delay_ms(1000);
  }
}