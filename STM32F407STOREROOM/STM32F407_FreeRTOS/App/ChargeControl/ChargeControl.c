#include "HvProcess_Chg.h"
#include "HvProcess_Types.h"
#include "ChargeControl.h"

extern const HvProcess_ProcessDataType HvProcess_ChgControlList[];
extern const u8 HvProcess_ChgControlListNum;

void ChargeControlTask(void const * argument)
{

	for(u8 i = 0; i< HvProcess_ChgControlListNum; i++)
	{
		HvProcess_ChgControlList[i].Init();
	}

	for(;;)
	{
		osDelay(10u);

    	for(u8 j = 0; j < HvProcess_ChgControlListNum; j++)
    	{
			HVProcess_Process(&HvProcess_ChgControlList[j]);
		}
	}

}


