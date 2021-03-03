/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-02 14:41:20
 * @LastEditTime: 2021-03-03 15:22:23
 * @LastEditors: HLLI8
 */
#include "ChargeInf.h"
#define vehiclemaxallowv 5000

VEHICLEPARAMINF  VehicleParamInf;

void ConfigureVehicleParam(void){
    VEHICLEPARAMINF *vehicle_inf;

    vehicle_inf->vehicle_maxallow_v = vehiclemaxallowv;
}

VEHICLEPARAMINF* Get_Vehicle_ParamInf(void){
    return &VehicleParamInf;
}