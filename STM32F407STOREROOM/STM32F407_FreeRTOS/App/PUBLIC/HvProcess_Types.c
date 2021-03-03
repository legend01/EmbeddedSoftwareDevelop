#include "HvProcess_Types.h"


void HVProcess_Process(const HvProcess_ProcessDataType *Process)
{
	u16 i;
	u16 state;
	const HvProcess_StateConfigType *config;

	if((Process != NULL) &&
		(Process->Config != NULL) &&
		(Process->GetState != NULL) &&
		(Process->SetState != NULL) )
	{
		state = (u16)Process->GetState();
		if(state < Process ->StateNum)
		{
			Process-> Poll();
			config = &Process->Config[state];

			for(i = 0U; i < config->Num; i++)
			{
				bool flag = false;

				if(config->State[i].Cond == NULL)
				{
					flag = true;
				}
				else if (config->State[i].Cond() == true)
				{
					flag = true;
				}
				else
				{
				}

				if(flag)
				{
					if(config->State[i].Action)
					{
						config->State[i].Action();
					}
					Process->SetState(config->State[i].Next);
					break;
				}
			}
		}
		else
		{
			Process->SetState(0U);
		}
	}
}