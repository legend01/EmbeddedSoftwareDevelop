/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-02-21 14:58:40
 * @LastEditTime: 2021-02-21 19:22:44
 * @LastEditors: HLLI8
 */
#include "remote.h"


//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
volatile uint8_t 	RmtSta=0;	  	  

volatile uint32_t RmtRec=0;	//红外接收到的数据	   		    
volatile uint8_t  RmtCnt=0;	//按键按下的次数	


//处理红外键盘
//返回值:
//	 0,没有任何按键按下
//其他,按下的按键键值.
uint8_t Remote_Scan(void)
{        
	uint8_t sta=0;       
    uint8_t t1,t2;  

	if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{ 
	    t1=RmtRec>>24;			//得到地址码
	    t2=(RmtRec>>16)&0xff;	//得到地址反码 
 	    if((t1==(uint8_t)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(uint8_t)~t2)sta=t1;//键值正确	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//按键数据错误/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);//清除接收到有效按键标识
			RmtCnt=0;		//清除按键次数计数器
		}
	}  
    
    return sta;
}