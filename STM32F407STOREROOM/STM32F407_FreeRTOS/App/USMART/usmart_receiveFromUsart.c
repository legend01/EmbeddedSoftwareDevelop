/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2020-12-27 15:16:20
 * @LastEditTime: 2021-01-04 20:46:41
 * @LastEditors: HLLI8
 */
#include "usmart_receiveFromUsart.h"
#include "APP_USART.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usmart.h"

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

////////////////////////////////////////////////////////////////////////////////////////
//移植注意:本例是以stm32为例,如果要移植到其他mcu,请做相应修改.
//usmart_reset_runtime,清除函数运行时间,连同定时器的计数寄存器以及标志位一起清零.并设置重装载值为最大,以最大限度的延长计时时间.
//usmart_get_runtime,获取函数运行时间,通过读取CNT值获取,由于usmart是通过中断调用的函数,所以定时器中断不再有效,此时最大限度
//只能统计2次CNT的值,也就是清零后+溢出一次,当溢出超过2次,没法处理,所以最大延时,控制在:2*计数器CNT*0.1ms.对STM32来说,是:13.1s左右
//其他的:TIM4_IRQHandler和Timer4_Init,需要根据MCU特点自行修改.确保计数器计数频率为:10Khz即可.另外,定时器不要开启自动重装载功能!!

#if USMART_ENTIMX_SCAN==1
//复位runtime
//需要根据所移植到的MCU的定时器参数进行修改
void usmart_reset_runtime(void)
{
    __HAL_TIM_CLEAR_FLAG(&htim4,TIM_FLAG_UPDATE);//清除中断标志位 
    __HAL_TIM_SET_AUTORELOAD(&htim4,0XFFFF);     //将重装载值设置到最大
    __HAL_TIM_SET_COUNTER(&htim4,0);             //清空定时器的CNT
	usmart_dev.runtime=0;	
}
//获得runtime时间
//返回值:执行时间,单位:0.1ms,最大延时时间为定时器CNT值的2倍*0.1ms
//需要根据所移植到的MCU的定时器参数进行修改
uint32_t usmart_get_runtime(void)
{
	if(__HAL_TIM_GET_FLAG(&htim4,TIM_FLAG_UPDATE)==SET)//在运行期间,产生了定时器溢出
	{
		usmart_dev.runtime+=0XFFFF;
	}
	usmart_dev.runtime+=__HAL_TIM_GET_COUNTER(&htim4);
	return usmart_dev.runtime;		//返回计数值
}  
//下面这两个函数,非USMART函数,放到这里,仅仅方便移植. 
//定时器4中断服务程序	 
void IRQ_TIM4_IRQHandler(void)
{ 		    		  			       
    if(__HAL_TIM_GET_IT_SOURCE(&htim4,TIM_IT_UPDATE)==SET)//溢出中断
    {
        usmart_dev.scan();	//执行usmart扫描
        __HAL_TIM_SET_COUNTER(&htim4,0);;    //清空定时器的CNT
        __HAL_TIM_SET_AUTORELOAD(&htim4,100);//恢复原来的设置
    }
    __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);//清除中断标志位
}

#endif
////////////////////////////////////////////////////////////////////////////////////////