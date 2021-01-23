/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-18 09:30:46
 * @LastEditTime: 2021-01-23 12:08:49
 * @LastEditors: HLLI8
 */
#ifndef __APP_ADC_H__
#define __APP_ADC_H__
#include "adc.h"
#include "dma.h"
#include "pub_data.h"
#define GPIOF5_Channel 0
#define GPIOF15_LightSensor_Channel 1

#define ADC_TOTAL_CH_NUM 2
#define SAMPLE_N 50
extern uint16_t ADC_Conversion[SAMPLE_N][ADC_TOTAL_CH_NUM];

bool GetADC3_value(uint8_t *value, uint8_t ch);
bool GetLightSensor_value(uint8_t *value, uint8_t ch);
short Get_Temprate(void);
#endif // !__APP_ADC_H__
