/*
 * @Description: 日志打印
 * @Author: HLLI8
 * @Date: 2021-01-09 16:46:53
 * @LastEditTime: 2021-01-15 11:09:03
 * @LastEditors: HLLI8
 */
#ifndef __LOG_PRINTF_H__
#define __LOG_PRINTF_H__
#include "sys_stdlib.h"
#define LOG_PRINTF(...) log_printf(__VA_ARGS__)
typedef struct{
    char SimpLog_Info[200];
}SimpLog_STR;

unsigned long int log_printf (const unsigned char *fmt, ...);
void simp_printf(unsigned char *format, ...);
void out_char(unsigned char *ch);
unsigned long int USMART_PRINT(const unsigned char *fmt, ...);
void PRTNT_USMART(unsigned char *ch);
#endif // !__LOG_PRINTF_H__
