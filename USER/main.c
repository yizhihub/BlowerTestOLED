#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "boled.h"
#include "bkey.h"
#include "bHMI.h"
#include "bBraoAdc.h"

const char GscBuildDate[]   __attribute__((section(".ARM.__at_0x8000200"))) = __DATE__;
const char GscSoftRelease[] __attribute__((section(".ARM.__at_0x800020C"))) = "00B";
const char GscBuildTIme[]   __attribute__((section(".ARM.__at_0x8000210"))) = __TIME__;

 int main(void)
 {
    hwInit();
    TIM3HallCapInit();
    uart_init(115200); 
    ledInit();    
    timInit();
    keyInit();     
    ec11Init();
    OLED_Init(); 
    msDelay(50);
    BraoAdcInit();                               /* 必须要在这里加上初始化一次才行，上电先初始化 */
//  OLED_PutStr(0,OLED_LINE0, (uint8_t *)"abcdefghijklmnopqrstuvwxyz!@#$%^&*()_123456789~!", 6, RED);
//  OLED_PutStr(0, OLED_LINE0, (uint8_t *)"123456789~!{}|AaBbCc", 6, GREEN);
//  OLED_PutStr(0, OLED_LINE1, (uint8_t *)"Aa123456789~!{}|", 8, BLUE);
//  OLED_PutStr(0, OLED_LINE2, (uint8_t *)"Am12345", 16, BLUE);
//  OLED_PutHan(22,  OLED_LINE0, (uint8_t *)"风机测试平台", 1);
    __enable_irq();

//    strcpy((char*)MenuItem[0] ,"1:MPU  ");
//    strcpy((char*)MenuItem[1] ,"2:NRF  ");
//    strcpy((char*)MenuItem[2] ,"3:RTC   ");
//    strcpy((char*)MenuItem[3] ,"4:DS1   ");
//    strcpy((char*)MenuItem[4] ,"5:IR    ");
//    strcpy((char*)MenuItem[5] ,"6:ADk   ");
//    strcpy((char*)MenuItem[6] ,"7:HMC   ");
//    sel=DrawMenu(MenuItem,7,0); 
    Menu_Display();
    while(1) 
    {
//        uint16_t lVelSetRpm;
//          if (USART1->SR & USART_SR_RXNE)     {   //  USART_GetFlagStatus    #define USART_FLAG_RXNE                      ((uint16_t)0x0020) 
//              ucData = USART1->DR;
//              while((USART1->SR & USART_SR_TXE) == 0);
//              USART1->DR = ucData;
//          }
//        static float t = 3.14f;
//        OLED_ShowChar(36,52,t,12,1);//显示ASCII字符    
//        OLED_ShowNum(94,52,t,3,12);    //显示ASCII字符的码值    
//        OLED_Refresh_Gram();//更新显示到OLED
//        t++;
//        if(t>'~')t=' ';
//        OLED_PutNumber(32, OLED_LINE0, t, 2, 1, 0, 6, GREEN);  // 92
//        OLED_PutNumber(64, OLED_LINE1, t, 2, 1, "RPM", 8, YELLOW); // 64
//        OLED_PutNumber(0, OLED_LINE2, t, 2, 0, 0, 16,GREEN);
//        LED0=!LED0;  if (t > 160)  t = 0;
    }
}

       /*SysTick->LOAD = 0xFFFFFF;
        SysTick->VAL  = 0x00;
        SysTick->CTRL|= SysTick_CTRL_ENABLE_Msk ;
        OLED_Fill(BLACK);
        SysTick->CTRL&= ~SysTick_CTRL_ENABLE_Msk; 
        OLED_PutNumber(0, OLED_LINE7, 9000000.0f / (0xFFFFFF - SysTick->VAL), 3, 2, "FPS", 6, YELLOW);*/
       /* 2B E4 00 5C 15 F1 BA */
//       USART_RX_BUF[0] = 0x2B;
//       USART_RX_BUF[1] = 0xE4;
//       USART_RX_BUF[2] = 0x00;
//       USART_RX_BUF[3] = 0x5c;
//       USART_RX_BUF[4] = 0x15;
//       USART_RX_BUF[5] = 0xF1;
//       USART_RX_BUF[6] = 0xBA;
//        
//       uartDrvPutBuf(USART_RX_BUF, 7);
