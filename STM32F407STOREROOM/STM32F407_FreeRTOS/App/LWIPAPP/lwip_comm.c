/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-19 22:07:40
 * @LastEditTime: 2021-03-22 21:05:30
 * @LastEditors: HLLI8
 */
#include "lwip_comm.h"
#include "remote_control.h"
#include "log_printf.h"
#include "ethernetif.h"
#include "tcpip.h"
#include "lan8720.h"
#include "dhcp.h"
#include "remote_control.h"

struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

__lwip_dev lwipdev;						//lwip控制结构体 
__lwip_dhcp lwip_dhcp; /* 开启DHCP服务结构体 */

__IO u32 DHCPfineTimer = 0; //DHCP精细处理计时器
__IO u32 DHCPcoarseTimer = 0; //DHCP粗糙处理计时器

//lwip 默认IP设置
//lwipx:lwip控制结构体指针
static void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	u32 sn0;
	sn0=*(vu32*)(0x1FFF7A10);//获取STM32的唯一ID的前24位作为MAC地址后三字节
	//默认远端IP为:192.168.1.100
	lwipx->remoteip[0]=192;	
	lwipx->remoteip[1]=168;
	lwipx->remoteip[2]=0;
	lwipx->remoteip[3]=101;
	//MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
	lwipx->mac[0]=2;//高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0
	lwipx->mac[1]=0;
	lwipx->mac[2]=0;
	lwipx->mac[3]=(sn0>>16)&0XFF;//低三字节用STM32的唯一ID
	lwipx->mac[4]=(sn0>>8)&0XFFF;
	lwipx->mac[5]=sn0&0XFF; 
	//默认本地IP为:192.168.1.30
	lwipx->ip[0]=192;	
	lwipx->ip[1]=168;
	lwipx->ip[2]=0;
	lwipx->ip[3]=30;
	//默认子网掩码:255.255.255.0
	lwipx->conf_netmask[0]=255;	
	lwipx->conf_netmask[1]=255;
	lwipx->conf_netmask[2]=255;
	lwipx->conf_netmask[3]=0;
	//默认网关:192.168.1.1
	lwipx->gateway[0]=192;	
	lwipx->gateway[1]=168;
	lwipx->gateway[2]=0;
	lwipx->gateway[3]=1;	
	lwipx->dhcpstatus=0;//没有DHCP	
} 

void ExternalConditionJudgeUseDHCP(LWIP_ENU opt){
	lwip_dhcp.useDHCPorNot = opt;
}

static void lwip_dhcp_config(void){
	u16 retry = 0;
	LOG_PRINTF("If open dhcp function \r\n");
	LOG_PRINTF("wiating.....\r\n");
	while (!lwip_dhcp.useDHCPorNot)
	{
		retry++;
		Remote_control();
		LOG_PRINTF("Search whether open dhcp function time: %d \r\n", retry);
		if (retry > 500)
		{
			LOG_PRINTF("Search over, Don't open dhcp function \r\n");
			retry = 0;
			break;
		}
	}
	if (lwip_dhcp.useDHCPorNot)
	{
		LOG_PRINTF("Open DHCP function \r\n");
	}
	
}

u8 lwip_comm_init(void){
	struct netif *Netif_Init_Flag; //调用netif_add()函数时返回值,判断网络初始化是否成功
	lwip_comm_default_ip_set(&lwipdev); //设置默认IP等信息
	lwip_dhcp_config();  /* 等待是否配置DHCP功能 */
	
	if (lwip_dhcp.useDHCPorNot)
	{
		ipaddr.addr = 0;
		netmask.addr = 0;
		gw.addr = 0;
	}else{
		IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
		IP4_ADDR(&netmask,lwipdev.conf_netmask[0],lwipdev.conf_netmask[1] ,lwipdev.conf_netmask[2],lwipdev.conf_netmask[3]);
		IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
		LOG_PRINTF("MAC:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
		LOG_PRINTF("Static IP:........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
		LOG_PRINTF("NetMask:..........................%d.%d.%d.%d\r\n",lwipdev.conf_netmask[0],lwipdev.conf_netmask[1],lwipdev.conf_netmask[2],lwipdev.conf_netmask[3]);
		LOG_PRINTF("Gateway:..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
	}
	Netif_Init_Flag = netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input); //向网卡列表中添加一个网口
	
	if(lwip_dhcp.useDHCPorNot){
		lwipdev.dhcpstatus = NOT_GET_ADDR; 
		dhcp_start(&gnetif);
	}
	if(Netif_Init_Flag == NULL) return ADD_NETCONFIG_FAILD; //添加网卡失败
	else{
		netif_set_default(&gnetif); //设置netif为默认网口
		netif_set_up(&gnetif); //打开netif网口
	}
	return SUCCESS;
}

void lwip_dhcp_process(void){
	if (lwip_dhcp.useDHCPorNot)
	{
		while ((lwipdev.dhcpstatus != GET_ADDR_SUCCESS) && (lwipdev.dhcpstatus != GET_ADDR_FAILD))
		{
			lwip_periodic_handle(); //定时处理函数
		}
	}
}

//LWIP轮询任务
void lwip_periodic_handle(void)
{
	if (lwip_dhcp.useDHCPorNot)
	{
		//每500ms调用一次dhcp_fine_tmr()
		if (sys_now() - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
		{
			DHCPfineTimer =  sys_now();
			dhcp_fine_tmr();
			if ((lwipdev.dhcpstatus != 2)&&(lwipdev.dhcpstatus != 0XFF))
			{ 
			lwip_dhcp_process_handle();  //DHCP处理
			}
		}

		//每60s执行一次DHCP粗糙处理
		if (sys_now() - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
		{
			DHCPcoarseTimer =  sys_now();
			dhcp_coarse_tmr();
		}  
	}
}

//DHCP处理任务
void lwip_dhcp_process_handle(void)
{
	u32 ip=0,netmask=0,gw=0;
	switch(lwipdev.dhcpstatus)
	{
		case 0: 	//开启DHCP
			dhcp_start(&gnetif);
			lwipdev.dhcpstatus = 1;		//等待通过DHCP获取到的地址
			LOG_PRINTF("正在查找DHCP服务器,请稍等...........\r\n");  
			break;
		case 1:		//等待获取到IP地址
		{
			ip=gnetif.ip_addr.addr;		//读取新IP地址
			netmask=gnetif.netmask.addr;//读取子网掩码
			gw=gnetif.gw.addr;			//读取默认网关 
			
			if(ip!=0)			//正确获取到IP地址的时候
			{
				lwipdev.dhcpstatus=2;	//DHCP成功
				LOG_PRINTF("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
				//解析出通过DHCP获取到的IP地址
				lwipdev.ip[3]=(uint8_t)(ip>>24); 
				lwipdev.ip[2]=(uint8_t)(ip>>16);
				lwipdev.ip[1]=(uint8_t)(ip>>8);
				lwipdev.ip[0]=(uint8_t)(ip);
				LOG_PRINTF("通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
				//解析通过DHCP获取到的子网掩码地址
				lwipdev.conf_netmask[3]=(uint8_t)(netmask>>24);
				lwipdev.conf_netmask[2]=(uint8_t)(netmask>>16);
				lwipdev.conf_netmask[1]=(uint8_t)(netmask>>8);
				lwipdev.conf_netmask[0]=(uint8_t)(netmask);
				LOG_PRINTF("通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",lwipdev.conf_netmask[0],lwipdev.conf_netmask[1],lwipdev.conf_netmask[2],lwipdev.conf_netmask[3]);
				//解析出通过DHCP获取到的默认网关
				lwipdev.gateway[3]=(uint8_t)(gw>>24);
				lwipdev.gateway[2]=(uint8_t)(gw>>16);
				lwipdev.gateway[1]=(uint8_t)(gw>>8);
				lwipdev.gateway[0]=(uint8_t)(gw);
				LOG_PRINTF("通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			}else if(netif_dhcp_data(&gnetif)->tries>LWIP_MAX_DHCP_TRIES) //通过DHCP服务获取IP地址失败,且超过最大尝试次数
			{
				lwipdev.dhcpstatus=0XFF;//DHCP超时失败.
				//使用静态IP地址
				IP4_ADDR(&(gnetif.ip_addr),lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
				IP4_ADDR(&(gnetif.netmask),lwipdev.conf_netmask[0],lwipdev.conf_netmask[1],lwipdev.conf_netmask[2],lwipdev.conf_netmask[3]);
				IP4_ADDR(&(gnetif.gw),lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
				LOG_PRINTF("DHCP服务超时,使用静态IP地址!\r\n");
				LOG_PRINTF("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
				LOG_PRINTF("静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
				LOG_PRINTF("子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.conf_netmask[0],lwipdev.conf_netmask[1],lwipdev.conf_netmask[2],lwipdev.conf_netmask[3]);
				LOG_PRINTF("默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			}
		}
		break;
		default : break;
	}
}

void lwip_net_inf(void){
	u8 speed = 0;
	if (lwipdev.dhcpstatus == GET_ADDR_SUCCESS)
	{
		LOG_PRINTF("DHCP IP:%d.%d.%d.%d \r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
	}else{
		LOG_PRINTF("Static IP:%d.%d.%d.%d \r\n", lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	}
	speed = LAN8720_Get_Speed(); /* 获取网速 */
	if (speed&1 << 1)
	{
		LOG_PRINTF("Ethernet Speed: 100M \r\n");
	}else{
		LOG_PRINTF("Ethernet Speed: 10M \r\n");
	}
	
}