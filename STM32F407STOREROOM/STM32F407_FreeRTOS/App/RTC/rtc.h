/*
 * @Description: RTCʱ��
 * @Author: HLLI8
 * @Date: 2021-01-09 14:54:49
 * @LastEditTime: 2021-01-09 14:56:28
 * @LastEditors: HLLI8
 */
#ifndef __RTC_H
#define __RTC_H

HAL_StatusTypeDef RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);      //RTCʱ������
HAL_StatusTypeDef RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);	//RTC��������
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec); //��������ʱ��(����������,24Сʱ��)
void RTC_Set_WakeUp(u32 wksel,u16 cnt);             //�����Ի��Ѷ�ʱ������
#endif
