/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2020 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
UART_STR   Uart1_Str,Uart2_Str,Uart3_Str;  // 定义串口发�?�接收缓冲区

/* USER CODE END 0 */

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Channel7;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
static void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  huart->gState = HAL_UART_STATE_READY;
}
static int HAL_UART_Transision_DMA(UART_HandleTypeDef *huart, char* buf, short buf_size){
  int ret = HAL_UART_Transmit_DMA(huart, (uint8_t*)buf, buf_size);
  HAL_UART_TxCpltCallback(huart);
  return ret;
}
/*
函数功能：串�?2DMA数据发�??
函数形参：Sendbuff ：缓冲数�?
          Bufflens ：数据长�?
函数返回值：数据长度
备注：无
*/
short Uart2_DMA_Sent(char * Sendbuff, short Bufflens)
{
 /**
  * @description: 
  * @param {*}
  * @return {*}
  * @TODO:设置一个发送flag标志，用户只需要每次将数据塞进一个循环链表中，在发送数据的Task中一直检查这个发送flag
  * 如果为需要发送，就直接推送出去。
  * 1.创建一个结构体，存放每次队列的首地址和标志位
  * 2.创建入链表函数和出链表函数
  * 3.封装供用户使用
  */ 
	short l_val = Bufflens > UART_BUFFSIZE ? UART_BUFFSIZE : Bufflens;
	int ret = 0x00;
  if(Bufflens <= 0)
	{
		return 0;
	}
	while(__HAL_DMA_GET_COUNTER(&hdma_usart2_tx));//�?测DMA发�?��?�道内还有没有数�?
	if(Sendbuff)
	{
		memcpy(Uart2_Str.Uart_SentBuff, Sendbuff, l_val);
	}
  ret = HAL_UART_Transision_DMA(&huart2, Uart2_Str.Uart_SentBuff, l_val);
  return l_val;
}

/*
*函数功能：串�?1接收中断函数
*/
void USART2_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
	{
		short l_val;
		l_val = UART_BUFFSIZE - DMA1_Channel6->CNDTR; // 通过DMA接收指针计算接收的字节数
		if(l_val > Uart2_Str.RecvQue_Tail)
		{
			Uart2_Str.Uart_RecvLens += l_val - Uart2_Str.RecvQue_Tail;
		}
		else
		{
			Uart2_Str.Uart_RecvLens += UART_BUFFSIZE - Uart2_Str.RecvQue_Tail + l_val;
		}
		Uart2_Str.RecvQue_Tail = l_val;
		Uart2_Str.Uart_RecvLens %= UART_BUFFSIZE;
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
	}
	HAL_UART_IRQHandler(&huart2); // 这一条保留就行了  
}
/*
函数功能：接收数据函�?
函数形参�?* Uart_Str �? 串口数据缓冲结构地址
	    RcvBuff ：接收数据缓冲区  
	    RevLen  ：接收缓冲区长度
函数返回值：接收数据长度
备注：无
*/
static short Uart_Receive_Data(UART_STR * Uart_Str, char * RcvBuff, short RevLen)
{
	short l_val = 0;            // �?部变�?   此次能拷贝的数据个数
	if(!RevLen || !Uart_Str->Uart_RecvLens)
	{
		return 0;
	}
	l_val = Uart_Str->Uart_RecvLens;
	
	if(l_val > RevLen)  // 缓冲区的数据数量比传进来的缓冲区容量�?
	{
		l_val = RevLen;
	}
	if(Uart_Str->RecvQue_Tail > Uart_Str->RecvQue_Head)
	{
		memcpy(RcvBuff, Uart_Str->Uart_RecvBuff + Uart_Str->RecvQue_Head, l_val);
	}
	else
	{
		if((UART_BUFFSIZE - Uart_Str->RecvQue_Head) >= l_val)
		{
			memcpy(RcvBuff, Uart_Str->Uart_RecvBuff + Uart_Str->RecvQue_Head, l_val);
		}
		else
		{
			memcpy(RcvBuff, Uart_Str->Uart_RecvBuff + Uart_Str->RecvQue_Head, UART_BUFFSIZE - Uart_Str->RecvQue_Head);
			memcpy(RcvBuff + (UART_BUFFSIZE - Uart_Str->RecvQue_Head), Uart_Str->Uart_RecvBuff, l_val - (UART_BUFFSIZE - Uart_Str->RecvQue_Head));
		}
	}
	Uart_Str->Uart_RecvLens -= l_val;
	Uart_Str->RecvQue_Head += l_val;
	Uart_Str->RecvQue_Head %= UART_BUFFSIZE;
	return l_val;
}
/*
函数功能：从串口获取数据
函数形参�?* Uartx ：串口地�?
         RcvBuff ：接收缓冲指�?
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
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
