/*
 * @Description: USMART串口处理函数
 * @Author: HLLI8
 * @Date: 2020-12-27 15:16:41
 * @LastEditTime: 2021-01-04 21:57:36
 * @LastEditors: HLLI8
 */
#ifndef __USMART_RECEIVEFROMUSART_C__
#define __USMART_RECEIVEFROMUSART_C__
#include "usart.h"
#define USMART_BUFFSIZE 200


#define USMART_ENTIMX_SCAN 	1	//使用TIM的定时中断来扫描SCAN函数,如果设置为0,需要自己实现隔一段时间扫描一次scan函数.
								//注意:如果要用runtime统计功能,必须设置USMART_ENTIMX_SCAN为1!!!!
                                
typedef struct{
    short Usmart_RecvLens;
    volatile short Usmart_Receive_Flag;
    char Usmart_RecvBuff[USMART_BUFFSIZE];
}USMART_RECV_STR;

void usmart_init(void);//初始化

void IRQ_USART1_IRQHandler(void);
uint32_t usmart_get_runtime(void);	//获取运行时间
void usmart_reset_runtime(void);//复位运行时间
void IRQ_TIM4_IRQHandler(void);
#endif // !__USMART_RECEIVEFROMUSART_C__