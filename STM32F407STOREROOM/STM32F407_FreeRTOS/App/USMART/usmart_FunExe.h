/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-23 11:07:45
 * @LastEditTime: 2021-01-24 21:08:27
 * @LastEditors: HLLI8
 */
#ifndef __USMART_FUNEXE_H__
#define __USMART_FUNEXE_H__
#include "app_adc.h"
#include "pwmadc.h"
#include "24cxx.h"

void usmart_print_TempeSensor(void);
void usmart_print_lightSensor(void);
void usmart_print_ADC3Sensor(void);
void usmart_set_DAC1Vol(uint16_t value);
void usmart_set_PWMCompare(uint32_t pulse);
void usmart_write_EEPROMData(void);
void usmart_read_EEPROMData(void);
#endif // !__USMART_FUNEXE_H__
