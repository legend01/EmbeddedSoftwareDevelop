/*
 * @Description: TCP客户端
 * @Author: HLLI8
 * @Date: 2021-03-21 12:01:08
 * @LastEditTime: 2021-03-27 10:29:37
 * @LastEditors: HLLI8
 */
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "lwip_comm.h"
#include "pub_data.h"
#include "err.h"
#include "tcp.h"

#define TCP_CLIENT_RX_BUFSIZE	200		//定义tcp client最大接收数据长度
#define TCP_CLIENT_TX_BUFSIZE	200		//tcp client发送数据长度
#define	TCP_SERVER_PORT			8087	//定义tcp client要连接的远端端口
#define LOCAL_PORT 8088
//tcp服务器连接状态
enum tcp_client_states
{
	ES_TCPCLIENT_NONE = 0,		//没有连接
	ES_TCPCLIENT_CONNECTED,		//连接到服务器了 
	ES_TCPCLIENT_CLOSING,		//关闭连接
}; 
//LWIP回调函数使用的结构体
struct tcp_client_struct
{
	u8 state;               //当前连接状
	struct tcp_pcb *pcb;    //指向当前的pcb
	struct pbuf *p;         //指向接收/或传输的pbuf
};  

typedef struct{
	u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];
}_TCP_STR;
extern _TCP_STR tcp_client_pack;

err_t tcp_client_usersent(struct tcp_pcb *tpcb, char* tcp_client_sendbuf); //发送TCP数据接口
void tcp_client_fun(void); //tcp client 主程序

err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err);
void tcp_client_error(void *arg,err_t err);
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb);

void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es);
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es);
#endif // !__TCP_CLIENT_H__
