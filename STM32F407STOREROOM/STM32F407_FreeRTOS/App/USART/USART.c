#include "USART.h"

#include "stdio.h"
UART_STR   Uart1_Str,Uart2_Str,Uart3_Str;  // ���崮�ڷ�???���ջ�����

static void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  huart->gState = HAL_UART_STATE_READY;
}
static int HAL_UART_Transision_DMA(UART_HandleTypeDef *huart, char* buf, short buf_size){
  int ret = HAL_UART_Transmit_DMA(huart, (uint8_t*)buf, buf_size);
  HAL_UART_TxCpltCallback(huart);
  return ret;
}
/*
�������ܣ���????3DMA data send
�����βΣ�Sendbuff ��data send buffer
          Bufflens: data length
��������ֵ�����ݳ���
��ע����
*/
short Uart3_DMA_Sent(char * Sendbuff, short Bufflens)
{
 /**
  * @description: 
  * @param {*}
  * @return {*}
  * @TODO:����??????������flag��־���û�ֻ??????Ҫÿ�ν���������??????��ѭ�������У��ڷ�???���ݵ�Task��һֱ����������flag
  * ���Ϊ��Ҫ���ͣ���ֱ�����ͳ�ȥ???
  * 1.����??????���ṹ�壬���ÿ�ζ��е��׵�??????�ͱ�־λ
  * 2.�������������ͳ�����??????
  * 3.��װ���û�ʹ??????
  */ 
	short l_val = Bufflens > UART_BUFFSIZE ? UART_BUFFSIZE : Bufflens;
	int ret = 0x00;
  if(Bufflens <= 0)
	{
		return 0;
	}
	while(__HAL_DMA_GET_COUNTER(&hdma_usart3_tx));//???????��DMA��??????���ڻ���û����???????
	if(Sendbuff)
	{
		memcpy(Uart3_Str.Uart_SentBuff, Sendbuff, l_val);
	}
  ret = HAL_UART_Transision_DMA(&huart3, Uart3_Str.Uart_SentBuff, l_val);
  return l_val;
}

/*
*�������ܣ�serial port 3 reseive exit function
*/
void IRQ_USART3_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET)
	{
		/* ���״???�Ĵ����ʹ������ݼĴ��� */
    __HAL_UART_CLEAR_IDLEFLAG(&huart3);
    /* ʧ��DMA���� */
    HAL_UART_DMAStop(&huart3);
    Uart3_Str.Uart_RecvLens  = UART_BUFFSIZE - DMA1_Channel3->CNDTR; // ͨ��DMA����ָ�������յ��ֽ���
		Uart3_Str.Receive_flag = 1;
    HAL_UART_Receive_DMA(&huart3, Uart3_Str.Uart_RecvBuff, UART_BUFFSIZE);
		__HAL_UART_CLEAR_IDLEFLAG(&huart3);
	}
}
/*
�������ܣ�receive data functions
�����β�???????* Uart_Str ??????? �������ݻ���ṹ��ַ
	    RcvBuff ���������ݻ�����  
	    RevLen  �����ջ���������
��������ֵ���������ݳ���
��ע����
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
�������ܣ��Ӵ��ڻ�ȡ����
�����β�???????* Uartx �����ڵ�???????
         RcvBuff �����ջ���ָ???????
         RevLen  �����ջ�������С
��������ֵ���������ݳ���
��ע����
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

//�ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
/* ��???һ���ֽ����ݵ�USART1 */
  // USART_SendData(USART1, (uint8_t) ch);
  Uart3_DMA_Sent((uint8_t*)&ch, 1);
 	/* �ȴ���???��??? */
 	while (HAL_UART_GetState(&huart3) == HAL_UART_STATE_RESET);
 	return (ch);
}