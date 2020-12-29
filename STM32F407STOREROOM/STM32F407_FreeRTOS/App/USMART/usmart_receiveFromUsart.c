/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2020-12-27 15:16:20
 * @LastEditTime: 2020-12-29 22:14:32
 * @LastEditors: HLLI8
 */
#include "usmart_receiveFromUsart.h"
#include "APP_USART.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

USMART_RECV_STR usmart_receiveSTR;
extern osMessageQId USMARTQueueHandle;

/*
*函数功能：serial port 3 reseive exit function
*@NOTE:hdma_usart1_rx.Instance->NDTR为获取DMA中未传输的数据个数
*/
void IRQ_USART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET) //获取IDLE标志位 //idle标志被置位
	{
		/* 清除状???寄存器和串口数据寄存器 */
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		/* 失能DMA接收 */
		HAL_UART_DMAStop(&huart1);
		usmart_receiveSTR.Usmart_RecvLens  = UART_BUFFSIZE -  hdma_usart1_rx.Instance->NDTR;; // 通过DMA接收指针计算接收的字节数
		usmart_receiveSTR.Usmart_Receive_Flag = 1;
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
        
        HAL_UART_Receive_DMA(&huart1, usmart_receiveSTR.Usmart_RecvBuff, UART_BUFFSIZE);
        xQueueSendFromISR(USMARTQueueHandle, &usmart_receiveSTR, &xHigherPriorityTaskWoken);
        /* 如果xHigherPriorityTaskWoken = pdTRUE,退出中断后切到当前最高优先级任务中执行 */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}