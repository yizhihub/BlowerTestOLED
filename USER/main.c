#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "boled.h"
#include "bkey.h"
#include "bHMI.h"

INT8U  ucTemperature;
INT16U ulVoltage10Mv;
INT16U uleRpm, uleRpmFilter;
INT16U uleRpmLPFilterArray[20];
INT8U  ucArrayIndex = 0;
INT32U uleRpmFilterSum = 0;
INT16S sPwmDutyValue  = 0;

uchar UartCMDSpeedTxBuf[10] = {0x2B, 0xC1, 0x3F};

uchar MenuItem[7][16];
uchar MenuValue[7][16];

uchar ucNowItem = 0;
uchar ucFirstItem = 0;
uchar ucTopDisp;
uchar ucEntrysEveryScreen = 5;
uchar ucEntrysTotal       = 5;
uchar bPwmRunning  = 0, bScreenIDFlg = 1, bScreenValFlg = 0, bEdit_flag   = 0;       

 int main(void)
 {
    KEYn_e eKeyPress;
                                    /* edit the selection */
    uchar  ii;
                  
    hwInit();
    uart_init(115200); 
    ledInit();
    timInit();
    keyInit();
    ec11Init();
    OLED_Init();
//  OLED_PutStr(0,OLED_LINE0, (uint8_t *)"abcdefghijklmnopqrstuvwxyz!@#$%^&*()_123456789~!", 6, RED);
//    OLED_PutStr(0, OLED_LINE0, (uint8_t *)"123456789~!{}|AaBbCc", 6, GREEN);
//    OLED_PutStr(0, OLED_LINE1, (uint8_t *)"Aa123456789~!{}|", 8, BLUE);
//    OLED_PutStr(0, OLED_LINE2, (uint8_t *)"Am12345", 16, BLUE);
      OLED_PutHan(22,  OLED_LINE0, (uint8_t *)"风机测试平台", 1);
//    OLED_PutNumber(0, OLED_LINE4, 0, 2, 1, "℃", 6, GREEN);  // 92
//    OLED_PutNumber(0, OLED_LINE5, 0, 2, 1, "℃", 8, YELLOW); // 64
//    OLED_PutNumber(0, OLED_LINE6, 0, 2, 0, "℃", 16,GREEN);
      OLED_PutStr(104, OLED_LINE3, (uint8_t *)"RPM", 8, 1);
    __enable_irq();
     
    strcpy((char*)MenuItem[0] ,"1:MPU  ");
    strcpy((char*)MenuItem[1] ,"2:NRF  ");
    strcpy((char*)MenuItem[2] ,"3:RTC   ");
    strcpy((char*)MenuItem[3] ,"4:DS1   ");
    strcpy((char*)MenuItem[4] ,"5:IR    ");
    strcpy((char*)MenuItem[5] ,"6:ADk   ");
    strcpy((char*)MenuItem[6] ,"7:HMC   ");
//    sel=DrawMenu(MenuItem,7,0); 
       
    PwmCtrl.tPwmCtrl.sPwmLowVal  = 500;
    PwmCtrl.tPwmCtrl.sPwmLowDur  = 100;
    PwmCtrl.tPwmCtrl.sPwmHighVal = 999;
    PwmCtrl.tPwmCtrl.sPwmHighDur = 100;
    PwmCtrl.tPwmCtrl.ePwmProtocol = 2;
    PWM_KeyCallback(4);
    PWM_KeyCallback(0);
    PWM_KeyCallback(2);
    while(1) 
    {
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
        
        if (GbUartRxDone) {
            
              if (GucUartRxIndex == 8) {
//                  ucTemperature = USART_RX_BUF[0];
//                  ulVoltage10Mv = USART_RX_BUF[1] << 8 | USART_RX_BUF[2];
                  uleRpm        = USART_RX_BUF[3] << 8 | USART_RX_BUF[4];
                  GbUartRxDone =  0;                                     /* 消费完成 */ 
                  LED0 = !LED0;
                  uleRpmFilterSum += uleRpm;
                  ucArrayIndex++;
                  if (ucArrayIndex == 10) {
                      ucArrayIndex = 0;
                      uleRpmFilter = uleRpmFilterSum / 10;
                      uleRpmFilterSum = 0;                      
                     
//                      OLED_PutNumber(0 , OLED_LINE1, ulVoltage10Mv / 100.0f, 2, 1, "V",  8, 1);
//                      OLED_PutNumber(48, OLED_LINE1, ucTemperature,          2, 0, "℃", 8, 1);
                      OLED_PutNum   (64,  OLED_LINE3,  uleRpmFilter,     5,          8, 1);
                  }
              } else {
                  GbUartRxDone = 0;                                     /* 直接消费完成 */
              }
        }
        
        
        if (GulPrintTimeCnt > 10) { 
            GulPrintTimeCnt = 0;
            uartDrvPutBuf(USART2, UartCMDSpeedTxBuf, 3);                    /* ask for speed frame */ 
            if(bScreenIDFlg)
            {
                bScreenIDFlg=0;
                
                strcpy((char*)MenuItem[0], "H:");
                strcpy((char*)MenuItem[1], "T:");
                strcpy((char*)MenuItem[2], "L:");
                strcpy((char*)MenuItem[3], "T:");
                strcpy((char*)MenuItem[4], "P:");
                
                sprintf((char*)MenuValue[0], "%4.1f%%", PwmCtrl.tPwmCtrl.sPwmLowVal / 10.0f);
                sprintf((char*)MenuValue[1] ,"%04dms", PwmCtrl.tPwmCtrl.sPwmLowDur * 10);
                sprintf((char*)MenuValue[2] ,"%4.1f%%", PwmCtrl.tPwmCtrl.sPwmHighVal / 10.0f);
                sprintf((char*)MenuValue[3] ,"%04dms", PwmCtrl.tPwmCtrl.sPwmHighDur * 10);
                sprintf((char*)MenuValue[4] ,"%02d", PwmCtrl.tPwmCtrl.ePwmProtocol);
                
                for(ii=0; ii < ucEntrysEveryScreen; ii++)                                          
                {
                    if((ucNowItem != ii) || bPwmRunning == 1)
                    {   
                        HMI_Draw(ii, 0, MenuItem[ucFirstItem+ii], 1); // 正常显示
                    }
                    else
                    {
                        HMI_Draw(ii, 0, MenuItem[ucFirstItem+ii], 0); // 高亮显示
                    }
                }

                for(ii=0; ii < ucEntrysEveryScreen; ii++)                                          
                {
                      HMI_Draw(ii, 16, MenuValue[ucFirstItem+ii], 1); // 正常显示
                }
            }
            
            if (bScreenValFlg)
            {
                bScreenValFlg = 0;
                sprintf((char*)MenuValue[0], "%4.1f%%", PwmCtrl.tPwmCtrl.sPwmLowVal / 10.0f);
                sprintf((char*)MenuValue[1] ,"%04dms", PwmCtrl.tPwmCtrl.sPwmLowDur*10);
                sprintf((char*)MenuValue[2] ,"%4.1f%%", PwmCtrl.tPwmCtrl.sPwmHighVal / 10.0f);
                sprintf((char*)MenuValue[3] ,"%04dms", PwmCtrl.tPwmCtrl.sPwmHighDur*10);
                sprintf((char*)MenuValue[4] ,"%02d", PwmCtrl.tPwmCtrl.ePwmProtocol);
                HMI_Draw(ucNowItem, 16, MenuValue[ucFirstItem+ucNowItem], 0);

            }
            eKeyPress = ADKey_Check();
            switch (eKeyPress) {
                
            case EC11_CW:

                if (!bEdit_flag) {
                    GsEc11CntCW                  = 0;
                    if(ucNowItem > 0)         ucNowItem   -= 1;               // 当前屏 的条目
                    else if(ucFirstItem > 0)  ucFirstItem -= 1;               // 当前屏 的第一个条目 
                    else                                                      // 已到达所有菜单的第一个条目
                    {
                        ucNowItem   =  ucEntrysEveryScreen - 1;
                        ucFirstItem =  ucEntrysTotal - ucEntrysEveryScreen;                                
                    }
                    bScreenIDFlg = 1;
                    bPwmRunning  = 0;                                   /* once rotate stop pwm run */ 
                } else {
                    PwmCtrl.sPwmCtrl[ucNowItem] -= (GsEc11CntCW + 1);
                    GsEc11CntCW                  = 0;
                   if (PwmCtrl.sPwmCtrl[ucNowItem] < 0) PwmCtrl.sPwmCtrl[ucNowItem] = 0;
                    bScreenValFlg = 1;
                    PWM_KeyCallback(ucNowItem);
                }
                break;
            
            case EC11_CCW:
                if (!bEdit_flag) {
                    GsEc11CntCCW                 = 0;
                    if(ucNowItem < (ucEntrysEveryScreen - 1))                      ucNowItem   += 1;
                    else if((ucFirstItem + ucEntrysEveryScreen) < ucEntrysTotal)   ucFirstItem += 1;
                    else
                    {
                      ucFirstItem = 0;
                      ucNowItem   = 0;
                    }
                    bScreenIDFlg = 1;
                    bPwmRunning  = 0;                                   /* once rotate stop pwm run */ 
                } else {
                    PwmCtrl.sPwmCtrl[ucNowItem] += (GsEc11CntCCW + 1);
                    GsEc11CntCCW                 = 0;
                    if (PwmCtrl.sPwmCtrl[ucNowItem] > 999) PwmCtrl.sPwmCtrl[ucNowItem] = 999;
                    bScreenValFlg = 1;
                    PWM_KeyCallback(ucNowItem);
                }
                break;
            
            case KEY_CANCEL: 
                bPwmRunning = !bPwmRunning;
                bScreenIDFlg = 1;
                bEdit_flag   = 0;
                break;
            
            case EC11_SET:
                if (!bPwmRunning) {
                    bEdit_flag = !bEdit_flag;
                    (bEdit_flag == 1) ? (bScreenValFlg = 1) : (bScreenIDFlg = 1);
                    if (bEdit_flag) 
                        PWM_KeyCallback(ucNowItem);
                } else {                                                /* enter adjust mode when Pwm is Runing, just like press KEY_CANCEL */
                    bPwmRunning = !bPwmRunning;
                    bScreenIDFlg = 1;
                    bEdit_flag   = 0;
                }
                break;
                
            default:
                break;
            }
            if (bEdit_flag)
                sPwmDutyValue = PwmCtrl.tPwmCtrl.sPwmHighVal;
            else 
                sPwmDutyValue = 0;
            
            if (sPwmDutyValue >= 999)  sPwmDutyValue = 999;
            if (sPwmDutyValue <= 0)      sPwmDutyValue = 0;
            
            
            if (bPwmRunning)  {
                PwmCtrl.tPwmCtrl.usTime10msCnt++;
                if (PwmCtrl.tPwmCtrl.usTime10msCnt < PwmCtrl.tPwmCtrl.sPwmLowDur) {
                    PwmCtrl.tPwmCtrl.sPwmDutyValue = PwmCtrl.tPwmCtrl.sPwmWidthLow;
                } else if (PwmCtrl.tPwmCtrl.usTime10msCnt < (PwmCtrl.tPwmCtrl.sPwmLowDur + PwmCtrl.tPwmCtrl.sPwmHighDur)) {
                    PwmCtrl.tPwmCtrl.sPwmDutyValue = PwmCtrl.tPwmCtrl.sPwmWidthHigh;
                } else {
                    PwmCtrl.tPwmCtrl.usTime10msCnt = 0;
                }
            } else if (bEdit_flag){                                                    /* set pwm output to default value when not run  */
                PwmCtrl.tPwmCtrl.usTime10msCnt = 0;
                PwmCtrl.tPwmCtrl.sPwmDutyValue = PwmCtrl.tPwmCtrl.sPwmWidthSetting;
            } else {
                PwmCtrl.tPwmCtrl.usTime10msCnt = 0;
                PwmCtrl.tPwmCtrl.sPwmDutyValue = PwmCtrl.tPwmCtrl.sPwmWidthDflt;
            }
            
            
//            OLED_PutNum   (0,  OLED_LINE2, sPwmDutyValue,  5,          8, 0);
//            OLED_PutNum   (0,  OLED_LINE3, GsEc11SetV,     5,          8, 0);
            TIM_SetCompare1(TIM3, PwmCtrl.tPwmCtrl.sPwmDutyValue);
        }
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
