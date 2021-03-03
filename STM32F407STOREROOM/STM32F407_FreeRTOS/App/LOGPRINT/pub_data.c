/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-03-03 14:45:43
 * @LastEditTime: 2021-03-03 14:45:52
 * @LastEditors: HLLI8
 */
#include "pub_data.h"

u32 GetTimeMs(void){
	return uwTick;
}

u32 TimeAfterMs(u32 StapTimeMs){
	u32 slot;
	if(StapTimeMs <= uwTick){
		slot = uwTick - StapTimeMs;
	}else{
		slot = ((u32)COUNT_MS_CYCLE - StapTimeMs) + uwTick;
	}
	return slot;
}