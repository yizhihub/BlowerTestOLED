/*2016Äê5ÔÂĞŞ¸ÄÎª   ¼¸¸ö·¢ËÍ½ÓÊÕº¯ÊıºÍÎªÒ»¸öº¯Êı*/  

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
#description:¶ÀÁ¢²¨ÌØÂÊ·¢ÉúÆ÷¸ù±¾²»ÄÜÓÃ.BRT 
#author::  2016-03-16 by'  yizhi 
****************************************************************/
void UART1Indep_Init(void)        //115200bps@11.0592MHz
{
    PCON &= 0x7F;        //²¨ÌØÂÊ²»±¶ËÙ
    SCON = 0x50;        //8Î»Êı¾İ¿É±ä²¨ÌØÂÊ
    AUXR |= 0x04;    //¶ÀÁ¢²¨ÌØÂÊ·¢ÉúÆ÷Ê±ÖÓÎª FOSC (1T)
  BRT = 0xFD;    //115200bps@11.0592MHz
    //BRT = 0xFA; //115200bps@22.1184MHz 
    //BRT = 0xFB; //115200bps@18.432 MHz   
    AUXR |= 0x01;        //´®¿ÚÒ»Ñ¡Ôñ¶ÀÁ¢²¨ÌØÂÊ·¢ÉúÆ÷Îª²¨ÌØÂÊ·¢ÉúÆ÷
    AUXR |= 0x10;        //Æô¶¯¶ÀÁ¢²¨ÌØÂÊ·¢ÉúÆ÷
    ES=0;    // Ê¹ÄÜ´®¿ÚÖĞ¶Ï 
}
/*********prepare to lanqiao competition*************************
#function: UART1_Init() 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: ¿ÉÑ¡ÓÃ T1 »òÕßT2 ×÷Îª²¨ÌØÂÊ·¢ÉúÆ÷
#author::  2016-03-16 by'  yizhi 
******************************************************************/
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7
void UART1_Init(Timer_e timer_sel)   
{ 
    PCON &=0x7f;        // ²¨ÌØÂÊ±¶ËÙÑ¡Ôñ 
    SCON = 0x50;        //8Î»Êı¾İ¿É±äbaudÂÊ ÔÊĞí´®¿Ú½ÓÊÕ£¨REN = 1£©
    if(timer_sel==timer0)
    { 
        while(1)   // ²»ÄÜÊ¹ÓÃtimer0 ×÷Îª²¨ÌØÂÊ·¢ÉúÆ÷
        {
            LED0=~LED0; msDelay(500);
        }            
    }
    else if(timer_sel==timer1)               // 413AS ²»ÄÜÊ¹ÓÃtimer1 ×÷Îª²¨ÌØÂÊ·¢ÉúÆ÷ 
    {
//        AUXR |= 0x40;		//¶¨Ê±Æ÷1Ê±ÖÓÎªFosc,¼´1T
//        AUXR &= 0xFE;		//´®¿Ú1Ñ¡Ôñ¶¨Ê±Æ÷1Îª²¨ÌØÂÊ·¢ÉúÆ÷
//        TMOD &= 0x0F;		//Éè¶¨¶¨Ê±Æ÷1Îª16Î»×Ô¶¯ÖØ×°·½Ê½
//        TL1 = 0xCC;		//Éè¶¨¶¨Ê±³õÖµ
//        TH1 = 0xFF;		//Éè¶¨¶¨Ê±³õÖµ
//        ET1 = 0;		//½ûÖ¹¶¨Ê±Æ÷1ÖĞ¶Ï
//        TR1 = 1;		//Æô¶¯¶¨Ê±Æ÷1
        AUXR |= 0x40;		//¶¨Ê±Æ÷1Ê±ÖÓÎªFosc,¼´1T
        AUXR &= 0xFE;		//´®¿Ú1Ñ¡Ôñ¶¨Ê±Æ÷1Îª²¨ÌØÂÊ·¢ÉúÆ÷
        TMOD &= 0x0F;		//Çå³ı¶¨Ê±Æ÷1Ä£Ê½Î»
        TMOD |= 0x20;		//Éè¶¨¶¨Ê±Æ÷1Îª8Î»×Ô¶¯ÖØ×°·½Ê½
        TL1 = 0xF9;		//Éè¶¨¶¨Ê±³õÖµ
        TH1 = 0xF9;		//Éè¶¨¶¨Ê±Æ÷ÖØ×°Öµ
        ET1 = 0;		//½ûÖ¹¶¨Ê±Æ÷1ÖĞ¶Ï
        TR1 = 1;		//Æô¶¯¶¨Ê±Æ÷1
        //STC12
        //TL1=0xC7;TH1=0xFE;                //9600bps @12.000MHz
        //TL1=0xFD;TH1=0xFD;(8Î»×Ô¶¯ÖØ×°)   // 115200bps@11.0592MHz
        //STC15
        //TL1= 0xE6;TH1= 0xFF; //115200bps@ 12MHz
        //TL1= 0xC7;TH1= 0xFE; //9600bps@ 12MHz
    }
    else if(timer_sel==timer2)
    {
        AUXR |= 0x04; 
        AUXR |= 0x01;  //¶¨Ê±Æ÷ Îª 1TÄ£Ê½ Ñ¡Ôñ¶¨Ê±Æ÷2Îªbaud·¢ÉúÆ÷ 
	T2L = 0xC7;			//ÉèÖÃ¶¨Ê±³õÊ¼Öµ
	T2H = 0xFE;			//ÉèÖÃ¶¨Ê±³õÊ¼Öµ
                              //T2L = 0xCC;T2H = 0xFF; 115200bps@24MHz
                              //           0xE6¡¢0xFF;115200bps@12MHz
                              // C7 FE 19200bps@24MHz
        AUXR |=0x10; // Æô¶¯¶¨Ê±Æ÷2 
        TI = 1;
    }
    ES = 0;  // ¿ª´®¿ÚÖĞ¶Ï */
} 
/*********prepare to lanqiao competition*************************
#function: UART2_Init 
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: ´®¿Ú2Ö»ÄÜ²ÉÓÃT2×÷Îª²¨ÌØÂÊ·¢ÉúÆ÷
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void UART2_Init(void)
{
    P_SW2=0x01;  // ¹Ü½ÅÉèÖÃ£ºRXD:P4^6 TXD:P4^7
    S2CON=0x50; // ¹¤×÷Ä£Ê½ÉèÖÃ 8Î»¿É±ä²¨ÌØÂÊ
    AUXR |=0x04;// T2Îª1TÄ£Ê½
    T2L=0xE6; 
    T2H=0xFF; // 115200bps @12M
    AUXR |=0x10;//²¢Æô¶¯¶¨Ê±Æ÷2 
    //IE2=0x01; // Ê¹ÄÜ ´®¿Ú2ÖĞ¶Ï
}


void  uartPutChar(Uart_e uartX,uchar dat)
{
    //while(UART_Busy);    // µÈ´ıÇ°ÃæµÄÊı¾İ·¢ËÍÍê³É Õâ¸ö·ÖºÅÕÛÄ¥¹ıÎÒ,ÇëÎŞÊÓÏÂÃæµÄÒ»¾ä»°, µ±Ê±ÉÙĞ´ÁËÒ»¸ö·ÖºÅã¶ÊÇÃ»·¢ÏÖ¡£
    // UARTcount++;         //  £¨Õâ¸öÓï¾äÒ»É¾³ı´®¿Ú·¢ËÍ¾Í³ö´í£©
    // UART_Busy=1; 
    /*
     * 2014Äê8ÔÂ16ÈÕÏÂÎç Ã»ÓĞÏë³öÎªÊ²Ã´ÔÚÖĞ¶ÏÖĞÇå³ıBusy±êÖ¾Î»¾Í²»ÄÜÓÃÁË£
     * ±íÏÖÎª£¬sometimes,TIÖÃÎ»ºóÎŞ·¨½øÈë´®¿ÚÖĞ¶Ï£¬¶øRIÖÃÎ»ºó¿ÉÒÔ£¬ÏÖÔÚÖ»ÄÜ²ÉÓÃµÈ´ıµÄ
     * ·½Ê½À´ÊµÏÖ´®¿Ú·¢ËÍ 
     * ¡ª¡ª¡ª¡ª¡ªÉÏÃæÊÇÀ´×Ô´ó¶şÊ±µÄ×¢ÊÍ±Ê¼Ç£¬µ±Ê±»¹Ã»ÓĞÑ§Ï°¹ı²Ù×÷ÏµÍ³ÖĞĞÅºÅÁ¿µÄ¸ÅÄî£¬ÎÒÒÑ¾­¿ÉÒÔÏëµ½ÓÃUART_BusyµÄ·½Ê½À´ÌáÉı´®¿Ú·¢ËÍµÄĞ§ÂÊÁË¡£
     */
    if(uartX==uart1)
    {
        while(TI == 0);      // µÈ´ıÇ°ÃæµÄÊı¾İ·¢ËÍÍê³É£¬·¢ËÍÍê³ÉºóTI»áÖÃ1  ´®¿Ú³õÊ¼»¯µÄÊ±ºòĞèÒª½«TIÖÃ1. 
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
        while(1)   // ³ö´í
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
    char xdata string[512];//·ÃÎÊÄÚ²¿À©Õ¹RAM        
    va_start(ap,fmt);  
    vsprintf(string,fmt,ap);//Ê¹ÓÃsprinfÒà¿É
    UART_SendString(uart1,string);  
    va_end(ap);  
}  */



/******************************************************************
 - ¹¦ÄÜÃèÊö£º½«Ò»¸ö32Î»µÄ±äÁ¿dat×ªÎª×Ö·û´®£¬±ÈÈç°Ñ1234×ªÎª"1234"
 - Á¥ÊôÄ£¿é£º¹«¿ªº¯ÊıÄ£¿é
 - º¯ÊıÊôĞÔ£ºÍâ²¿£¬ÓÃ»§¿Éµ÷ÓÃ
 - ²ÎÊıËµÃ÷£ºdat:´ø×ªµÄlongĞÍµÄ±äÁ¿
             str:Ö¸Ïò×Ö·ûÊı×éµÄÖ¸Õë£¬×ª»»ºóµÄ×Ö½Ú´®·ÅÔÚÆäÖĞ           
 - ·µ»ØËµÃ÷£ºÎŞ
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
 - ¹¦ÄÜÃèÊö£º½«Ò»¸ö×Ö·û´®×ªÎª32Î»µÄ±äÁ¿£¬±ÈÈç"1234"×ªÎª1234
 - Á¥ÊôÄ£¿é£º¹«¿ªº¯ÊıÄ£¿é
 - º¯ÊıÊôĞÔ£ºÍâ²¿£¬ÓÃ»§¿Éµ÷ÓÃ
 - ²ÎÊıËµÃ÷£ºstr:Ö¸Ïò´ı×ª»»µÄ×Ö·û´®           
 - ·µ»ØËµÃ÷£º×ª»»ºóµÄÊıÖµ
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
** @effect setupÒª»ñÈ¡µÄ±äÁ¿
** @import pAddr Òª»ñÈ¡±äÁ¿µÄµØÖ· ºÍ ±äÁ¿µÄ´óĞ¡£¬ucNum Òª»ñÈ¡¼¸¸ö±äÁ¿
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
    
    uartPutBuf(uart1, GucUartTxBuf, j + 1);    // PS ÆäÊµÕâ¼¸¸öAppº¯Êı¿ÉÒÔ½èÖúFreeMasterµÄĞ­Òé²ã·¢ËÍ£¬Ğ­Òé²ãÃ¿¸öÑ­»··¢ËÍÒ»¸ö×Ö½Ú¸üºÏÀí¡£
}

void uartAppReadScope(void)
{
    GucUartTxBuf[1] = 0xC5;   // commander message
    GucUartTxBuf[2] = 0x3B;   // checksum
    
    uartPutBuf(uart1, GucUartTxBuf, 3);
}

// ´®¿ÚµÄ¶ÓÁĞË¼Ïë 
// ´Ó»·ĞÎ¶ÓÁĞÖĞÈ¡Ò»¸öÊı³öÀ´ 
/*uchar GetOneByte(void)
{
     uchar pos;
    if(ByteNum>0) // »º³åÇøÖĞÓĞÓĞÊı¾İÎ´¶Á³ö
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
// Ïë»·ĞÎ¶ÓÁĞÖĞ·ÅÈëÒ»¸öÔªËØ 
void PutOneByte(uchar val)
{
     if(ByteNum<ByteMax) // »¹ÓĞµØ·½
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
