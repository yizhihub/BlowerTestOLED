#ifndef _uart_H_
#define _uart_H_

#include "common.h"
#include "timer.h"

#define REMOTER_FU6832       // REMOTER_FU6832   REMOTER_FU6812

#if defined(REMOTER_FU6812)
    #define MAX_SPEED_SCALE 5000
    #define MAX_CURRENT_SCALE 100.0f
    #define MAX_VOLTAG_SCALE (732.941f)
    #define ADDR_SPEEDREF    0x014A
    #define ADDR_BUSVOL      0x001A
    #define ADDR_SPEEDACT    0x002C
    #define ADDR_IS          0x0044
    #define ADDR_FAULT       0x007F
    
#elif defined(REMOTER_FU6832)
    #define MAX_SPEED_SCALE 7000
    #define MAX_CURRENT_SCALE 41.6f
    #define MAX_VOLTAG_SCALE (60.0f)
    #define ADDR_SPEEDREF    0x005C
    #define ADDR_BUSVOL      0x0001
    #define ADDR_SPEEDACT    0x001B
    #define ADDR_IS          0x4092     // ADDR_FOC_IQREF  
    #define ADDR_FAULT       0x00B6
#else
    #error "NO REMOTER PLATFORM SELECT"
#endif

#define RPM2Q15_FACTOR  (32768.0f / MAX_SPEED_SCALE)

typedef enum UART_yizhi
{
	uart0=0,
	uart1,
	uart2,
	uartMax,
}Uart_e;


extern volatile uchar GbUartBusyFlg;
extern uint8_t xdata GucUartTxBuf[20];

extern short            Gq15ReportData[4];
extern unsigned char    GucThrotSize[2];                              // max support 4 throttle cmd
extern short            Gq15ThrotCmd[2]; // _at_ 0x014A;
extern unsigned char    GbReportFlg, GbSetupScopeSent, GbBluetoothOK;

void UART1_Init(Timer_e timer_sel); 
void UART2_Init(void);
//void UART1Indep_Init(void);
void uartPutBuf(Uart_e uart,uint8 *buf,uint8 len);
void  uartPutChar(Uart_e uart,uchar dat);
//void UART2_Putchar(uchar dat);
void uartPutStr(Uart_e uart,char *s);
//void UART2_SendString(char *s);
void UART_Print(Uart_e uart,char *s,uint m,uint n,uint o,uint32 p);
//void UART2_Print(char *s,uint m,uint n,uint o,uint p);
//void UART2_printf(const char *fmt,...);
void uartAppSendThrot(int16_t sValueRpm);
void uartAppSetupScope(uint16_t ppAddr[][2], uint8_t ucNum);
void uartAppReadScope(void);
void FMSTR_WriteVar16(uint16_t usAddr, int16_t sValue);
void FMSTR_WriteVar8(uint16_t usAddr,  uint8_t ucValue);



#endif