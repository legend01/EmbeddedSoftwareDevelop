/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-24 15:00:08
 * @LastEditTime: 2021-01-24 15:17:13
 * @LastEditors: HLLI8
 */
#include "pwmadc.h"

void SetPWMCompare(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t pulse){
    if (channel = TIM_CHANNEL_2)
    {
        TIM_OC_InitTypeDef sConfigOC;
        
        HAL_TIM_PWM_Stop(htim, channel);

        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = pulse;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, channel);

        HAL_TIM_PWM_Start(htim, channel);
    }
    
}