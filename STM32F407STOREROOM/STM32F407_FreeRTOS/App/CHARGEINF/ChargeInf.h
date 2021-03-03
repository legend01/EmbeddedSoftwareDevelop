/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-02 14:41:32
 * @LastEditTime: 2021-03-03 13:56:43
 * @LastEditors: HLLI8
 */
#ifndef __CHARGEINF_H__
#define __CHARGEINF_H__
#include "pub_data.h"

typedef enum
{
    CHAG_CV = 0x01,     //恒压
    CHAG_CC,            //横流
}CHAG_MOD;

typedef struct
{
    u16 ChargeTimeTotal; //累计充电时间  
    u16 ChargePout;      //输出能量 
    u32 ChargeNum;       //充电机编号
}CHAG_COU;

typedef struct
{
    u32      BMS_Version;  //车辆BMS版本
    u16      Bat_Type;   //电池类型
    u16      Bat_TolRCap;     //整车电池额定总容量0.1AH/位
    u16      Bat_TolV;        //整车电池额定总电压0.1V/位
    u8       Bat_Id[8];       //电池ID号（有电池生产厂商与厂商编码组成）
    u8       Bat_Birth[3];    //电池生产时间
    u32      Bat_CNum;        //电池充电次数
    u8       Bat_Mod;         //0:租赁，1:自用
    u8       Car_VIN[17];     //车辆识别码
    u16      Bat_SHV;         //单体电池最高允许充电电压0.01v
    u16      Bat_SHC;         //最高允许充电电流
    u16      Bat_TolNCap;     //总电池标称总能量
    u16      Bat_THV;         //最高允许充电总电压
    s16      Bat_THT;         //最高允许温度
    u16      SOC_INIT;         // 初始SOC
    u8       Bat_CHS;         //整车电池当前荷电状态,分辨率1%
    u16      Bat_NowV;        //整车电池当前总电压
    u16      Bat_CHV;         //电池充电电压
    u16      Bat_CHI;         //电池充电电流
    u16      Bat_StartTime;    //起始充电时间
    u16      Bat_CurrentTime;  //当前充电时间
    u16      Dur_Time;        //累计充电时间 分钟
    u32      Dur_Time_s;      //累计充电时间 秒
    u16      Sur_Time;        //剩余充电时间
    u16      Bat_ELV;         //蓄电池最低电压
    u16      Bat_EHV;         //蓄电池最高电压
    s16      Bat_EHT;         //蓄电池最高温度
    u8       Bat_EHTN;        //蓄电池最高温度编号
    s16      Bat_ELT;         //蓄电池最低温度  
    u8       Bat_ELTN;        //蓄电池最低温度编号
    CHAG_MOD Bat_CMOD;   //蓄电池要求充电机充电模式
    CHAG_COU Bat_COU;    //接收到的统计信息
    u8       Bat_SHVN;      //最高单体动力蓄电池所在编号
    u8       Bat_SHVGN;     //最高单体动力蓄电池所在组号
    u16      Bat_status;     //充电过程中电池的错误状态  
    u16      Bat_SH_V;       //最高单体电池电压0.01V
    u16      Bat_SL_V;       //最低单体电池电压0.01V
    u8       StopWork;          //电池要求暂停充电
    u8       BATstate_sV;       //bms 单体电压状态
    u8       BATstate_SOC;      //bms 的SOC状态
    u8       BATstate_I;        //bms 电流状态
    u8       BATstate_T;        //bms 温度状态
    u8       BATstate_Link;     //bms 连接器状态
    u8       BATstate_MID;      //bms 绝缘状态
    u16      Bat_TargSOC;     //soc目标值
    u16	     Bat_SELV;         //蓄电池单体最低电压
    u16	     Bat_SEHV;         //蓄电池单体最高电压
    u16      Set_CTime;       //设定的总充电时间
    u16      Bat_sinBatV;     //单体电池最高电压值
    u16      Bat_sinBatVn;    //单体电池最高电压值所在组号
}BAT_INFOR;

typedef struct 
{
    u8 chargeComVersion_L: 8;
    u8 chargeComVersion_M: 8;
    u8 chargeComVersion_H: 8;
}RCV_CHM;

typedef struct
{
    u32 	charg_CommVersion; //充电机通信协议版本号
}CHG_INFOR;
#endif // !__CHARGEINF_H__
