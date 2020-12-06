#ifndef __BEEP_H__
#define __BEEP_H__
#include "main.h"

#define BEEP_1 HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET)
#define BEEP_0 HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET)


#endif // !__BEEP_H__
