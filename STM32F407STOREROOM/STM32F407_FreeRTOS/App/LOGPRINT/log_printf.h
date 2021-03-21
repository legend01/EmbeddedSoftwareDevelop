/*
 * @Description: 日志打印
 * @Author: HLLI8
 * @Date: 2021-01-09 16:46:53
 * @LastEditTime: 2021-03-21 11:41:37
 * @LastEditors: HLLI8
 */
#ifndef __LOG_PRINTF_H__
#define __LOG_PRINTF_H__
#include "sys_stdlib.h"
#define LOG_PRINTF(...) log_printf(__VA_ARGS__)
#define USMART_RETURN(...) USMART_PRINT(__VA_ARGS__)
#define LWIPMESSAGE_PRINTF(...) LWIP_PRINTF(__VA_ARGS__)

typedef struct{
    char SimpLog_Info[200];
}SimpLog_STR;

unsigned long int log_printf (const unsigned char *fmt, ...);
void out_char(unsigned char *ch);
unsigned long int USMART_PRINT(const unsigned char *fmt, ...);
void PRTNT_USMART(unsigned char *ch);

unsigned long int LWIP_PRINT(const unsigned char *fmt, ...);
void PRINT_LWIP(unsigned char *ch);
#endif // !__LOG_PRINTF_H__
