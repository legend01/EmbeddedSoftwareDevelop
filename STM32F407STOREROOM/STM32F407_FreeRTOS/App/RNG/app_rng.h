/*
 * @Description: 随机数发生器
 * @Author: HLLI8
 * @Date: 2021-01-12 21:21:41
 * @LastEditTime: 2021-01-12 21:24:42
 * @LastEditors: HLLI8
 */
#ifndef __APP_RNG_H__
#define __APP_RNG_H__
#include "rng.h"

uint32_t RNG_Get_RandomNum(void);
int RNG_Get_RandomRange(int min,int max);
#endif // !__APP_RNG_H__
