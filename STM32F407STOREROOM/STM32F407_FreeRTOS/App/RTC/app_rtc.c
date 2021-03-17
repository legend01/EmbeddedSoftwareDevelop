/*
 * @Description: RTC时钟 设置时间 获取时间
 * @Author: HLLI8
 * @Date: 2021-01-09 14:54:49
 * @LastEditTime: 2021-03-17 22:23:37
 * @LastEditors: HLLI8
 */
#include "app_rtc.h"
#include "sys_stdlib.h"
#include "log_printf.h"

//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions:RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
HAL_StatusTypeDef RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_TimeStructure.Hours=hour;
	RTC_TimeStructure.Minutes=min;
	RTC_TimeStructure.Seconds=sec;
	RTC_TimeStructure.TimeFormat=ampm;
	RTC_TimeStructure.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStructure.StoreOperation=RTC_STOREOPERATION_RESET;
	return HAL_RTC_SetTime(&hrtc,&RTC_TimeStructure,RTC_FORMAT_BIN);	
}

//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
HAL_StatusTypeDef RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	RTC_DateTypeDef RTC_DateStructure;
    
	RTC_DateStructure.Date=date;
	RTC_DateStructure.Month=month;
	RTC_DateStructure.WeekDay=week;
	RTC_DateStructure.Year=year;
	return HAL_RTC_SetDate(&hrtc,&RTC_DateStructure,RTC_FORMAT_BIN);
}

void RTC_Get_SysTime(void){
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    OutPut_Time_STR OutPutInfo_Container;
    
    memset(&OutPutInfo_Container.Time_Info, 0, sizeof(OutPutInfo_Container.Time_Info)/sizeof(OutPutInfo_Container.Time_Info[0]));
    HAL_RTC_GetTime(&hrtc,&RTC_TimeStruct,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc,&RTC_DateStruct,RTC_FORMAT_BIN);
    
    sys_sprintf(OutPutInfo_Container.Time_Info, "20%02d-%02d-%02d_%02d:%02d:%02d>>>", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date, RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);

    Uart3_DMA_Sent(OutPutInfo_Container.Time_Info, sizeof(OutPutInfo_Container.Time_Info)/sizeof(OutPutInfo_Container.Time_Info[0]));
}

//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_WeekDay_Definitions
//hour,min,sec:小时,分钟,秒钟
HAL_StatusTypeDef  RTC_Set_AlarmA(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec)
{ 
    RTC_AlarmTypeDef RTC_AlarmSturuct;
    
    RTC_AlarmSturuct.AlarmTime.Hours=hour;  //小时
    RTC_AlarmSturuct.AlarmTime.Minutes=min; //分钟
    RTC_AlarmSturuct.AlarmTime.Seconds=sec; //秒
    RTC_AlarmSturuct.AlarmTime.SubSeconds=0;
    RTC_AlarmSturuct.AlarmTime.TimeFormat=RTC_HOURFORMAT12_AM;
    
    RTC_AlarmSturuct.AlarmMask=RTC_ALARMMASK_NONE;//精确匹配星期，时分秒
    RTC_AlarmSturuct.AlarmSubSecondMask=RTC_ALARMSUBSECONDMASK_NONE;
    RTC_AlarmSturuct.AlarmDateWeekDaySel=RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;//按星期
    RTC_AlarmSturuct.AlarmDateWeekDay=week; //星期
    RTC_AlarmSturuct.Alarm=RTC_ALARM_A;     //闹钟A
    if(HAL_RTC_SetAlarm_IT(&hrtc,&RTC_AlarmSturuct,RTC_FORMAT_BIN)) return HAL_ERROR;
    return HAL_OK;
}

//周期性唤醒定时器设置  
/*wksel:  @ref RTCEx_Wakeup_Timer_Definitions
#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(uint32_t wksel,uint16_t cnt)
{ 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
	
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc,cnt,wksel);            //设置重装载值和时钟 
}

void RTC_WakeUp_Init(void){
    RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); //配置WAKE UP中断,1秒钟中断一次  
}

//RTC闹钟A中断处理回调函数
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    log_printf("ALARM A!\r\n");
    __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRAF);
}

//RTC WAKE UP中断处理
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    log_printf("RTC WAKE UP \r\n");
}

