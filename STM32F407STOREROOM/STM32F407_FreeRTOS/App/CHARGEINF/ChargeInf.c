/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-02 14:41:20
 * @LastEditTime: 2021-03-09 14:37:16
 * @LastEditors: HLLI8
 */
#include "ChargeInf.h"
#define vehiclemaxallowv 5000
#define BMSProtocalVersion 0x000101
#define VehicleBAT_CAP 4000 /* 0.1AH/位 */
#define VehicleBAT_EV 0 /* 0.1V/位 */
#define BSerialNum 0x12345678
#define BManufactureData_Year 0x01
#define BManufactureData_Month 0x02
#define BManufactureData_Day 0x03
#define BChargeTime 0x123456
#define BMSSoftV_SerialNum 0x10 
#define BMSSoftV_Day 0x01
#define BMSSoftV_Month 0x02
#define BMSSoftV_Year_L 0x07
#define BMSSoftV_Year_H 0xDF
#define RESERVED 0xFF
VEHICLEPARAMINF  VehicleParamInf;

void ConfigureVehicleParam(void){
    VEHICLEPARAMINF *vehicle_inf = &VehicleParamInf;

    vehicle_inf->vehicle_maxallow_v = vehiclemaxallowv;
    
    vehicle_inf->BMSComProtocolVersion = BMSProtocalVersion;
    vehicle_inf->BatteryType = LeadAcidCell;
    vehicle_inf->VehicleBatterySysRatedCapacity = VehicleBAT_CAP;
    vehicle_inf->VehicleBatterySysRatedVoltage = VehicleBAT_EV;
    vehicle_inf->BatteryManufacturers[0] = 'C';
    vehicle_inf->BatteryManufacturers[1] = 'S';
    vehicle_inf->BatteryManufacturers[2] = 'G';
    vehicle_inf->BatteryManufacturers[3] = ' ';
    vehicle_inf->BatterySerialNum = BSerialNum;
    vehicle_inf->BatteryManufactureData[0] = BManufactureData_Year;
    vehicle_inf->BatteryManufactureData[1] = BManufactureData_Month;
    vehicle_inf->BatteryManufactureData[2] = BManufactureData_Day;
    vehicle_inf->ChargeTime = BChargeTime;
    vehicle_inf->BatteryPropertyIden = BatteryLease;
    for (u8 i = 0; i < 17; i++)
    {
        vehicle_inf->VehicleVIN[i] = i;							
    }
    vehicle_inf->BMSSoftVersion[0] = BMSSoftV_SerialNum;
    vehicle_inf->BMSSoftVersion[1] = BMSSoftV_Day;
    vehicle_inf->BMSSoftVersion[2] = BMSSoftV_Month;
    vehicle_inf->BMSSoftVersion[3] = BMSSoftV_Year_L;
    vehicle_inf->BMSSoftVersion[4] = BMSSoftV_Year_H;
    vehicle_inf->BMSSoftVersion[5] = RESERVED;
    vehicle_inf->BMSSoftVersion[6] = RESERVED;
    vehicle_inf->BMSSoftVersion[7] = RESERVED;

    
}

VEHICLEPARAMINF* Get_Vehicle_ParamInf(void){
    return &VehicleParamInf;
}