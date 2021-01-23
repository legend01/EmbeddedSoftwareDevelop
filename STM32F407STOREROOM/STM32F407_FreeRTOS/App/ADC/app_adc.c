/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-18 09:30:40
 * @LastEditTime: 2021-01-23 11:11:21
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
        ad += ADC_Conversion[i][ch];
    }
    ad /= SAMPLE_N;
    if(ad > 4000){ad = 4000;}
    else(100-(ad/40));
    *value = ad;
    return true;
}

//获得ADC值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
uint16_t Get_Adc(uint32_t ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //通道
    ADC1_ChanConf.Rank=1;                                       //1个序列
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_480CYCLES;        //采样时间
    ADC1_ChanConf.Offset=0;                 
    HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);        //通道配置
	
    HAL_ADC_Start(&hadc1);                               //开启ADC
	
    HAL_ADC_PollForConversion(&hadc1,10);                //轮询转换
   
	return (uint16_t)HAL_ADC_GetValue(&hadc1);	            //返回最近一次ADC1规则组的转换结果
}
//获取指定通道的转换值，取times次,然后平均 
//times:获取次数
//返回值:通道ch的times次转换结果平均值
uint16_t Get_Adc_Average(uint32_t ch,uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 

//得到温度值
//返回值:温度值(扩大了100倍,单位:℃.)
short Get_Temprate(void)
{
	uint32_t adcx;
	short result;
 	double temperate;
	adcx=Get_Adc_Average(ADC_CHANNEL_TEMPSENSOR,10);//读取内部温度传感器通道,10次取平均 STM32F407ZGT6 的内部温度传感器固定的连接在 ADC1 的通道 16 上
	temperate=(float)adcx*(3.3/4096);		//电压值
	temperate=(temperate-0.76)/0.0025 + 25; //转换为温度值 
	result=temperate*=100;					//扩大100倍.
	return result;
}