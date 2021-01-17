#include "Ringbuff_manger.h"

static uint8_t BuffSND_addr_write;      //在缓冲区中的写入位置
static uint8_t BuffSND_addr_read;      //在缓冲区的读取位置
sJ1939_buff_message  RingSNDbuff[Ringbuff_sedSize];  // 环形缓冲区
//注意此信息读出内部带有malloc分配的内存，当用完需要释放它

static uint8_t BuffRCV_addr_write;      //在缓冲区中的写入位置
static uint8_t BuffRCV_addr_read;      //在缓冲区的读取位置
sJ1939_buff_message RingRCVbuff[Ringbuff_rcvSize];  // 环形缓冲区

/*******************************************
功能:   清空收发环形缓冲区
参数:   无
返回值: 无
*********************************************/
void Ringbuff_setEmpty(void)
{
    BuffSND_addr_write = 0;
    BuffSND_addr_read  = 0;  
    BuffRCV_addr_write = 0;
    BuffRCV_addr_read  = 0;
}

/**************************************************
功能:   将发送和接收两个环形缓冲区内部数据全部清空
参数:   无
返回值: 无
***************************************************/
void Ringbuff_init(void)
{
    uint8_t i,j;
    for(i =0;i<Ringbuff_sedSize;i++)  //清空环形缓冲区数据
    {
        RingSNDbuff[i].PGN         = -1;
        RingSNDbuff[i].PGNnum  = 0;
        for(j=0;j<8;j++)
            RingSNDbuff[i].Data[j]= 0;
    }
    BuffSND_addr_write = 0;  // 读写位置都从0开始，缓冲区空
    BuffSND_addr_read  = 0;

    for(i =0;i<Ringbuff_rcvSize;i++)  //清空环形缓冲区数据
    {
        RingRCVbuff[i].PGN         = -1;
        RingRCVbuff[i].PGNnum  = 0;
        for(j=0;j<8;j++)
            RingRCVbuff[i].Data[j]= 0;
    }
    BuffRCV_addr_write = 0;  // 读写位置都从0开始，缓冲区空
    BuffRCV_addr_read  = 0;
}
/*****************************************************************
功能:   将信息写入BUF中
参数:   Ringbuff:     将要写入的信息写入的缓冲区指针
        pPushMessage: 将要写入的信息指针
返回值: 0 写入成功；-1写入失败
******************************************************************/
int Ringbuff_write(psJ1939_buff_message Ringbuff, psJ1939_buff_message pPushMessage)
{
    char i;
    if(Ringbuff == NULL)
    {
        return BUF_POINT_NULL;   
    }
    if(Ringbuff == RingSNDbuff)  //根据写入的缓冲区区别，判断是否已经满了
    {
        if(RingSNDbuff_isfull())
        {
            return BUF_FULL;   //缓冲满，写入错误
        }
    
        Ringbuff[BuffSND_addr_write].PGN       = pPushMessage->PGN;     //PGN号
        Ringbuff[BuffSND_addr_write].PGNnum    = pPushMessage->PGNnum;  //PGN号
        
        Ringbuff[BuffSND_addr_write].Priority = pPushMessage->Priority; //优先级
        Ringbuff[BuffSND_addr_write].Reserved = pPushMessage->Reserved; //保留位
        
        for(i=0; i<8; i++)
        {
            Ringbuff[BuffSND_addr_write].Data[i] = pPushMessage->Data[i];  //拷贝数据
        }
        
        BuffSND_addr_write = (BuffSND_addr_write+1) % Ringbuff_sedSize; //循环指向下一个项    
    }
    else if(Ringbuff == RingRCVbuff)  //根据写入的缓冲区区别，判断是否已经满了
    {
        if(RingRCVbuff_isfull())
        {
            return BUF_FULL;   //缓冲满，写入错误
        }       
        Ringbuff[BuffRCV_addr_write].PGN    = pPushMessage->PGN;  //PGN号      
        Ringbuff[BuffRCV_addr_write].PGNnum = pPushMessage->PGNnum;  //PGN号 
        
        Ringbuff[BuffRCV_addr_write].Priority = pPushMessage->Priority; //优先级
        Ringbuff[BuffRCV_addr_write].Reserved = pPushMessage->Reserved; //保留位   
        
        for(i=0; i<8; i++)
        {
            Ringbuff[BuffRCV_addr_write].Data[i] = pPushMessage->Data[i];  //拷贝数据
        }
        
        BuffRCV_addr_write = (BuffRCV_addr_write+1) % Ringbuff_rcvSize; //循环指向下一个项    
        return BUF_WRSUCC;
    }
    else                             // 缓存 BUFF  不存在
    {
        return BUF_NEXIT; 
    }
    return BUF_WRSUCC;
}

/********************************************
功能:将信息读出BUF中 到指针pPullMessage
参数: 获取读出信息的指针
返回值: 0 读成功；-1读失败
*********************************************/
char Ringbuff_read(psJ1939_buff_message Ringbuff, psJ1939_buff_message pPullMessage)
{
    char i;
    if(Ringbuff == RingSNDbuff)  //根据写入的缓冲区区别，判断是否已经满了
    {
        if(RingSNDbuff_isempty())
        {
            return BUF_EMPTY;  
        }
    
        pPullMessage->PGN       = Ringbuff[BuffSND_addr_read].PGN;  //PGN号
        pPullMessage->PGNnum    = Ringbuff[BuffSND_addr_read].PGNnum;  //PGN号
        
        
        pPullMessage->Priority  = Ringbuff[BuffSND_addr_read].Priority; //优先级
        pPullMessage->Reserved = Ringbuff[BuffSND_addr_read].Reserved;//保留位
        
        for(i=0; i<8; i++)
        {
             pPullMessage->Data[i] = Ringbuff[BuffSND_addr_read].Data[i];  //拷贝数据
        }
        BuffSND_addr_read = (BuffSND_addr_read+1) % Ringbuff_sedSize; //循环指向下一个项 
    }
    else if(Ringbuff == RingRCVbuff)  //根据写入的缓冲区区别，判断是否已经满了
    {
        if(RingRCVbuff_isempty())
        {
            return BUF_EMPTY;   //缓冲空，读取错误
        }
    
        pPullMessage->PGN        = Ringbuff[BuffRCV_addr_read].PGN;  //PGN号
        pPullMessage->PGNnum = Ringbuff[BuffRCV_addr_read].PGNnum;  //PGN号
        
        pPullMessage->Priority  = Ringbuff[BuffRCV_addr_read].Priority; //优先级
        pPullMessage->Reserved = Ringbuff[BuffRCV_addr_read].Reserved;//保留位

        for(i=0; i<8; i++)
        {
            pPullMessage->Data[i] = Ringbuff[BuffRCV_addr_read].Data[i];  //拷贝数据
        }
        
        BuffRCV_addr_read = (BuffRCV_addr_read+1) % Ringbuff_rcvSize; //循环指向下一个项 
        return BUF_WRSUCC;
    }
    else                             // 缓存 BUFF  不存在
    {
        return BUF_NEXIT; 
    }
    return BUF_RDSUCC;
}

/**************************************************
功能: 检查BUFF是否为空
参数: 无
返回值: 
****************************************************/
int RingSNDbuff_isempty(void)
{
    if( BuffSND_addr_write == BuffSND_addr_read )
        return BUF_EMPTY; //缓冲空
    else 
        return BUF_NOR; //缓冲不空
}

/**************************************************
功能: 检查BUFF是否为满
参数: 无
返回值: 
****************************************************/
int RingSNDbuff_isfull(void)
{
    if( (BuffSND_addr_write+1) % Ringbuff_sedSize == BuffSND_addr_read )
        return BUF_FULL; //缓冲满
    else 
        return BUF_NOR; //缓冲不满
}

/**************************************************
功能: 检查BUFF是否为空
参数: 无
返回值: 
****************************************************/
int RingRCVbuff_isempty(void)
{
    if( BuffRCV_addr_write == BuffRCV_addr_read )
        return BUF_EMPTY; //缓冲空
    else 
        return BUF_NOR; //缓冲不空
}

/**************************************************
功能: 检查BUFF是否为满
参数: 无
返回值: 
****************************************************/
int RingRCVbuff_isfull(void)
{
    if( (BuffRCV_addr_write+1) % Ringbuff_rcvSize == BuffRCV_addr_read )
        return BUF_FULL; //缓冲满
    else 
        return BUF_NOR; //缓冲不满
}
