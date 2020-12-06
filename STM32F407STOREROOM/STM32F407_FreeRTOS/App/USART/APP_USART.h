#ifndef __APP_USART_H__
#define __APP_USART_H__
#include "usart.h"
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
#define UART_BUFFSIZE    200   
typedef struct
{
	short Uart_SendLens;  
	short Uart_RecvLens;  
	volatile short Receive_flag;
	char Uart_SentBuff[UART_BUFFSIZE];
	char Uart_RecvBuff[UART_BUFFSIZE];
}UART_STR;
typedef enum
{
  usart_DMA_OK = 0x00,
  usart_DMA_ERROR = 0x01,
  usart_DMA_BUSY = 0x02,
  usart_DMA_TIMEOUT = 0x03
}USART2_DMAStatus;

short Uart1_DMA_Sent(char * Sendbuff, short Bufflens);
short Get_Uart_Data(USART_TypeDef* Uartx,char * RcvBuff, short RevLen);
extern void IRQ_USART1_IRQHandler(void);

#endif // !__USART_H__
