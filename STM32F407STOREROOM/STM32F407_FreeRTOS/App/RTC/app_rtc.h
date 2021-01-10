/*
 * @Description: RTCʱ��
 * @Author: HLLI8
 * @Date: 2021-01-09 14:54:49
 * @LastEditTime: 2021-01-09 16:39:47
 * @LastEditors: HLLI8
 */
#ifndef __RTC_H
#define __RTC_H
#include "rtc.h"
extern RTC_HandleTypeDef hrtc;

typedef struct{
    char Time_Info[150];
}OutPut_Time_STR;

HAL_StatusTypeDef RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm);      //RTCʱ������
HAL_StatusTypeDef RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week);	//RTC��������
HAL_StatusTypeDef RTC_Set_AlarmA(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec); //��������ʱ��(����������,24Сʱ��)
void RTC_Set_WakeUp(uint32_t wksel,uint16_t cnt);             //�����Ի��Ѷ�ʱ������
void RTC_WakeUp_Init(void);
void RTC_Get_SysTime(void);
#endif
