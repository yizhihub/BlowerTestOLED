#ifndef _timer_h
#define _timer_h 

#include "common.h"


typedef enum timer
{
	timer0=0,
	timer1,
	timer2,
	timerMax,
}Timer_e;  


/*
#define Timer0Rst()  TH0=0               
#define Timer0To(X)	while(TCNT2<(X/8)) _nop_() */ //这种延时的思想源自KK飞控源程序

void Timer0_Init(uint16 cnt);
void Timer1_Init(uint16 cnt);
void Timer2_Init(uint16 cnt);
void Timer0_RST(void);
void Timer1_Rst(void);
void Timer2_Rst(void); 



#endif 