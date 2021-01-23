/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2020-12-27 13:44:53
 * @LastEditTime: 2021-01-23 12:07:14
 * @LastEditors: HLLI8
 */
#include "usmart.h"
#include "usmart_str.h"
////////////////////////////用户配置区///////////////////////////////////////////////
#include "usmart_receiveFromUsart.h"
#include "usmart_FunList.h"
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   



















