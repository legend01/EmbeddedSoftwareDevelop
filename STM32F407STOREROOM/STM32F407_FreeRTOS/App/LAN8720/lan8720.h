/*
 * @Description: 网卡LAN8720驱动
 * @Author: HLLI8
 * @Date: 2021-03-18 22:17:35
 * @LastEditTime: 2021-03-20 12:50:16
 * @LastEditors: HLLI8
 */
#ifndef __LAN8720_H__
#define __LAN8720_H__

#include "pub_data.h"

u32 LAN8720_ReadPHY(u16 reg);
void LAN8720_WritePHY(u16 reg,u16 value);
u8 LAN8720_Get_Speed(void);

#endif // !__LAN8720_H__