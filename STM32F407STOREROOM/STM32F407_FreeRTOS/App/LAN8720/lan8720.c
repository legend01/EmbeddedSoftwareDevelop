/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-18 22:17:27
 * @LastEditTime: 2021-03-20 12:54:00
 * @LastEditors: HLLI8
 */
#include "lan8720.h"
#include "malloc.h"
#include "lwip.h"

//读取PHY寄存器值
u32 LAN8720_ReadPHY(u16 reg)
{
    u32 regval;
    HAL_ETH_ReadPHYRegister(&heth,reg,&regval);
    return regval;
}
//向LAN8720指定寄存器写入值
//reg:要写入的寄存器
//value:要写入的值
void LAN8720_WritePHY(u16 reg,u16 value)
{
    uint32_t temp=value;
    HAL_ETH_WritePHYRegister(&heth,reg,temp);
}
//得到8720的速度模式
//返回值:
//001:10M半双工
//101:10M全双工
//010:100M半双工
//110:100M全双工
//其他:错误.
u8 LAN8720_Get_Speed(void)
{
    u8 speed;
    speed=((LAN8720_ReadPHY(31)&0x1C)>>2); //从LAN8720的31号寄存器中读取网络速度和双工模式
    return speed;
}

