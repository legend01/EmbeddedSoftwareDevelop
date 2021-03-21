/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "stm32f4xx.h"
#include "led.h"
#include "beep.h"
#include "APP_USART.h"
#include "tim.h"
#include "pwm.h"
#include "inputCapture.h"
#include "tpad.h"
#include "usmart_receiveFromUsart.h"
#include "sys_stdlib.h"
#include "log_printf.h"
#include "app_rtc.h"
#include "J1939x.h"
#include "app_can.h"
#include "app_adc.h"
#include "w25qxx.h"
#include "BMS.h"
#include "remote_control.h"
#include "XMRAM.h"
#include "BMS_BMS.h"
#include "lwip_comm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern USMART_RECV_STR usmart_receiveSTR;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId ThreadTask02Handle;
osThreadId ThreadTask03Handle;
osThreadId ThreadTask04Handle;
osThreadId ThreadTask05Handle;
osMessageQId USMARTQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void USART1ManageFuc(void const * argument);
void USMARTManageFuc(void const * argument);
void J1939ManageFuc(void const * argument);
void LwipManageFuc(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of ThreadTask02 */
  osThreadDef(ThreadTask02, USART1ManageFuc, osPriorityNormal, 0, 128);
  ThreadTask02Handle = osThreadCreate(osThread(ThreadTask02), NULL);

  /* definition and creation of ThreadTask03 */
  osThreadDef(ThreadTask03, USMARTManageFuc, osPriorityIdle, 0, 128);
  ThreadTask03Handle = osThreadCreate(osThread(ThreadTask03), NULL);

  /* definition and creation of ThreadTask04 */
  osThreadDef(ThreadTask04, J1939ManageFuc, osPriorityIdle, 0, 128);
  ThreadTask04Handle = osThreadCreate(osThread(ThreadTask04), NULL);

  /* definition and creation of ThreadTask05 */
  osThreadDef(ThreadTask05, LwipManageFuc, osPriorityIdle, 0, 128);
  ThreadTask05Handle = osThreadCreate(osThread(ThreadTask05), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of USMARTQueue */
/* what about the sizeof here??? cd native code */
  osMessageQDef(USMARTQueue, 8, usmart_receiveSTR);
  USMARTQueueHandle = osMessageCreate(osMessageQ(USMARTQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  Hal_TimStart_Init();
  // TPAD_Init();
  usmart_init();
  Uart3_DMA_Init();
  RTC_WakeUp_Init();
  Ringbuff_init();
  CAN_Filter_Config();
  W25QXX_Init();
  HAL_ADC_Start_DMA(&hadc3, (uint32_t *)&ADC_Conversion[0], SAMPLE_N * ADC_TOTAL_CH_NUM);
  uint16_t w25qxxID = W25QXX_ReadID();
  LOG_PRINTF("W25QXX ID:0x%x \r\n", w25qxxID);
  XmRamInit(); /* 初始化XMRAM */

  while (lwip_comm_init())
  {
    LOG_PRINTF("LWIP Init Falied \r\n");
    delay_ms(500);
    LOG_PRINTF("Retrying ...\r\n");
  }
  lwip_dhcp_process();

  for (;;)
  {
    char receive_buf[200];
    memset(receive_buf, 0, sizeof(receive_buf) / sizeof(receive_buf[0]));
    Get_Uart_Data(USART1, receive_buf, sizeof(receive_buf) / sizeof(receive_buf[0]));
    // if(TPAD_Scan(0)){
    //   LED1_Toggle;
    // }
    // delay_ms(150);

    InputCapture();
    // printf("%s\n", receive_buf);
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_USART1ManageFuc */
/**
* @brief Function implementing the ThreadTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_USART1ManageFuc */
void USART1ManageFuc(void const * argument)
{
  /* USER CODE BEGIN USART1ManageFuc */
  /* Infinite loop */
  for (;;)
  {
    // SetLight0Pwm();
    // delay_ms(1000);
    osDelay(1);
  }
  /* USER CODE END USART1ManageFuc */
}

/* USER CODE BEGIN Header_USMARTManageFuc */
/**
* @brief Function implementing the ThreadTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_USMARTManageFuc */
void USMARTManageFuc(void const * argument)
{
  /* USER CODE BEGIN USMARTManageFuc */
  /* Infinite loop */
  for (;;)
  {
    Remote_control();
    osDelay(1);
  }
  /* USER CODE END USMARTManageFuc */
}

/* USER CODE BEGIN Header_J1939ManageFuc */
/**
* @brief Function implementing the ThreadTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_J1939ManageFuc */
void J1939ManageFuc(void const * argument)
{
  /* USER CODE BEGIN J1939ManageFuc */
  CAN_Register(GetmessageToRcvbuff, NULL);
  BMSmanager_Init();
  /* Infinite loop */
  for (;;)
  {
    GtmgFrRcvbufToPGN();
    GetmsgconvertToSend();

    // char data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x11, 0x12};
    // BMS_Send_message(BRM, data);

    // uint8_t Getmsglen = BMS_Get_message(CHM, &(BMSmanager.messageData));
    // LOG_PRINTF("GetCHM message: \r\n");
    // for (uint8_t i = 0; i < 8; i++)
    // {
    //   LOG_PRINTF("0x %x \r\n", BMSmanager.messageData[i]);
    // }
    osDelay(1);
  }
  /* USER CODE END J1939ManageFuc */
}

/* USER CODE BEGIN Header_LwipManageFuc */
/**
* @brief Function implementing the myTask05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LwipManageFuc */
void LwipManageFuc(void const * argument)
{
  /* USER CODE BEGIN LwipManageFuc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LwipManageFuc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
