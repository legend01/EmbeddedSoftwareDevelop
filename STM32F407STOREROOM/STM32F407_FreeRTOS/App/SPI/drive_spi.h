/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-24 22:30:14
 * @LastEditTime: 2021-01-25 22:18:25
 * @LastEditors: HLLI8
 */
#ifndef __DRIVE_SPI_H__
#define __DRIVE_SPI_H__

#include "gpio.h"
#include "main.h"

#define SPI_SCK_H HAL_GPIO_WritePin(SPI1_SCK_GPIO_Port, SPI1_SCK_Pin, GPIO_PIN_SET)
#define SPI_SCK_L HAL_GPIO_WritePin(SPI1_SCK_GPIO_Port, SPI1_SCK_Pin, GPIO_PIN_RESET)

#define SPI_MOSI_H HAL_GPIO_WritePin(SPI1_MOSI_GPIO_Port, SPI1_MOSI_Pin, GPIO_PIN_SET)
#define SPI_MOSI_L HAL_GPIO_WritePin(SPI1_MOSI_GPIO_Port, SPI1_MOSI_Pin, GPIO_PIN_RESET)

#define SPI_MISO_H HAL_GPIO_ReadPin(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin)

#define SPI_CS_H HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)
#define SPI_CS_L HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)

void SPI_SEND_BYTE(uint8_t TxData);
uint8_t SPI_RECV_BYTE(void);
#endif // !__DRIVE_SPI_H__
