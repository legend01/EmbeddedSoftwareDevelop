/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2020-12-20 12:04:35
 * @LastEditTime: 2021-01-10 16:54:11
 * @LastEditors: HLLI8
 */
#include "tim.h"
#include "inputCapture.h"
uint32_t temp = 0;	 	// 定义一个变量用以存储捕获到的时间 long long型是为了防止数据溢出
/*	bit7 捕获完成标识	bit6 捕获到高电平标识	bit5~0 捕获高电平后定时器溢出的次数 */
volatile uint8_t	TIM5CH1_CAPTURE_STA = 0;	// 输入捕获状态
volatile uint32_t	TIM5CH1_CAPTURE_VAL;		// 输入捕获值(TIM2/TIM5是32位的定时器所以这里定义为uint32_t)

// 定时器输入捕获中断处理回调函数，该函数在 HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim) 中会被调用 
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if ( (TIM5CH1_CAPTURE_STA & 0x80) == 0 )	// 还未成功捕获
  {
    if (TIM5CH1_CAPTURE_STA & 0x40)			// 捕获到一个下降沿
    {
      TIM5CH1_CAPTURE_STA |= 0x80;		// 标记成功捕获到一次高电平脉宽
      TIM5CH1_CAPTURE_VAL = HAL_TIM_ReadCapturedValue(&htim5, TIM_CHANNEL_1);	// 获取当前的捕获值. 即CCRx2
      TIM_RESET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_1);						// 清除原来的设置
      TIM_SET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);	// 配置TIM5通道1上升沿捕获
    }else{
      TIM5CH1_CAPTURE_STA = 0;	// 清空自定义的状态寄存器
      TIM5CH1_CAPTURE_VAL = 0;	// 清空捕获值
      TIM5CH1_CAPTURE_STA |= 0x40;// 标记捕获到了上升沿
      __HAL_TIM_DISABLE(&htim5);	//关闭定时器5
      __HAL_TIM_SET_COUNTER(&htim5,0);
      TIM_RESET_CAPTUREPOLARITY(&htim5,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
      TIM_SET_CAPTUREPOLARITY(&htim5,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//定时器5通道1设置为下降沿捕获
      __HAL_TIM_ENABLE(&htim5);//使能定时器5
    }
  }
}

void InputCapture(void){
    if (TIM5CH1_CAPTURE_STA & 0x80)   // 如果捕获完成
    {
        temp = TIM5CH1_CAPTURE_STA & 0x3f;
        temp *= 0xffff;				// Total Overflow Time(总的溢出时间)
        temp += TIM5CH1_CAPTURE_VAL;    // Get Total High Level Time(获取总的高电平时长)
        log_printf("HIGH: %04dms\r\n", temp/1000); // Print Total High Level Time(打印总的高电平时长)
        TIM5CH1_CAPTURE_STA = 0;			    // Clear Capture State , Open The Next Capture(清除捕获状态，打开下一次捕获)
    }
}