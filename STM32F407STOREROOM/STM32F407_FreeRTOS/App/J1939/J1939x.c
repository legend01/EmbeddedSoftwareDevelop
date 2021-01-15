#include "J1939x.h"
#include "app_can.h"
#include "log_printf.h"

sJ1939_transfeManger J1939_connect;

J1939_message_Rcv PGN_MessageRcv[PGN_MAX_Rcv];          // PDU ��Ӧ����Ϣ ��������ָ��
static char PGN_MessageRcvBuff[PGN_MessageRcvBuff_Max];    // pdu �������ڴ����

sPGNInfo TP_CM = {TP_CM_PF, 7, 8, 0, };
sPGNInfo TP_DT = {TP_DT_PF, 7, 8, 0, };

unsigned int time_record  = 0;  // ���ʱ�����ж϶�ʱ���мӣ������жϳ�ʱ����

sPGNInfo PGNInfoSend[PGN_MAX_Send+1] =
{
    
    { 256,      6,  8,  250, },  //CRM   //������ֽ׶�
    {9728,      6,  3,  250, },  //CHM
    
    {1792,      6,  7,  500, },  //cts //���������ý׶�
    {2048,      6,  6,  250, },  //cml
    {2560,      4,  1,  250, },  //cro
    {4608,      6,  6,   50, },  //ccs

    {6656,      4,  4,   10, },  //cst //���׶�
    
    {7424,      6,  5,  250, },  //csd //�������׶�
    
    {7936,      2,  4,  250, },  //CEM //������
    
    {TP_CM_PF,  7,  8,    0, },  //������� //�������
    {TP_DT_PF,  7,  8,    0, },  //��������
    
    {      0,   0,  0,    0, },  //MAX //������
};

sPGNInfo PGNInfoRcv[PGN_MAX_Rcv+1] =
{
    
    { 512,      6,  41,  250, },   //BRM  //������ֽ׶�
    {9984,      6,   2,  250, },  //BHM

    {1536,      6,  13,  500, },  //bcp //���������ý׶�
    {2304,      4,   1,  250, },    //bro
    
    {4096,      6,  5,   500, },    //bcl //���׶�
    {4352,      6,  9,   250, },    //bcs
    {4864,      6,  7,   250, },    //bsm

    {5376,      6,  8,  1000, },  //bmv
    {5632,      6,  8,  1000, },  //bmt
    {5888,      6,  8,  1000, },  //bsp

    {6400,      4,  4,   10, },  //bst
    
    {7168,      6,  7,  250, },  //BSD //�������׶�
    
    {7680,      2,  4,  250, },  //BEM //������
    
    {TP_CM_PF,  7,  8,    0, },  //�������  //�������
    {TP_DT_PF,  7,  8,    0, }, //��������
    
    {0,         0,  0,    0, },   //MAX  //������
};

/**************************************************
����: ��ʼ��PGN����͸�PGN�����ݲ��ַ���ռ�
����: ��
����: ��
****************************************************/
int PGN_MessageRcv_Init(void)
{
    PGNTypeRcv PGN;
    char* PGN_MessageRcvBuff_END = PGN_MessageRcvBuff + PGN_MessageRcvBuff_Max;
        char* PGNDataBUF_start = PGN_MessageRcvBuff;

    PGN_MessageRcvBuff[0] = 0XAA;//��0XAA ��ÿ��PGN�����ݸ���
    PGNDataBUF_start++;

    for(PGN = BRM; PGN<TPCM_rcv; PGN++)
    {
        PGN_MessageRcv[PGN].PDUformat  = PGNInfoRcv[PGN].PGNnum;
        PGN_MessageRcv[PGN].dataLen    = PGNInfoRcv[PGN].dataLen;
        PGN_MessageRcv[PGN].valid      = 0;
        PGN_MessageRcv[PGN].data       = (char*)PGNDataBUF_start;


  //      if(PGN != BMV && PGN != BMT && PGN != BSP)                //����������PGN���������ڴ��⣬��������PGN�������ݲ��ֵ��ڴ�
        if(PGN != BMV && PGN != BSP)
            {
            PGNDataBUF_start  = PGNDataBUF_start+(PGNInfoRcv[PGN].dataLen + 1);

            if(PGNDataBUF_start > PGN_MessageRcvBuff_END)
            {
                LOG_PRINTF("PGN_MessageRcvBuff is small!\r\n");
                return -1;
            }
            *(PGNDataBUF_start-1) = 0xAA;                   //��0XAA ��ÿ��PGN�����ݸ���, ��ÿ��PGN���ݵ�β��
        }
        else
        {
            PGN_MessageRcv[PGN].data = NULL;                   // ������PGN��ʱ������Ҳ�����������ڴ棬ָ��ָ��NULL
        }
        if(PGN_MessageRcv[PGN].data && *(PGN_MessageRcv[PGN].data-1) != 0xAA)  //����ڴ�����Ƿ���ȷ
        {
            LOG_PRINTF("PGN %d data Memery init border Error!\r\n",PGN);
            return -1;
        }
    }
    return 0;
}

/**************************************************
����: ���PGN����ʹ��������Ч
����: ��
����: -1 Ϊ����  0Ϊ����
****************************************************/
char PGN_MessageRcv_clear(void)
{
    PGNTypeRcv PGN;
    for(PGN = BRM; PGN<TPCM_rcv; PGN++)
    {
        PGN_MessageRcv[PGN].valid = 0; // ʹPGN����ʹ��������Ч

        if(PGN_MessageRcv[PGN].data && *(PGN_MessageRcv[PGN].data-1) != 0xAA)  //����ڴ�����Ƿ���ȷ
        {
            LOG_PRINTF("PGN %d data Memery init border Error!\r\n",PGN);
            return -1;
        }
    }
    return 0;
}


/**************************************************
����: ��PDU������װCanTxMsg,��CAN�ײ���뷢�ͳ�ȥ
����: PDUָ��
����: 0�ɹ� ��Ϊ0��ʧ��
****************************************************/
int SendOnePacket(void *arg, uint32_t* id, uint8_t *dat, uint8_t* len)
{
    J1939_message *msg = (J1939_message *)arg;
    if(msg->dataLen > 8)
        return -1;
    *len = msg->dataLen;
    *id = 0x1fffffff & (msg->priority<<26 |msg->PGNnum<<8 | msg->destAddr << 8| msg->sourceAddr); //29λ
    *id = (*id)|0xf400;
    memcpy(dat, msg->data, msg->dataLen);
    return 0;                                     //���ͳɹ�
}

int J1939_SendOnePacket( J1939_message *msg)
{
    return CAN1_Send_Msg(SendOnePacket, msg);
}

/**************************************************
����: �ӷ���RingSNDbuff�ж�ȡsJ1939_buff_message����ת����J1939_message, ���ͳ�ȥ
����: ��
����: 0�ɹ� ��Ϊ0��ʧ��, ������ѭ����Ϊ����
****************************************************/
int GetmsgconvertToSend(void)
{
    char i;
    int Ringbuff_readstate = 0;
    sJ1939_buff_message message;
    J1939_message J1939_message_send;

    Ringbuff_readstate = Ringbuff_read(RingSNDbuff, &message); //�ӷ���BUF�ж������ݵ�message
    if(Ringbuff_readstate == BUF_NEXIT)      //�����ڷ���-1
    {
        LOG_PRINTF("not support this ringbuff!\r\n");
         return -1;
    }
    else if(Ringbuff_readstate == BUF_EMPTY) //��ȡ������Ϊ�գ�����������
    {
        return 0;
    }
    //������ȡ������,�������Ϣ
    if(message.PGN >= CRM && message.PGN < PGN_MAX_Send)//֧�ֵ�PGN����
    {
        /*********************��sJ1939_buff_message���͵���������J1939_message���ͷ���������***************************/
        J1939_message_send.priority     = PGNInfoSend[message.PGN].priority;//ͨ��SPN������SPN����Ϣ

        if(PGNInfoSend[message.PGN].dataLen <= 8)
            J1939_message_send.dataLen  = PGNInfoSend[message.PGN].dataLen;
        else
            J1939_message_send.dataLen  = 8;

        J1939_message_send.destAddr     = BMS_Addr;
        J1939_message_send.PGNnum       = PGNInfoSend[message.PGN].PGNnum;

        if(J1939_message_send.PGNnum > 240)  //PGN����240���ϣ��� PS��ָ����ַ
        {
            J1939_message_send.PGNnum = (J1939_message_send.PGNnum & 0xffffff00)|J1939_message_send.destAddr;
        }

        J1939_message_send.sourceAddr   = Charger_Addr;   //Դ��ַ

        for(i=0; i<J1939_message_send.dataLen; i++)   //PDU 8������λ
        {
            J1939_message_send.data[i] = message.Data[i];
        }
    }
    else                                                //��֧�ֵ�PGN����
    {
        LOG_PRINTF("NOT support PGN = %d!\r\n",message.PGN);
        return -1;
    }

        int temp =  J1939_SendOnePacket(&J1939_message_send);
     if(temp != 0) //�������
     {
        LOG_PRINTF("J1939_SendOnePacket Error! %d\r\n",temp);
        return -1;
     }
    return 0;
}
/***************************************************************************
����: CAN �жϴ����� ,��������CanTxMsg �˶���Ϣ��
        ����sJ1939_buff_message�������BUF
����: ��
����ֵ:
*****************************************************************************/
int GetmessageToRcvbuff(void* rcv)
{
    unsigned char j;
    J1939_message msg_temp;
    sJ1939_buff_message sJ1939_buff_message_temp;
    CAN_RxPacketTypeDef* RxMessage = (CAN_RxPacketTypeDef*)rcv; /* ��CAN�жϵ��øú���rcv�ǽ��յ���CAN���� */
    CAN_RxHeaderTypeDef RxHeaderInf = RxMessage->hdr;
#ifdef LOG_CAN_RCV
    LOG_PRINTF("rcv: ID %x ,DATA:",    RxHeaderInf.ExtId );
    for(uint8_t i =0; i< RxHeaderInf.DLC; i++)
        LOG_PRINTF(" %x ",    RxMessage->message[i]);
    LOG_PRINTF("\r\n" );
#endif
    /**
    +------------+-------+--------+-----------------+------------------+-----------------+
    |     P      |  R    |   DP   |    PF           |      PS          |      SA         |
    +------------+-------+--------+-----------------+------------------+-----------------+
    |  ����Ȩ    | ����λ | ����ҳ  |    PDU��ʽ      |      Ŀ���ַ     |   Դ��ַ        |
    +------------+-------+--------+-----------------+------------------+-----------------+
    |  3bit      |  1bit | 1bit   |    8bit         |     8bit         |    8bit         | 
    +------------+-------+--------+-----------------+------------------+-----------------+
     */
    if((RxHeaderInf.ExtId & 0xffff)!= 0x56f4) return 0; /* �ж�PS+SAĿ���ַ��Դ��ַ�Ƿ�Ϊ ����0x56 BMS0xF4 */

    sJ1939_buff_message_temp.PGN = -1;

    msg_temp.PGNnum     = (RxHeaderInf.ExtId>>8) & 0x3ff00;   //PGNֵ /* PGN = R, DP, PF, PS */
    msg_temp.sourceAddr =  RxHeaderInf.ExtId & 0xff;          //Դ��ַ
    
    /****************************************************/ 
    msg_temp.priority = (RxHeaderInf.ExtId>>26) & 0x07;       //���ȼ�
    msg_temp.Reserved = (RxHeaderInf.ExtId >>24 )&0x03;       //Ԥ��
    
    /****************************************************/
    

    //sJ1939_buff_message_temp.Prio=(RxHeaderInf.ExtId>>24)& 0x1c; 
    sJ1939_buff_message_temp.PGNnum = msg_temp.PGNnum;
    sJ1939_buff_message_temp.PGN    = -1;  //ֻ�����ʵ��PGNֵ����ȷ�ϵ�PGN����ֵ�Ĺ���������ѭ����
    
   /****************************************************/
    
    sJ1939_buff_message_temp.Priority = msg_temp.priority;
    sJ1939_buff_message_temp.Reserved = msg_temp.Reserved;
    
   /****************************************************/

    for(j=0; j<8; j++)
    {
        sJ1939_buff_message_temp.Data[j] = RxMessage->message[j]; //��������
    }

    if(Ringbuff_write(RingRCVbuff, &sJ1939_buff_message_temp) != BUF_WRSUCC) //д�������
    {
        LOG_PRINTF("Interrupt Ringbuff_write Error!\r\n"); 
        return -1;                                //������֧�ֵ�PGN�����ж�
    }
        return 0;
}

/***************************************************************************
����: �ӽ���BUF�ж�ȡ���ݣ��ٸ����жϰ�PDU��Ϣ������Ž�PGN������
����: ��
����ֵ: -1 ���� 0��ȷ
*****************************************************************************/
int GtmgFrRcvbufToPGN(void)
{
    sJ1939_buff_message message;
    uint8_t i;
    pJ1939_message_Rcv  pJ1939_message_Rcv_temp;

    char* pJ1939mg_data = message.Data;
    PGNTypeRcv PGNrcv_INDEX;

    int Ringbuff_readstate = Ringbuff_read(RingRCVbuff, &message); //�ӽ���BUF�ж������ݵ�message

    if(Ringbuff_readstate == BUF_NEXIT)//�����ڷ���-1
    {
        LOG_PRINTF("convert_BUFmsgToJ1939msgArray-------Ringbuff_read Fail!\r\n");
        return -1;
    }
    else if(Ringbuff_readstate == BUF_EMPTY) //��ȡ������Ϊ�գ�����������
    {
        return 0;
    }
    /******************����PGN����ʵֵ������PGN*******************/
    for(PGNrcv_INDEX=BRM; PGNrcv_INDEX <= PGN_MAX_Rcv; PGNrcv_INDEX++)  //��ѯ��Ӧ��PGN �������е�����
    //ע������Ⱥ��Ǳ�Ҫ�ģ��������һ��˵���ڽ���PGN��������û�б���������
    {
       // if(PGNrcv_INDEX == BMT)
       //     continue;
        if(PGNrcv_INDEX >= PGN_MAX_Rcv)              //������֧�ֵ�PGNֱ������
        {
            LOG_PRINTF("this recive PGNNUM %x is not support!\r\n", message.PGNnum);
            return -1;                             
        }

        if(message.PGNnum == PGNInfoRcv[PGNrcv_INDEX].PGNnum)
        {
            if(PGNInfoRcv[PGNrcv_INDEX].dataLen > 8)
            {
                LOG_PRINTF("this recive PGNNUM %d data length is longer than 8!\r\n", message.PGNnum);
                return -1;
            }

            message.PGN = PGNrcv_INDEX; //�ҵ�PGN������
            if((message.PGN == BMV) || (message.PGN == BSP) || (message.PGN == BMT))
                return -1;
            
            
             /****************************************************/
             if(message.Priority != PGNInfoRcv[PGNrcv_INDEX].priority)
             {
                LOG_PRINTF("this recive PGNNUM %d priority is error", message.PGNnum);
                return -1;
             }
           
            if(message.Reserved != 0x00)
            {
                LOG_PRINTF("this PGNNUM %d reserved data is wrong!\r\n", message.PGNnum);
                return -1;
            }
             /****************************************************/
            
            break;
        }
    }

    pJ1939_message_Rcv_temp = &(PGN_MessageRcv[message.PGN]);
    /*************************************************��������******************************************************/
    if(message.PGN >= BRM && message.PGN < TPCM_rcv)
    {
        if(pJ1939_message_Rcv_temp->dataLen > 8)
        {
            LOG_PRINTF("this recive PGNNUM %d data length is longer than 8!\r\n", message.PGNnum);
            return -1;
        }
        pJ1939_message_Rcv_temp->valid = 0;           //д֮ǰ��������Ч
        for(i=0; i<pJ1939_message_Rcv_temp->dataLen; i++)
        {
            pJ1939_message_Rcv_temp->data[i] = message.Data[i];
        }
        if(pJ1939_message_Rcv_temp->data[i] == 0XAA && *(char*)(pJ1939_message_Rcv_temp->data-1) == 0XAA )  //�˶Էֽ��ߣ��ֽ�������
        {
            pJ1939_message_Rcv_temp->valid = 1;       //д��ɽ���������Ч
            return 0;
        }
        else
        {
            LOG_PRINTF("this PGN %d date Border Error !\r\n", message.PGNnum);
            pJ1939_message_Rcv_temp->valid = 0;       //�ֽ��ߴ���
            return -1;
        }
    }
    /*************************************************�����ӹ���****************************************************/
    if(message.PGN == TPCM_rcv)
    {
      //  char* pJ1939mg_data = message.Data;
      //  sPGNInfo PGNrcv_INDEX;
        if(pJ1939mg_data[0] == TP_CM_RTS)                   //������,������յ�����RTS��ôҪ����CTS
        {
            //if(J1939_connect.connectState == Idle)   //�����ӹ��� ����ʱ�����������ӣ��������������ϵĶ�����ͬʱ����
            {
                /**
            +-------------+-------------+--------------+------------------+-----------------------+
            |   �����ֽ�16 | �ֽ���Ŀ    | ȫ�����ݰ���Ŀ | 0xFF����SAEʹ��  |  װ�����ݵĲ���Ⱥ���   |
            +-------------+-------------+--------------+------------------+-----------------------+
            |   1byte     |  2,3byte    |    4byte     |    5byte         |      6,7,8byte        |
            +-------------+-------------+--------------+------------------+-----------------------+
                 */
                J1939_connect.data_num         = (pJ1939mg_data[2]<<8) | pJ1939mg_data[1]; //���ݳ���
                J1939_connect.num_packet   =  pJ1939mg_data[3];                        //����
                J1939_connect.PGNnum       = (pJ1939mg_data[7]<<16) | (pJ1939mg_data[6]<<8) | pJ1939mg_data[5];
                J1939_connect.cur_packet       = 0;     //��ʼ�ڼ������ݰ�,��1��ʼ, 0Ϊ��δ���ô�������׼��

                //��PGN���������ѯ��ǰ�����Ӵ����PGN �Ƿ�֧��
                for(PGNrcv_INDEX = BRM; PGNrcv_INDEX <= TPCM_rcv; PGNrcv_INDEX++)
                {
                    if(PGNrcv_INDEX == TPCM_rcv)  //�Ѿ��������һ����˵����֧����������Ķ������  ERROR_SEND_PGNmulti_NOSUPPORT
                    {
                        LOG_PRINTF("this recive PGNmulti %d is not support!\r\n", message.PGNnum);
                        //���ͷ�������
                        MultiTrans_Manage_SEND(&J1939_connect, TP_CM_Abort);  //���ͷ�������
                        J1939_connect_clear();                               //��յ�ǰ���ӹ���
                        return -1;
                    }
                    if(J1939_connect.PGNnum == PGNInfoRcv[PGNrcv_INDEX].PGNnum)  //�˶�PGNʵ��ֵ
                    {
                        if( PGNInfoRcv[PGNrcv_INDEX].dataLen > 8 && J1939_connect.data_num == PGNInfoRcv[PGNrcv_INDEX].dataLen && J1939_connect.num_packet == (J1939_connect.data_num+6)/7)
                      
                        //if( PGNInfoRcv[PGNrcv_INDEX].dataLen > 8 && J1939_connect.num_packet == (J1939_connect.data_num+6)/7)
                            //�˶Գ��ȣ��ͳ�����������������, �˶��ֽ������˶԰���
                        {
                            J1939_connect.PGNindex = PGNrcv_INDEX;  //�����ӵ�PGN��ö��ֵ
                            break;
                        }
                        else
                        {
                            MultiTrans_Manage_SEND(&J1939_connect, TP_CM_Abort);  //���ͷ�������
                            J1939_connect_clear();                                  //��յ�ǰ���ӹ���
                            return -1;
                        }
                    }
                }

                if(PGNrcv_INDEX == BMV || PGNrcv_INDEX == BMT || PGNrcv_INDEX == BSP) //����������PGN���Ǳ�Ҫ
                {
                    LOG_PRINTF("Ignore  The PGN  BMV BMT BSP!!\r\n ");
                    return 0;
                }

                J1939_connect.cur_packet++;
                if(MultiTrans_Manage_SEND(&J1939_connect, TP_CM_CTS) == 0)  //���ͳɹ�,˵�����������Ѿ�����
                {
                    J1939_connect.connectState     = connected;    //�����Ѿ�����
                    J1939_connect.destAddr     = Charger_Addr; // ����������BMS���𣬷��͸����׮
                    J1939_connect.sourceAddr       = BMS_Addr;
                    time_record = 0;                            //��ʱ��ʼ  �ȴ�BMS������Ӧ
                }
                else                                            //������Ͳ��ɹ�
                {
                    //J1939_connect.connectState = Idle;
                }

            }
            //else                                      //˵��֮ǰ�Ѿ������������, ������ǰ��������
            //{

            //}
        }
        else if(pJ1939mg_data[0] == TP_CM_CTS)          //׼������
        {}
        else if(pJ1939mg_data[0] == TP_CM_EndofMsgAck)  //��Ϣ����Ӧ��
        {}
        else if(pJ1939mg_data[0] == TP_CM_Abort)        //���յ���������
        {
            if(J1939_connect.connectState == Idle)  //δ���� ȴ�յ������������
            {
                LOG_PRINTF("1939_connect is not connect But recive TP_CM_Abort PGN :%x,%x,%x \r\n", pJ1939mg_data[5], pJ1939mg_data[6], pJ1939mg_data[7]);
                return -1;
            }
            else if(J1939_connect.PGNnum == ((pJ1939mg_data[7]<<16) | (pJ1939mg_data[6]<<8) | pJ1939mg_data[5]))
            //�˶ԶԷ����������ӹ����PGN��
            {
                J1939_connect_clear();        //
            }
            else
            {
                LOG_PRINTF("TP_CM_Abort but PGN is Error!\r\n");
            }
        }
        else if(pJ1939mg_data[0] == TP_CM_BAM)          //�㲥������Ϣ
        {}
    }
    /*************************************************���������ݴ���************************************************/
    else if(message.PGN == TPDT_rcv)        //���ӹ��� �µ����ݴ���
    {
        char n;
        if(J1939_connect.connectState != Idle)  //�Ѿ�����,��PGN�˶���ȷ
        {
            if(time_record > timeout_T2)   //�����ʱ�� ��������
            {
                LOG_PRINTF("timeout in TP_DT !\r\n", message.PGNnum);
                MultiTrans_Manage_SEND(&J1939_connect, TP_CM_Abort);  //���ͷ�������
                J1939_connect_clear();
            }
            else                          //δ��ʱ�� �˶Բ���ȡ����
            {
                if(J1939_connect.cur_packet == pJ1939mg_data[0])// �˶Ե�ǰ�����
                {
                    J1939_connect.data_position = (J1939_connect.cur_packet-1)*7; //����BUF�������ݵ�λ��,�����һ����Ϊ7�ı�����ת

                    if(J1939_connect.cur_packet < J1939_connect.num_packet)  //�������һ��, ֱ�ӿ���
                    {
                        PGN_MessageRcv[J1939_connect.PGNindex].valid = 0; //д����֮ǰʹPGN���ݲ�����Ч����ֹδд�꣬Ӧ�ò��
                        for(n=1; n<8; n++)   //�������ݵ��������ݵ�BUF��, 7������
                        {
                            PGN_MessageRcv[J1939_connect.PGNindex].data[J1939_connect.data_position+n-1] = pJ1939mg_data[n];//ʣ�µ�7�����ݿ�����BUFF��
                        }

                        J1939_connect.cur_packet++;
                        MultiTrans_Manage_SEND(&J1939_connect, TP_CM_CTS);  //���һ�����ٷ��ʹ�����������һ��
                        time_record = 0;                //��ʱ��ʼ
                    }

                    else if(J1939_connect.cur_packet == J1939_connect.num_packet) //���յ����һ��
                    {
                        PGN_MessageRcv[J1939_connect.PGNindex].valid = 0; //д����֮ǰʹPGN���ݲ�����Ч����ֹδд�꣬Ӧ�ò��
                        for(n=1; n<(J1939_connect.data_num-J1939_connect.data_position+1); n++)   //�������ݵ��������ݵ�BUF��, ���ܲ���7������
                        {
                            PGN_MessageRcv[J1939_connect.PGNindex].data[J1939_connect.data_position+n-1] = pJ1939mg_data[n];
                        }

                        J1939_connect.data_position = J1939_connect.data_position + n- 1;
                        if(J1939_connect.data_num == J1939_connect.data_position) //�˶Կ����������ܳ���, ��ȷ����Ӧ��
                        {
                            MultiTrans_Manage_SEND(&J1939_connect, TP_CM_EndofMsgAck);  //�������Ӧ��

                            PGN_MessageRcv[J1939_connect.PGNindex].valid = 1;//ʹ�ܻ���������Ч
                            J1939_connect_clear();
                        }
                        else
                        {
                            PGN_MessageRcv[J1939_connect.PGNindex].valid = 0; //������Ч
                            J1939_connect.cur_packet = 1;                     //�ӵ�һ����ʼ���½�������
                            MultiTrans_Manage_SEND(&J1939_connect, TP_CM_CTS);
                            J1939_connect.cur_packet++;
                        }
                    }
                    else
                    {
                        /*��ǰ�����������,�������������û����ɣ��ٴη���CTS ������BMS��������*/
                        MultiTrans_Manage_SEND(&J1939_connect, TP_CM_EndofMsgAck);  //�������Ӧ��
                        //PGN_MessageRcv[J1939_connect.PGNindex].valid = 1;//ʹ�ܻ���������Ч
                        J1939_connect_clear();             //��ʱ��ʼ
                    }

                }
                else  //�����˶Բ��ϣ������� ��ǰ��¼�İ��������
                {
                    //if(J1939_connect.cur_packet <= 1 || J1939_connect.cur_packet > J1939_connect.num_packet)//��鵱ǰ���ķ�Χ
                    //{
                    //  J1939_connect.cur_packet = 1; //�����ǰ�����磬��ô��ǰ�������1��ʼ
                    //}
                    //else
                    //{
                        //J1939_connect.cur_packet;
                    //}

                    J1939_connect.cur_packet = 1;
                    LOG_PRINTF("the Package num is Error, rstart!\r\n");
                    MultiTrans_Manage_SEND(&J1939_connect, TP_CM_CTS);

                    time_record = 0;                //��ʱ��ʼ
                }
            }
        }
        else  //��δ����ȴ�յ�������� ��������һ�ȡ�������������ݵ�PGN����ֱ�ӷ���
        {
            LOG_PRINTF("not connet but the TPDT!\r\n");
            J1939_connect_clear();
            return 0;
        }
    }
        return 0;
}

/***************************************************************************
����: ���ӹ���������
����: 0 ���ӹ���ָ�룬1 ����
����ֵ:0 ���ͳɹ� -1 ����ʧ��
*****************************************************************************/
char MultiTrans_Manage_SEND(psJ1939_transfeManger pJ1939_connect_arrys, unsigned char CMMD)
{
    sJ1939_buff_message msg_send;
    psJ1939_transfeManger pJ1939_connect = pJ1939_connect_arrys;

    msg_send.PGN = TPCM_send;
    
    if(pJ1939_connect == NULL ||(CMMD !=TP_CM_CTS && CMMD !=TP_CM_EndofMsgAck && CMMD !=TP_CM_Abort))
    {
        LOG_PRINTF("pJ1939_connect is NULL or  NO support CMMD!\r\n");
        return -1;
    }

    msg_send.Data[0] = CMMD;
    switch(CMMD)
    {
        case TP_CM_CTS:           //׼������
        {
            /**
             * @brief:����ģʽ��׼������
            +-----------------+--------------------+--------------------------+---------------------+-------------------------+
            |  �����ֽ�17      |  �ɷ������ݰ���Ŀ   |  ��һ����Ҫ�������ݰ����  |   ����SAE�趨0xFF    |    װ�����ݲ���Ⱥ���     |
            +-----------------+--------------------+--------------------------+---------------------+-------------------------+
            |  1byte          |    2byte           |       3byte              |     4 5byte         |       6 7 8byte         |
            +-----------------+--------------------+--------------------------+---------------------+-------------------------+
             */
            msg_send.Data[1]        = 1;            //�ɷ��͵����ݰ�
            msg_send.Data[2]        = pJ1939_connect->cur_packet;       //���յ�һ������
#ifdef J1939_OneTime_Trans
            msg_send.Data[1]        = pJ1939_connect->num_packet;   //�ɷ��͵����ݰ�
            if(pJ1939_connect->cur_packet > 1)
                return 0;
#endif
            msg_send.Data[3]        = 0XFF;         //�涨
            msg_send.Data[4]        = 0XFF;
            break;
        }
       
        case TP_CM_EndofMsgAck:  //��Ϣ����Ӧ��
        {
            /**
             * @brief: ��Ϣ����Ӧ��
            +---------------------+-----------------+-------------------------+----------------------+---------------------+
            |  �����ֽ�19          |  ������Ϣ��С    |  ȫ�����ݰ���Ŀ          |    ������ASEʹ��0xFF  |      ����Ⱥ���      |
            +---------------------+-----------------+-------------------------+----------------------+---------------------+
            |     1byte           |   2 3byte       |     4byte               |      5byte           |       6 7 8byte     |
            +---------------------+-----------------+-------------------------+----------------------+---------------------+ 
             */
            msg_send.Data[1]        =  J1939_connect.data_num & 0xff;//�������ݳ���
            msg_send.Data[2]        = (J1939_connect.data_num>>8) & 0xff;
            msg_send.Data[3]        =  J1939_connect.num_packet;  //���ĳ���
            msg_send.Data[4]        = 0XFF;  //�涨
            break;
        }
        case TP_CM_Abort:          //��������
        {  
          
           J1939_message msg_cst;
            msg_cst.sourceAddr  = 0x56;
            msg_cst.destAddr      = 0xf4;
            msg_cst.priority        = PGNInfoSend[TPCM_send].priority;
            msg_cst.PGNnum      = PGNInfoSend[TPCM_send].PGNnum;
            msg_cst.dataLen       = PGNInfoSend[TPCM_send].dataLen;
            /**
             * @brief: J1939��������
            +-------------------+------------------+-----------------------+
            | �����ֽ�FF�������� |  ������SAEʹ�� FF |    װ�����ݲ���Ⱥ���   +
            +-------------------+------------------+-----------------------+
            |   1byte           |    2 3 4 5byte   |    6 7 8 byte         | 
            +-------------------+------------------+-----------------------+ 
             */
            msg_cst.data[0]        = 0xFF;
            msg_cst.data[1]        = 0xFF;
            msg_cst.data[2]        = 0xFF;
            msg_cst.data[3]        = 0XFF;
            msg_cst.data[4]        = 0XFF;
            msg_cst.data[5]        = J1939_connect.PGNnum & 0xff;
            msg_cst.data[6]        = (J1939_connect.PGNnum & 0xff00)  >> 8;
            msg_cst.data[7]        = (J1939_connect.PGNnum & 0xff0000)>>16;
            J1939_SendOnePacket(&msg_cst);
            return 0;  //�ɹ�����
        }
        default:
        {
            LOG_PRINTF("MultiTrans_Manage_SEND------------ CMMD is not support!\r\n ");
            return -1;  //����
        }
    }

    msg_send.Data[5]    =  J1939_connect.PGNnum & 0xff;
    msg_send.Data[6]    = (J1939_connect.PGNnum & 0xff00)  >> 8;
    msg_send.Data[7]    = (J1939_connect.PGNnum & 0xff0000)>>16;

    if(Ringbuff_write(RingSNDbuff, &msg_send) != BUF_WRSUCC) //д�������
    {
        LOG_PRINTF(" Ringbuff_write Error!\r\n");
        return -1;
    }

    return 0;  //�ɹ�����
}

/***************************************************************************
����: ��ʼ��J1939_connect
����: ��
����ֵ:��
*****************************************************************************/
void J1939_connect_clear(void)
{
    J1939_connect.connectState  = Idle; //�������, ���ӶϿ�
    J1939_connect.data_position = 0;
    J1939_connect.data_num     = 0;
    J1939_connect.cur_packet = 0;
    J1939_connect.destAddr = 0;
    J1939_connect.sourceAddr   = 0;
    J1939_connect.PGNnum   = 0;
    J1939_connect.num_packet= 0;
    J1939_connect.PGNindex = 0;
}

// static uint8_t J1939_pro_module(system_state_t state)
// {
//     if (STATE_INITIALIZING == state)
//     {
//         CAN_Reg(GetmessageToRcvbuff, 0);
//         J1939_connect_clear();
//         if(PGN_MessageRcv_Init() != 0)
//             return -1;
//     }
//     else if ((STATE_UP == state) || (STATE_RUN == state))
//     {
//         GtmgFrRcvbufToPGN();
//         GetmsgconvertToSend();
//     }
//     else if (STATE_DESTROYING == state)
//     {
//         ;
//     }

//     return 0;
// }


// void J1939_register()
// {
//     module_arg_t module_arg;

//     memset(&module_arg, 0, sizeof(module_arg));
//     module_arg.cycle = J1939_CYCLE_TIME;
//     module_arg.unit  = MODULE_MS;
//     module_register("J1939 process frame", MODULE_J1939, FRAMEWORK_LEVEL1,
//         J1939_pro_module, &module_arg);
// }
