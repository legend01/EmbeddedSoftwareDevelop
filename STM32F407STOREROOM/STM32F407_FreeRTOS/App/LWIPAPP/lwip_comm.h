/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-19 22:24:49
 * @LastEditTime: 2021-03-21 10:13:01
 * @LastEditors: HLLI8
 */
#ifndef __LWIP_COMM_H__
#define __LWIP_COMM_H__

#include "pub_data.h"

#define LWIP_MAX_DHCP_TRIES		4   //DHCP服务器最大重试次数

typedef enum{
	CLOSE,
	OPEN,
	ADD_NETCONFIG_FAILD,
}LWIP_ENU;

typedef enum{
	NOT_GET_ADDR,
	ENTERY_GET_ADDR,
	GET_ADDR_SUCCESS,
	GET_ADDR_FAILD = 0xFF,
}DHCP_STATUS_ENU;
//lwip控制结构体
typedef struct  
{
	u8 mac[6];      //MAC地址
	u8 remoteip[4];	//远端主机IP地址 
	u8 ip[4];       //本机IP地址
	u8 conf_netmask[4]; 	//子网掩码
	u8 gateway[4]; 	//默认网关的IP地址
	
	vu8 dhcpstatus;	//dhcp状态 
					//0,未获取DHCP地址;
					//1,进入DHCP获取状态
					//2,成功获取DHCP地址
					//0XFF,获取失败.
}__lwip_dev;
extern __lwip_dev lwipdev;	//lwip控制结构体

typedef struct 
{
	__IO u8 useDHCPorNot; // 是否使用DHCP功能
}__lwip_dhcp;
extern __lwip_dhcp lwip_dhcp;

void lwip_periodic_handle(void);
void lwip_dhcp_process_handle(void);

void ExternalConditionJudgeUseDHCP(LWIP_ENU opt);

u8 lwip_comm_init(void);
void lwip_dhcp_process(void);
void lwip_net_inf(void);

#endif // !__LWIP_COMM_H__
