/*
 * @Description:断言Assert
 * @Author: HLLI8
 * @Date: 2020-12-07 21:15:04
 * @LastEditTime: 2021-01-05 21:39:41
 * @LastEditors: HLLI8
 */
#ifndef __ASSERT_H__
#define __ASSERT_H__

#include "stdio.h"

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
