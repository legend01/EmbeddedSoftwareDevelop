/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-23 11:52:01
 * @LastEditTime: 2021-02-28 14:30:20
 * @LastEditors: HLLI8
 */
#ifndef __USMART_FUNLIST_H__
#define __USMART_FUNLIST_H__
#include "usmart.h"

//这下面要包含所用到的函数所申明的头文件(用户自己添加) 
#include "tim.h"	 	
#include "led.h"
#include "usmart_receiveFromUsart.h"
#include "app_rtc.h"
#include "usmart_FunExe.h"
//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   
	(void*)delay_ms,"void delay_ms(uint16_t nms)",
 	(void*)delay_us,"void delay_us(uint16_t nus)",	 
		
	(void*)test_fun,"void test_fun(uint8_t sta)",

	(void*)RTC_Set_Time, "u8 RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)",
	(void*)RTC_Set_Date, "u8 RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)",
	
	(void*)RTC_Set_AlarmA, "u8 RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)",
	(void*)RTC_Set_WakeUp, "void RTC_Set_WakeUp(u32 wksel,u16 cnt)",
	(void *)usmart_print_TempeSensor, "void usmart_print_TempeSensor(void)",
	(void *)usmart_print_lightSensor, "void usmart_print_lightSensor(void)",
	(void *)usmart_print_ADC3Sensor, "void usmart_print_ADC3Sensor(void)",
	(void *)usmart_set_DAC1Vol, "void usmart_set_DAC1Vol(uint16_t value)",
	(void *)usmart_set_PWMCompare, "void usmart_set_PWMCompare(uint32_t pulse)",
	(void *)usmart_write_EEPROMData, "void usmart_write_EEPROMData(void)",
	(void *)usmart_read_EEPROMData, "void usmart_read_EEPROMData(void)",
	(void *)usmart_write_FlashData, "void usmart_write_FlashData(void)",
	(void *)usmart_read_FlashData, "void usmart_read_FlashData(void)",
	(void *)usmart_write_LocalFlashData, "void usmart_write_LocalFlashData(void)",
	(void *)usmart_read_LocalFlashData, "void usmart_read_LocalFlashData(void)",
};		

#endif // !__USMART_FUNLIST_H__

