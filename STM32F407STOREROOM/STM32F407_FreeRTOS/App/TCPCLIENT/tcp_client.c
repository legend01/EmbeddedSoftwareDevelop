/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-21 12:00:59
 * @LastEditTime: 2021-03-24 21:26:42
 * @LastEditors: HLLI8
 */
#include "tcp_client.h"
#include "malloc.h"
#include "log_printf.h"
#include "api.h"

struct netconn *tcppcb;  	//定义一个TCP服务器控制块
ip4_addr_t rmtipaddr;  	//远端ip地址

//TCP Client数据缓冲区
_TCP_STR tcp_client_pack;

//TCP Client 测试全局状态标记变量
//bit7:0,没有数据要发送;1,有数据要发送
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有连接上服务器;1,连接上服务器了.
//bit4~0:保留
u8 tcp_client_flag;	 

void tcp_client_connect_config(void){
	err_t err = -1;
    LOG_PRINTF("Local IP:%d.%d.%d.%d \r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]); //本地IP地址
    LOG_PRINTF("Remote IP:%d.%d.%d.%d  Remote Port:%d \r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3],TCP_CLIENT_PORT); //远端IP地址
    LOG_PRINTF("Status: Disconnected \r\n");
	tcppcb = netconn_new(NETCONN_TCP);	//创建一个新的pcb
	if(tcppcb)			//创建成功
	{
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]); 
		err = netconn_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT);  //连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
		if (err != ERR_OK)
		{
			LWIPMESSAGE_PRINTF("TCP server disconnect");
			netconn_delete(tcppcb); /* 返回值不等于ERR_OK 删除tcppcb连接 */
		}else if(err == ERR_OK){
			
		}
		
	 }else{ 
        while(1){
            LOG_PRINTF("Build tcp PCB ERROR \r\n"); //创建失败
        }
    }
}
//TCP Client
void tcp_client_fun(void)
{
	u8 res=0;		
	u8 t=0; 
	u8 connflag=0;		//连接标记
     
    if(tcp_client_flag&1<<6)//是否收到数据?
    {
        LWIPMESSAGE_PRINTF("TCP Receive Data: %s \r\n", tcp_client_pack.tcp_client_recvbuf); //显示接收到的数据
        tcp_client_flag&=~(1<<6);//标记数据已经被处理了.
    }
    if(tcp_client_flag&1<<5)//是否连接上?
    {
        if(connflag==0)
        { 
            connflag=1;//标记连接了
        } 
    }else if(connflag)
    {
		LWIPMESSAGE_PRINTF("TCP client disconnect \r\n");
        connflag=0;	//标记连接断开了
    } 
    lwip_periodic_handle();
    t++;
    if(t==200)
    {
        if(connflag==0&&(tcp_client_flag&1<<5)==0)//未连接上,则尝试重连
        { 
            netconn_delete(tcppcb);;//关闭连接
            tcppcb = netconn_new(NETCONN_TCP);	//创建一个新的pcb
            if(tcppcb)			//创建成功
            { 
				LWIPMESSAGE_PRINTF("Retry connect tcp server \r\n");
                netconn_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT);//连接到目的地址的指定端口上
            }
        }
        t=0;
    }		

} 