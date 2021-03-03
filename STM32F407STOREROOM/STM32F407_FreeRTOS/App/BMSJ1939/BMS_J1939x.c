#include "BMS_J1939x.h"
#include "app_can.h"
#include "log_printf.h"

sJ1939_transfeManger J1939_connect;
sJ1939_transfeManger J1939_connect_send;

J1939_message_Rcv PGN_MessageRcv[PGN_MAX_Rcv];          // PDU 对应的信息 包括数据指针
static char PGN_MessageRcvBuff[PGN_MessageRcvBuff_Max];    // pdu 的数据内存分配

sJ1939_buff_message sJ1939_message_send;
static uint8_t J1939connect_senddata[J1939_Connectsed_Max];
static PGNTypeSend J1939connect_Temp_PGNindex = PGN_MAX_Send;

sPGNInfo TP_CM = {TP_CM_PF, 7, 8, 0, };
sPGNInfo TP_DT = {TP_DT_PF, 7, 8, 0, };

unsigned int time_record  = 0;  // 这个时间在中断定时器中加，用来判断超时作用

sPGNInfo PGNInfoRcv[PGN_MAX_Rcv+1] =
{
    {9728,      6,  3,  250, },  //CHM
    { 256,      6,  8,  250, },  //CRM   //充电握手阶段
    
    {1792,      6,  7,  500, },  //cts //充电参数配置阶段
    {2048,      6,  6,  250, },  //cml
    {2560,      4,  1,  250, },  //cro
    {4608,      6,  6,   50, },  //ccs

    {6656,      4,  4,   10, },  //cst //充电阶段
    
    {7424,      6,  5,  250, },  //csd //充电结束阶段
    
    {7936,      2,  4,  250, },  //CEM //错误报文
    
    {TP_CM_PF,  7,  8,    0, },  //传输管理 //多包传输
    {TP_DT_PF,  7,  8,    0, },  //传输数据
    
    {      0,   0,  0,    0, },  //MAX //结束符
};

sPGNInfo PGNInfoSend[PGN_MAX_Send+1] =
{
    {9984,      6,   2,  250, },  //BHM
    { 512,      6,  41,  250, },   //BRM  //充电握手阶段

    {1536,      6,  13,  500, },  //bcp //充电参数配置阶段
    {2304,      4,   1,  250, },    //bro
    
    {4096,      6,  5,   500, },    //bcl //充电阶段
    {4352,      6,  9,   250, },    //bcs
    {4864,      6,  7,   250, },    //bsm

    {5376,      6,  8,  1000, },  //bmv
    {5632,      6,  8,  1000, },  //bmt
    {5888,      6,  8,  1000, },  //bsp

    {6400,      4,  4,   10, },  //bst
    
    {7168,      6,  7,  250, },  //BSD //充电结束阶段
    
    {7680,      2,  4,  250, },  //BEM //错误报文
    
    {TP_CM_PF,  7,  8,    0, },  //多包传输  //传输管理
    {TP_DT_PF,  7,  8,    0, }, //传输数据
    
    {0,         0,  0,    0, },   //MAX  //结束符
};

/**************************************************
功能: 初始化PGN数组和给PGN的数据部分分配空间
参数: 无
返回: 1 success
****************************************************/
int PGN_MessageRcv_Init(void)
{
    PGNTypeRcv PGN;
    char* PGN_MessageRcvBuff_END = PGN_MessageRcvBuff + PGN_MessageRcvBuff_Max;
        char* PGNDataBUF_start = PGN_MessageRcvBuff;

    PGN_MessageRcvBuff[0] = 0XAA;//用0XAA 把每个PGN的数据隔开
    PGNDataBUF_start++;

    for(PGN = CRM; PGN<TPCM_rcv; PGN++)
    {
        PGN_MessageRcv[PGN].PDUformat  = PGNInfoRcv[PGN].PGNnum;
        PGN_MessageRcv[PGN].dataLen    = PGNInfoRcv[PGN].dataLen;
        PGN_MessageRcv[PGN].valid      = 0;
        PGN_MessageRcv[PGN].data       = (char*)PGNDataBUF_start;

        PGNDataBUF_start  = PGNDataBUF_start+(PGNInfoRcv[PGN].dataLen + 1);

        if(PGNDataBUF_start > PGN_MessageRcvBuff_END)
        {
            LOG_PRINTF("PGN_MessageRcvBuff is small!\r\n");
            return RET_ERROR;
        }
        *(PGNDataBUF_start-1) = 0xAA;                   //用0XAA 把每个PGN的数据隔开, 在每个PGN数据的尾部

        if(PGN_MessageRcv[PGN].data && *(PGN_MessageRcv[PGN].data-1) != 0xAA)  //检查内存界限是否正确
        {
            LOG_PRINTF("PGN %d data Memery init border Error!\r\n",PGN);
            return RET_ERROR;
        }
    }
    return SUCCESS_RET;
}

/**************************************************
功能: 清空PGN数组使其数据无效
参数: 无
返回: -1 为错误  1为正常
****************************************************/
uint8_t PGN_MessageRcv_clear(void)
{
    PGNTypeRcv PGN;
    for(PGN = CRM; PGN<TPCM_rcv; PGN++)
    {
        PGN_MessageRcv[PGN].valid = 0; // 使PGN数组使其数据无效

        if(PGN_MessageRcv[PGN].data && *(PGN_MessageRcv[PGN].data-1) != 0xAA)  //检查内存界限是否正确
        {
            LOG_PRINTF("PGN %d data Memery init border Error!\r\n",PGN);
            return RET_ERROR;
        }
    }
    return SUCCESS_RET;
}


/**************************************************
功能: 将PDU重新填装CanTxMsg,从CAN底层代码发送出去
参数: PDU指针
返回: 1成功 不为0则失败
****************************************************/
int SendOnePacket(void *arg, uint32_t* id, uint8_t *dat, uint8_t* len)
{
    J1939_message *msg = (J1939_message *)arg;
    if(msg->dataLen > 8)
        return DATA_LENGTH_FALSE;
    *len = msg->dataLen;
    *id = 0x1fffffff & (msg->priority<<26 |msg->PGNnum<<8 | msg->destAddr << 8| msg->sourceAddr); //29位
    // *id = (*id)|0xf400;
    memcpy(dat, msg->data, msg->dataLen);
    return SUCCESS_RET;                                     //发送成功
}

int J1939_SendOnePacket( J1939_message *msg)
{
    return CAN1_Send_Msg(SendOnePacket, msg);
}

/**************************************************
功能: 从发送RingSNDbuff中读取sJ1939_buff_message数据转换成J1939_message, 发送出去
参数: 无
返回: 1成功 不为0则失败, 放在主循环中为任务
****************************************************/
int GetmsgconvertToSend(void)
{
    char i;
    int Ringbuff_readstate = 0;
    sJ1939_buff_message message;
    J1939_message J1939_message_send;

    Ringbuff_readstate = Ringbuff_read(RingSNDbuff, &message); //从发送BUF中读出数据到message
    if(Ringbuff_readstate == BUF_NEXIT)      //不存在返回-1
    {
        LOG_PRINTF("not support this ringbuff!\r\n");
        return RET_ERROR;
    }
    else if(Ringbuff_readstate == BUF_EMPTY) //读取的数据为空，算正常现象
    {
        return SUCCESS_RET;
    }
    //正常读取到数据,则填充信息
    if(message.PGN >= CRM && message.PGN < PGN_MAX_Send)//支持的PGN处理
    {
        /*********************把sJ1939_buff_message类型的数据填充成J1939_message类型放入数组中***************************/
        J1939_message_send.priority     = PGNInfoSend[message.PGN].priority;//通过SPN索引查SPN的信息

        if(PGNInfoSend[message.PGN].dataLen <= 8)
            J1939_message_send.dataLen  = PGNInfoSend[message.PGN].dataLen;
        else
            J1939_message_send.dataLen  = 8;

        J1939_message_send.destAddr     = Charger_Addr;
        J1939_message_send.PGNnum       = PGNInfoSend[message.PGN].PGNnum;

        if(J1939_message_send.PGNnum > 240)  //PGN大于240以上，则 PS则指定地址
        {
            J1939_message_send.PGNnum = (J1939_message_send.PGNnum & 0xffffff00)|J1939_message_send.destAddr;
        }

        J1939_message_send.sourceAddr   = BMS_Addr;   //源地址

        for(i=0; i<J1939_message_send.dataLen; i++)   //PDU 8个数据位
        {
            J1939_message_send.data[i] = message.Data[i];
        }
    }
    else                                                //不支持的PGN处理
    {
        LOG_PRINTF("NOT support PGN = %d!\r\n",message.PGN);
        return RET_ERROR;
    }

    if(J1939_SendOnePacket(&J1939_message_send) != SUCCESS_RET) //打包发送
    {
        LOG_PRINTF("J1939_SendOnePacket Error! \r\n");
        return RET_ERROR;
    }
    return SUCCESS_RET;
}
/***************************************************************************
功能: CAN 中断处理函数 ,将读出的CanTxMsg 核对信息，
        构建sJ1939_buff_message放入接收BUF
参数: 无
返回值:
*****************************************************************************/
int GetmessageToRcvbuff(void* rcv)
{
    unsigned char j;
    J1939_message msg_temp;
    sJ1939_buff_message sJ1939_buff_message_temp;
    CAN_RxPacketTypeDef* RxMessage = (CAN_RxPacketTypeDef*)rcv; /* 从CAN中断调用该函数rcv是接收到的CAN数据 */
    CAN_RxHeaderTypeDef RxHeaderInf = RxMessage->hdr;

    /**
    +------------+-------+--------+-----------------+------------------+-----------------+
    |     P      |  R    |   DP   |    PF           |      PS          |      SA         |
    +------------+-------+--------+-----------------+------------------+-----------------+
    |  优先权    | 保留位 | 数据页  |    PDU格式      |      目标地址     |   源地址        |
    +------------+-------+--------+-----------------+------------------+-----------------+
    |  3bit      |  1bit | 1bit   |    8bit         |     8bit         |    8bit         | 
    +------------+-------+--------+-----------------+------------------+-----------------+
     */
    if((RxHeaderInf.ExtId & 0xffff)!= 0xf456) return FALSE_ADDRESS; /* 判断PS+SA目标地址和源地址是否为 充电机0x56 BMS0xF4 */

    sJ1939_buff_message_temp.PGN = -1;

    msg_temp.PGNnum     = (RxHeaderInf.ExtId>>8) & 0x3ff00;   //PGN值 /* PGN = R, DP, PF, PS */
    msg_temp.sourceAddr =  RxHeaderInf.ExtId & 0xff;          //源地址
    
    /****************************************************/ 
    msg_temp.priority = (RxHeaderInf.ExtId>>26) & 0x07;       //优先级
    msg_temp.Reserved = (RxHeaderInf.ExtId >>24 )&0x03;       //预留
    
    /****************************************************/
    
    sJ1939_buff_message_temp.PGNnum = msg_temp.PGNnum;
    sJ1939_buff_message_temp.PGN    = -1;  //只提出真实的PGN值，把确认的PGN索引值的工作交给主循环做
    
   /****************************************************/
    
    sJ1939_buff_message_temp.Priority = msg_temp.priority;
    sJ1939_buff_message_temp.Reserved = msg_temp.Reserved;
    
   /****************************************************/

    for(j=0; j<8; j++)
    {
        sJ1939_buff_message_temp.Data[j] = RxMessage->message[j]; //拷贝数据
    }
    
    if(Ringbuff_write(RingRCVbuff, &sJ1939_buff_message_temp) != BUF_WRSUCC) //写入队列中
    {
        LOG_PRINTF("Interrupt Ringbuff_write Error!\r\n"); 
        return RET_ERROR;                                //碰到不支持的PGN跳出中断
    }
    return SUCCESS_RET;
}

/***************************************************************************
功能: 把信息推入环形缓冲区
参数: PGNindex: 推送的PGN索引号，pdata 数据指针
返回值: -1 错误 0正确
*****************************************************************************/

void PushMsgToRBUFF(PGNTypeSend PGNindex, char* pdata)
{
    if(PGNInfoSend[PGNindex].dataLen <= 8)
    {
        /**************************单包送************************/
        sJ1939_message_send.PGN = PGNindex;
        
        for(uint8_t i=0; i<PGNInfoSend[PGNindex].dataLen; i++)
        {
            sJ1939_message_send.Data[i] = pdata[i];
        }

        Ringbuff_write(RingSNDbuff, &sJ1939_message_send);
    }
    else
    {
        /**************************多连接发送************************/
        if(PGNInfoSend[PGNindex].dataLen > J1939_Connectsed_Max)
        {
            LOG_PRINTF("the PGN num is longer than  J1939_Connectsed_Max!\n");
            return;
        }

        if(J1939_connect_send.connectState == Idle)          //当空闲， 则发送连接管理请求
        {
            /*********设置好发送管理信息，等待返回第一包CTS确认连接*********/
            
            J1939_connect_send.data_num     = PGNInfoSend[PGNindex].dataLen;
            J1939_connect_send.num_packet   = (PGNInfoSend[PGNindex].dataLen+6)/7;
            J1939_connect_send.destAddr     = Charger_Addr;
            J1939_connect_send.sourceAddr   = BMS_Addr;
            J1939_connect_send.PGNindex     = PGNindex;
            J1939_connect_send.PGNnum       = PGNInfoSend[PGNindex].PGNnum;
            
            J1939connect_Temp_PGNindex  = PGNindex;
            sJ1939_message_send.PGN     = TPCM_send;
            sJ1939_message_send.Data[0] = 0x10;            //连接管理
            sJ1939_message_send.Data[1] = (J1939_connect_send.data_num >> 0) & 0xff;
            sJ1939_message_send.Data[2] = (J1939_connect_send.data_num >> 8) & 0xff;
            sJ1939_message_send.Data[3] = J1939_connect_send.num_packet;
            sJ1939_message_send.Data[4] = 0xff;
            sJ1939_message_send.Data[5] = (PGNInfoSend[PGNindex].PGNnum>>0)  & 0xff;
            sJ1939_message_send.Data[6] = (PGNInfoSend[PGNindex].PGNnum>>8)  & 0xff;
            sJ1939_message_send.Data[7] = (PGNInfoSend[PGNindex].PGNnum>>16) & 0xff;
            
            if(Ringbuff_write(RingSNDbuff, &sJ1939_message_send) == BUF_WRSUCC)
            {
                //J1939_connect_send.connectState = waitRcvData;  //等待接收数据状态
            }

            for(uint8_t j=0; j< PGNInfoSend[PGNindex].dataLen; j++)  // 需要发送的数据需要拷贝到连接管理数据缓存
            {
                J1939connect_senddata[j] = pdata[j];
            }
        }
    }     

}


static uint8_t FindPGNInPGNList(PGNTypeRcv PGNrcv_INDEX, sJ1939_transfeManger *J1939_connect, sJ1939_buff_message *message){
    for(PGNrcv_INDEX = BRM; PGNrcv_INDEX <= TPCM_rcv; PGNrcv_INDEX++)
    {
        if(PGNrcv_INDEX == TPCM_rcv)  //已经跳到最后一个，说明不支持这种种类的多包处理  ERROR_SEND_PGNmulti_NOSUPPORT
        {
            LOG_PRINTF("this recive PGNmulti %d is not support!\r\n", message->PGNnum);
            //发送放弃连接
            MultiTrans_Manage_SEND(J1939_connect, TP_CM_Abort);  //发送放弃连接
            J1939_connect_clear();                               //清空当前连接管理
            return RET_ERROR;
        }
        if(J1939_connect->PGNnum == PGNInfoRcv[PGNrcv_INDEX].PGNnum)  //核对PGN实际值
        {
            if( PGNInfoRcv[PGNrcv_INDEX].dataLen > 8 && J1939_connect->data_num == PGNInfoRcv[PGNrcv_INDEX].dataLen \
            && J1939_connect->num_packet == (J1939_connect->data_num+6)/7) //核对长度，和长度满足多包传输条件, 核对字节数，核对包数
            {
                J1939_connect->PGNindex = PGNrcv_INDEX;  //多连接的PGN的枚举值
                break;
            }
            else
            {
                MultiTrans_Manage_SEND(J1939_connect, TP_CM_Abort);  //发送放弃连接
                J1939_connect_clear();                                  //清空当前连接管理
                return RET_ERROR;
            }
        }
    }
}

static uint8_t Find_PGNNumFindPGN(PGNTypeRcv PGNrcv_INDEX, sJ1939_buff_message *message){
    for(PGNrcv_INDEX=CRM; PGNrcv_INDEX <= PGN_MAX_Rcv; PGNrcv_INDEX++)  //查询对应的PGN 在数组中的索引
    //注意这个等号是必要的，跳到最后一个说明在接收PGN数组里面没有遍历到索引
    {
       // if(PGNrcv_INDEX == BMT)
       //     continue;
        if(PGNrcv_INDEX >= PGN_MAX_Rcv)              //碰到不支持的PGN直接跳出
        {
            LOG_PRINTF("this recive PGNNUM %x is not support!\r\n", message->PGNnum);
            return RET_ERROR;                             
        }

        if(message->PGNnum == PGNInfoRcv[PGNrcv_INDEX].PGNnum)
        {
            if(PGNInfoRcv[PGNrcv_INDEX].dataLen > 8)
            {
                LOG_PRINTF("this recive PGNNUM %d data length is longer than 8!\r\n", message->PGNnum);
                return RET_ERROR;
            }

            message->PGN = PGNrcv_INDEX; //找到PGN的索引
            
            return SUCCESS_RET;
        }
    }
}

static uint8_t Tranmission_SinglePackage(pJ1939_message_Rcv pJ1939_message_Rcv_temp, sJ1939_buff_message *message){
    uint8_t i = 0;
    if(pJ1939_message_Rcv_temp->dataLen > 8)
    {
        LOG_PRINTF("this recive PGNNUM %d data length is longer than 8!\r\n", message->PGNnum);
        return RET_ERROR;
    }
    pJ1939_message_Rcv_temp->valid = 0;           //写之前置数据无效
    for(i=0; i<pJ1939_message_Rcv_temp->dataLen; i++)
    {
        pJ1939_message_Rcv_temp->data[i] = message->Data[i];
    }
    if(pJ1939_message_Rcv_temp->data[i] == 0XAA && *(char*)(pJ1939_message_Rcv_temp->data-1) == 0XAA )  //核对分界线，分界线正常
    {
        pJ1939_message_Rcv_temp->valid = 1;       //写完成将数据置有效
        pJ1939_message_Rcv_temp->PDUformat = message->PGNnum;
        return SUCCESS_RET;
    }
    else
    {
        LOG_PRINTF("this PGN %d date Border Error !\r\n", message->PGNnum);
        pJ1939_message_Rcv_temp->valid = 0;       //分界线错误
        return RET_ERROR;
    }
}



static uint8_t DataRequest(sJ1939_buff_message *message){
    uint8_t connect_packnum_once = 0;  //下一次发送的包数
    if(message->Data[3] != 0xff || message->Data[4] != 0xff) //检查 3，4个数据是否为FF
    {
        LOG_PRINTF("CTS 4,5 is not FF!\n");
        return RET_ERROR;
    }

    if(J1939_connect_send.PGNnum != (message->Data[5] | message->Data[6]<<8 | message->Data[7]<<16)) //检验PGN是否相同
    {
        LOG_PRINTF("PGN is not equal to J1939_connect_send PGNnum!\n");
        return RET_ERROR;
    }

    connect_packnum_once = message->Data[1]; //下一次发送的包数
    if(connect_packnum_once == 0)           //要求发送0个包则返回
    {
        return RET_ERROR;
    }

    J1939_connect_send.cur_packet = message->Data[2]; //下一次发送的数据包编号

    if(connect_packnum_once + J1939_connect_send.cur_packet -1 > J1939_connect_send.num_packet)
    {
        LOG_PRINTF(" out rang of the J1939_connect_send packnum!\n");
        return RET_ERROR;
    }

    for(uint8_t i=0; i<connect_packnum_once; i++) //一次发connect_packnum_once包
    {
        sJ1939_buff_message msg_send;
        uint8_t date_len = 0;
        
        msg_send.PGN     = TPDT_send;
        msg_send.Data[0] = J1939_connect_send.cur_packet + i;

        if( J1939_connect_send.num_packet == J1939_connect_send.cur_packet + i) //最后一包
        {
            date_len = J1939_connect_send.data_num % 7;
        }
        else{
            date_len = 7;    //不是最后一包
        }
        
        J1939_connect_send.data_position = (J1939_connect_send.cur_packet + i - 1) * 7;

        for(uint8_t j=0; j<date_len; j++)
        {
            msg_send.Data[j+1] = J1939connect_senddata[J1939_connect_send.data_position + j];
        }

        if(Ringbuff_write(RingSNDbuff, &msg_send) == BUF_WRSUCC)
        {

        }
    }

    J1939_connect_send.cur_packet += connect_packnum_once; //跟新当前发送到的包数
}

static uint8_t DataOverResponse(sJ1939_buff_message *message, uint8_t *frist_rcvTPCM){
    if(J1939_connect_send.data_num != message->Data[1] | message->Data[2]<<8)
    {
        LOG_PRINTF("EndofMsgAck data_num is not ringht!\n");
    //  return -1;
    }
    
    if(J1939_connect_send.num_packet !=  message->Data[3])
    {
        LOG_PRINTF("EndofMsgAck num_packet is not ringht!\n");
        return RET_ERROR;
    }

    if( 0xff !=  message->Data[4])
    {
        LOG_PRINTF("EndofMsgAck message->Data[4] is not 0Xff!\n");
        return RET_ERROR;
    }

    if(J1939_connect_send.PGNnum != (message->Data[5] | message->Data[6]<<8 | message->Data[7]<<16))
    {
        LOG_PRINTF("EndofMsgAck J1939_connect_send.PGNnum is not right!\n");
        return RET_ERROR;
    }
    J1939_connect_send_clear();
    *frist_rcvTPCM = 0;
}
 
static uint8_t DataInterrupt(sJ1939_buff_message *message, uint8_t *frist_rcvTPCM){
    if(message->Data[1]!= 0xff || message->Data[2]!= 0xff || message->Data[3]!= 0xff || message->Data[4]!= 0xff )
    {
        LOG_PRINTF("TP.CM_Abort 2 3 4 5 is not all 0xff !\n");
        return RET_ERROR;
    }
    
    if(J1939_connect_send.PGNnum != message->Data[5] | message->Data[6]<<8 | message->Data[7]<<16)
    {
        LOG_PRINTF("TP.CM_Abort J1939_connect_send.PGNnum is not right!\n");
        return RET_ERROR;
    }
    *frist_rcvTPCM = 0;
    J1939_connect_send_clear();
}
/***************************************************************************
功能: 从接收BUF中读取数据，再根据判断把PDU信息按分类放进PGN数组中
参数: 无
返回值: -1 错误 1正确
*****************************************************************************/
int GtmgFrRcvbufToPGN(void)
{
    sJ1939_buff_message message;
    
    uint8_t ret_value = 0;
    uint8_t singlePac_tra_retvalue = 0;
    uint8_t findpgningpnlist_retvalue = 0;
    pJ1939_message_Rcv  pJ1939_message_Rcv_temp;

    PGNTypeRcv PGNrcv_INDEX;

    uint8_t DataReqRetVal = 0;
    uint8_t DataOverResRetVal = 0;
    uint8_t DataInterruptRetVal = 0;

    int Ringbuff_readstate = Ringbuff_read(RingRCVbuff, &message); //从接收BUF中读出数据到message
    if(Ringbuff_readstate == BUF_NEXIT)//不存在返回-1
    {
        LOG_PRINTF("convert_BUFmsgToJ1939msgArray-------Ringbuff_read Fail!\r\n");
        return RET_ERROR;
    }
    if(Ringbuff_readstate == BUF_EMPTY) //读取的数据为空，算正常现象
    {
        return SUCCESS_RET;
    }
    /******************根据PGN的真实值找索引PGN*******************/
    ret_value = Find_PGNNumFindPGN(PGNrcv_INDEX, &message);
    if(ret_value == RET_ERROR){
        return RET_ERROR;
    }

    pJ1939_message_Rcv_temp = &(PGN_MessageRcv[message.PGN]);
    /*************************************************单包传输******************************************************/
    if(message.PGN >= CRM && message.PGN < TPCM_rcv)
    {
        singlePac_tra_retvalue = Tranmission_SinglePackage(pJ1939_message_Rcv_temp, &message);
        if(singlePac_tra_retvalue == RET_ERROR){
            return RET_ERROR;
        }else{
            return SUCCESS_RET;
        }
    }
    /*************************************************多连接管理****************************************************/
    switch(message.PGN){
        case TPCM_rcv:{
            uint8_t frist_rcvTPCM = 0;
        
            //未连接的多包管理CTS的第一包，判断是否符合连接要求
            if(frist_rcvTPCM == 0 && message.Data[0] == TP_CM_CTS && J1939_connect_send.connectState == Idle)
            {
                if(PGNInfoSend[J1939connect_Temp_PGNindex].PGNnum != (message.Data[5] | message.Data[6]<<8 | message.Data[7]<<16))
                {
                    LOG_PRINTF("the PGNnum is not the one which I want connect!\n");
                    return RET_ERROR;
                }

                if(PGNInfoSend[J1939connect_Temp_PGNindex].dataLen <= 8 || J1939connect_Temp_PGNindex >= PGN_MAX_Send)
                {
                    LOG_PRINTF("the J1939connect_Temp_PGNindex datalen is short than 8 or index is out of range !\n");
                    return RET_ERROR;
                }

                if(message.Data[3] != 0xff || message.Data[4] != 0xff)
                {
                    LOG_PRINTF("the first DT info [3] [4] is not ringht!\n");
                    return RET_ERROR ;
                }
                
                J1939_connect_send.connectState = connected;  //信息完成，连接
                frist_rcvTPCM = 1;
            }
            
            if(J1939_connect_send.connectState != connected)  // 所有的数据在未连接的情况下不可以进行数据处理
            {
                LOG_PRINTF(" J1939_connect_send.connectState is not connect! but rcv DT!\n");
                //return -1;
            }

            switch (message.Data[0])
            {
                case TP_CM_CTS: //检查头部是否为CTS 数据请求
                    DataReqRetVal = DataRequest(&message); //TODO:对返回值进行处理
                    if (DataReqRetVal == RET_ERROR){
                        return RET_ERROR;
                    }
                    break;
                case TP_CM_EndofMsgAck:
                    DataOverResRetVal = DataOverResponse(&message, &frist_rcvTPCM); //数据结束应答 //TODO:对返回值进行处理
                    if (DataOverResRetVal == RET_ERROR){
                        return RET_ERROR;
                    }
                    break;
                case TP_CM_Abort:
                    DataInterruptRetVal = DataInterrupt(&message, &frist_rcvTPCM); //数据中断 //TODO:对返回值进行处理
                    if (DataInterruptRetVal == RET_ERROR){
                        return RET_ERROR;
                    }
                    break;
                default:
                    break;
            }
            break;
        }
    }
    return SUCCESS_RET;
}

/***************************************************************************
功能: 初始化J1939_connect
参数: 无
返回值:无
*****************************************************************************/
void J1939_connect_clear(void)
{
    J1939_connect.connectState  = Idle; //传输结束, 连接断开
    J1939_connect.data_position = 0;
    J1939_connect.data_num     = 0;
    J1939_connect.cur_packet = 0;
    J1939_connect.destAddr = 0;
    J1939_connect.sourceAddr   = 0;
    J1939_connect.PGNnum   = 0;
    J1939_connect.num_packet= 0;
    J1939_connect.PGNindex = 0;
}

void J1939_connect_send_clear(void)
{
	J1939_connect_send.connectState  = Idle; //传输结束, 连接断开
	J1939_connect_send.data_position = 0;
	J1939_connect_send.data_num 	 = 0;
	J1939_connect_send.cur_packet 	 = 1;
	J1939_connect_send.destAddr		 = 0;
	J1939_connect_send.sourceAddr	 = 0;
	J1939_connect_send.PGNnum		 = 0;
	J1939_connect_send.num_packet	 = 0;
	J1939_connect_send.PGNindex		 = PGN_MAX_Send;
}