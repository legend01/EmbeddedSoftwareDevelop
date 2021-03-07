#include "APP_USART.h"
#include "stm32f407xx.h"

#include "stdio.h"
#include "assert.h"

UART_STR   Uart1_Str,Uart6_Str;  

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
short Uart1_DMA_Sent(char * Sendbuff, short Bufflens)
{
 /**
  * @description: TODO:可以利用FreeRTOS中的Queue函数利用生产者和消费者模型
  */   
 	assert(*Sendbuff != NULL);
	short l_val = Bufflens > UART_BUFFSIZE ? UART_BUFFSIZE : Bufflens;
	int ret = 0x00;
	if(Bufflens <= 0)
	{
		return 0;
	}
	while(__HAL_DMA_GET_COUNTER(&hdma_usart1_tx));
	if(Sendbuff)
	{
		memcpy(Uart1_Str.Uart_SentBuff, Sendbuff, l_val);
	}
	ret = HAL_UART_Transision_DMA(&huart1, Uart1_Str.Uart_SentBuff, l_val);
	return l_val;
}
/**
 * @description: 串口6初始化
 * @param {*}
 * !_DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
 */
void Uart6_DMA_Init(void){
	HAL_UART_Receive_DMA(&huart6, Uart6_Str.Uart_RecvBuff, UART_BUFFSIZE);
}

/*
函数功能：串????3DMA data send
函数形参：Sendbuff ：data send buffer
          Bufflens: data length
函数返回值：数据长度
备注：无
*/
short Uart6_DMA_Sent(char * Sendbuff, short Bufflens)
{
 /**
  * @description: TODO:可以利用FreeRTOS中的Queue函数利用生产者和消费者模型
  */   
 	assert(*Sendbuff != NULL);
	short l_val = Bufflens > UART_BUFFSIZE ? UART_BUFFSIZE : Bufflens;
	int ret = 0x00;
	if(Bufflens <= 0)
	{
		return 0;
	}
	while(__HAL_DMA_GET_COUNTER(&hdma_usart6_tx));
	if(Sendbuff)
	{
		memcpy(Uart6_Str.Uart_SentBuff, Sendbuff, l_val);
	}
	ret = HAL_UART_Transision_DMA(&huart6, Uart6_Str.Uart_SentBuff, l_val);
	return l_val;
}
/*
*函数功能：serial port 3 reseive exit function
*@NOTE:hdma_usart6_rx.Instance->NDTR为获取DMA中未传输的数据个数
*/
void IRQ_UART6_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&huart6, UART_FLAG_IDLE) != RESET) //获取IDLE标志位 //idle标志被置位
	{
		/* 清除状???寄存器和串口数据寄存器 */
		__HAL_UART_CLEAR_IDLEFLAG(&huart6);
		/* 失能DMA接收 */
		HAL_UART_DMAStop(&huart6);
		Uart6_Str.Uart_RecvLens  = UART_BUFFSIZE -  hdma_usart6_rx.Instance->NDTR;; // 通过DMA接收指针计算接收的字节数
		Uart6_Str.Receive_flag = 1;
		HAL_UART_Receive_DMA(&huart6, Uart6_Str.Uart_RecvBuff, UART_BUFFSIZE);
		__HAL_UART_CLEAR_IDLEFLAG(&huart6);
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
	/* if(Uartx == USART2){
		return(Uart_Receive_Data(&Uart2_Str, RcvBuff, RevLen));
	}
	else  */if(Uartx == USART6)
	{
		return(Uart_Receive_Data(&Uart6_Str, RcvBuff, RevLen));
	}
	return FALSE;
}

//重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
	if(ch != NULL){
		/* 发???一个字节数据到USART1 */
		Uart1_DMA_Sent((uint8_t*)&ch, 1);
		/* 等待发送完成 */
		while (HAL_UART_GetState(&huart1) == HAL_UART_STATE_RESET);
	}
	return (ch);
}