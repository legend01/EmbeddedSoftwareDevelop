/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-18 09:30:40
 * @LastEditTime: 2021-01-20 21:37:46
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

/**
 * @description: 获取光感电阻的阻值
 * @param {uint8_t} *value
 * @param {uint8_t} ch
 * @return {*}
 */
bool GetLightSensor_value(uint8_t *value, uint8_t ch){
    uint32_t ad = 0;
    if (ch >= ADC_TOTAL_CH_NUM)
    {
        return false;
    }
    for(int i=0; i < SAMPLE_N; i++){
        ad += ADC_Vonversion[i][ch];
    }
    ad /= SAMPLE_N;
    if(ad > 4000){ad = 4000;}
    else(100-(ad/40));
    *value = ad;
    return true;
}