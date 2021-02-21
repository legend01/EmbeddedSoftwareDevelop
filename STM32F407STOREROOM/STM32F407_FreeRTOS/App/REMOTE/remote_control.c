/*
 * @Description: 
 * @Author: HLLI8
 * @Date: 2021-02-21 19:16:05
 * @LastEditTime: 2021-02-21 19:32:02
 * @LastEditors: HLLI8
 */
#include "remote_control.h"
#include "remote.h"
#include "log_printf.h"
#include "beep.h"
#include "pwm.h"

void Remote_control(void){
    uint8_t sta = 0;
    uint8_t *str = 0;

    sta = Remote_Scan();
    switch(sta)
    {
        case 0:str="ERROR";break;			   
        case 162:str="POWER";break;	    
        case 98:str="UP";break;	    
        case 2:str="PLAY";break;		 
        case 226:str="ALIENTEK";break;		  
        case 194:str="RIGHT";break;	   
        case 34:str="LEFT";break;		  
        case 224:str="VOL-";break;		  
        case 168:str="DOWN";break;		   
        case 144:str="VOL+";break;		    
        case 104:str="1";BEEP_1;break;		  
        case 152:str="2";BEEP_0;break;	   
        case 176:str="3";SetLight0Pwm();break;	    
        case 48:str="4";break;		    
        case 24:str="5";break;		    
        case 122:str="6";break;		  
        case 16:str="7";break;			   					
        case 56:str="8";break;	 
        case 90:str="9";break;
        case 66:str="0";break;
        case 82:str="DELETE";break;		 
    }
    if(sta != 0){
        LOG_PRINTF("REMOTE Light Recognize: %s \r\n", str);
    }
}