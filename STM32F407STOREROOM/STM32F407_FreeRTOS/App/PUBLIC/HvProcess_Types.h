/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-02 16:29:22
 * @LastEditTime: 2021-03-02 17:28:28
 * @LastEditors: HLLI8
 */


/**
 * \addtogroup HvProcess 流程控制
 * \{
 */

#ifndef HV_PROCESS_TYPES_H_
#define HV_PROCESS_TYPES_H_

#include "pub_data.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/**
 * \brief 流程控制条件函数类型
 */
typedef bool (*HvProcess_CondFunc)(void);

/**
 * \brief 流程控制动作函数类型
 */
typedef void (*HvProcess_ActionFunc)(void);

typedef void (*HvProcess_PreInit)(void);

/**
 * \brief 流程控制状态信息类型
 */
typedef struct{
    HvProcess_CondFunc Cond; /**< 条件函数指针 */
    HvProcess_ActionFunc Action; /**< 动作函数指针 */
    u16 Next; /**< 下一状态 */
}HvProcess_StateInfoType;

/**
 * \brief 高压流程控制状态配置类型
 */
typedef struct{
    u8 Num; /**< 状态信息个数 */
    const HvProcess_StateInfoType *State; /**< 状态信息指针 */
    HvProcess_PreInit Init; /* 每个状态点初始化 */
}HvProcess_StateConfigType;

/**
 * \brief 读取当前状态函数指针类型定义
 */
typedef u16 (*HvProcess_GetStateFuncPtr)(void);

/**
 * \brief 设置当前状态函数指针类型定义
 */
typedef void (*HvProcess_SetStateFuncPtr)(u16 state);

/**
 * \brief 流程轮循函数指针类型定义
 */
typedef void (*HvProcess_InitFuncPtr)(void);

/**
 * \brief 流程轮循函数指针类型定义
 */
typedef void (*HvProcess_PollFuncPtr)(void);

/**
 * \brief 流程数据类型定义
 */
typedef struct{
    u16 StateNum; /**< 流程状态数 */
    const HvProcess_StateConfigType *Config; /**< 流程配置指针 */
    HvProcess_GetStateFuncPtr GetState; /**< 读取状态函数指针 */
    HvProcess_SetStateFuncPtr SetState; /**< 设置状态函数指针 */
    HvProcess_InitFuncPtr Init; /**< 初始化函数指针 */
    HvProcess_PollFuncPtr Poll; /**< 轮循函数指针 */
}HvProcess_ProcessDataType;

void HVProcess_Process(const HvProcess_ProcessDataType *Process);

#endif


/**
 * \}
 */