/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-23 11:07:34
 * @LastEditTime: 2021-01-24 21:07:14
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

void usmart_set_DAC1Vol(uint16_t value){
    DAC1_Set_Vol(value);
}

void usmart_set_PWMCompare(uint32_t pulse){
    SetPWMCompare(&htim9, TIM_CHANNEL_2, pulse/2);
}

void usmart_write_EEPROMData(void){
    uint8_t Write_buffer[] = {"Write to the EEPROM"};
    AT24CXX_Write(0, Write_buffer, sizeof(Write_buffer));
}

void usmart_read_EEPROMData(void){
    uint8_t Read_buffer[20];
    AT24CXX_Read(0, Read_buffer, 20);
    USMART_RETURN("Read AT24CXX: %s \r\n", Read_buffer);
}