/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-01 11:10:29
 * @LastEditTime: 2021-03-03 13:58:04
 * @LastEditors: HLLI8
 */
#include "BMS_BMS.h"
#include "ChargeInf.h"

sBMS_Manage BMSmanager;
BAT_INFOR BAT_inf;
RCV_CHM Rcv_CHM;

void BMSmanager_Init(void){
    Ringbuff_setEmpty();
    J1939_connect_send_clear();
    J1939_connect_clear();
    PGN_MessageRcv_Init();
    PGN_MessageRcv_clear();
    memset(&BAT_inf, 0, sizeof(BAT_inf));
    memset(&Rcv_CHM, 0, sizeof(Rcv_CHM));
}

void BMS_Send_message(PGNTypeSend ePGNTypeSend, char *data){
    if(data != NULL)
    {
        for (int i = 0; i < PGNInfoSend[ePGNTypeSend].dataLen; i++)
        {
            BMSmanager.msgSendData[i] = data[i] & 0xff;
        }
        PushMsgToRBUFF(ePGNTypeSend, BMSmanager.msgSendData);
    }
    
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
    
    PGN_MessageRcv[ePGNTypeRcv].valid = 0; //数据读取成功后无效
    
    return PGNInfoRcv[ePGNTypeRcv].dataLen;
}

bool BMS_Check_Valid(PGNTypeRcv ePGNTypeRcv){
    if (PGN_MessageRcv[ePGNTypeRcv].valid == 1)
    {
        return true;
    }else{
        return false;
    }
}