#ifndef __TPAD_H__
#define __TPAD_H__
#include "stm32f4xx.h"


uint8_t TPAD_Scan(uint8_t mode);
uint16_t TPAD_Get_MaxVal(uint8_t n);
uint16_t TPAD_Get_Val(void);
uint8_t TPAD_Init(void);
#endif // !__TPAD_H__
