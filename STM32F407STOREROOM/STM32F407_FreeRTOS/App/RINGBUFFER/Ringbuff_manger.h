#ifndef __ring_manger_h__
#define __ring_manger_h__
#include "stdio.h"
#include "error.h"
#include "module.h"
/*********BUFF state**********/
#define BUF_NOR      0 
#define BUF_EMPTY    1
#define BUF_FULL     2
#define BUF_NEXIT    -1

/*********BUFF operate********/
#define BUF_RDFAIL  -1
#define BUF_RDSUCC   0
#define BUF_WRFAIL  -1
#define BUF_WRSUCC   0

#define BUF_INFOError  -3
/*
typedef enum 
{
  //BUFF state
  BUF_NOR      =0, 
  BUF_EMPTY    =1,
  BUF_FULL     =2,
  BUF_NEXIT    =-1,

  //BUFF operate
  BUF_RDFAIL  =-1,
  BUF_RDSUCC  =0,
  BUF_WRFAIL  =-1,
  BUF_WRSUCC  =0,
} BUFstat;
*/

typedef struct sJ1939_buff_message        //发送
{
	int 			PGN;   	//在这的SPN不是真实的PGN值，而是BMS交互中用到的索引，通过索引查找到PGN详细的信息, 发送信息时使用
	char 			Data[8];
	int			PGNnum; //真实的PGN值
        
  char 	     Priority; //优先级
  char         Reserved; //预留
}sJ1939_buff_message,*psJ1939_buff_message;


#define Ringbuff_rcvSize     24
#define Ringbuff_sedSize    12
extern sJ1939_buff_message  RingSNDbuff[Ringbuff_sedSize];  // 环形缓冲区
extern sJ1939_buff_message  RingRCVbuff[Ringbuff_rcvSize];  // 环形缓冲区


void Ringbuff_init(void);
int Ringbuff_write(psJ1939_buff_message , psJ1939_buff_message);
char Ringbuff_read(psJ1939_buff_message , psJ1939_buff_message);
int  RingSNDbuff_isempty(void);
int  RingSNDbuff_isfull(void);
int  RingRCVbuff_isempty(void);
int  RingRCVbuff_isfull(void);
void Ringbuff_setEmpty(void);

#endif
