#ifndef __LED_H__
#define __LED_H__
#include "main.h"

#define LED0_1 HAL_GPIO_WritePin(Light_LED0_GPIO_Port, Light_LED0_Pin, GPIO_PIN_SET)
#define LED0_0 HAL_GPIO_WritePin(Light_LED0_GPIO_Port, Light_LED0_Pin, GPIO_PIN_RESET)
#define LED0_Toogle HAL_GPIO_TogglePin(Light_LED0_GPIO_Port, Light_LED0_Pin)

#define LED1_1 HAL_GPIO_WritePin(Light_LED1_GPIO_Port, Light_LED1_Pin, GPIO_PIN_SET)
#define LED1_0 HAL_GPIO_WritePin(Light_LED1_GPIO_Port, Light_LED1_Pin,, GPIO_PIN_RESET)
#define LED1_Toggle HAL_GPIO_TogglePin(Light_LED1_GPIO_Port, Light_LED1_Pin)

#endif // !__LED_H__