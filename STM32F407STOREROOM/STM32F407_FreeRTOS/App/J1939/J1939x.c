#include "J1939x.h"
#include "module.h"
#include "CANbus.h"
#include "pub_data.h"

sJ1939_transfeManger J1939_connect;

J1939_message_Rcv PGN_MessageRcv[PGN_MAX_Rcv];          // PDU 对应的信息 包括数据指针
static char PGN_MessageRcvBuff[PGN_MessageRcvBuff_Max];    // pdu 的数据内存分配

sPGNInfo TP_CM = {TP_CM_PF, 7, 8, 0, };
sPGNInfo TP_DT = {TP_DT_PF, 7, 8, 0, };

unsigned int time_record  = 0;  // 这个时间在中断定时器中加，用来判断超时作用

sPGNInfo PGNInfoSend[PGN_MAX_Send+1] =
{
    //充电握手阶段
    {
        {256,   6,  8,  250, },     //CRM   
        {9728,  6,  3,  250, },     //CHM
        //充电参数配置阶段
        {1792,  6,  7,  500, },     //cts
        {2048,  6,  6,  250, },     //cml
        {2560,  4,  1,  250, },     //cro
        {4608,  6,  6,   50, },     //ccs

        //充电阶段
        {6656,  4,  4,    10,},     //cst
        //充电结束阶段
        {7424,  6,  5,   250,},     //csd
        //错误报文
        {7936,  2,  4,   250,},     //CEM
        //多包传输
        {TP_CM_PF, 7, 8,  0, },     //传输管理
        {TP_DT_PF, 7, 8,  0, },     //传输数据
        //MAX
        {0,     0,  0,    0, },     //结束符
    },
};

sPGNInfo PGNInfoRcv[PGN_MAX_Rcv+1] =
{
    {
        //充电握手阶段
        {512,   6,  41,  250, },            //BRM   
        {9984,  6,  2,    250, },           //BHM
        //充电参数配置阶段
        {1536,  6,  13,   500, },           //bcp
        {2304,  4,  1,  250, },         //bro
        //充电阶段
        {4096,  6,  5,  500, },         //bcl
        {4352,  6,  9,  250, },         //bcs
        {4864,  6,  7,  250, },         //bsm

        {5376,  6,  8,   1000, },       //bmv
        {5632,  6,  8,   1000, },        //bmt
        {5888,  6,  8,   1000, },       //bsp

        {6400,  4,  4,    10, },        //bst
        //充电结束阶段
        {7168,  6,  7,   250, },        //BSD
        //错误报文
        {7680,  2,  4,   250, },        //BEM
        //多包传输
        {TP_CM_PF, 7, 8,        0,  },        //传输管理
        {TP_DT_PF, 7, 8,         0,  },     //传输数据
        //MAX
        {       0,       0, 0,        0,  },        //结束符
    },
};

enum
{
    DEBUG_J1939 = ERROR_BEGIN_DEBUG(MODULE_J1939),
};


/**************************************************
功能: 初始化PGN数组和给PGN的数据部分分配空间
参数: 无
返回: 无
****************************************************/
int PGN_MessageRcv_Init(void)
{
    PGNTypeRcv PGN;
    char* PGN_MessageRcvBuff_END = PGN_MessageRcvBuff + PGN_MessageRcvBuff_Max;
        char* PGNDataBUF_start = PGN_MessageRcvBuff;

    PGN_MessageRcvBuff[0] = 0XAA;//用0XAA 把每个PGN的数据隔开
    PGNDataBUF_start++;

    for(PGN = BRM; PGN<TPCM_rcv; PGN++)
    {
        PGN_MessageRcv[PGN].PDUformat  = PGNInfoRcv[PGN].PGNnum;
        PGN_MessageRcv[PGN].dataLen    = PGNInfoRcv[PGN].dataLen;
        PGN_MessageRcv[PGN].valid      = 0;
        PGN_MessageRcv[PGN].data       = (char*)PGNDataBUF_start;


  //      if(PGN != BMV && PGN != BMT && PGN != BSP)                //除了这三个PGN不给分配内存外，都给其他PGN分配数据部分的内存
        if(PGN != BMV && PGN != BSP)
            {
            PGNDataBUF_start  = PGNDataBUF_start+(PGNInfoRcv[PGN].dataLen + 1);

            if(PGNDataBUF_start > PGN_MessageRcvBuff_END)
            {
                //err_printf(ERROR_T(DEBUG_J1939), "PGN_MessageRcvBuff is small!\n");
                return -1;
            }
            *(PGNDataBUF_start-1) = 0xAA;                   //用0XAA 把每个PGN的数据隔开, 在每个PGN数据的尾部
        }
        else
        {
            PGN_MessageRcv[PGN].data = NULL;                   // 这三个PGN暂时不处理，也不分配数据内存，指针指向NULL
        }
        if(PGN_MessageRcv[PGN].data && *(PGN_MessageRcv[PGN].data-1) != 0xAA)  //检查内存界限是否正确
        {
            //err_printf(ERROR_T(DEBUG_J1939), "PGN %d data Memery init border Error!\n",PGN);
            return -1;
        }
    }
    return 0;
}

/**************************************************
功能: 清空PGN数组使其数据无效
参数: 无
返回: -1 为错误  0为正常
****************************************************/
char PGN_MessageRcv_clear(void)
{
    PGNTypeRcv PGN;
    for(PGN = BRM; PGN<TPCM_rcv; PGN++)
    {
        PGN_MessageRcv[PGN].valid = 0; // 使PGN数组使其数据无效

        if(PGN_MessageRcv[PGN].data && *(PGN_MessageRcv[PGN].data-1) != 0xAA)  //检查内存界限是否正确
        {
            //err_printf(ERROR_T(DEBUG_J1939), "PGN %d data Memery init border Error!\n",PGN);
            // PRINT(ERR, "PGN %d data Memery init border Error!\n",PGN);
            return -1;
        }
    }
    return 0;
}


/**************************************************
功能: 将PDU重新填装CanTxMsg,从CAN底层代码发送出去
参数: PDU指针
返回: 0成功 不为0则失败
****************************************************/
int SendOnePacket(void *arg, u32* id, u8 *dat, u8* len)
{
    J1939_message *msg = (J1939_message *)arg;
    if(msg->dataLen > 8)
        return -1;
    *len = msg->dataLen;
    *id = 0x1fffffff & (msg->priority<<26 |msg->PGNnum<<8 | msg->destAddr << 8| msg->sourceAddr); //29位
    *id = (*id)|0xf400;
    memcpy(dat, msg->data, msg->dataLen);
    return 0;                                     //发送成功
}

int J1939_SendOnePacket( J1939_message *msg)
{
    return CAN_Send(SendOnePacket, msg);
}

/**************************************************
功能: 从发送RingSNDbuff中读取sJ1939_buff_message数据转换成J1939_message, 发送出去
参数: 无
返回: 0成功 不为0则失败, 放在主循环中为任务
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
        //err_printf(ERROR_T(DEBUG_J1939), "not support this ringbuff!\n");
         return -1;
    }
    else if(Ringbuff_readstate == BUF_EMPTY) //读取的数据为空，算正常现象
    {
        return 0;
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

        J1939_message_send.destAddr     = BMS_Addr;
        J1939_message_send.PGNnum       = PGNInfoSend[message.PGN].PGNnum;

        if(J1939_message_send.PGNnum > 240)  //PGN大于240以上，则 PS则指定地址
        {
            J1939_message_send.PGNnum = (J1939_message_send.PGNnum & 0xffffff00)|J1939_message_send.destAddr;
        }

        J1939_message_send.sourceAddr   = Charger_Addr;   //源地址

        for(i=0; i<J1939_message_send.dataLen; i++)   //PDU 8个数据位
        {
            J1939_message_send.data[i] = message.Data[i];
        }
    }
    else                                                //不支持的PGN处理
    {
        //err_printf(ERROR_T(DEBUG_J1939), "NOT support PGN = %d!\n",message.PGN);
        return -1;
    }

        int temp =  J1939_SendOnePacket(&J1939_message_send);
     if(temp != 0) //打包发送
     {
        //err_printf(ERROR_T(ERROR_PACK_SENDFAIL), "J1939_SendOnePacket Error! %d\n",temp);
        return -1;
     }
    return 0;
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
    CanRxMsg* TxMessage = (CanRxMsg*)rcv; /* 从CAN中断调用该函数rcv是接收到的CAN数据 */

#ifdef LOG_CAN_RCV
    printf("rcv: ID %x ,DATA:",    TxMessage->ExtId );
    for(u8 i =0; i< TxMessage->DLC; i++)
        printf(" %x ",    TxMessage->Data[i] );
    printf("\r\n" );
#endif
    /**
    +------------+-------+--------+-----------------+------------------+-----------------+
    |     P      |  R    |   DP   |    PF           |      PS          |      SA         |
    +------------+-------+--------+-----------------+------------------+-----------------+
    |  优先权    | 保留位 | 数据页  |    PDU格式      |      目标地址     |   源地址        |
    +------------+-------+--------+-----------------+------------------+-----------------+
    |  3bit      |  1bit | 1bit   |    8bit         |     8bit         |    8bit         | 
    +------------+-------+--------+-----------------+------------------+-----------------+
     */
    if((TxMessage->ExtId & 0xffff)!= 0x56f4) return 0; /* 判断PS+SA目标地址和源地址是否为 充电机0x56 BMS0xF4 */

    sJ1939_buff_message_temp.PGN = -1;

    msg_temp.PGNnum     = (TxMessage->ExtId>>8) & 0x3ff00;   //PGN值 /* PGN = R, DP, PF, PS */
    msg_temp.sourceAddr =  TxMessage->ExtId & 0xff;          //源地址
    
    /****************************************************/ 
    msg_temp.priority = (TxMessage->ExtId>>26) & 0x07;       //优先级
    msg_temp.Reserved = (TxMessage->ExtId >>24 )&0x03;       //预留
    
    /****************************************************/
    

    //sJ1939_buff_message_temp.Prio=(TxMessage->ExtId>>24)& 0x1c; 
    sJ1939_buff_message_temp.PGNnum = msg_temp.PGNnum;
    sJ1939_buff_message_temp.PGN    = -1;  //只提出真实的PGN值，把确认的PGN索引值的工作交给主循环做
    
   /****************************************************/
    
    sJ1939_buff_message_temp.Priority = msg_temp.priority;
    sJ1939_buff_message_temp.Reserved = msg_temp.Reserved;
    
   /****************************************************/

    for(j=0; j<8; j++)
    {
        sJ1939_buff_message_temp.Data[j] = TxMessage->Data[j]; //拷贝数据
    }

    if(Ringbuff_write(RingRCVbuff, &sJ1939_buff_message_temp) != BUF_WRSUCC) //写入队列中
    {
        //printf("Interrupt Ringbuff_write Error!\n ");
        return -1;                                //碰到不支持的PGN跳出中断
    }
        return 0;
}

/***************************************************************************
功能: 从接收BUF中读取数据，再根据判断把PDU信息按分类放进PGN数组中
参数: 无
返回值: -1 错误 0正确
*****************************************************************************/
int GtmgFrRcvbufToPGN(void)
{
    sJ1939_buff_message message;
    u8 i;
    pJ1939_message_Rcv  pJ1939_message_Rcv_temp;

    char* pJ1939mg_data = message.Data;
    PGNTypeRcv PGNrcv_INDEX;

    int Ringbuff_readstate = Ringbuff_read(RingRCVbuff, &message); //从接收BUF中读出数据到message

    if(Ringbuff_readstate == BUF_NEXIT)//不存在返回-1
    {
         //printf("convert_BUFmsgToJ1939msgArray-------Ringbuff_read Fail!\n");
         return -1;
    }
    else if(Ringbuff_readstate == BUF_EMPTY) //读取的数据为空，算正常现象
    {
        return 0;
    }
    /******************根据PGN的真实值找索引PGN*******************/
    for(PGNrcv_INDEX=BRM; PGNrcv_INDEX <= PGN_MAX_Rcv; PGNrcv_INDEX++)  //查询对应的PGN 在数组中的索引
    //注意这个等号是必要的，跳到最后一个说明在接收PGN数组里面没有遍历到索引
    {
       // if(PGNrcv_INDEX == BMT)
       //     continue;
        if(PGNrcv_INDEX >= PGN_MAX_Rcv)              //碰到不支持的PGN直接跳出
        {
            //err_printf( ERROR_T(DEBUG_J1939), "this recive PGNNUM %x is not support!\n", message.PGNnum);
            return -1;                             
        }

        if(message.PGNnum == PGNInfoRcv[PGNrcv_INDEX].PGNnum)
        {
            if(PGNInfoRcv[PGNrcv_INDEX].dataLen > 8)
            {
                //err_printf( ERROR_T(DEBUG_J1939), "this recive PGNNUM %d data length is longer than 8!\n", message.PGNnum);
                return -1;
            }

            message.PGN = PGNrcv_INDEX; //找到PGN的索引
            if((message.PGN == BMV) || (message.PGN == BSP) || (message.PGN == BMT))
                return -1;
            
            
             /****************************************************/
             if(message.Priority != PGNInfoRcv[PGNrcv_INDEX].priority)
             {
                //  err_printf(ERROR_T(DEBUG_J1939), "this recive PGNNUM %d priority is error", message.PGNnum);
                 return -1;
             }
           
            if(message.Reserved != 0x00)
            {
                //  err_printf(ERROR_T(DEBUG_J1939), "this PGNNUM %d reserved data is wrong!\n", message.PGNnum);
                 return -1;
            }
             /****************************************************/
            
            break;
        }
    }

    pJ1939_message_Rcv_temp = &(PGN_MessageRcv[message.PGN]);
    /*************************************************单包传输******************************************************/
    if(message.PGN >= BRM && message.PGN < TPCM_rcv)
    {
        if(pJ1939_message_Rcv_temp->dataLen > 8)
        {
            //err_printf(ERROR_T(DEBUG_J1939), "this recive PGNNUM %d data length is longer than 8!\n", message.PGNnum);
            // PRINT(ERR, "this recive PGNNUM %d data length is longer than 8!\n", message.PGNnum);
            return -1;
        }
        pJ1939_message_Rcv_temp->valid = 0;           //写之前置数据无效
        for(i=0; i<pJ1939_message_Rcv_temp->dataLen; i++)
        {
            pJ1939_message_Rcv_temp->data[i] = message.Data[i];
        }
        if(pJ1939_message_Rcv_temp->data[i] == 0XAA && *(char*)(pJ1939_message_Rcv_temp->data-1) == 0XAA )  //核对分界线，分界线正常
        {
            pJ1939_message_Rcv_temp->valid = 1;       //写完成将数据置有效
            return 0;
        }
        else
        {
            //err_printf( ERROR_T(DEBUG_J1939), "this PGN %d date Border Error !\n", message.PGNnum);
            // PRINT(ERR, "this PGN %d date Border Error !\n", message.PGNnum);
            pJ1939_message_Rcv_temp->valid = 0;       //分界线错误
            return -1;
        }
    }
    /*************************************************多连接管理****************************************************/
    if(message.PGN == TPCM_rcv)
    {
      //  char* pJ1939mg_data = message.Data;
      //  sPGNInfo PGNrcv_INDEX;
        if(pJ1939mg_data[0] == TP_CM_RTS)                   //请求发送,如果接收到的是RTS那么要发送CTS
        {
            //if(J1939_connect.connectState == Idle)   //在连接管理 空闲时才能正常连接，不允许两个以上的多连接同时进行
            {
                /**
            +-------------+-------------+--------------+------------------+-----------------------+
            |   控制字节16 | 字节数目    | 全部数据包数目 | 0xFF保留SAE使用  |  装载数据的参数群编号   |
            +-------------+-------------+--------------+------------------+-----------------------+
            |   1byte     |  2,3byte    |    4byte     |    5byte         |      6,7,8byte        |
            +-------------+-------------+--------------+------------------+-----------------------+
                 */
                J1939_connect.data_num         = (pJ1939mg_data[2]<<8) | pJ1939mg_data[1]; //数据长度
                J1939_connect.num_packet   =  pJ1939mg_data[3];                        //包数
                J1939_connect.PGNnum       = (pJ1939mg_data[7]<<16) | (pJ1939mg_data[6]<<8) | pJ1939mg_data[5];
                J1939_connect.cur_packet       = 0;     //开始第几个数据包,从1开始, 0为还未做好传输数据准备

                //在PGN表项里面查询当前多连接传输的PGN 是否支持
                for(PGNrcv_INDEX = BRM; PGNrcv_INDEX <= TPCM_rcv; PGNrcv_INDEX++)
                {
                    if(PGNrcv_INDEX == TPCM_rcv)  //已经跳到最后一个，说明不支持这种种类的多包处理  ERROR_SEND_PGNmulti_NOSUPPORT
                    {
                        //err_printf(ERROR_T(DEBUG_J1939), "this recive PGNmulti %d is not support!\n", message.PGNnum);
                        // PRINT(ERR, "this recive PGNmulti %d is not support!\n", message.PGNnum);
                        //发送放弃连接
                        MultiTrans_Manage_SEND(&J1939_connect, TP_CM_Abort);  //发送放弃连接
                        J1939_connect_clear();                               //清空当前连接管理
                        return -1;
                    }
                    if(J1939_connect.PGNnum == PGNInfoRcv[PGNrcv_INDEX].PGNnum)  //核对PGN实际值
                    {
                        if( PGNInfoRcv[PGNrcv_INDEX].dataLen > 8 && J1939_connect.data_num == PGNInfoRcv[PGNrcv_INDEX].dataLen && J1939_connect.num_packet == (J1939_connect.data_num+6)/7)
                      
                        //if( PGNInfoRcv[PGNrcv_INDEX].dataLen > 8 && J1939_connect.num_packet == (J1939_connect.data_num+6)/7)
                            //核对长度，和长度满足多包传输条件, 核对字节数，核对包数
                        {
                            J1939_connect.PGNindex = PGNrcv_INDEX;  //多连接的PGN的枚举值
                            break;
                        }
                        else
                        {
                            MultiTrans_Manage_SEND(&J1939_connect, TP_CM_Abort);  //发送放弃连接
                            J1939_connect_clear();                                  //清空当前连接管理
                            return -1;
                        }
                    }
                }

                if(PGNrcv_INDEX == BMV || PGNrcv_INDEX == BMT || PGNrcv_INDEX == BSP) //忽略这三个PGN，非必要
                {
                    //printf("Ignore  The PGN  BMV BMT BSP!!\n ");
                    return 0;
                }

                J1939_connect.cur_packet++;
                if(MultiTrans_Manage_SEND(&J1939_connect, TP_CM_CTS) == 0)  //发送成功,说明虚拟连接已经建立
                {
                    J1939_connect.connectState     = connected;    //连接已经建立
                    J1939_connect.destAddr     = Charger_Addr; // 本次连接由BMS发起，发送给充电桩
                    J1939_connect.sourceAddr       = BMS_Addr;
                    time_record = 0;                            //计时开始  等待BMS数据响应
                }
                else                                            //如果发送不成功
                {
                    //J1939_connect.connectState = Idle;
                }

            }
            //else                                      //说明之前已经建立起多连接, 放弃当前连接请求
            //{

            //}
        }
        else if(pJ1939mg_data[0] == TP_CM_CTS)          //准备发送
        {}
        else if(pJ1939mg_data[0] == TP_CM_EndofMsgAck)  //消息结束应答
        {}
        else if(pJ1939mg_data[0] == TP_CM_Abort)        //接收到放弃连接
        {
            if(J1939_connect.connectState == Idle)  //未连接 却收到放弃，则错误
            {
                //err_printf(ERROR_T(DEBUG_J1939), "1939_connect is not connect But recive TP_CM_Abort PGN :%x,%x,%x", \
                        //pJ1939mg_data[5], pJ1939mg_data[6], pJ1939mg_data[7]);
                // PRINT(ERR, "1939_connect is not connect But recive TP_CM_Abort PGN :%x,%x,%x", \
                //         pJ1939mg_data[5], pJ1939mg_data[6], pJ1939mg_data[7]);
                return -1;
            }
            else if(J1939_connect.PGNnum == ((pJ1939mg_data[7]<<16) | (pJ1939mg_data[6]<<8) | pJ1939mg_data[5]))
            //核对对方放弃的连接管理的PGN号
            {
                J1939_connect_clear();        //
            }
            else
            {
                //err_printf(ERROR_T(DEBUG_J1939), "TP_CM_Abort but PGN is Error!\n");
                // PRINT(ERR, "TP_CM_Abort but PGN is Error!\n");
            }
        }
        else if(pJ1939mg_data[0] == TP_CM_BAM)          //广播公告消息
        {}
    }
    /*************************************************多连接数据传输************************************************/
    else if(message.PGN == TPDT_rcv)        //连接管理 下的数据传输
    {
        char n;
        if(J1939_connect.connectState != Idle)  //已经连接,且PGN核对正确
        {
            if(time_record > timeout_T2)   //如果超时则 放弃连接
            {
                //err_printf(ERROR_T(DEBUG_J1939), "timeout in TP_DT !\n", message.PGNnum);
                // PRINT(ERR, "timeout in TP_DT !\n", message.PGNnum);
                MultiTrans_Manage_SEND(&J1939_connect, TP_CM_Abort);  //发送放弃连接
                J1939_connect_clear();
            }
            else                          //未超时则 核对并提取数据
            {
                if(J1939_connect.cur_packet == pJ1939mg_data[0])// 核对当前包序号
                {
                    J1939_connect.data_position = (J1939_connect.cur_packet-1)*7; //接收BUF拷贝数据的位置,除最后一包都为7的倍数跳转

                    if(J1939_connect.cur_packet < J1939_connect.num_packet)  //不是最后一包, 直接拷贝
                    {
                        PGN_MessageRcv[J1939_connect.PGNindex].valid = 0; //写数据之前使PGN数据部分无效，防止未写完，应用层读
                        for(n=1; n<8; n++)   //拷贝数据到接收数据的BUF中, 7个数字
                        {
                            PGN_MessageRcv[J1939_connect.PGNindex].data[J1939_connect.data_position+n-1] = pJ1939mg_data[n];//剩下的7个数据拷贝到BUFF中
                        }

                        J1939_connect.cur_packet++;
                        MultiTrans_Manage_SEND(&J1939_connect, TP_CM_CTS);  //完成一包后再发送传输请求发送下一包
                        time_record = 0;                //计时开始
                    }

                    else if(J1939_connect.cur_packet == J1939_connect.num_packet) //接收到最后一包
                    {
                        PGN_MessageRcv[J1939_connect.PGNindex].valid = 0; //写数据之前使PGN数据部分无效，防止未写完，应用层读
                        for(n=1; n<(J1939_connect.data_num-J1939_connect.data_position+1); n++)   //拷贝数据到接收数据的BUF中, 可能不足7个数字
                        {
                            PGN_MessageRcv[J1939_connect.PGNindex].data[J1939_connect.data_position+n-1] = pJ1939mg_data[n];
                        }

                        J1939_connect.data_position = J1939_connect.data_position + n- 1;
                        if(J1939_connect.data_num == J1939_connect.data_position) //核对拷贝的数据总长度, 正确则发送应答
                        {
                            MultiTrans_Manage_SEND(&J1939_connect, TP_CM_EndofMsgAck);  //发送完成应答

                            PGN_MessageRcv[J1939_connect.PGNindex].valid = 1;//使能缓存数据有效
                            J1939_connect_clear();
                        }
                        else
                        {
                            PGN_MessageRcv[J1939_connect.PGNindex].valid = 0; //数据无效
                            J1939_connect.cur_packet = 1;                     //从第一包开始从新接收数据
                            MultiTrans_Manage_SEND(&J1939_connect, TP_CM_CTS);
                            J1939_connect.cur_packet++;
                        }
                    }
                    else
                    {
                        /*当前单包数据完成,但整个多包传输没有完成，再次发送CTS ，请求BMS发送数据*/
                        MultiTrans_Manage_SEND(&J1939_connect, TP_CM_EndofMsgAck);  //发送完成应答
                        //PGN_MessageRcv[J1939_connect.PGNindex].valid = 1;//使能缓存数据有效
                        J1939_connect_clear();             //计时开始
                    }

                }
                else  //包数核对不上，请求发送 当前记录的包编号数据
                {
                    //if(J1939_connect.cur_packet <= 1 || J1939_connect.cur_packet > J1939_connect.num_packet)//检查当前包的范围
                    //{
                    //  J1939_connect.cur_packet = 1; //如果当前包超界，那么当前包请求从1开始
                    //}
                    //else
                    //{
                        //J1939_connect.cur_packet;
                    //}

                    J1939_connect.cur_packet = 1;
                    //err_printf(ERROR_T(DEBUG_J1939), "the Package num is Error, rstart!\n");
                    // PRINT(ERR, "the Package num is Error, rstart!\n");
                    MultiTrans_Manage_SEND(&J1939_connect, TP_CM_CTS);

                    time_record = 0;                //计时开始
                }
            }
        }
        else  //还未连接却收到多包数据 ，则错误且获取不到多连接数据的PGN，故直接返回
        {

            //err_printf(ERROR_T(ERROR_NOCONT_BUT_TPDATE), "not connet but the TPDT!\n");
            J1939_connect_clear();
            return 0;
        }
    }
        return 0;
}

/***************************************************************************
功能: 连接管理发送命令
参数: 0 连接管理指针，1 命令
返回值:0 发送成功 -1 发送失败
*****************************************************************************/
char MultiTrans_Manage_SEND(psJ1939_transfeManger pJ1939_connect_arrys, unsigned char CMMD)
{
    sJ1939_buff_message msg_send;
    psJ1939_transfeManger pJ1939_connect = pJ1939_connect_arrys;

    msg_send.PGN = TPCM_send;
    
    
    

    if(pJ1939_connect == NULL ||(CMMD !=TP_CM_CTS && CMMD !=TP_CM_EndofMsgAck && CMMD !=TP_CM_Abort))
    {
        //err_printf( ERROR_T(DEBUG_J1939), "pJ1939_connect is NULL or  NO support CMMD!\n");
        // PRINT(ERR, "pJ1939_connect is NULL or  NO support CMMD!\n");
        return -1;
    }

    msg_send.Data[0] = CMMD;
    switch(CMMD)
    {
        case TP_CM_CTS:           //准备发送
        {
            /**
             * @brief:连接模式下准备发送
            +-----------------+--------------------+--------------------------+---------------------+-------------------------+
            |  控制字节17      |  可发送数据包数目   |  下一个将要发送数据包编号  |   保留SAE设定0xFF    |    装载数据参数群编号     |
            +-----------------+--------------------+--------------------------+---------------------+-------------------------+
            |  1byte          |    2byte           |       3byte              |     4 5byte         |       6 7 8byte         |
            +-----------------+--------------------+--------------------------+---------------------+-------------------------+
             */
            msg_send.Data[1]        = 1;            //可发送的数据包
            msg_send.Data[2]        = pJ1939_connect->cur_packet;       //接收第一包数据
#ifdef J1939_OneTime_Trans
            msg_send.Data[1]        = pJ1939_connect->num_packet;   //可发送的数据包
            if(pJ1939_connect->cur_packet > 1)
                return 0;
#endif
            msg_send.Data[3]        = 0XFF;         //规定
            msg_send.Data[4]        = 0XFF;
            break;
        }
       
        
        
        
        case TP_CM_EndofMsgAck:  //消息结束应答
        {
            /**
             * @brief: 消息结束应答
            +---------------------+-----------------+-------------------------+----------------------+---------------------+
            |  控制字节19          |  整个消息大小    |  全部数据包数目          |    保留给ASE使用0xFF  |      参数群编号      |
            +---------------------+-----------------+-------------------------+----------------------+---------------------+
            |     1byte           |   2 3byte       |     4byte               |      5byte           |       6 7 8byte     |
            +---------------------+-----------------+-------------------------+----------------------+---------------------+ 
             */
            msg_send.Data[1]        =  J1939_connect.data_num & 0xff;//整个数据长度
            msg_send.Data[2]        = (J1939_connect.data_num>>8) & 0xff;
            msg_send.Data[3]        =  J1939_connect.num_packet;  //包的长度
            msg_send.Data[4]        = 0XFF;  //规定
            break;
        }
        case TP_CM_Abort:          //放弃连接
        {  
          
           J1939_message msg_cst;
            msg_cst.sourceAddr  = 0x56;
            msg_cst.destAddr      = 0xf4;
            msg_cst.priority        = PGNInfoSend[TPCM_send].priority;
            msg_cst.PGNnum      = PGNInfoSend[TPCM_send].PGNnum;
            msg_cst.dataLen       = PGNInfoSend[TPCM_send].dataLen;
            /**
             * @brief: J1939放弃连接
            +-------------------+------------------+-----------------------+
            | 控制字节FF放弃连接 |  保留给SAE使用 FF |    装载数据参数群编号   +
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
            return 0;  //成功发送
        }
        default:
        {
            //printf("MultiTrans_Manage_SEND------------ CMMD is not support!\n ");
            return -1;  //错误
        }
    }

    msg_send.Data[5]    =  J1939_connect.PGNnum & 0xff;
    msg_send.Data[6]    = (J1939_connect.PGNnum & 0xff00)  >> 8;
    msg_send.Data[7]    = (J1939_connect.PGNnum & 0xff0000)>>16;

    if(Ringbuff_write(RingSNDbuff, &msg_send) != BUF_WRSUCC) //写入队列中
    {
        //err_printf( ERROR_T(DEBUG_J1939), " Ringbuff_write Error!\n");
        // PRINT(ERR, "Ringbuff_write Error!\n");
        return -1;
    }

    return 0;  //成功发送
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

static error_t J1939_pro_module(system_state_t state)
{
    if (STATE_INITIALIZING == state)
    {
        CAN_Reg(GetmessageToRcvbuff, 0);
        J1939_connect_clear();
        if(PGN_MessageRcv_Init() != 0)
            return -1;
    }
    else if ((STATE_UP == state) || (STATE_RUN == state))
    {
        GtmgFrRcvbufToPGN();
        GetmsgconvertToSend();
    }
    else if (STATE_DESTROYING == state)
    {
        ;
    }

    return 0;
}


void J1939_register()
{
    module_arg_t module_arg;

    memset(&module_arg, 0, sizeof(module_arg));
    module_arg.cycle = J1939_CYCLE_TIME;
    module_arg.unit  = MODULE_MS;
    module_register("J1939 process frame", MODULE_J1939, FRAMEWORK_LEVEL1,
        J1939_pro_module, &module_arg);
}
