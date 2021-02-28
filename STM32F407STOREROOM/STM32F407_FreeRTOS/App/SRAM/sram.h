/*
 * @Description: SRAM处理函数
 * @Author: HLLI8
 * @Date: 2021-02-28 16:49:52
 * @LastEditTime: 2021-02-28 18:05:27
 * @LastEditors: HLLI8
 */
#ifndef __SRAM_H__
#define __SRAM_H__

#include "pub_data.h"

//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))	

void FSMC_SRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void FSMC_SRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);

void fsmc_sram_size(uint8_t* read_buffer);
#endif // !__SRAM_H__
