/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-06 21:46:34
 * @LastEditTime: 2021-03-07 16:46:11
 * @LastEditors: HLLI8
 */
#ifndef __APP_I2S_H__
#define __APP_I2S_H__
#include "pub_data.h"    									

extern I2S_HandleTypeDef hi2s2;
extern DMA_HandleTypeDef hdma_spi2_tx;
extern DMA_HandleTypeDef hdma_i2s2_ext_rx;

extern void (*i2s_tx_callback)(void);		//IIS TX回调函数指针  

void I2S2_Init(u32 I2S_Standard,u32 I2S_Mode,u32 I2S_Clock_Polarity,u32 I2S_DataFormat);
u8 I2S2_SampleRate_Set(u32 samplerate);
void I2S2_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num); 
void I2S_Play_Start(void); 
void I2S_Play_Stop(void); 
void DMA1_Stream4_IRQHandler_callback(void); 
void DMA1_Stream3_IRQHandler_callback(void);
#endif // !__APP_I2S_H__





















