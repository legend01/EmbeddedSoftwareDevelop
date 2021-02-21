/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2020-12-20 12:04:35
 * @LastEditTime: 2021-02-21 15:25:09
 * @LastEditors: HLLI8
 */
#include "tim.h"
#include "inputCapture.h"
#include "remote.h"
uint32_t temp = 0;	 	// 定义一个变量用以存储捕获到的时间 long long型是为了防止数据溢出
/*	bit7 捕获完成标识	bit6 捕获到高电平标识	bit5~0 捕获高电平后定时器溢出的次数 */
volatile uint8_t	TIM5CH1_CAPTURE_STA = 0;	// 输入捕获状态
volatile uint32_t	TIM5CH1_CAPTURE_VAL;		// 输入捕获值(TIM2/TIM5是32位的定时器所以这里定义为uint32_t)

uint16_t Dval;		//下降沿时计数器的值

// 定时器输入捕获中断处理回调函数，该函数在 HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim) 中会被调用 
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM5){
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
  }else if(htim->Instance == TIM1){
    if(RDATA)//上升沿捕获
    {
      TIM_RESET_CAPTUREPOLARITY(&htim1,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
      TIM_SET_CAPTUREPOLARITY(&htim1,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//CC1P=1 设置为下降沿捕获
      __HAL_TIM_SET_COUNTER(&htim1,0);  //清空定时器值   	  
      RmtSta|=0X10;					//标记上升沿已经被捕获
    }else{ //下降沿捕获
      Dval=HAL_TIM_ReadCapturedValue(&htim1,TIM_CHANNEL_1);//读取CCR1也可以清CC1IF标志位
      TIM_RESET_CAPTUREPOLARITY(&htim1,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
      TIM_SET_CAPTUREPOLARITY(&htim1,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//配置TIM5通道1上升沿捕获
      if(RmtSta&0X10)					//完成一次高电平捕获 
      {
        if(RmtSta&0X80)//接收到了引导码
        {
              
          if(Dval>300&&Dval<800)			//560为标准值,560us
          {
            RmtRec<<=1;	//左移一位.
            RmtRec|=0;	//接收到0	   
          }else if(Dval>1400&&Dval<1800)	//1680为标准值,1680us
          {
            RmtRec<<=1;	//左移一位.
            RmtRec|=1;	//接收到1
          }else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
          {
            RmtCnt++; 		//按键次数增加1次
            RmtSta&=0XF0;	//清空计时器		
          }
        }else if(Dval>4200&&Dval<4700)		//4500为标准值4.5ms
        {
          RmtSta|=1<<7;	//标记成功接收到了引导码
          RmtCnt=0;		//清除按键次数计数器
        }						 
      }
      RmtSta&=~(1<<4);
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