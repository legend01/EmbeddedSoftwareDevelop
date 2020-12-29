/*
 * @Description: USMART串口处理函数
 * @Author: HLLI8
 * @Date: 2020-12-27 15:16:41
 * @LastEditTime: 2020-12-27 16:46:30
 * @LastEditors: HLLI8
 */
#ifndef __USMART_RECEIVEFROMUSART_C__
#define __USMART_RECEIVEFROMUSART_C__
#include "usart.h"
#define USMART_BUFFSIZE 200

typedef struct{
    short Usmart_RecvLens;
    volatile short Usmart_Receive_Flag;
    char Usmart_RecvBuff[USMART_BUFFSIZE];
}USMART_RECV_STR;

void IRQ_USART1_IRQHandler(void);
#endif // !__USMART_RECEIVEFROMUSART_C__