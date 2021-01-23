/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-23 11:07:34
 * @LastEditTime: 2021-01-23 12:09:46
 * @LastEditors: HLLI8
 */
#include "usmart_FunExe.h"
#include "log_printf.h"

void usmart_print_lightSensor(void){
    uint16_t value = 0;
    if(GetLightSensor_value((uint8_t *)&value, GPIOF15_LightSensor_Channel)){
        USMART_RETURN("LightSensor: %d \r\n", value);
    }else{
        USMART_RETURN("don`t get value \r\n");
    }
}

void usmart_print_TempeSensor(void){
    USMART_RETURN("Temperature: %d \r\n", Get_Temprate());
}

void usmart_print_ADC3Sensor(void){
    uint16_t value = 0;
    if(GetADC3_value((uint8_t *)&value, GPIOF5_Channel)){
        USMART_RETURN("ADC3Sensor: %d \r\n", value);
    }else{
        USMART_RETURN("don`t get value \r\n");
    }
}