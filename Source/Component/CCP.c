#include "ccp.h"


/***********PCA定时器初始***************

***************************************/ 
void  PCA_Init()
{
	 CMOD=0x00;  //  the clock source is the timer0 overflow 
	             // sys/12 the system clock percale 12 
	 CCON=0x00;
	 CL=0;
	 CH=0;
}

void  PCA_PWM_Init(uchar width)    // uchar ch,uint freq,uchar duty
{
	 AUXR1 &=0xCF; // set the pin  P1.1 P1.0 P3.7 
	 
	 CMOD=0x0A;  //  the clock source is the timer0 overflow 
	             // sys/4 the system clock percale 4 
	 CCON=0x00;
	 CL=0;
	 CH=0;       // PCA initialize 
	 PCA_PWM0=0x00; // work as 8 bit value 
	 CCAP0H=width; //  auto reload value 
	 CCAP0L=width; //  control the duty cycle
	 CCAPM0=0x42; // 0100 0010    ECOM0  PWM0 
	
	 PCA_PWM1=0x00; // work as 8 bit value 
	 CCAP1H=width; //  auto reload value 
	 CCAP1L=width; //  control the duty cycle
	 CCAPM1=0x42; // 0100 0010    ECOM0  PWM0 
	
	 /*PCA_PWM2=0x00; // work as 8 bit value 
	 CCAP2H=width; //  auto reload value 
	 CCAP2L=width; //  control the duty cycle
	 CCAPM2=0x42; // 0100 0010    ECOM0  PWM0 */
	
	 CR=1; 
}