/*
 * @Description: W5500消息处理模块
 * @Author: HLLI8
 * @Date: 2020-11-28 20:53:13
 * @LastEditTime: 2020-12-02 19:45:12
 * @LastEditors: HLLI8
 */
#include "w5500.h"
/*******************************************************************************
* 函数名  : W5500_Initialization
* 描述    : W5500初始货配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//初始化W5500寄存器函数
	Detect_Gateway();	//检查网关服务器 
	Socket_Init(0);		//指定Socket(0~7)初始化,初始化端口0
}
/*******************************************************************************
* 函数名  : Load_Net_Parameters
* 描述    : 装载网络参数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/
void Load_Net_Parameters(void)
{
	Gateway_IP[0] = 192;//加载网关参数
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 0;
	Gateway_IP[3] = 1;

	Sub_Mask[0]=255;//加载子网掩码
	Sub_Mask[1]=255;
	Sub_Mask[2]=255;
	Sub_Mask[3]=0;

	Phy_Addr[0]=0x0c;//加载物理地址
	Phy_Addr[1]=0x29;
	Phy_Addr[2]=0xab;
	Phy_Addr[3]=0x7c;
	Phy_Addr[4]=0x00;
	Phy_Addr[5]=0x01;

	IP_Addr[0]=192;//加载本机IP地址
	IP_Addr[1]=168;
	IP_Addr[2]=0;
	IP_Addr[3]=199;

	S0_Port[0] = 0x13;//加载端口0的端口号5000 
	S0_Port[1] = 0x88;

	S0_DIP[0]=192;//加载端口0的目的IP地址
	S0_DIP[1]=168;
	S0_DIP[2]=0;
	S0_DIP[3]=101;
	
	S0_DPort[0] = 0x17;//加载端口0的目的端口号8086
	S0_DPort[1] = 0x70;

	S0_Mode=TCP_CLIENT;//加载端口0的工作模式,TCP客户端模式
}

/*******************************************************************************
* 函数名  : W5500_Socket_Set
* 描述    : W5500端口初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 分别设置4个端口,根据端口工作模式,将端口置于TCP服务器、TCP客户端或UDP模式.
*			从端口状态字节Socket_State可以判断端口的工作情况
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State==0)//端口0初始化配置
	{
		if(S0_Mode==TCP_SERVER)//TCP服务器模式 
		{
			if(Socket_Listen(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else if(S0_Mode==TCP_CLIENT)//TCP客户端模式 
		{
			if(Socket_Connect(0)==TRUE) /* 当Socket_Connect没调用一次，就与服务器产生一次连接 */
				S0_State=S_INIT; /* Port_0 Init success */
			else
				S0_State=0; /* Port_0 Init error */
		}
		else//UDP模式 
		{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
		}
	}
}

void W5500ProcessMessageF(void){
	unsigned int rev_size = 0;
	W5500_Socket_Set();//W5500端口初始化配置

	if(W5500_Interrupt)//处理W5500中断		
	{
		W5500_Interrupt_Process();//W5500中断处理程序框架
	}
	/**
	 * @brief Receive data from TCP_server and Send the same data
	 */
	if((S0_Data & S_RECEIVE) == S_RECEIVE)//如果Socket0接收到数据
	{
		S0_Data&=~S_RECEIVE;
		rev_size = Read_SOCK_Data_Buffer(0, Rx_Buffer);
		Uart3_DMA_Sent(Rx_Buffer, rev_size);
		// Process_Socket_Data(0);//W5500接收并发送接收到的数据
	}
}