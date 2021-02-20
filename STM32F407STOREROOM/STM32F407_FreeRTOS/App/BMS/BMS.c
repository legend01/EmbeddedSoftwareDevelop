/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-02-20 08:59:56
 * @LastEditTime: 2021-02-20 11:28:05
 * @LastEditors: HLLI8
 */
#include "BMS.h"

void BMS_Send_message(PGNTypeSend ePGNTypeSend, char *data, int dataLen){
    J1939_message msg_cst;
    msg_cst.sourceAddr  = 0x56;
    msg_cst.destAddr    = 0xf4;
    msg_cst.priority    = PGNInfoSend[ePGNTypeSend].priority;
    msg_cst.PGNnum      = PGNInfoSend[ePGNTypeSend].PGNnum;
    msg_cst.dataLen     = PGNInfoSend[ePGNTypeSend].dataLen;
    memset(msg_cst.data, 0, 8);
    for (int i = 0; i < dataLen; i++)
    {
        msg_cst.data[i] = data[i] & 0xff;
    }
    
    J1939_SendOnePacket(&msg_cst);
}

/***************************************************************************
功能: 通过PGN的索引号读取得到的实际值PGN的数据部分
参数: 1 PGN 索引    2 读取到的数据指针
返回: -1为错误 1 为数据无效 messageData_len 为数据正常更新并且被读取
*****************************************************************************/
int BMS_Get_message(PGNTypeRcv ePGNTypeRcv, char** messageData)
{
    uint8_t messageData_len = 0;

    if(ePGNTypeRcv >= TPCM_rcv)//判断PGN范围
    {
        return -1;
    }

    messageData_len = PGNInfoRcv[ePGNTypeRcv].dataLen;
    
    if(messageData_len > messageData_MAXnum)        //判断PGN的最大长度是否大于最大数据
    {
        return -1;
    }
    
    if(PGN_MessageRcv[ePGNTypeRcv].valid == 0) //所查看的PGN的数据的有效性,如果无效则直接返回0
    {
        return 0;
    }
    
    *messageData = PGN_MessageRcv[ePGNTypeRcv].data;  //直接赋值指针
    
    if(ePGNTypeRcv != BRM && ePGNTypeRcv != BCP)        //长型管理包 更新一次即可
        PGN_MessageRcv[ePGNTypeRcv].valid = 0; //数据读取成功后无效
    else
        PGN_MessageRcv[ePGNTypeRcv].valid = 1; //数据读取成功后有效
    
    return PGNInfoRcv[ePGNTypeRcv].dataLen;
}