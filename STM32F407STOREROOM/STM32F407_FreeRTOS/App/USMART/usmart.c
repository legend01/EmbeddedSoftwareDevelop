#include "usmart.h"
#include "usart.h"
#include "tim.h"
#include "usmart_receiveFromUsart.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

USMART_RECV_STR usmart_receivePOI;
extern osMessageQId USMARTQueueHandle;
//系统命令
uint8_t *sys_cmd_tab[]=
{
	"?",
	"help",
	"list",
	"id",
	"hex",
	"dec",
	"runtime",	   
};	    
//处理系统指令
//0,成功处理;其他,错误代码;
uint8_t usmart_sys_cmd_exe(uint8_t *str)
{
	uint8_t i;
	uint8_t sfname[MAX_FNAME_LEN];//存放本地函数名
	uint8_t pnum;
	uint8_t rval;
	uint32_t res;  
	res=usmart_get_cmdname(str,sfname,&i,MAX_FNAME_LEN);//得到指令及指令长度
	if(res)return USMART_FUNCERR;//错误的指令 
	str+=i;	 	 			    
	for(i=0;i<sizeof(sys_cmd_tab)/4;i++)//支持的系统指令
	{
		if(usmart_strcmp(sfname,sys_cmd_tab[i])==0)break;
	}
	switch(i)
	{					   
		case 0:
		case 1://帮助指令
			printf("\r\n");
#if USMART_USE_HELP 
			printf("USMART have 7 commands(must be lowercase):\r\n");
			printf("?:      get help info\r\n");
			printf("help:   get help info\r\n");
			printf("list:   avaliable function list\r\n\n");
			printf("id:     avaliable function id list\r\n");
			printf("hex:    The parameter is displayed in hexadecimal\r\n");
			printf("dec:    The parameter is displayed in decimal\r\n");
			printf("runtime:1,Turn on function running timing;0,Turn off function running timing;\r\n");
#else
			printf("指令失效\r\n");
#endif
			break;
		case 2://查询指令
			printf("\r\n");
			printf("-------------------------Function List--------------------------- \r\n");
			for(i=0;i<usmart_dev.fnum;i++)printf("%s\r\n",usmart_dev.funs[i].name);
			printf("\r\n");
			break;	 
		case 3://查询ID
			printf("\r\n");
			printf("-------------------------Function ID --------------------------- \r\n");
			for(i=0;i<usmart_dev.fnum;i++)
			{
				usmart_get_fname((uint8_t*)usmart_dev.funs[i].name,sfname,&pnum,&rval);//得到本地函数名 
				printf("%s id is:\r\n0X%08X\r\n",sfname,usmart_dev.funs[i].func); //显示ID
			}
			printf("\r\n");
			break;
		case 4://hex指令
			printf("\r\n");
			usmart_get_aparm(str,sfname,&i);
			if(i==0)//参数正常
			{
				i=usmart_str2num(sfname,&res);	   	//记录该参数	
				if(i==0)						  	//进制转换功能
				{
					printf("HEX:0X%X\r\n",res);	   	//转为16进制
				}else if(i!=4)return USMART_PARMERR;//参数错误.
				else 				   				//参数显示设定功能
				{
					printf("Hexadecimal parameter display!\r\n");
					usmart_dev.sptype=SP_TYPE_HEX;  
				}

			}else return USMART_PARMERR;			//参数错误.
			printf("\r\n"); 
			break;
		case 5://dec指令
			printf("\r\n");
			usmart_get_aparm(str,sfname,&i);
			if(i==0)//参数正常
			{
				i=usmart_str2num(sfname,&res);	   	//记录该参数	
				if(i==0)						   	//进制转换功能
				{
					printf("DEC:%lu\r\n",res);	   	//转为10进制
				}else if(i!=4)return USMART_PARMERR;//参数错误.
				else 				   				//参数显示设定功能
				{
					printf("Decimal parameter display!\r\n");
					usmart_dev.sptype=SP_TYPE_DEC;  
				}

			}else return USMART_PARMERR;			//参数错误. 
			printf("\r\n"); 
			break;	 
		case 6://runtime指令,设置是否显示函数执行时间
			printf("\r\n");
			usmart_get_aparm(str,sfname,&i);
			if(i==0)//参数正常
			{
				i=usmart_str2num(sfname,&res);	   		//记录该参数	
				if(i==0)						   		//读取指定地址数据功能
				{
					if(USMART_ENTIMX_SCAN==0)printf("\r\nError! \r\nTo EN RunTime function,Please set USMART_ENTIMX_SCAN = 1 first!\r\n");//报错
					else
					{
						usmart_dev.runtimeflag=res;
						if(usmart_dev.runtimeflag)printf("Run Time Calculation ON\r\n");
						else printf("Run Time Calculation OFF\r\n"); 
					}
				}else return USMART_PARMERR;   			//未带参数,或者参数错误	 
 			}else return USMART_PARMERR;				//参数错误. 
			printf("\r\n"); 
			break;	    
		default://非法指令
			return USMART_FUNCERR;
	}
	return 0;
}
	
//从str中获取函数名,id,及参数信息
//*str:字符串指针.
//返回值:0,识别成功;其他,错误代码.
uint8_t usmart_cmd_rec(uint8_t*str) 
{
	uint8_t sta,i,rval;//状态	 
	uint8_t rpnum,spnum;
	uint8_t rfname[MAX_FNAME_LEN];//暂存空间,用于存放接收到的函数名  
	uint8_t sfname[MAX_FNAME_LEN];//存放本地函数名
	sta=usmart_get_fname(str,rfname,&rpnum,&rval);//得到接收到的数据的函数名及参数个数	  
	if(sta)return sta;//错误
	for(i=0;i<usmart_dev.fnum;i++)
	{
		sta=usmart_get_fname((uint8_t*)usmart_dev.funs[i].name,sfname,&spnum,&rval);//得到本地函数名及参数个数
		if(sta)return sta;//本地解析有误	  
		if(usmart_strcmp(sfname,rfname)==0)//相等
		{
			if(spnum>rpnum)return USMART_PARMERR;//参数错误(输入参数比源函数参数少)
			usmart_dev.id=i;//记录函数ID.
			break;//跳出.
		}	
	}
	if(i==usmart_dev.fnum)return USMART_NOFUNCFIND;	//未找到匹配的函数
 	sta=usmart_get_fparam(str,&i);					//得到函数参数个数	
	if(sta)return sta;								//返回错误
	usmart_dev.pnum=i;								//参数个数记录
    return USMART_OK;
}
//usamrt执行函数
//该函数用于最终执行从串口收到的有效函数.
//最多支持10个参数的函数,更多的参数支持也很容易实现.不过用的很少.一般5个左右的参数的函数已经很少见了.
//该函数会在串口打印执行情况.以:"函数名(参数1，参数2...参数N)=返回值".的形式打印.
//当所执行的函数没有返回值的时候,所打印的返回值是一个无意义的数据.
void usmart_exe(void)
{
	uint8_t id,i;
	uint32_t res;		   
	uint32_t temp[MAX_PARM];//参数转换,使之支持了字符串 
	uint8_t sfname[MAX_FNAME_LEN];//存放本地函数名
	uint8_t pnum,rval;
	id=usmart_dev.id;
	if(id>=usmart_dev.fnum)return;//不执行.
	usmart_get_fname((uint8_t*)usmart_dev.funs[id].name,sfname,&pnum,&rval);//得到本地函数名,及参数个数 
	printf("\r\n%s(",sfname);//输出正要执行的函数名
	for(i=0;i<pnum;i++)//输出参数
	{
		if(usmart_dev.parmtype&(1<<i))//参数是字符串
		{
			printf("%c",'"');			 
			printf("%s",usmart_dev.parm+usmart_get_parmpos(i));
			printf("%c",'"');
			temp[i]=(uint32_t)&(usmart_dev.parm[usmart_get_parmpos(i)]);
		}else						  //参数是数字
		{
			temp[i]=*(uint32_t*)(usmart_dev.parm+usmart_get_parmpos(i));
			if(usmart_dev.sptype==SP_TYPE_DEC)printf("%ld",temp[i]);//10进制参数显示
			else printf("0X%X",temp[i]);//16进制参数显示 	   
		}
		if(i!=pnum-1)printf(",");
	}
	printf(")");
#if USMART_ENTIMX_SCAN==1
	usmart_reset_runtime();	//计时器清零,开始计时
#endif
	switch(usmart_dev.pnum)
	{
		case 0://无参数(void类型)											  
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)();
			break;
	    case 1://有1个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0]);
			break;
	    case 2://有2个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1]);
			break;
	    case 3://有3个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2]);
			break;
	    case 4://有4个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3]);
			break;
	    case 5://有5个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4]);
			break;
	    case 6://有6个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5]);
			break;
	    case 7://有7个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6]);
			break;
	    case 8://有8个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6],temp[7]);
			break;
	    case 9://有9个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6],temp[7],temp[8]);
			break;
	    case 10://有10个参数
			res=(*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6],temp[7],temp[8],temp[9]);
			break;
	}
#if USMART_ENTIMX_SCAN==1
	usmart_get_runtime();//获取函数执行时间
#endif
	if(rval==1)//需要返回值.
	{
		if(usmart_dev.sptype==SP_TYPE_DEC)printf("=%lu;\r\n",res);//输出执行结果(10进制参数显示)
		else printf("=0X%X;\r\n",res);//输出执行结果(16进制参数显示)	   
	}else printf(";\r\n");		//不需要返回值,直接输出结束
	if(usmart_dev.runtimeflag)	//需要显示函数执行时间
	{ 
		printf("Function Run Time:%d.%1dms\r\n",usmart_dev.runtime/10,usmart_dev.runtime%10);//打印函数执行时间 
	}	
}
//usmart扫描函数
//通过调用该函数,实现usmart的各个控制.该函数需要每隔一定时间被调用一次
//以及时执行从串口发过来的各个函数.
//本函数可以在中断里面调用,从而实现自动管理.
//如果非ALIENTEK用户,则USART_RX_STA和USART_RX_BUF[]需要用户自己实现
void usmart_scan(void)
{
	uint8_t sta,len;  
	BaseType_t err;
	BaseType_t xTaskWokenByReceive = pdFALSE;
	if(USMARTQueueHandle != NULL){
		err = xQueueReceiveFromISR(USMARTQueueHandle, &usmart_receivePOI, &xTaskWokenByReceive);

		if(err == pdTRUE && usmart_receivePOI.Usmart_Receive_Flag)//串口接收完成？
		{					   
			len=usmart_receivePOI.Usmart_RecvLens;	//得到此次接收到的数据长度
			usmart_receivePOI.Usmart_RecvBuff[len]='\0';	//在末尾加入结束符. 
			sta=usmart_dev.cmd_rec(usmart_receivePOI.Usmart_RecvBuff);//得到函数各个信息
			if(sta==0)usmart_dev.exe();	//执行函数 
			else 
			{  
				len=usmart_sys_cmd_exe(usmart_receivePOI.Usmart_RecvBuff);
				if(len!=USMART_FUNCERR)sta=len;
				if(sta)
				{   
					switch(sta)
					{
						case USMART_FUNCERR:
							printf("Function Error!\r\n");
							break;	
						case USMART_PARMERR:
							printf("Parameter Error!\r\n");   			
							break;				
						case USMART_PARMOVER:
							printf("Parameter Number too much!\r\n");   			
							break;		
						case USMART_NOFUNCFIND:
							printf("Cant find the match function!\r\n");   			
							break;		
					}
				}
			}
			usmart_receivePOI.Usmart_Receive_Flag = 0;
		}
		portYIELD_FROM_ISR(xTaskWokenByReceive);//如果需要的话进行一次任务切换
	}
}
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
//读取指定地址的值		 
uint32_t read_addr(uint32_t addr)
{
	return *(uint32_t*)addr;//	
}
//在指定地址写入指定的值		 
void write_addr(uint32_t addr,uint32_t val)
{
	*(uint32_t*)addr=val; 	
}
#endif













