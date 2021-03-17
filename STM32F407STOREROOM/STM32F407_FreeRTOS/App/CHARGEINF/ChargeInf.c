/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-02 14:41:20
 * @LastEditTime: 2021-03-16 14:36:45
 * @LastEditors: HLLI8
 */
#include "ChargeInf.h"

#define vehiclemaxallowv 5000

#define BMSProtocalVersion 0x000101
#define VehicleBAT_CAP 4000 /* 0.1AH/位 */
#define VehicleBAT_EV 0 /* 0.1V/位 */
#define BSerialNum 0x12345678
#define BManufactureData_Year 0x24
#define BManufactureData_Month 0x02
#define BManufactureData_Day 0x10
#define BChargeTime 0x123456
#define BMSSoftV_SerialNum 0x10 
#define BMSSoftV_Day 0x01
#define BMSSoftV_Month 0x02
#define BMSSoftV_Year_L 0x07
#define BMSSoftV_Year_H 0xDF
#define RESERVED 0xFF

#define SingleBATMaxAllowV 360
#define MaxAllowCurrent 4000
#define BatNominalTotalEn 1000
#define MaxAllowChargeVol 5000
#define MaxTempe 80
#define VehicleSOC 30
#define VehicleCurrentBatVol 4500

#define BAT_NeedV 4500
#define BAT_NeedI 2000
#define BAT_ChargeMode 1 /* 1：恒压 2：恒流 */

#define BCS_CheckV 3500
#define BCS_CheckI 2965
#define BCS_SinBATMaxVol 330
#define BCS_SinBATMaxVol_SerialNum 1
#define BCS_CurSOC 30
#define BCS_TMleft 0

#define BSM_SinMaxVolNum 1
#define BSM_MaxTemp 78
#define BSM_MaxTempCheckNum 1
#define BSM_MinTemp 74
#define BSM_MinTempNum 2
#define BSM_SinVolStatus 0x00
#define BSM_SOCStatus 0x00
#define BSM_ChargeOverI 0x00
#define BSM_TempHighStatus 0x00
#define BSM_InsulationStatus 0x00
#define BSM_BatPackLinkerStatus 0x00
#define BSM_ChargePermission 0x01

#define BCS_soc_goal 100
#define BCS_charge_time 60 
#define BCS_start_time 0xFFFF

#define BSD_sinBatMinVol 333
#define BSD_sinBatMaxVol 355
#define BSD_batMinTemp 55
#define BSD_batMaxTemp 60

VEHICLEPARAMINF  VehicleParamInf;
VEHICLECHARGECALCULATION ChargeCalculationParamInf;

static void ConfigChargeCalculationParam(void){
    VEHICLECHARGECALCULATION *Calcu_inf = &ChargeCalculationParamInf;
    
    Calcu_inf->BCS_SocGoal = BCS_soc_goal;
    Calcu_inf->BCS_ChargeTime = BCS_charge_time;
    Calcu_inf->BCS_StartTime = BCS_start_time;
}

VEHICLECHARGECALCULATION* Get_ChargeCalculation_ParamInf(void){
    return &ChargeCalculationParamInf;
}

VEHICLECHARGECALCULATION* Set_ChargeCalculation_ParamInf(void){
    return &ChargeCalculationParamInf;
}

void ConfigureVehicleParam(void){
    VEHICLEPARAMINF *vehicle_inf = &VehicleParamInf;

    vehicle_inf->vehicle_maxallow_v = vehiclemaxallowv;
    
    vehicle_inf->BMSComProtocolVersion = BMSProtocalVersion;
    vehicle_inf->BatteryType = LithiumIronPhosphateBattery;
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

    vehicle_inf->SingleBatMaxAllowVoltage = SingleBATMaxAllowV;
    vehicle_inf->MaxAllowChargeCurrent = MaxAllowCurrent;
    vehicle_inf->BatNominalTotalEnergy = BatNominalTotalEn;
    vehicle_inf->MaxAllowTotalChargeVol = MaxAllowChargeVol;
    vehicle_inf->MaxAllowTempe = MaxTempe;
    vehicle_inf->VehicleBatChargeState = VehicleSOC;
    vehicle_inf->CurrentVehicleBatVol = VehicleCurrentBatVol;

    vehicle_inf->NeedV = BAT_NeedV;
    vehicle_inf->NeedI = BAT_NeedI;
    vehicle_inf->ChargeMode = BAT_ChargeMode;

    vehicle_inf->BCS_MeasureVol = BCS_CheckV;
    vehicle_inf->BCS_MeasureI = BCS_CheckI;
    vehicle_inf->BCS_SinMaxBatVol = BCS_SinBATMaxVol;
    vehicle_inf->BCS_SinMaxBATVol_Num = BCS_SinBATMaxVol_SerialNum;
    vehicle_inf->BCS_NowSOC = BCS_CurSOC;
    vehicle_inf->BCS_LeftChargeTIM = BCS_TMleft;

    vehicle_inf->BSM_SinBatMaxVolNum = BSM_SinMaxVolNum;
    vehicle_inf->BSM_BatMaxTemp = BSM_MaxTemp;
    vehicle_inf->BSM_BatMaxTempCheckNum = BSM_MaxTempCheckNum;
    vehicle_inf->BSM_BatMinTemp = BSM_MinTemp;  
    vehicle_inf->BSM_BatMinTempCheckNum = BSM_MinTempNum;  
    vehicle_inf->BSM_SinBatVolHighOrLow = BSM_SinVolStatus; 
    vehicle_inf->BSM_VehicleBatSOCHighOrLow = BSM_SOCStatus; 
    vehicle_inf->BSM_VehicleBatChargeOvercurrent = BSM_ChargeOverI;
    vehicle_inf->BSM_BatTempTooHight = BSM_TempHighStatus;
    vehicle_inf->BSM_BatInsulationStatus = BSM_InsulationStatus;
    vehicle_inf->BSM_BatPackOutputConnectStatus = BSM_BatPackLinkerStatus;
    vehicle_inf->BSM_VehicleChargePermission = BSM_ChargePermission;

    vehicle_inf->BST_ReachSOCTargetAbort = ReachTargetValue;
    vehicle_inf->BST_ReachVolTargetAbort = ReachTargetValue;
    vehicle_inf->BST_ReachSinVolTargetAbort = ReachTargetValue;
    vehicle_inf->BST_ChargeActiveAbort = Normal;
    vehicle_inf->BST_InsulationFault = Normal;
    vehicle_inf->BST_OutputLinkerOverTempFault = Normal;
    vehicle_inf->BST_BMSComponentOverTempFault = Normal;
    vehicle_inf->BST_ChargeLinkerFault = Normal;
    vehicle_inf->BST_BatPackOverTempFault = Normal;
    vehicle_inf->BST_HighVolRelayFault = Normal;
    vehicle_inf->BST_CheckPoint2VolFault = Normal;
    vehicle_inf->BST_ElseFault = Normal;
    vehicle_inf->BST_IOverHighError = Normal;
    vehicle_inf->BST_VolAbnormalError = Normal;

    vehicle_inf->BSD_SinBatMinVol = BSD_sinBatMinVol;
    vehicle_inf->BSD_SinBatMaxVol = BSD_sinBatMaxVol;
    vehicle_inf->BSD_BatMinTemp = BSD_batMinTemp;
    vehicle_inf->BSD_BatMaxTemp = BSD_batMaxTemp;

    ConfigChargeCalculationParam();
}

VEHICLEPARAMINF* Get_Vehicle_ParamInf(void){
    return &VehicleParamInf;
}
