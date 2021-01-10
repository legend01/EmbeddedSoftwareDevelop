/*
 * @Description: 日志打印
 * @Author: HLLI8
 * @Date: 2021-01-09 16:46:53
 * @LastEditTime: 2021-01-10 17:07:45
 * @LastEditors: HLLI8
 */
#ifndef __LOG_PRINTF_H__
#define __LOG_PRINTF_H__
#include "sys_stdlib.h"

typedef struct{
    char SimpLog_Info[200];
}SimpLog_STR;

unsigned long int log_printf (const unsigned char *fmt, ...);
void simp_printf(unsigned char *format, ...);
void out_char(unsigned char *ch);
unsigned long int USMART_PRINT(const unsigned char *fmt, ...)
#endif // !__LOG_PRINTF_H__
