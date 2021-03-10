/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-02 14:41:32
 * @LastEditTime: 2021-03-10 09:42:48
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
    u8 MaxAllowTempe:8; /* 最高允许温度 */
    u16 VehicleBatChargeState:8; /* 整车动力蓄电池荷电状态 */
    u16 CurrentVehicleBatVol:8; /* 整车动力蓄电池当前电池电压 */
}VEHICLEPARAMINF;

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
    u8 TimeSyncYear:8; /* 年 */
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
void ConfigureVehicleParam(void);
VEHICLEPARAMINF* Get_Vehicle_ParamInf(void);
#endif // !__CHARGEINF_H__
