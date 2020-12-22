/*
 * @Description: 触摸按键
 * @Author: HLLI8
 * @Date: 2020-12-20 20:09:56
 * @LastEditTime: 2020-12-22 20:35:08
 * @LastEditors: HLLI8
 */
#include "tpad.h"
#include "tim.h"
#include "main.h"
#define TPAD_ARR_MAX_VAL 0XFFFFFFFF //最大的 ARR 值(TIM2 是 32 位定时器)
uint16_t tpad_default_val=0; //空载的时候(没有手按下),计数器需要的时间
//初始化触摸按键
//获得空载的时候触摸按键的取值.
//psc:分频系数,越小,灵敏度越高.
//返回值:0,初始化成功;1,初始化失败
uint8_t TPAD_Init(void)
{
    uint16_t buf[10];
    uint16_t temp;
    uint8_t j,i;
    for(i=0;i<10;i++)//连续读取 10 次
    {
        buf[i]=TPAD_Get_Val();
        delay_ms(10);
    }
    for(i=0;i<9;i++)//排序
    {
        for(j=i+1;j<10;j++) {
            if(buf[i]>buf[j]) {//升序排列
                temp=buf[i];
                buf[i]=buf[j];
                buf[j]=temp;
            }
        }
    }
    temp=0;
    for(i=2;i<8;i++)temp+=buf[i];//取中间的 8 个数据进行平均
    tpad_default_val=temp/6;
    printf("tpad_default_val:%d\r\n",tpad_default_val);
    if(tpad_default_val>(uint16_t)TPAD_ARR_MAX_VAL/2)return 1; //初始化遇到超过 TPAD_ARR_MAX_VAL/2 的数值,不正常!
    return 0;
}
//复位一次
//释放电容电量，并清除定时器的计数值
void TPAD_Reset(void){
    GPIO_InitTypeDef GPIO_Initure;
    GPIO_Initure.Pin=GPIO_PIN_5; //PA5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP; //推挽输出
    GPIO_Initure.Pull=GPIO_PULLDOWN; //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH; //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    HAL_GPIO_WritePin(TPAD_Key_GPIO_Port,TPAD_Key_Pin,GPIO_PIN_RESET); //PA5 输出 0，放电
    delay_ms(5);
    __HAL_TIM_CLEAR_FLAG(&htim2,TIM_FLAG_CC1|TIM_FLAG_UPDATE); //清除标志位
    __HAL_TIM_SET_COUNTER(&htim2,0); //计数器值归 0
    GPIO_Initure.Mode=GPIO_MODE_AF_PP; //推挽复用
    GPIO_Initure.Pull=GPIO_NOPULL; //不带上下拉
    GPIO_Initure.Alternate=GPIO_AF1_TIM2; //PA5 复用为 TIM2 通道 1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}
//得到定时器捕获值
//如果超时,则直接返回定时器的计数值.
//返回值：捕获值/计数值（超时的情况下返回）
uint16_t TPAD_Get_Val(void)
{
    TPAD_Reset();
    while(__HAL_TIM_GET_FLAG(&htim2,TIM_FLAG_CC1)==RESET)
    //等待捕获上升沿
    {
        if(__HAL_TIM_GET_COUNTER(&htim2)>TPAD_ARR_MAX_VAL-500){
            return __HAL_TIM_GET_COUNTER(&htim2); //超时了，直接返回 CNT 的值
        }
    };
    return HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1);
}

//读取 n 次,取最大值
//n：连续获取的次数
//返回值： n 次读数里面读到的最大读数值
uint16_t TPAD_Get_MaxVal(uint8_t n)
{
    uint16_t temp=0;
    uint16_t res=0;
    uint8_t lcntnum=n*2/3;//至少 2/3*n 的有效个触摸,才算有效
    uint8_t okcnt=0;
    while(n--){
        temp=TPAD_Get_Val();//得到一次值
        if(temp>(tpad_default_val*5/4))okcnt++;//至少大于默认值的 5/4 才算有效
        if(temp>res)res=temp;
    }
    if(okcnt>=lcntnum){ 
        return res;//至少 2/3 的概率,要大于默认值的 5/4 才算有效
    }else{ 
        return 0;
    }
}

//扫描触摸按键
//mode:0,不支持连续触发(按下一次必须松开才能按下一次);
//1,支持连续触发(可以一直按下)
//返回值:0,没有按下;1,有按下;
uint8_t TPAD_Scan(uint8_t mode)
{
    static uint8_t keyen=0; //0,可以开始检测;>0,还不能开始检测
    uint8_t res=0;
    uint8_t sample=3; //默认采样次数为 3 次
    uint16_t rval;
    if(mode)
    {
        sample=6; //支持连按的时候，设置采样次数为 6 次
        keyen=0; //支持连按
    }
    rval=TPAD_Get_MaxVal(sample);
    if(rval>(tpad_default_val*4/3)&&rval<(10*tpad_default_val))//大于 tpad_default_val+(1/3)*tpad_default_val,且小于 10 倍 tpad_default_val,则有效
    {
        if(keyen==0)res=1; //keyen==0,有效
        printf("r:%d\r\n",rval);
        keyen=3; //至少要再过 3 次之后才能按键有效
    }
    if(keyen) keyen--;
    return res;
}