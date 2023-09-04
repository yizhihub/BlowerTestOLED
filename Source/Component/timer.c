#include "timer.h"
/*********prepare to lanqiao competition*************************
#function: Timer0_function 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void Timer0_Init(uint16 cnt)		//   @12.000MHz
{
	AUXR &= 0x7F;   //定时器时钟为12T模式(fsoc/12)// 这样慢一点,可供定时的实间更长  
	//AUXR |= 0x80; //定时器时钟为1 T模式(fsoc) //   
	TMOD &= 0xF0;		//16位自动重装的  
	TL0 = (65536-cnt);		
	TH0 = (65536-cnt)>>8;		 //  1ms 
	TF0 = 0;
  ET0=1;	// 打开中断
	TR0 = 1;		
}
/*********prepare to lanqiao competition*************************
#function: Timer1_function
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-03-16 by yizhi 
******************************************************************/
void Timer1_Init(uint16 cnt)
{
  AUXR &= 0xBF;	 //定时器时钟为12T模式 
	//AUXR |= 0x40;//定时器时钟为 1T模式, 更快
	TMOD &= 0x0F;	//设定工作模式
	TL1 = (65536-cnt)&0x00ff;		//
	TH1 = (65536-cnt)>>8;		//8ms定时器初值
	TF1 = 0;		 //清标志位
	ET1=1;	     // 打开定时器中断
	TR1 = 1;		 //开始计时
}
//void Timer1_Rst(void) // 计时器计时归位
//{
//  TL1=0xC0;
//  TH1=0xE0;    // delay for 8ms @12.000MHz
//}
/*********prepare to lanqiao competition*************************
#function: Timer2_Init
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void Timer2_Init(uint16 cnt)
{
	AUXR &=0xFB;  // 定时器时钟为12T模式
  T2L=(65536-cnt)&0x00ff;
  T2H=(65536-cnt)>>8;//55536>>8;   
	AUXR|=0x10;  // start the timer2 
 	IE2|=0x04;   // enable the interrupt 	
}
//void Timer2_Rst(void)
//{
//	T2L=0xC0;
//  T2H=0xE0;    // delay for 20ms @12.000MHz
//}