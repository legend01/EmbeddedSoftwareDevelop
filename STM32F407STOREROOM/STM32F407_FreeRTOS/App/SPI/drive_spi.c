/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-24 22:30:07
 * @LastEditTime: 2021-01-26 22:14:57
 * @LastEditors: HLLI8
 */
#include "drive_spi.h"

//spi数据写入驱动
void SPI_SEND_BYTE(uint8_t TxData)
{
    uint8_t i = 0;
    
    for (i = 0; i < 8; i++)
    {
        SPI_SCK_L;
        SysDelay_us(1);
        
        if (TxData & 0x80)
        {
            SPI_MOSI_H;
        }
        else
        {
            SPI_MOSI_L;
        }
        TxData <<= 1;
        
        SysDelay_us(1);
        SPI_SCK_H;
        SysDelay_us(1);
    }
}

//spi数据读取驱动
uint8_t SPI_RECV_BYTE(void)
{
    uint8_t RxData = 0;
    uint8_t i = 0;

    
    for (i = 0; i < 8; i++)
    {
        SPI_SCK_L;
        SysDelay_us(1);

        RxData <<= 1;
        if (SPI_MISO_H)
        {
            RxData |= 0x01;
        }
        
        SPI_SCK_H;
        SysDelay_us(1);
    }

    return RxData;
}
