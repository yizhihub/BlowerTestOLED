/*2016年5月修改为   几个发送接收函数和为一个函数*/  

#include "uart.h"
#include "stdio.h"
#include "string.h"  // the use of strlen 
#include "stdarg.h"  // the use of vsprintf


volatile uchar GbUartBusyFlg = 0;
uint8_t xdata GucUartTxBuf[20] = {0x2B, 0x16};


unsigned long strtou32(char *str); 
void u32tostr(unsigned long dat,char *str);
/*********prepare to lanqiao competition*************************
#function: UART1Indep_Init 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:独立波特率发生器根本不能用.BRT 
#author::  2016-03-16 by'  yizhi 
****************************************************************/
void UART1Indep_Init(void)        //115200bps@11.0592MHz
{
    PCON &= 0x7F;        //波特率不倍速
    SCON = 0x50;        //8位数据可变波特率
    AUXR |= 0x04;    //独立波特率发生器时钟为 FOSC (1T)
  BRT = 0xFD;    //115200bps@11.0592MHz
    //BRT = 0xFA; //115200bps@22.1184MHz 
    //BRT = 0xFB; //115200bps@18.432 MHz   
    AUXR |= 0x01;        //串口一选择独立波特率发生器为波特率发生器
    AUXR |= 0x10;        //启动独立波特率发生器
    ES=0;    // 使能串口中断 
}
/*********prepare to lanqiao competition*************************
#function: UART1_Init() 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: 可选用 T1 或者T2 作为波特率发生器
#author::  2016-03-16 by'  yizhi 
******************************************************************/
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
void UART1_Init(Timer_e timer_sel)   
{ 
    PCON &=0x7f;        // 波特率倍速选择 
    SCON = 0x50;        //8位数据可变baud率 允许串口接收（REN = 1）
    if(timer_sel==timer0)
    { 
        while(1)   // 不能使用timer0 作为波特率发生器
        {
            LED0=~LED0; msDelay(500);
        }            
    }
    else if(timer_sel==timer1)               // 413AS 不能使用timer1 作为波特率发生器 
    {
//        AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
//        AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
//        TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
//        TL1 = 0xCC;		//设定定时初值
//        TH1 = 0xFF;		//设定定时初值
//        ET1 = 0;		//禁止定时器1中断
//        TR1 = 1;		//启动定时器1
        AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
        AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
        TMOD &= 0x0F;		//清除定时器1模式位
        TMOD |= 0x20;		//设定定时器1为8位自动重装方式
        TL1 = 0xF9;		//设定定时初值
        TH1 = 0xF9;		//设定定时器重装值
        ET1 = 0;		//禁止定时器1中断
        TR1 = 1;		//启动定时器1
        //STC12
        //TL1=0xC7;TH1=0xFE;                //9600bps @12.000MHz
        //TL1=0xFD;TH1=0xFD;(8位自动重装)   // 115200bps@11.0592MHz
        //STC15
        //TL1= 0xE6;TH1= 0xFF; //115200bps@ 12MHz
        //TL1= 0xC7;TH1= 0xFE; //9600bps@ 12MHz
    }
    else if(timer_sel==timer2)
    {
        AUXR |= 0x04; 
        AUXR |= 0x01;  //定时器 为 1T模式 选择定时器2为baud发生器 
	T2L = 0xC7;			//设置定时初始值
	T2H = 0xFE;			//设置定时初始值
                              //T2L = 0xCC;T2H = 0xFF; 115200bps@24MHz
                              //           0xE6、0xFF;115200bps@12MHz
                              // C7 FE 19200bps@24MHz
        AUXR |=0x10; // 启动定时器2 
        TI = 1;
    }
    ES = 0;  // 开串口中断 */
} 
/*********prepare to lanqiao competition*************************
#function: UART2_Init 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: 串口2只能采用T2作为波特率发生器
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void UART2_Init(void)
{
    P_SW2=0x01;  // 管脚设置：RXD:P4^6 TXD:P4^7
    S2CON=0x50; // 工作模式设置 8位可变波特率
    AUXR |=0x04;// T2为1T模式
    T2L=0xE6; 
    T2H=0xFF; // 115200bps @12M
    AUXR |=0x10;//并启动定时器2 
    //IE2=0x01; // 使能 串口2中断
}


void  uartPutChar(Uart_e uartX,uchar dat)
{
    //while(UART_Busy);    // 等待前面的数据发送完成 这个分号折磨过我,请无视下面的一句话, 当时少写了一个分号愣是没发现。
    // UARTcount++;         //  （这个语句一删除串口发送就出错）
    // UART_Busy=1; 
    /*
     * 2014年8月16日下午 没有想出为什么在中断中清除Busy标志位就不能用了
     * 表现为，sometimes,TI置位后无法进入串口中断，而RI置位后可以，现在只能采用等待的
     * 方式来实现串口发送 
     * —————上面是来自大二时的注释笔记，当时还没有学习过操作系统中信号量的概念，我已经可以想到用UART_Busy的方式来提升串口发送的效率了。
     */
    if(uartX==uart1)
    {
        while(TI == 0);      // 等待前面的数据发送完成，发送完成后TI会置1  串口初始化的时候需要将TI置1. 
        SBUF=dat;
        TI = 0;
    }
    else if(uartX==uart2)
    {
        S2BUF=dat;
        while(!(S2CON&0x02));
        S2CON&=0xfd;
    }
    else
    {
        while(1)   // 出错
        {
            LED0=~LED0; msDelay(200);
        }
    }
}

void uartPutBuf(Uart_e uartX,uint8 *buf,uint8 len)
{
    while(len--)
    {
        uartPutChar(uartX,*buf++); 
    }
}

void uartPutStr(Uart_e uartX,char *s)
{
     while(*s)
     uartPutChar(uartX,*s++);
}

void UART_Print(Uart_e uartX,char *s,uint m,uint n,uint o,uint32 p)
{
    uchar pdata ge,shi,bai,qian;
    char  temp[20];
    uartPutStr(uartX,s);
    ge=m%10;
    shi=m/10%10;
    bai=m/100%10;
    qian=m/1000;    
    uartPutChar(uartX,48+qian);
    uartPutChar(uartX,48+bai);
    uartPutChar(uartX,48+shi);
    uartPutChar(uartX,48+ge);
    uartPutChar(uartX,' ');
    ge=n%10;
    shi=n/10%10;
    bai=n/100%10;
    qian=n/1000;    
    uartPutChar(uartX,48+qian);
    uartPutChar(uartX,48+bai);
    uartPutChar(uartX,48+shi);
    uartPutChar(uartX,48+ge);
    uartPutChar(uartX,' ');
    ge=o%10;
    shi=o/10%10;
    bai=o/100%10;
    qian=o/1000;    
    uartPutChar(uartX,48+qian);
    uartPutChar(uartX,48+bai);
    uartPutChar(uartX,48+shi);
    uartPutChar(uartX,48+ge);
    uartPutChar(uartX,' '); 
//     ge=p%10;
//     shi=p/10%10;
//     bai=p/100%10;
//     qian=p/1000;    
//     uartPutChar(uartX,48+qian);
//     uartPutChar(uartX,48+bai);
//     uartPutChar(uartX,48+shi);
//     uartPutChar(uartX,48+ge);
//     uartPutChar(uartX,' '); 

   u32tostr(p,temp);
   uartPutStr(uartX,temp);
   uartPutStr(uartX,"\r\n");    
}



/*void UART2_printf(const char *fmt,...)  
{  
    va_list ap;  
    char xdata string[512];//访问内部扩展RAM        
    va_start(ap,fmt);  
    vsprintf(string,fmt,ap);//使用sprinf亦可
    UART_SendString(uart1,string);  
    va_end(ap);  
}  */



/******************************************************************
 - 功能描述：将一个32位的变量dat转为字符串，比如把1234转为"1234"
 - 隶属模块：公开函数模块
 - 函数属性：外部，用户可调用
 - 参数说明：dat:带转的long型的变量
             str:指向字符数组的指针，转换后的字节串放在其中           
 - 返回说明：无
 ******************************************************************/
void u32tostr(unsigned long dat,char *str) 
{
 char temp[20];
 unsigned char i=0,j=0;
 i=0;
 while(dat)
 {
  temp[i]=dat%10+0x30;
  i++;
  dat/=10;
 }
 j=i;
 for(i=0;i<j;i++)
 {
  str[i]=temp[j-i-1];
 }
 if(!i) {str[i++]='0';}
 str[i]=0;
}

/******************************************************************
 - 功能描述：将一个字符串转为32位的变量，比如"1234"转为1234
 - 隶属模块：公开函数模块
 - 函数属性：外部，用户可调用
 - 参数说明：str:指向待转换的字符串           
 - 返回说明：转换后的数值
 ******************************************************************/

unsigned long strtou32(char *str) 
{
 unsigned long temp=0;
 unsigned long fact=1;
 unsigned char len=strlen(str);
 unsigned char i;
 for(i=len;i>0;i--)
 {
  temp+=((str[i-1]-0x30)*fact);
  fact*=10;
 }
 return temp;
}
 
unsigned char    GucThrotSize[2];                              // max support 4 throttle cmd
short            Gq15ThrotCmd[2]; // _at_ 0x014A;
short            Gq15ReportData[4];
unsigned char    GbReportFlg = 0, GbSetupScopeSent = 0, GbBluetoothOK = 0;

void uartAppSendThrot(int16_t sValueRpm)
{
    uint8_t cSumCheck = 0;
	uint8_t counter = 1;
    int     qTemp1; 
    
    GucUartTxBuf[counter++] = 0x16;   // commander message
	cSumCheck += GucUartTxBuf[counter - 1];
	
    GucUartTxBuf[counter++] = 4u;     // length of payload
	cSumCheck += GucUartTxBuf[counter - 1];
	
    GucUartTxBuf[counter++] = 1u;     // number of var. to send
	cSumCheck += GucUartTxBuf[counter - 1];
	
    GucUartTxBuf[counter++] = 2u;     // size of var.
	cSumCheck += GucUartTxBuf[counter - 1];
    
    qTemp1 = (int)((float)sValueRpm * RPM2Q15_FACTOR);
    
    GucUartTxBuf[counter++] = *((uint8 *)(&qTemp1));
	cSumCheck += GucUartTxBuf[counter - 1];
	if (GucUartTxBuf[counter - 1] == 0x2B)
	{
		GucUartTxBuf[counter++] = 0x2B;
	}

    GucUartTxBuf[counter++] = *((uint8 *)(&qTemp1) + 1);
    cSumCheck += GucUartTxBuf[counter - 1];
	if (GucUartTxBuf[counter] == 0x2B)
	{
		GucUartTxBuf[counter++] = 0x2B;
	}

    GucUartTxBuf[counter++] = 0x100 - cSumCheck;
    
    
    uartPutBuf(uart1, GucUartTxBuf, counter);
}

void FMSTR_WriteVar16(uint16_t usAddr, int16_t sValue)
{
    uint8_t ucSumCheck = 0, i;
    
    GucUartTxBuf[1] = 0xE4;   // commander message
    GucUartTxBuf[2] = *((uint8_t *)&usAddr);
    GucUartTxBuf[3] = *((uint8_t *)&usAddr + 1);
    GucUartTxBuf[4] = *((uint8 *)(&sValue));
    GucUartTxBuf[5] = *((uint8 *)(&sValue) + 1);
    
    for (i = 1; i < 6; i++) {
        ucSumCheck += GucUartTxBuf[i];
    }
    GucUartTxBuf[6] = 0x100 - ucSumCheck;
    
    uartPutBuf(uart1, GucUartTxBuf, 7);
}

void FMSTR_WriteVar8(uint16_t usAddr, uint8_t ucValue)
{
    uint8_t ucSumCheck = 0, i;
    
    GucUartTxBuf[1] = 0xE3;   // commander message
    GucUartTxBuf[2] = *((uint8_t *)&usAddr);
    GucUartTxBuf[3] = *((uint8_t *)&usAddr + 1);
    GucUartTxBuf[4] = *((uint8 *)(&ucValue));
    GucUartTxBuf[5] = 0;
    
    for (i = 1; i < 6; i++) {
        ucSumCheck += GucUartTxBuf[i];
    }
    GucUartTxBuf[6] = 0x100 - ucSumCheck;
    
    uartPutBuf(uart1, GucUartTxBuf, 7);
}
/**
*********************************************************************************************************
** @nameis uartAppSetupScope
** @effect setup要获取的变量
** @import pAddr 要获取变量的地址 和 变量的大小，ucNum 要获取几个变量
** @export none
** @return none
** @create yizhi 2023.03.27
** @modify  
*********************************************************************************************************/
void uartAppSetupScope(uint16_t ppAddr[][2], uint8_t ucNum)
{
    uint8_t i, j = 1, ucSumCheck = 0;
    
    GucUartTxBuf[j++] = 0x08;   // commander message
    GucUartTxBuf[j++] = 1 + 3 * ucNum;   // length of payload
    GucUartTxBuf[j++] = ucNum;   // number of var's addrresses to send
    
    for (i = 0; i < ucNum; i++) {
        
        GucUartTxBuf[j++] = ppAddr[i][1];   // size of var1 
        GucUartTxBuf[j++] = *((uint8_t *)&ppAddr[i][0]);
        GucUartTxBuf[j++] = *((uint8_t *)&ppAddr[i][0] + 1);
    }
    
    for (i = 1; i < j; i++) {
        ucSumCheck +=GucUartTxBuf[i];
    }
    GucUartTxBuf[j] = 0x100 - ucSumCheck;   // checksum 
    
    uartPutBuf(uart1, GucUartTxBuf, j + 1);    // PS 其实这几个App函数可以借助FreeMaster的协议层发送，协议层每个循环发送一个字节更合理。
}

void uartAppReadScope(void)
{
    GucUartTxBuf[1] = 0xC5;   // commander message
    GucUartTxBuf[2] = 0x3B;   // checksum
    
    uartPutBuf(uart1, GucUartTxBuf, 3);
}

// 串口的队列思想 
// 从环形队列中取一个数出来 
/*uchar GetOneByte(void)
{
     uchar pos;
    if(ByteNum>0) // 缓冲区中有有数据未读出
    {
         pos=read_head;
        if(read_head+1==ByteMax)
        {
      read_head=0;            
    }
        else
          read_head=read_head+1;
        ByteNum--;
        return RoundBuffer[pos];
    }
    else
    {
        UART2_printf("The buffer is empty \n");
        return 0;
  }
}
// 想环形队列中放入一个元素 
void PutOneByte(uchar val)
{
     if(ByteNum<ByteMax) // 还有地方
     {
         RoundBuffer[write_head]=val;
    if(write_head+1==ByteMax)
        {
      write_head=0;            
    }
        else
          write_head=write_head+1;    
    ByteNum++;        
   }
     else
     {
         UART2_printf("The buffer is full \n");
   }
}
*/
