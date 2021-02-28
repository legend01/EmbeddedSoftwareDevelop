/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-02-28 16:49:44
 * @LastEditTime: 2021-02-28 18:09:20
 * @LastEditors: HLLI8
 */
#include "sram.h"
#include "log_printf.h"

//在指定地址(WriteAddr+Bank1_SRAM3_ADDR)开始,连续写入n个字节.
//pBuffer:字节指针
//WriteAddr:要写入的地址
//n:要写入的字节数
void FSMC_SRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

//在指定地址((WriteAddr+Bank1_SRAM3_ADDR))开始,连续读出n个字节.
//pBuffer:字节指针
//ReadAddr:要读出的起始地址
//n:要写入的字节数
void FSMC_SRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);
		ReadAddr++;
	}
}

/**
 * @NOTE: 定义一个超大数组, 指定该数组定义在外部sram起始地址(__attribute__((at(0X68000000)))) 
 */
// u32 testsram[250000] __attribute__((at(0X68000000)));//测试用数组 
//外部内存测试(最大支持1M字节内存测试)	    
void fsmc_sram_size(uint8_t* read_buffer)
{  
	u32 i=0;  	  
	u8 temp=0;	   
	u8 sval=0;	//在地址0读到的数据	  				   

	//每隔4K字节,写入一个数据,总共写入256个数据,刚好是1M字节
	for(i=0;i<1024*1024;i+=4096)
	{
		FSMC_SRAM_WriteBuffer(&temp,i,1);
		temp++;
	}
        temp = 0;
	//依次读出之前写入的数据,进行校验		  
 	for(i=0;i<1024*1024;i+=4096) 
	{
  		FSMC_SRAM_ReadBuffer(&temp,i,1);
		if(i==0)sval=temp;
 		else if(temp<=sval)break;//后面读出的数据一定要比第一次读到的数据大.	   		   
 	}					 
    u8 text_buffer[] = {"hello world\r\n"};
	FSMC_SRAM_WriteBuffer(text_buffer, 0, strlen(text_buffer));

	FSMC_SRAM_ReadBuffer(read_buffer, 0, strlen(text_buffer));
}	