/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-23 11:07:34
 * @LastEditTime: 2021-02-28 18:08:12
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

void usmart_write_FlashData(void){
    uint8_t write_buffer[] = {"hello"};
    W25QXX_Write(write_buffer, FLASH_TEST_Addr, sizeof(write_buffer));
}

void usmart_read_FlashData(void){
    uint8_t read_buffer[13];
    W25QXX_Read(read_buffer, FLASH_TEST_Addr, 13);
    USMART_RETURN("Read W25QXX: %s \r\n", read_buffer);
}

const uint8_t TEXT_Buffer[]={"Test local Flash"};
void usmart_write_LocalFlashData(void){
    STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)TEXT_Buffer,SIZE(TEXT_Buffer));
}

void usmart_read_LocalFlashData(void){
    u8 data_buffer[24];
    STMFLASH_Read(FLASH_SAVE_ADDR, (u32*)data_buffer, SIZE(TEXT_Buffer));
    USMART_RETURN("Read Local Flash: %s \r\n", data_buffer);
}

void usmart_read_externSramSize(void){
    u16 len = 0;
    uint8_t textbuffer[24];
    fsmc_sram_size(textbuffer);
    USMART_RETURN("External SRAM data: %s \r\n", textbuffer);
}