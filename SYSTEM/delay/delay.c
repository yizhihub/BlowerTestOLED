#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////      
//如果需要使用OS,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"                    //ucos 使用      
#endif
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//使用SysTick的普通计数模式对延迟进行管理（适合STM32F10x系列）
//包括usDelay,msDelay
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2010/1/1
//版本：V1.8
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.2修改说明
//修正了中断中调用出现死循环的错误
//防止延时不准确,采用do while结构!
//V1.3修改说明
//增加了对UCOSII延时的支持.
//如果使用ucosII,delay_init会自动设置SYSTICK的值,使之与ucos的TICKS_PER_SEC对应.
//msDelay和usDelay也进行了针对ucos的改造.
//usDelay可以在ucos下使用,而且准确度很高,更重要的是没有占用额外的定时器.
//msDelay在ucos下,可以当成OSTimeDly来用,在未启动ucos时,它采用usDelay实现,从而准确延时
//可以用来初始化外设,在启动了ucos之后msDelay根据延时的长短,选择OSTimeDly实现或者usDelay实现.
//V1.4修改说明 20110929
//修改了使用ucos,但是ucos未启动的时候,msDelay中中断无法响应的bug.
//V1.5修改说明 20120902
//在usDelay加入ucos上锁，防止由于ucos打断usDelay的执行，可能导致的延时不准。
//V1.6修改说明 20150109
//在msDelay加入OSLockNesting判断。
//V1.7修改说明 20150319
//修改OS支持方式,以支持任意OS(不限于UCOSII和UCOSIII,理论上任意OS都可以支持)
//添加:delay_osrunning/delay_ostickspersec/delay_osintnesting三个宏定义
//添加:delay_osschedlock/delay_osschedunlock/delay_ostimedly三个函数
//V1.8修改说明 20150519
//修正UCOSIII支持时的2个bug：
//delay_tickspersec改为：delay_ostickspersec
//delay_intnesting改为：delay_osintnesting
//////////////////////////////////////////////////////////////////////////////////  

static u8  fac_us=0;                            //us延时倍乘数               
static u16 fac_ms=0;                            //ms延时倍乘数,在ucos下,代表每个节拍的ms数

//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init()
{
#if SYSTEM_SUPPORT_OS                              //如果需要支持OS.
    u32 reload;
#endif
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);    //选择外部时钟  HCLK/8
    fac_us=SystemCoreClock/8000000;                //为系统时钟的1/8  
#if SYSTEM_SUPPORT_OS                              //如果需要支持OS.
    reload=SystemCoreClock/8000000;                //每秒钟的计数次数 单位为M  
    reload*=1000000/delay_ostickspersec;        //根据delay_ostickspersec设定溢出时间
                                                //reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右    
    fac_ms=1000/delay_ostickspersec;            //代表OS可以延时的最少单位       

    SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;       //开启SYSTICK中断
    SysTick->LOAD=reload;                         //每1/delay_ostickspersec秒中断一次    
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;       //开启SYSTICK    

#else
    fac_ms=(u16)fac_us*1000;                    //非OS下,代表每个ms需要的systick时钟数   
#endif
}                                    

void us_Delay(u32 nus)
{        
    u32 temp;             
    SysTick->LOAD=nus*fac_us;                     //时间加载               
    SysTick->VAL=0x00;                            //清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;    //开始倒数      
    do
    {
        temp=SysTick->CTRL;
    }while((temp&0x01)&&!(temp&(1<<16)));        //等待时间到达   
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;    //关闭计数器
    SysTick->VAL =0X00;                           //清空计数器     
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void ms_Delay(u16 nms)
{                     
    u32 temp;           
    SysTick->LOAD=(u32)nms*fac_ms;                //时间加载(SysTick->LOAD为24bit)
    SysTick->VAL =0x00;                            //清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;    //开始倒数  
    do
    {
        temp=SysTick->CTRL;
    }while((temp&0x01)&&!(temp&(1<<16)));        //等待时间到达   
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;    //关闭计数器
    SysTick->VAL =0X00;                           //清空计数器              
}

/*********************************************************************************************************
  End Of File
*********************************************************************************************************/









































