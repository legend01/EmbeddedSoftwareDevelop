/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-21 12:00:59
 * @LastEditTime: 2021-03-27 10:36:51
 * @LastEditors: HLLI8
 */
#include "tcp_client.h"
#include "log_printf.h"
#include "api.h"

struct netconn *tcp_clientconn;  	
static ip_addr_t server_ipaddr, loca_ipaddr;
static u16_t server_port, loca_port;

//TCP Client数据缓冲区
_TCP_STR tcp_client_pack;


void tcp_client_connect_config(void){
	err_t err = -1;
    LOG_PRINTF("Local IP:%d.%d.%d.%d \r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]); //本地IP地址
    LOG_PRINTF("Remote IP:%d.%d.%d.%d  Remote Port:%d \r\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3],TCP_SERVER_PORT); //远端IP地址
    LOG_PRINTF("Status: Disconnected \r\n");
    IP4_ADDR(&server_ipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
    while(1)
    {
        tcp_clientconn = netconn_new(NETCONN_TCP);	//创建一个新的pcb
        netconn_bind(tcp_clientconn, IP4_ADDR_ANY, LOCAL_PORT);

        err = netconn_connect(tcp_clientconn,&server_ipaddr,TCP_SERVER_PORT);  //连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
        if (err != ERR_OK)
        {
            LWIPMESSAGE_PRINTF("TCP server disconnect-----------\r\n");
            netconn_delete(tcp_clientconn); /* 返回值不等于ERR_OK 删除tcppcb连接 */
        }else if(err == ERR_OK){
            struct netbuf *recv_buf;
            //tcp_clientconn->recv_timeout = 10;
            netconn_getaddr(tcp_clientconn, &loca_ipaddr, &loca_port, 1);
            LWIPMESSAGE_PRINTF("Connect to TCP SERVER ------------------\r\n");
            break;
        }
    }
}
//TCP Client
void tcp_client_fun(void)
{

} 