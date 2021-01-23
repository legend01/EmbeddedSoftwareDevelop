/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-23 13:52:04
 * @LastEditTime: 2021-01-23 14:00:12
 * @LastEditors: HLLI8
 */
#include "app_dac.h"

//设置通道1输出电压
//vol:0~3300,代表0~3.3V
void DAC1_Set_Vol(uint16_t vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
    HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);//12位右对齐数据格式设置DAC值
}
