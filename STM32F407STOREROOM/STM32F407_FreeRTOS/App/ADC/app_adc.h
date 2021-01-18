/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-18 09:30:46
 * @LastEditTime: 2021-01-18 10:00:00
 * @LastEditors: HLLI8
 */
#ifndef __APP_ADC_H__
#define __APP_ADC_H__
#include "adc.h"
#include "dma.h"
#include "pub_data.h"
#define GPIOF5_Channel 1

#define ADC_TOTAL_CH_NUM 1
#define SAMPLE_N 20
extern uint16_t ADC_Conversion[SAMPLE_N][ADC_TOTAL_CH_NUM];
#endif // !__APP_ADC_H__