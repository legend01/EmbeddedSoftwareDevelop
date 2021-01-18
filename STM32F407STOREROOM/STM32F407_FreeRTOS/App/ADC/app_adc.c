/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-18 09:30:40
 * @LastEditTime: 2021-01-18 10:00:15
 * @LastEditors: HLLI8
 */
#include "app_adc.h"
uint16_t ADC_Conversion[SAMPLE_N][ADC_TOTAL_CH_NUM];

bool GetADC3_value(uint8_t *value, uint8_t ch){
    uint32_t ad = 0;
    if(ch >= ADC_TOTAL_CH_NUM){
        return false;
    }
    for(int i=0; i < SAMPLE_N; i++){
        ad += ADC_Conversion[i][ch];
    }
    ad /= SAMPLE_N;

    *value = ad;
    return true;
}