#include "Ringbuff_manger.h"
#include <string.h>
#include "pub_data.h"

static u8 BuffSND_addr_write;      //�ڻ������е�д��λ��
static u8 BuffSND_addr_read;      //�ڻ������Ķ�ȡλ��
sJ1939_buff_message  RingSNDbuff[Ringbuff_sedSize];  // ���λ�����
//ע�����Ϣ�����ڲ�����malloc������ڴ棬��������Ҫ�ͷ���

static u8 BuffRCV_addr_write;      //�ڻ������е�д��λ��
static u8 BuffRCV_addr_read;      //�ڻ������Ķ�ȡλ��
sJ1939_buff_message RingRCVbuff[Ringbuff_rcvSize];  // ���λ�����



/*******************************************
����:   ����շ����λ�����
����:   ��
����ֵ: ��
*********************************************/
void Ringbuff_setEmpty(void)
{
    BuffSND_addr_write = 0;
    BuffSND_addr_read  = 0;  
    BuffRCV_addr_write = 0;
    BuffRCV_addr_read  = 0;
}

/**************************************************
����:   �����ͺͽ����������λ������ڲ�����ȫ�����
����:   ��
����ֵ: ��
***************************************************/
void Ringbuff_init(void)
{
    u8 i,j;
    for(i =0;i<Ringbuff_sedSize;i++)  //��ջ��λ���������
    {
        RingSNDbuff[i].PGN         = -1;
        RingSNDbuff[i].PGNnum  = 0;
        for(j=0;j<8;j++)
            RingSNDbuff[i].Data[j]= 0;
    }
    BuffSND_addr_write = 0;  // ��дλ�ö���0��ʼ����������
    BuffSND_addr_read  = 0;

    for(i =0;i<Ringbuff_rcvSize;i++)  //��ջ��λ���������
    {
        RingRCVbuff[i].PGN         = -1;
        RingRCVbuff[i].PGNnum  = 0;
        for(j=0;j<8;j++)
            RingRCVbuff[i].Data[j]= 0;
    }
    BuffRCV_addr_write = 0;  // ��дλ�ö���0��ʼ����������
    BuffRCV_addr_read  = 0;
}
/*****************************************************************
����:   ����Ϣд��BUF��
����:   Ringbuff:     ��Ҫд�����Ϣд��Ļ�����ָ��
        pPushMessage: ��Ҫд�����Ϣָ��
����ֵ: 0 д��ɹ���-1д��ʧ��
******************************************************************/
int Ringbuff_write(psJ1939_buff_message Ringbuff, psJ1939_buff_message pPushMessage)
{
    char i;
    if(Ringbuff == NULL)
    {
        return -1;   
    }
    if(Ringbuff == RingSNDbuff)  //����д��Ļ����������ж��Ƿ��Ѿ�����
    {
        if(RingSNDbuff_isfull())
        {
            return BUF_FULL;   //��������д�����
        }
    
        Ringbuff[BuffSND_addr_write].PGN       = pPushMessage->PGN;     //PGN��
        Ringbuff[BuffSND_addr_write].PGNnum    = pPushMessage->PGNnum;  //PGN��
        
        Ringbuff[BuffSND_addr_write].Priority = pPushMessage->Priority; //���ȼ�
        Ringbuff[BuffSND_addr_write].Reserved = pPushMessage->Reserved; //����λ
        
        for(i=0; i<8; i++)
        {
            Ringbuff[BuffSND_addr_write].Data[i] = pPushMessage->Data[i];  //��������
        }
        
        BuffSND_addr_write = (BuffSND_addr_write+1) % Ringbuff_sedSize; //ѭ��ָ����һ����    
    }
    else if(Ringbuff == RingRCVbuff)  //����д��Ļ����������ж��Ƿ��Ѿ�����
    {
        if(RingRCVbuff_isfull())
        {
            return BUF_FULL;   //��������д�����
        }       
        Ringbuff[BuffRCV_addr_write].PGN    = pPushMessage->PGN;  //PGN��      
        Ringbuff[BuffRCV_addr_write].PGNnum = pPushMessage->PGNnum;  //PGN�� 
        
        Ringbuff[BuffRCV_addr_write].Priority = pPushMessage->Priority; //���ȼ�
        Ringbuff[BuffRCV_addr_write].Reserved = pPushMessage->Reserved; //����λ   
        
        for(i=0; i<8; i++)
        {
            Ringbuff[BuffRCV_addr_write].Data[i] = pPushMessage->Data[i];  //��������
        }
        
        BuffRCV_addr_write = (BuffRCV_addr_write+1) % Ringbuff_rcvSize; //ѭ��ָ����һ����    
        return BUF_WRSUCC;
    }
    else                             // ���� BUFF  ������
    {
        return BUF_NEXIT; 
    }
    return BUF_WRSUCC;
}

/********************************************
����:����Ϣ����BUF�� ��ָ��pPullMessage
����: ��ȡ������Ϣ��ָ��
����ֵ: 0 ���ɹ���-1��ʧ��
*********************************************/
char Ringbuff_read(psJ1939_buff_message Ringbuff, psJ1939_buff_message pPullMessage)
{
    char i;
    if(Ringbuff == RingSNDbuff)  //����д��Ļ����������ж��Ƿ��Ѿ�����
    {
        if(RingSNDbuff_isempty())
        {
            return BUF_EMPTY;  
        }
    
        pPullMessage->PGN       = Ringbuff[BuffSND_addr_read].PGN;  //PGN��
        pPullMessage->PGNnum    = Ringbuff[BuffSND_addr_read].PGNnum;  //PGN��
        
        
        pPullMessage->Priority  = Ringbuff[BuffSND_addr_read].Priority; //���ȼ�
        pPullMessage->Reserved = Ringbuff[BuffSND_addr_read].Reserved;//����λ
        
        for(i=0; i<8; i++)
        {
             pPullMessage->Data[i] = Ringbuff[BuffSND_addr_read].Data[i];  //��������
        }
        BuffSND_addr_read = (BuffSND_addr_read+1) % Ringbuff_sedSize; //ѭ��ָ����һ���� 
    }
    else if(Ringbuff == RingRCVbuff)  //����д��Ļ����������ж��Ƿ��Ѿ�����
    {
        if(RingRCVbuff_isempty())
        {
            return BUF_EMPTY;   //��������д�����
        }
    
        pPullMessage->PGN        = Ringbuff[BuffRCV_addr_read].PGN;  //PGN��
        pPullMessage->PGNnum = Ringbuff[BuffRCV_addr_read].PGNnum;  //PGN��
        
        pPullMessage->Priority  = Ringbuff[BuffRCV_addr_read].Priority; //���ȼ�
        pPullMessage->Reserved = Ringbuff[BuffRCV_addr_read].Reserved;//����λ

        for(i=0; i<8; i++)
        {
            pPullMessage->Data[i] = Ringbuff[BuffRCV_addr_read].Data[i];  //��������
        }
        
        BuffRCV_addr_read = (BuffRCV_addr_read+1) % Ringbuff_rcvSize; //ѭ��ָ����һ���� 
        return BUF_WRSUCC;
    }
    else                             // ���� BUFF  ������
    {
        return BUF_NEXIT; 
    }
    return BUF_RDSUCC;
}


/**************************************************
����: ���BUFF�Ƿ�Ϊ��
����: ��
����ֵ: 
****************************************************/
int RingSNDbuff_isempty(void)
{
    if( BuffSND_addr_write == BuffSND_addr_read )
        return BUF_EMPTY; //�����
    else 
        return BUF_NOR; //���岻��
}

/**************************************************
����: ���BUFF�Ƿ�Ϊ��
����: ��
����ֵ: 
****************************************************/
int RingSNDbuff_isfull(void)
{
    if( (BuffSND_addr_write+1) % Ringbuff_sedSize == BuffSND_addr_read )
        return BUF_FULL; //������
    else 
        return BUF_NOR; //���岻��
}

/**************************************************
����: ���BUFF�Ƿ�Ϊ��
����: ��
����ֵ: 
****************************************************/
int RingRCVbuff_isempty(void)
{
    if( BuffRCV_addr_write == BuffRCV_addr_read )
        return BUF_EMPTY; //�����
    else 
        return BUF_NOR; //���岻��
}

/**************************************************
����: ���BUFF�Ƿ�Ϊ��
����: ��
����ֵ: 
****************************************************/
int RingRCVbuff_isfull(void)
{
    if( (BuffRCV_addr_write+1) % Ringbuff_rcvSize == BuffRCV_addr_read )
        return BUF_FULL; //������
    else 
        return BUF_NOR; //���岻��
}

static error_t Ringbuff_pro_module(system_state_t state)
{
    if (STATE_INITIALIZING == state)
    {
        Ringbuff_init();
    }
    else if ((STATE_UP == state) || (STATE_RUN == state))
    {
    ;
    }
    else if (STATE_DESTROYING == state)
    {
        ;
    }
    
    return 0;
}
