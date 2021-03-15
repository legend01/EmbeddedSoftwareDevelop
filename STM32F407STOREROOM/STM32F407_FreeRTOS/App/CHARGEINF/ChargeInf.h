/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-02 14:41:32
 * @LastEditTime: 2021-03-15 13:42:36
 * @LastEditors: HLLI8
 */
#ifndef __CHARGEINF_H__
#define __CHARGEINF_H__
#include "pub_data.h"

typedef enum{
    LeadAcidCell = 0x01, /* 铅酸电池 */
    NI_MHBattery, /* 镍氢电池 */
    LithiumIronPhosphateBattery, /* 磷酸铁锂电池 */
    LithiumFerromanganeseBattery, /* 锰酸锂电池 */
    LithiumCobaltAcidBattery, /* 钴酸锂电池 */
    TernaryMaterialBattery, /* 三元材料电池 */
    PolymerLithiumIonBatteries, /* 聚合物锂离子电池 */
    LithiumTitanateBattery, /* 钛酸锂电池 */
    AnatherBattery = 0xFF,
}BatteryTypeEnu;

typedef enum{
    BatteryLease, /* 租赁 */
    BatteryVehicleOwer, /* 车自由 */
}BatteryPropertyEnu;
typedef struct {
    u16 vehicle_maxallow_v;         /* 配置车辆最大允许电压 */   

    u32 BMSComProtocolVersion; /* BMS通信协议版本号 */
    u8 BatteryType; /* 电池类型 */
    u16 VehicleBatterySysRatedCapacity; /* 整车动力蓄电池系统额定容量 */
    u16 VehicleBatterySysRatedVoltage; /* 整车动力蓄电池系统额定电压 */
    u8 BatteryManufacturers[3]; /* 电池生产厂商名称 */
    u32 BatterySerialNum; /* 电池组序号 */
    u8 BatteryManufactureData[2]; /* 电池组生产日期 年月日 */
    u32 ChargeTime; /* 电池充电次数 */
    u8 BatteryPropertyIden; /* 电池组产权标识 */
    u8 VehicleVIN[16]; /* 车辆识别码 */
    u8 BMSSoftVersion[7]; /* BMS软件版本号 */

    u16 SingleBatMaxAllowVoltage; /* 单体动力蓄电池最高允许充电电压 */
    u16 MaxAllowChargeCurrent; /* 最高允许充电电流 */
    u16 BatNominalTotalEnergy; /* 动力蓄电池标称总能量 */
    u16 MaxAllowTotalChargeVol; /* 最高允许充电总电压 */
    u8 MaxAllowTempe; /* 最高允许温度 */
    u16 VehicleBatChargeState; /* 整车动力蓄电池荷电状态 */
    u16 CurrentVehicleBatVol; /* 整车动力蓄电池当前电池电压 */

    u16 NeedV; /* 电压需求 */
    u16 NeedI; /* 电流需求 */
    u8 ChargeMode; /* 充电模式 */

    u16 BCS_MeasureVol; /* 充电电压测量值 */
    u16 BCS_MeasureI; /* 充电电流测量值 */
    u16 BCS_SinMaxBatVol; /* 最高单体动力蓄电池电压 */
    u8 BCS_SinMaxBATVol_Num; /* 最高单体动力蓄电池电压组号 */
    u8 BCS_NowSOC; /* 当前荷电状态 */
    u16 BCS_LeftChargeTIM; /* 估算剩余充电时间 */

    u8 BSM_SinBatMaxVolNum; /* 最高单体动力蓄电池电压所在编号 */
    u8 BSM_BatMaxTemp; /* 最高动力蓄电池温度 */
    u8 BSM_BatMaxTempCheckNum; /* 最高温度检测点编号 */
    u8 BSM_BatMinTemp; /* 最低动力蓄电池温度 */
    u8 BSM_BatMinTempCheckNum; /* 最低动力蓄电池温度检测点编号 */
    u8 BSM_SinBatVolHighOrLow; /* 单体动力蓄电池电压过高/过低 */
    u8 BSM_VehicleBatSOCHighOrLow; /* 整车动力蓄电池荷电状态SOC过高/过低 */
    u8 BSM_VehicleBatChargeOvercurrent; /* 动力蓄电池充电过电流 */
    u8 BSM_BatTempTooHight; /* 动力蓄电池温度过高 */
    u8 BSM_BatInsulationStatus; /* 动力蓄电池绝缘状态 */
    u8 BSM_BatPackOutputConnectStatus; /* 动力蓄电池组输出连接器连接状态 */
    u8 BSM_VehicleChargePermission; /* 充电允许 */
}VEHICLEPARAMINF;

typedef struct{
    u8 BCS_SocGoal; /* 蓄电池目标值 */
    u8 BCS_ChargeTime; /* 充电时间设定值 */
    u16 BCS_StartTime; /* 起始充电时间 */
    u16 BCS_CurrentTime; /* 当前充电时间 */
    u16 BCS_DurTime; /* 累计充电时间 分钟 */
}VEHICLECHARGECALCULATION; 

typedef struct 
{
    u8 chargeComVersion_L: 8; /* 充电机通信协议版本号 */
    u8 chargeComVersion_M: 8;
    u8 chargeComVersion_H: 8;
}RCV_CHM;

typedef struct{
    u8 vehicleAllowMaxV_L: 8; /* 车辆最高允许总电压 */
    u8 vehicleAllowMaxV_H: 8;
}SEND_BHM;

typedef struct{
    u8 identifyResult: 8; /* 辨识结果 */

    u8 chargeNumber_L: 8; /* 充电机编号 */
    u8 chargeNumber_M: 8;
    u8 chargeNumber_M1: 8;
    u8 chargeNumber_H: 8;

    u8 chargeRegin_L: 8; /* 充电机区域 */
    u8 chargeRegin_M: 8;
    u8 chargeRegin_H: 8;
}RCV_CRM;

typedef enum {
    ErrorType_Normal,
    ErrorType_Timeout,
    ErrorType_Unbelive,
}CHARGE_ERRORTYPE;

typedef struct{
    u8 CRM0x00ErrorState:2;
    u8 CRM0xaaErrorState:2;
    u8 reserved1:4;

    u8 CTSErrorState:2;
    u8 CROErrorState:2;
    u8 reserved2:4;
    
    u8 CCSErrorState:2;
    u8 CSTErrorState:2;
    u8 reserved3:4;

    u8 CSDErrorState:2;
    u8 reserved4:6;
}SEND_BEM;

typedef struct{
    u8 BMSComProtocolVersion_L:8; /* BMS通信协议版本号 */
    u8 BMSComProtocolVersion_M:8;
    u8 BMSComProtocolVersion_H:8;

    u8 BatteryType:8; /* 电池类型 */

    u8 VehicleBatterySysRatedCapacity_L:8; /* 整车动力蓄电池系统额定容量 */
    u8 VehicleBatterySysRatedCapacity_H:8;

    u8 VehicleBatterySysRatedVoltage_L:8; /* 整车动力蓄电池系统额定电压 */
    u8 VehicleBatterySysRatedVoltage_H:8;

    u8 BatteryManufacturers[3]; /* 电池生产厂商名称 */

    u8 BatterySerialNum_L:8; /* 电池组序号 */
    u8 BatterySerialNum_M1:8;
    u8 BatterySerialNum_M2:8;
    u8 BatterySerialNum_H:8;

    u8 BatteryManufactureData_Year:8; /* 电池生产日期 年 */
    u8 BatteryManufactureData_Month:8;
    u8 BatteryManufactureData_Day:8;

    u8 ChargeTime_L:8; /* 电池组充电次数 */
    u8 ChargeTime_M:8;
    u8 ChargeTime_H:8;

    u8 BatteryPropertyIden:8; /* 电池组产权标识 */

    u8 reserved:8;

    u8 VehicleVIN[16]; /* 车辆识别码 */

    u8 BMSSoftVersion[8]; /* BMS软件版本号 */
}SEND_BRM;

typedef struct{
    u8 SingleBatMaxAllowVoltage_L:8; /* 单体动力蓄电池最高允许充电电压 */
    u8 SingleBatMaxAllowVoltage_H:8;

    u8 MaxAllowChargeCurrent_L:8; /* 最高允许充电电流 */
    u8 MaxAllowChargeCurrent_H:8; 

    u8 BatNominalTotalEnergy_L:8; /* 动力蓄电池标称总能量 */
    u8 BatNominalTotalEnergy_H:8;

    u8 MaxAllowTotalChargeVol_L:8; /* 最高允许充电总电压 */
    u8 MaxAllowTotalChargeVol_H:8;

    u8 MaxAllowTempe:8; /* 最高允许温度 */

    u8 VehicleBatChargeState_L:8; /* 整车动力蓄电池荷电状态 */
    u8 VehicleBatChargeState_H:8;

    u8 CurrentVehicleBatVol_L:8; /* 整车动力蓄电池当前电池电压 */
    u8 CurrentVehicleBatVol_H:8;
}SEND_BCP;

typedef struct{
    u8 TimeSyncSecond:8; /* 秒 压缩BCD */
    u8 TimeSyncMinute:8; /* 分 */
    u8 TimeSyncHour:8; /* 小时 */
    u8 TimeSyncDay:8; /* 日 */
    u8 TimeSyncMonth:8; /* 月 */
    u8 TimeSyncYear_L:8; /* 年 */
    u8 TimeSyncYear_H:8; 
}RECV_CTS;

typedef struct{
    u8 MaxOutputVol_L:8; /* 最高输出电压 */
    u8 MaxOutputVol_H:8; 

    u8 MinOutputVol_L:8; /* 最低输出电压 */
    u8 MinOutputVol_H:8;

    u8 MaxOutputI_L:8; /* 最高输出电流 */
    u8 MaxOutputI_H:8;

    u8 MinOutputI_L:8; /* 最低输出电流 */
    u8 MinOutputI_H:8;
}RECV_CML;

typedef struct{
    u8 VehicleBMSReadyOrNot:8; /* BMS是否准备好 */
}SEND_BRO;

typedef struct{
    u8 ChargeReadyOrNot:8; /* 充电桩是否准备好 */ 
}RECV_CRO;

typedef struct{
    u8 NeedV_L:8; /* 电压需求 */
    u8 NeedV_H:8; 

    u8 NeedI_L:8; /* 电流需求 */
    u8 NeedI_H:8; 

    u8 ChargeMode:8; /* 充电模式 */
}SEND_BCL;

typedef struct{
    u8 MeasureChargeVol_L:8; /* 充电电压测量值 */
    u8 MeasureChargeVol_H:8; 

    u8 MeasureChargeI_L:8; /* 充电电流测量值 */
    u8 MeasureChargeI_H:8;

    u8 MaxSinBatVol_L:8; /* 最高单体动力蓄电池电压 */
    u8 MaxSinBatVol_H:4; 
    u8 MaxSinBatVolNum:4; /* 最高单体动力蓄电池组号 */

    u8 NowSOC:8; /* 当前荷电状态 */

    u8 LeftChargeTime_L:8; /* 估算剩余充电时间 */
    u8 LeftChargeTime_H:8; 
}SEND_BCS;

typedef struct{
    u8 CH_VolOutput_L:8; /* 电压输出 */
    u8 CH_VolOutput_H:8; 

    u8 CH_IOutput_L:8; /* 电流输出 */
    u8 CH_IOutput_H:8;

    u8 CH_ChargeTime_L:8; /* 累计充电时间 */
    u8 CH_ChargeTime_H:8;

    u8 CH_ChargePermision:2; /* 充电允许 */
    u8 reserved:6;
}RECV_CCS;

typedef struct{
    u8 SinBatMaxVolNum:8; /* 最高单体动力蓄电池电压所在编号 */
    u8 BatMaxTemp:8; /* 最高动力蓄电池温度 */
    u8 BatMaxTempCheckNum:8; /* 最高温度监测点编号 */
    u8 BatMinTemp:8; /* 最低动力蓄电池温度 */
    u8 BatMinTempCheckNum:8; /* 最低动力蓄电池温度检测点编号 */

    u8 SinBatVolHighOrLow:2; /* 单体动力蓄电池电压过高/过低 */
    u8 VehicleBatSOCHightOrLow:2; /* 整车动力蓄电池荷电状态SOC过高/过低 */
    u8 VehicleBatChargeOvercurrent:2; /* 动力蓄电池充电过电流 */
    u8 BatTempTooHight:2; /* 动力蓄电池温度过高 */
    
    u8 BatInsulationStatus:2; /* 动力蓄电池绝缘状态 */
    u8 BatPackOutputConnectStatus:2; /* 动力蓄电池组输出连接器连接状态 */
    u8 VehicleChargePermission:2; /* 充电允许 */
    u8 reserved:2;
}SEND_BSM;

void ConfigureVehicleParam(void);
VEHICLEPARAMINF* Get_Vehicle_ParamInf(void);
VEHICLECHARGECALCULATION* Get_ChargeCalculation_ParamInf(void);
VEHICLECHARGECALCULATION* Set_ChargeCalculation_ParamInf(void);
#endif // !__CHARGEINF_H__
