/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-19 22:07:40
 * @LastEditTime: 2021-03-20 13:03:31
 * @LastEditors: HLLI8
 */
#include "lwip_comm.h"
#include "remote_control.h"
#include "log_printf.h"
#include "ethernetif.h"
#include "tcpip.h"
#include "lwip.h"

__lwip_dev lwipdev;						//lwip控制结构体 
__lwip_dhcp lwip_dhcp; /* 开启DHCP服务结构体 */

//lwip 默认IP设置
//lwipx:lwip控制结构体指针
static void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	u32 sn0;
	sn0=*(vu32*)(0x1FFF7A10);//获取STM32的唯一ID的前24位作为MAC地址后三字节
	//默认远端IP为:192.168.1.100
	lwipx->remoteip[0]=192;	
	lwipx->remoteip[1]=168;
	lwipx->remoteip[2]=1;
	lwipx->remoteip[3]=11;
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
	lwipx->ip[2]=1;
	lwipx->ip[3]=30;
	//默认子网掩码:255.255.255.0
	lwipx->conf_netmask[0]=255;	
	lwipx->conf_netmask[1]=255;
	lwipx->conf_netmask[2]=255;
	lwipx->conf_netmask[3]=0;
	//默认网关:192.168.1.1
	lwipx->gateway[0]=192;	
	lwipx->gateway[1]=168;
	lwipx->gateway[2]=1;
	lwipx->gateway[3]=1;	
	lwipx->dhcpstatus=0;//没有DHCP	
} 

void ExternalConditionJudgeUseDHCP(LWIP_ENU opt){
	lwip_dhcp.useDHCPorNot = opt;
}

static void lwip_dhcp_config(void){
	u8 retry = 0;
	LOG_PRINTF("If open dhcp function \r\n");
	LOG_PRINTF("wiating.....\r\n");
	while (!lwip_dhcp.useDHCPorNot)
	{
		retry++;
		LOG_PRINTF("Search whether open dhcp function time: %d \r\n", retry);
		if (retry > 50)
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
	if (!lwip_dhcp.useDHCPorNot)
	{
		ipaddr.addr = 0;
		netmask.addr = 0;
		gw.addr = 0;
	}else{
		IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
		IP4_ADDR(&netmask,lwipdev.conf_netmask[0],lwipdev.conf_netmask[1] ,lwipdev.conf_netmask[2],lwipdev.conf_netmask[3]);
		IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
		printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
		printf("静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
		printf("子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.conf_netmask[0],lwipdev.conf_netmask[1],lwipdev.conf_netmask[2],lwipdev.conf_netmask[3]);
		printf("默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
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