/*
 * @Description:断言Assert
 * @Author: HLLI8
 * @Date: 2020-12-07 21:15:04
 * @LastEditTime: 2021-01-09 16:54:31
 * @LastEditors: HLLI8
 */
#ifndef __ASSERT_H__
#define __ASSERT_H__

#include "log_printf.h"

#ifndef NDEBUG

#define assert(expr) \
  if (expr) { ; } \
  else  {\
    log_printf("Assert failed: " #expr " (file %s line %d)\n", __FILE__, (int) __LINE__ );\
    while (1);\
  }
#else
#define assert(expr)
#endif


#endif // !__ASSERT_H__
