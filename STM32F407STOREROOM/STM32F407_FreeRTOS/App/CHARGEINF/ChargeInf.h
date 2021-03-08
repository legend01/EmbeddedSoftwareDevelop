/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-02 14:41:32
 * @LastEditTime: 2021-03-08 11:02:22
 * @LastEditors: HLLI8
 */
#ifndef __CHARGEINF_H__
#define __CHARGEINF_H__
#include "pub_data.h"

typedef struct {
    u16 vehicle_maxallow_v;         /* 配置车辆最大允许电压 */   
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

void ConfigureVehicleParam(void);
VEHICLEPARAMINF* Get_Vehicle_ParamInf(void);
#endif // !__CHARGEINF_H__
