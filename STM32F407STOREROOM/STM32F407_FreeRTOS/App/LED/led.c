/*
 * @Description: LED灯设置函数
 * @Author: HLLI8
 * @Date: 2020-12-06 10:47:10
 * @LastEditTime: 2020-12-29 20:35:21
 * @LastEditors: HLLI8
 */
#include "led.h"
#include "stm32f4xx.h"

//LED状态设置函数
static void led_set(LED_STATE sta)
{
    if (sta == LED_dark)
    {
        LED0_0;
    }else if (sta == LED_light){
        LED0_1;
    }
} 
//函数参数调用测试函数
void test_fun(LED_STATE sta)
{
	led_set(sta);
}   