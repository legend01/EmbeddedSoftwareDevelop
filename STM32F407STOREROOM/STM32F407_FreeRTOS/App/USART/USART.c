#include "USART.h"

#include "stdio.h"
UART_STR   Uart1_Str,Uart2_Str,Uart3_Str;  // 定义串口发???接收缓冲区

static void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  huart->gState = HAL_UART_STATE_READY;
}
static int HAL_UART_Transision_DMA(UART_HandleTypeDef *huart, char* buf, short buf_size){
  int ret = HAL_UART_Transmit_DMA(huart, (uint8_t*)buf, buf_size);
  HAL_UART_TxCpltCallback(huart);
  return ret;
}
/*
函数功能：串????3DMA data send
函数形参：Sendbuff ：data send buffer
          Bufflens: data length
函数返回值：数据长度
备注：无
*/
short Uart3_DMA_Sent(char * Sendbuff, short Bufflens)
{
 /**
  * @description: 
  * @param {*}
  * @return {*}
  * @TODO:设置??????个发送flag标志，用户只??????要每次将数据塞进??????个循环链表中，在发???数据的Task中一直检查这个发送flag
  * 如果为需要发送，就直接推送出去???
  * 1.创建??????个结构体，存放每次队列的首地??????和标志位
  * 2.创建入链表函数和出链表函??????
  * 3.封装供用户使??????
  */ 
	short l_val = Bufflens > UART_BUFFSIZE ? UART_BUFFSIZE : Bufflens;
	int ret = 0x00;
  if(Bufflens <= 0)
	{
		return 0;
	}
	while(__HAL_DMA_GET_COUNTER(&hdma_usart3_tx));//???????测DMA发??????道内还有没有数???????
	if(Sendbuff)
	{
		memcpy(Uart3_Str.Uart_SentBuff, Sendbuff, l_val);
	}
  ret = HAL_UART_Transision_DMA(&huart3, Uart3_Str.Uart_SentBuff, l_val);
  return l_val;
}

/*
*函数功能：serial port 3 reseive exit function
*/
void IRQ_USART3_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET)
	{
		/* 清除状???寄存器和串口数据寄存器 */
    __HAL_UART_CLEAR_IDLEFLAG(&huart3);
    /* 失能DMA接收 */
    HAL_UART_DMAStop(&huart3);
    Uart3_Str.Uart_RecvLens  = UART_BUFFSIZE - DMA1_Channel3->CNDTR; // 通过DMA接收指针计算接收的字节数
		Uart3_Str.Receive_flag = 1;
    HAL_UART_Receive_DMA(&huart3, Uart3_Str.Uart_RecvBuff, UART_BUFFSIZE);
		__HAL_UART_CLEAR_IDLEFLAG(&huart3);
	}
}
/*
函数功能：receive data functions
函数形参???????* Uart_Str ??????? 串口数据缓冲结构地址
	    RcvBuff ：接收数据缓冲区  
	    RevLen  ：接收缓冲区长度
函数返回值：接收数据长度
备注：无
*/
static short Uart_Receive_Data(UART_STR * Uart_Str, char * RcvBuff, short RevLen)
{
  if (Uart_Str->Receive_flag)
  {
    if(Uart_Str->Uart_RecvLens > UART_BUFFSIZE){
      memcpy(RcvBuff, Uart_Str->Uart_RecvBuff, UART_BUFFSIZE);
      Uart_Str->Receive_flag = 0;
      return UART_BUFFSIZE;
    }else{
      memcpy(RcvBuff, Uart_Str->Uart_RecvBuff, Uart_Str->Uart_RecvLens);
      Uart_Str->Receive_flag = 0;
      return Uart_Str->Uart_RecvLens;
    }
  }
  
}
/*
函数功能：从串口获取数据
函数形参???????* Uartx ：串口地???????
         RcvBuff ：接收缓冲指???????
         RevLen  ：接收缓冲区大小
函数返回值：接收数据长度
备注：无
*/
short Get_Uart_Data(USART_TypeDef* Uartx,char * RcvBuff, short RevLen)
{
	if(Uartx == USART1)
	{
		return(Uart_Receive_Data(&Uart1_Str, RcvBuff, RevLen));
	}
	else if(Uartx == USART2)
	{
		return(Uart_Receive_Data(&Uart2_Str, RcvBuff, RevLen));
	}
	else if(Uartx == USART3)
	{
		return(Uart_Receive_Data(&Uart3_Str, RcvBuff, RevLen));
	}
	return 0;
}

//重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
/* 发???一个字节数据到USART1 */
  // USART_SendData(USART1, (uint8_t) ch);
  Uart3_DMA_Sent((uint8_t*)&ch, 1);
 	/* 等待发???完??? */
 	while (HAL_UART_GetState(&huart3) == HAL_UART_STATE_RESET);
 	return (ch);
}