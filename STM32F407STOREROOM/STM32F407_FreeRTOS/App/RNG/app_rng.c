/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-01-12 21:21:21
 * @LastEditTime: 2021-01-12 21:29:43
 * @LastEditors: HLLI8
 */
#include "app_rng.h"

//得到随机数
//返回值:获取到的随机数
uint32_t RNG_Get_RandomNum(void)
{
    return HAL_RNG_GetRandomNumber(&hrng);
}

//生成[min,max]范围的随机数
int RNG_Get_RandomRange(int min,int max)
{ 
   return HAL_RNG_GetRandomNumber(&hrng)%(max-min+1) +min;
}
