/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-02 14:41:32
 * @LastEditTime: 2021-03-09 15:12:05
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

    u8 BatteryManufacturers_L:8; /* 电池生产厂商名称 */
    u8 BatteryManufacturers_M1:8;
    u8 BatteryManufacturers_M2:8;
    u8 BatteryManufacturers_H:8;

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

void ConfigureVehicleParam(void);
VEHICLEPARAMINF* Get_Vehicle_ParamInf(void);
#endif // !__CHARGEINF_H__
