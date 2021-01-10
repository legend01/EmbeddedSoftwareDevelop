/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-09 16:46:05
 * @LastEditTime: 2021-01-10 17:07:16
 * @LastEditors: HLLI8
 */
#include "log_printf.h"
#include "sys_stdlib.h"
#include "app_rtc.h"
#include "assert.h"

/********************************************************************/
unsigned long int log_printf (const unsigned char *fmt, ...) //!_TODO:修改函数名
{
    va_list ap;
    unsigned long int rvalue;
    PRINTK_INFO info;


    info.dest = DEST_CONSOLE;
    info.func = &out_char;
    /* 打印时间 */
    RTC_Get_SysTime();
    /*
     * Initialize the pointer to the variable length argument list.
     * 初始化指向可变长度参数列表的指针
     */
    va_start(ap, fmt);
    assert(fmt!=NULL);
   
    rvalue = printk(&info, fmt, ap);
    /*
     * Cleanup the variable length argument list.
     */
    va_end(ap);
    return rvalue;
}

void simp_printf(unsigned char *format, ...){
    va_list args;
    const char *args1;
    SimpLog_STR simpLog_container;
    memset(simpLog_container.SimpLog_Info, 0, sizeof(simpLog_container.SimpLog_Info)/sizeof(simpLog_container.SimpLog_Info[0]));
    /* 打印时间 */
    RTC_Get_SysTime();

    va_start(args, format);
    args1 = va_arg(args, const char *);
    va_end(args);
    
    // itoa(args1);
    memcpy(simpLog_container.SimpLog_Info, format, sizeof(simpLog_container.SimpLog_Info)/sizeof(simpLog_container.SimpLog_Info[0]));
    
    Uart5_DMA_Sent((uint8_t*)simpLog_container.SimpLog_Info, sizeof(simpLog_container.SimpLog_Info)/sizeof(simpLog_container.SimpLog_Info[0]));
}

unsigned long int USMART_PRINT(const unsigned char *fmt, ...){
    va_list ap;
    unsigned long int rvalue;
    PRINTK_INFO info;


    info.dest = DEST_CONSOLE;
    info.func = &PRTNT_USMART;
    /*
     * Initialize the pointer to the variable length argument list.
     * 初始化指向可变长度参数列表的指针
     */
    va_start(ap, fmt);
    assert(fmt!=NULL);
   
    rvalue = printk(&info, fmt, ap);
    /*
     * Cleanup the variable length argument list.
     */
    va_end(ap);
    return rvalue;
}
/********************************************************************/
void out_char(unsigned char *ch) //!_修改输出出口
{
	Uart5_DMA_Sent((uint8_t*)ch, 1);
}

void PRTNT_USMART(unsigned char *ch){ //!USMART专用打印出口
    Uart1_DMA_Sent((uint8_t*)ch, 1);
}
/* **************************************************************** */