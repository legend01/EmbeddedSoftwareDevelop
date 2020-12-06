#ifndef __USART_H__
#define __USART_H__

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

short Uart3_DMA_Sent(char * Sendbuff, short Bufflens);
short Get_Uart_Data(USART_TypeDef* Uartx,char * RcvBuff, short RevLen);
extern void IRQ_USART3_IRQHandler(void);

#endif // !__USART_H__
