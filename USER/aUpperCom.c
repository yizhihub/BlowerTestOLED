/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               aUpperCom.c
** Created By  :            Y1740
** Created Date:            2023-06-23
** Last Version:            v1.0 
** Descriptions:            foc peripherals ADC PWM Drv Code and so on
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
**--------------------------------------------------------------------------------------------------------
**
*********************************************************************************************************/
#include "aUpperCom.h"
#include "bKey.h"
#include "bOLED.h"
#include "bBraoAdc.h"
#include "bINA226.h"
#include "usart.h"

#include "ZMC_QMath.h"
#include "ZMC_Pid.h"
#include "ZMC_Filter.h"

#include "bHMI.h"

/*********************************************************************************************************
  内部函数声明
*********************************************************************************************************/

/*********************************************************************************************************
  内部变量定义
*********************************************************************************************************/
//INT16U  __GusDataAdcIa[1];                                                /* 保存ADC1数值     */
//INT16U  __GusDataAdcIb[1];                                                /* 保存ADC2数值     */
//INT16U  __GusDataAdcIc[1];
//void (*GpfuncIntPwm) (INT8U) = NULL;
//void (*GpfuncIntFault) (void) = NULL;

static uchar UartAckSpdCmdTxBuf[10] = {0xfa, 0xfa, 0xfa, 0x01, 0x55, 0x55, 0xfe, 0xfe};
static uchar UartAckSpeedTxBuf[10] = {0xfa, 0xfa, 0xfa, 0x03, 0x01, 0x00, 0xfe, 0xfe};

static uchar MenuItem[7][17];
static uchar NowItem;
static uchar FirstItem;
static uchar UartCMDSpeedTxBuf[10] = {0x2B, 0xC1, 0x3F};
static uchar MenuValue[7][16];



static INT8U  ucTemperature;
static INT16U ulVoltage10Mv;
static INT32S lEscEleVel100Rpm, lEscEleRpmFilter;
static INT16U uleRpmLPFilterArray[20];
static INT8U  ucArrayIndex = 0;
static INT32U uleRpmFilterSum = 0;
static INT16S sPwmDutyValue  = 0;
static uchar ucNowItem = 0;
static uchar ucFirstItem = 0;
static uchar ucTopDisp;
static uchar ucEntrysEveryScreen = 0;
static uchar ucEntrysTotal       = 0;
static uchar bPwmRunning  = 0, bScreenIDFlg = 1, bScreenValFlg = 0, bEdit_flag   = 0;

ZMC_LOWPASS_DATA  _GtLowPassVelOne, _GtLowPassVelOneX;
ZMC_PID_DATA      _GtPidVelOne;                             /* 速度环Vel PID                */
INT32S            _GlSetMecVelRpm = 0;

/*********************************************************************************************************
  全局变量定义
*********************************************************************************************************/

 
/**
********************************************************************************************************
** @nameis zmcFocHallDrvInit
** @effect hall io initilize
** @import none
** @export none
** @return none
** @create yizhi 2023.06.24
** @modify 
*********************************************************************************************************/
void UpperComCXD7054(uint8_t ucX)
{
    KEYn_e eKeyPress;
    uint8_t ii = 0, uc10msCnt = 0, bFlip = 0;
    INT16U  usVol;
    INT16S  sCur;
    INT8U   GbUartRxReply1 = 0;
    INT8U   GucPolePairs   = 2;
    INT8U   GbSpeedReportEn = 0;
    INT32S  lSetMecVeRpm;
    INT16U  GusRxFromPcCnt = 0;
    INT16U  GusTx2PcCnt    = 0;

    bPwmRunning  = 0;
    bScreenIDFlg = 0;
    bScreenValFlg = 1,
    bEdit_flag   = 0;
    ucEntrysEveryScreen = 4;
    ucEntrysTotal       = 4;
    sPwmDutyValue       = 0;
    _GlSetMecVelRpm      = 0;
    INA226_Init();
    BraoCalibrate();
    
    OLED_Fill(0x00);
    OLED_Print(0, OLED_LINE0, (uint8_t *)"CXD 7054:", 1);
    OLED_PutStr(120, OLED_LINE3, (uint8_t *)"R", 8, 1);
    OLED_PutStr(0,  OLED_LINE3 + LINE_HEIGHT / 2,  "-----", 6, 1);
//    OLED_PutStr(0, OLED_LINE3, (uint8_t *)"UperCmd:", 8, 1);  


    /*
     *  PID-Vel 初始化  By HHG 151106    
     *  下面速度环参数， 关于qKi qKd的初始化需要按照速度环的执行周期，如果改变速度环执行周期，需要
     *  做相应的修改 by lgd ，已更正 @ 2019年9月10日。
     */
    _GtPidVelOne.ulQn      = ZMC_QMATH_SIZE;
    _GtPidVelOne.qRef      = 0;
    _GtPidVelOne.qKp       = 0.8f;
    _GtPidVelOne.qUp       = 0;
    _GtPidVelOne.qUi       = 0;
    _GtPidVelOne.qUd       = 0;
    _GtPidVelOne.qOutMax   = 999;
    _GtPidVelOne.qOutMin   = 0;
    _GtPidVelOne.qSatErr   = 0;
    _GtPidVelOne.qKi       = 0.2f;
    _GtPidVelOne.qKc       = 0.2f;
    _GtPidVelOne.qKd       = 0;
    _GtPidVelOne.qUp1      = 0; 
    
    zmcLowPassInit(&_GtLowPassVelOne,  0.032f, 2, 0);
    zmcLowPassInit(&_GtLowPassVelOneX, 0.032f, 1, 0);

    
    while(ADKey_Scan()!=KEY_CANCEL)
    {
        if (GbUartRxDone) {
            if (GucUartRxIndex == 10) {
                ucTemperature = USART_RX_BUF[0];
                ulVoltage10Mv = USART_RX_BUF[1] << 8 | USART_RX_BUF[2];
                lEscEleVel100Rpm        = USART_RX_BUF[7] << 8 | USART_RX_BUF[8];
                GbUartRxDone =  0;                                     /* 消费完成 */                    
                 /* OLED_PutNumber(0 , OLED_LINE1, ulVoltage10Mv / 100.0f, 2, 1, "V",  8, 1);
                    OLED_PutNumber(48, OLED_LINE1, ucTemperature,          2, 0, "℃", 8, 1); */
                LED0_TOG();
                
                ZMC_LOW_PASS(&_GtLowPassVelOne, lEscEleVel100Rpm);                    /* 用作PID运算当前转速 */
                ZMC_LOW_PASS(&_GtLowPassVelOneX, lEscEleVel100Rpm);                   /* 用作显示和上报      */ 
                lEscEleRpmFilter = _GtLowPassVelOne.qOut;
                
                /* 
                 * 在此处进行PID运算  给定值就是 全局变量 输出值 注意此处设定分辨率是 100RPM
                 */
                _GtPidVelOne.qRef = _GlSetMecVelRpm * GucPolePairs / 100;               /* 转换为设定电气转速 */ 
                ZMC_PID(&_GtPidVelOne, lEscEleRpmFilter)
                sPwmDutyValue = _GtPidVelOne.qOut;
                OLED_PutNum(0,  OLED_LINE3, sPwmDutyValue, 5, 6, 1);
                OLED_PutNum(80,  OLED_LINE3,  (INT32S)(_GtLowPassVelOneX.qOut + 0.5f) * (100 / GucPolePairs) ,     5,          8, 1);
                
             } else {
                GbUartRxDone = 0;                                     /* 直接消费完成 */
             }
        } else if (GbUartRxDone1) {
            
            if (GucUartRxIndex1 == 8) {
                lSetMecVeRpm    = USART1_RX_BUF[4] << 8 | USART1_RX_BUF[5];
                GbUartRxDone1 =  0;                                     /* 消费完成 */
                UartAckSpdCmdTxBuf[3] = USART1_RX_BUF[3];
                GusRxFromPcCnt++;                                       /* 电脑数据帧计数    */
                
                switch (USART1_RX_BUF[3]) {
                    
                case 0x01:                                               /* 设定电机转速     */
                    _GlSetMecVelRpm = lSetMecVeRpm;
                    GbUartRxReply1  =  1;                                /* send reply signal */
                    break;
                    
                case 0x02:                                               /* 关机 */
                    _GlSetMecVelRpm = 0;
                    GbUartRxReply1  =  1;                                /* send reply signal */
                    break;
                
                case 0x03:
                    GbSpeedReportEn = 1;                                /* 使能转速回传 */
                    uc10msCnt       = 101;                              /* 转速回传立刻 */
                    break;
                
                case 0x04:
                    GbSpeedReportEn = 0;                               /* 关闭转速回传 */
                    GbUartRxReply1  =  1;                              /* send reply signal */
                    break;
                
                default:
                    break;
                }
                OLED_PutNum(0,  OLED_LINE3 + LINE_HEIGHT / 2,  _GlSetMecVelRpm,     5,          6, 1);
            } else {
                    
                GbUartRxDone1 = 0;                                     /* 直接消费完成 */
            }
        }  else  {
            if (GulPrintTimeCnt > 10) { 
                GulPrintTimeCnt = 0;
                
                if (GbUartRxReply1) {
                    GbUartRxReply1 = 0;
                    uartDrvPutBuf(USART1, UartAckSpdCmdTxBuf, 8);          /* danger 返回速度慢 */ 
                }
                /* uartDrvPutBuf(USART2, UartCMDSpeedTxBuf, 3);    ask for speed frame */ 
                if (uc10msCnt++ > 25) {
                    uc10msCnt = 0;
                    bFlip = !bFlip;
                    
                    if (bFlip) {
                        if (GbSpeedReportEn) {
                            lSetMecVeRpm         = (INT32S)(_GtLowPassVelOneX.qOut + 0.5f);
                            UartAckSpeedTxBuf[4] = (lSetMecVeRpm * (100 / GucPolePairs)) >> 8;
                            UartAckSpeedTxBuf[5] = (lSetMecVeRpm * (100 / GucPolePairs)) & 0x00ff;
                            uartDrvPutBuf(USART1, UartAckSpeedTxBuf, 8);
                            GusTx2PcCnt++;                                                  /* 发送给电脑计数  */ 
                        }
                    } else {
                        INA226_Read(&usVol, &sCur);
                        OLED_PutNumber(0,  OLED_LINE2, usVol / 100.0f, 2, 2, "V", 8, 1);
                        OLED_PutNumber(72, OLED_LINE2, sCur / 1000.0f, 2, 3, "A", 8, 1);
                    }
                } else {
                    OLED_PutNum(40, OLED_LINE3,   GusRxFromPcCnt, 5, 6, 1);                  /* 显示电脑数据帧计数 */
                    OLED_PutNum(40, OLED_LINE3 + LINE_HEIGHT / 2, GusTx2PcCnt, 5, 6, 1);     /* 显示发送计数       */
                }
                eKeyPress = ADKey_Check();
                switch (eKeyPress) {
                    
                case EC11_CW:
                     _GlSetMecVelRpm -= (GsEc11CntCW + 1) << 5;
                     if (_GlSetMecVelRpm < 0l)  _GlSetMecVelRpm = 0l;
                     GsEc11CntCW                  = 0;
                     OLED_PutNum(0,  OLED_LINE3 + LINE_HEIGHT / 2,  _GlSetMecVelRpm,     5,          6, 1);
                     bScreenValFlg = 1;
                     break;
                
                case EC11_CCW:
                    _GlSetMecVelRpm += (GsEc11CntCCW + 1) << 5;
                     if (_GlSetMecVelRpm > 45000l)  _GlSetMecVelRpm = 45000l; 
                    GsEc11CntCCW                 = 0;
                    OLED_PutNum(0,  OLED_LINE3 + LINE_HEIGHT / 2,  _GlSetMecVelRpm,     5,          6, 1);
                    bScreenValFlg = 1;
                    break;
                
                case KEY_UP: 
                    bScreenIDFlg = 0;
                    bEdit_flag   = 0;
                    break;
                
                case EC11_SET:
                    bScreenIDFlg = 0;
                    bEdit_flag   = 0;
                    break;
                    
                default:
                    break;
                }
    //            OLED_PutNum(0,  OLED_LINE3, 1234,  4, 8, 1);
                if (sPwmDutyValue >= 999)  sPwmDutyValue = 999;
                if (sPwmDutyValue <= 0)      sPwmDutyValue = 0;
                PWM_DUTY_SETA(1000 + sPwmDutyValue);
            }
        }
    }
    GsEc11CntCW = 0;
    GsEc11CntCCW = 0;
    PWM_DUTY_SETA(1000);
}
/**
********************************************************************************************************
** @nameis UpperComBFC68S1
** @effect hall io initilize
** @import none
** @export none
** @return none
** @create yizhi 2023.06.24
** @modify 
*********************************************************************************************************/
void UpperComBFC68S1(uint8_t ucX)
{
    KEYn_e eKeyPress;
    uint8_t ii = 0, uc10msCnt = 0, bFlip = 0;
    INT16U  usVol;
    INT16S  sCur;
    INT8U   GbUartRxReply1 = 0;
    INT8U   GucPolePairs   = 1;
    INT8U   GbSpeedReportEn = 0;
    INT32S  lSetMecVeRpm;
    INT16U  GusRxFromPcCnt = 0;
    INT16U  GusTx2PcCnt    = 0;

    bPwmRunning  = 0;
    bScreenIDFlg = 0;
    bScreenValFlg = 1,
    bEdit_flag   = 0;
    ucEntrysEveryScreen = 4;
    ucEntrysTotal       = 4;
    sPwmDutyValue       = 0;
    _GlSetMecVelRpm      = 0;
    INA226_Init();
    BraoCalibrate();
    
    OLED_Fill(0x00);
    OLED_Print(0, OLED_LINE0, (uint8_t *)"BOREASA C68S1:", 1);
    OLED_PutStr(120, OLED_LINE3, (uint8_t *)"R", 8, 1);
    OLED_PutStr(0,  OLED_LINE3 + LINE_HEIGHT / 2,  "-----", 6, 1);
//    OLED_PutStr(0, OLED_LINE3, (uint8_t *)"UperCmd:", 8, 1);


    /*
     *  PID-Vel 初始化  By HHG 151106    
     *  下面速度环参数， 关于qKi qKd的初始化需要按照速度环的执行周期，如果改变速度环执行周期，需要
     *  做相应的修改 by lgd ，已更正 @ 2019年9月10日。
     */
    _GtPidVelOne.ulQn      = ZMC_QMATH_SIZE;
    _GtPidVelOne.qRef      = 0;
    _GtPidVelOne.qKp       = 1.6f;
    _GtPidVelOne.qUp       = 0;
    _GtPidVelOne.qUi       = 0;
    _GtPidVelOne.qUd       = 0;
    _GtPidVelOne.qOutMax   = 999;
    _GtPidVelOne.qOutMin   = 0;
    _GtPidVelOne.qSatErr   = 0;
    _GtPidVelOne.qKi       = 0.2f;
    _GtPidVelOne.qKc       = 0.2f;
    _GtPidVelOne.qKd       = 0;
    _GtPidVelOne.qUp1      = 0; 
    
    zmcLowPassInit(&_GtLowPassVelOne,  0.032f, 2, 0);
    zmcLowPassInit(&_GtLowPassVelOneX, 0.032f, 1, 0);

    
    while(ADKey_Scan()!=KEY_CANCEL)
    {
        if (GbUartRxDone) {
            if (GucUartRxIndex == 10) {
                ucTemperature = USART_RX_BUF[0];
                ulVoltage10Mv = USART_RX_BUF[1] << 8 | USART_RX_BUF[2];
                lEscEleVel100Rpm        = USART_RX_BUF[7] << 8 | USART_RX_BUF[8];
                GbUartRxDone =  0;                                     /* 消费完成 */                    
                 /* OLED_PutNumber(0 , OLED_LINE1, ulVoltage10Mv / 100.0f, 2, 1, "V",  8, 1);
                    OLED_PutNumber(48, OLED_LINE1, ucTemperature,          2, 0, "℃", 8, 1); */
                LED0_TOG();
                
                ZMC_LOW_PASS(&_GtLowPassVelOne, lEscEleVel100Rpm);                    /* 用作PID运算当前转速 */
                ZMC_LOW_PASS(&_GtLowPassVelOneX, lEscEleVel100Rpm);                   /* 用作显示和上报      */ 
                lEscEleRpmFilter = _GtLowPassVelOne.qOut;
                
                /* 
                 * 在此处进行PID运算  给定值就是 全局变量 输出值 注意此处设定分辨率是 100RPM
                 */
                _GtPidVelOne.qRef = _GlSetMecVelRpm * GucPolePairs / 100;               /* 转换为设定电气转速 */ 
                ZMC_PID(&_GtPidVelOne, lEscEleRpmFilter)
                sPwmDutyValue = _GtPidVelOne.qOut;
                OLED_PutNum(0,  OLED_LINE3, sPwmDutyValue, 5, 6, 1);
                OLED_PutNum(80,  OLED_LINE3,  (INT32S)(_GtLowPassVelOneX.qOut + 0.5f) * (100 / GucPolePairs) ,     5,          8, 1);
                
             } else {
                GbUartRxDone = 0;                                     /* 直接消费完成 */
             }
        } else if (GbUartRxDone1) {
            
            if (GucUartRxIndex1 == 8) {
                lSetMecVeRpm    = USART1_RX_BUF[4] << 8 | USART1_RX_BUF[5];
                GbUartRxDone1 =  0;                                     /* 消费完成 */
                UartAckSpdCmdTxBuf[3] = USART1_RX_BUF[3];
                GusRxFromPcCnt++;                                       /* 电脑数据帧计数    */
                
                switch (USART1_RX_BUF[3]) {
                    
                case 0x01:                                               /* 设定电机转速     */
                    _GlSetMecVelRpm = lSetMecVeRpm;
                    GbUartRxReply1  =  1;                                /* send reply signal */
                    break;
                    
                case 0x02:                                               /* 关机 */
                    _GlSetMecVelRpm = 0;
                    GbUartRxReply1  =  1;                                /* send reply signal */
                    break;
                
                case 0x03:
                    GbSpeedReportEn = 1;                                /* 使能转速回传 */
                    uc10msCnt       = 101;                              /* 转速回传立刻 */
                    break;
                
                case 0x04:
                    GbSpeedReportEn = 0;                               /* 关闭转速回传 */
                    GbUartRxReply1  =  1;                              /* send reply signal */
                    break;
                
                default:
                    break;
                }
                OLED_PutNum(0,  OLED_LINE3 + LINE_HEIGHT / 2,  _GlSetMecVelRpm,     5,          6, 1);
            } else {
                    
                GbUartRxDone1 = 0;                                     /* 直接消费完成 */
            }
        }  else  {
            if (GulPrintTimeCnt > 10) { 
                GulPrintTimeCnt = 0;
                
                if (GbUartRxReply1) {
                    GbUartRxReply1 = 0;
                    uartDrvPutBuf(USART1, UartAckSpdCmdTxBuf, 8);          /* danger 返回速度慢 */ 
                }
                /* uartDrvPutBuf(USART2, UartCMDSpeedTxBuf, 3);    ask for speed frame */ 
                if (uc10msCnt++ > 25) {
                    uc10msCnt = 0;
                    bFlip = !bFlip;
                    
                    if (bFlip) {
                        if (GbSpeedReportEn) {
                            lSetMecVeRpm         = (INT32S)(_GtLowPassVelOneX.qOut + 0.5f);
                            UartAckSpeedTxBuf[4] = (lSetMecVeRpm * (100 / GucPolePairs)) >> 8;
                            UartAckSpeedTxBuf[5] = (lSetMecVeRpm * (100 / GucPolePairs)) & 0x00ff;
                            uartDrvPutBuf(USART1, UartAckSpeedTxBuf, 8);
                            GusTx2PcCnt++;                                                  /* 发送给电脑计数  */ 
                        }
                    } else {
                        INA226_Read(&usVol, &sCur);
                        OLED_PutNumber(0,  OLED_LINE2, usVol / 100.0f, 2, 2, "V", 8, 1);
                        OLED_PutNumber(72, OLED_LINE2, sCur / 1000.0f, 2, 3, "A", 8, 1);
                    }
                } else {
                    OLED_PutNum(40, OLED_LINE3,   GusRxFromPcCnt,              5, 6, 1);                  /* 显示电脑数据帧计数 */
                    OLED_PutNum(40, OLED_LINE3 + LINE_HEIGHT / 2, GusTx2PcCnt, 5, 6, 1);     /* 显示发送计数       */
                }
                eKeyPress = ADKey_Check();
                switch (eKeyPress) {
                    
                case EC11_CW:
                     _GlSetMecVelRpm -= (GsEc11CntCW + 1) << 5;
                     if (_GlSetMecVelRpm < 0l)  _GlSetMecVelRpm = 0l;
                     GsEc11CntCW                  = 0;
                     OLED_PutNum(0,  OLED_LINE3 + LINE_HEIGHT / 2,  _GlSetMecVelRpm,     5,          6, 1);
                     bScreenValFlg = 1;
                     break;
                
                case EC11_CCW:
                    _GlSetMecVelRpm += (GsEc11CntCCW + 1) << 5;
                     if (_GlSetMecVelRpm > 45000l)  _GlSetMecVelRpm = 45000l; 
                    GsEc11CntCCW                 = 0;
                    OLED_PutNum(0,  OLED_LINE3 + LINE_HEIGHT / 2,  _GlSetMecVelRpm,     5,          6, 1);
                    bScreenValFlg = 1;
                    break;
                
                case KEY_UP: 
                    bScreenIDFlg = 0;
                    bEdit_flag   = 0;
                    break;
                
                case EC11_SET:
                    bScreenIDFlg = 0;
                    bEdit_flag   = 0;
                    break;
                    
                default:
                    break;
                }
    //            OLED_PutNum(0,  OLED_LINE3, 1234,  4, 8, 1);
                if (sPwmDutyValue >= 999)  sPwmDutyValue = 999;
                if (sPwmDutyValue <= 0)      sPwmDutyValue = 0;
                PWM_DUTY_SETA(1000 + sPwmDutyValue);
            }
        }
    }
    GsEc11CntCW = 0;
    GsEc11CntCCW = 0;
    PWM_DUTY_SETA(1000);
}

/**
********************************************************************************************************
** @nameis UpperComNidecTF029B
** @effect hall io initilize
** @import none
** @export none
** @return none
** @create yizhi 2023.09.20
** @modify 
*********************************************************************************************************/
void UpperComNidecTF029B(uint8_t ucX)
{
    KEYn_e eKeyPress;
    uint8_t ii = 0, uc10msCnt = 0, bFlip = 0;
    INT16U  usVol;
    INT16S  sCur;
    INT8U   GbUartRxReply1 = 0;
    INT8U   GucPolePairs   = 2;
    INT8U   GbSpeedReportEn = 0;
    INT32S  lSetMecVeRpm;
    INT16U  GusRxFromPcCnt = 0;
    INT16U  GusTx2PcCnt    = 0;

    bPwmRunning  = 0;
    bScreenIDFlg = 0;
    bScreenValFlg = 1,
    bEdit_flag   = 0;
    ucEntrysEveryScreen = 4;
    ucEntrysTotal       = 4;
    sPwmDutyValue       = 0;
    _GlSetMecVelRpm      = 0;
    INA226_Init();
    BraoCalibrate();
    
    OLED_Fill(0x00);
    OLED_Print(0, OLED_LINE0, (uint8_t *)"Nidec TF029B:", 1);
    OLED_PutStr(120, OLED_LINE3, (uint8_t *)"RPM", 8, 1);
//    OLED_PutStr(0, OLED_LINE3, (uint8_t *)"UperCmd:", 8, 1);    


    /*
     *  PID-Vel 初始化  By HHG 151106    
     *  下面速度环参数， 关于qKi qKd的初始化需要按照速度环的执行周期，如果改变速度环执行周期，需要
     *  做相应的修改 by lgd ，已更正 @ 2019年9月10日。
     */
    _GtPidVelOne.ulQn      = ZMC_QMATH_SIZE;
    _GtPidVelOne.qRef      = 0;
    _GtPidVelOne.qKp       = 0.6f;
    _GtPidVelOne.qUp       = 0;
    _GtPidVelOne.qUi       = 0;
    _GtPidVelOne.qUd       = 0;
    _GtPidVelOne.qOutMax   = 999;
    _GtPidVelOne.qOutMin   = 0;
    _GtPidVelOne.qSatErr   = 0;
    _GtPidVelOne.qKi       = 0.1f;
    _GtPidVelOne.qKc       = 0.1f;
    _GtPidVelOne.qKd       = 0;
    _GtPidVelOne.qUp1      = 0; 
    
    zmcLowPassInit(&_GtLowPassVelOne,  0.032f, 2, 0);
    zmcLowPassInit(&_GtLowPassVelOneX, 0.032f, 1, 0);

    
    while(ADKey_Scan()!=KEY_CANCEL)
    {
        if (GbUartRxDone) {
            if (GucUartRxIndex == 10) {
                ucTemperature = USART_RX_BUF[0];
                ulVoltage10Mv = USART_RX_BUF[1] << 8 | USART_RX_BUF[2];
                lEscEleVel100Rpm        = USART_RX_BUF[7] << 8 | USART_RX_BUF[8];
                GbUartRxDone =  0;                                     /* 消费完成 */                    
                 /* OLED_PutNumber(0 , OLED_LINE1, ulVoltage10Mv / 100.0f, 2, 1, "V",  8, 1);
                    OLED_PutNumber(48, OLED_LINE1, ucTemperature,          2, 0, "℃", 8, 1); */
                LED0_TOG();
                
                ZMC_LOW_PASS(&_GtLowPassVelOne, lEscEleVel100Rpm);                    /* 用作PID运算当前转速 */
                ZMC_LOW_PASS(&_GtLowPassVelOneX, lEscEleVel100Rpm);                   /* 用作显示和上报      */ 
                lEscEleRpmFilter = _GtLowPassVelOne.qOut;
                
                /* 
                 * 在此处进行PID运算  给定值就是 全局变量 输出值 注意此处设定分辨率是 100RPM
                 */
                _GtPidVelOne.qRef = _GlSetMecVelRpm * GucPolePairs / 100;               /* 转换为设定电气转速 */ 
                ZMC_PID(&_GtPidVelOne, lEscEleRpmFilter)
                sPwmDutyValue = _GtPidVelOne.qOut;
                OLED_PutNum(0,  OLED_LINE3, sPwmDutyValue, 5, 6, 1);
                OLED_PutNum(0,  OLED_LINE3 + LINE_HEIGHT / 2,  (INT32S)(_GtLowPassVelOneX.qOut + 0.5f) * (100 / GucPolePairs) ,     5,          6, 1);
                
             } else {
                GbUartRxDone = 0;                                     /* 直接消费完成 */
             }
        } else if (GbUartRxDone1) {
            
            if (GucUartRxIndex1 == 8) {
                lSetMecVeRpm    = USART1_RX_BUF[4] << 8 | USART1_RX_BUF[5];
                GbUartRxDone1 =  0;                                     /* 消费完成 */
                UartAckSpdCmdTxBuf[3] = USART1_RX_BUF[3];
                GusRxFromPcCnt++;                                       /* 电脑数据帧计数    */
                
                switch (USART1_RX_BUF[3]) {
                    
                case 0x01:                                               /* 设定电机转速     */
                    _GlSetMecVelRpm = lSetMecVeRpm;
                    GbUartRxReply1  =  1;                                /* send reply signal */
                    break;
                    
                case 0x02:                                               /* 关机 */
                    _GlSetMecVelRpm = 0;
                    GbUartRxReply1  =  1;                                /* send reply signal */
                    break;
                
                case 0x03:
                    GbSpeedReportEn = 1;                                /* 使能转速回传 */
                    uc10msCnt       = 101;                              /* 转速回传立刻 */
                    break;
                
                case 0x04:
                    GbSpeedReportEn = 0;                               /* 关闭转速回传 */
                    GbUartRxReply1  =  1;                              /* send reply signal */
                    break;
                
                default:
                    break;
                }
                OLED_PutNum(80,  OLED_LINE3,  _GlSetMecVelRpm,     5,          8, 1);
            } else {
                    
                GbUartRxDone1 = 0;                                     /* 直接消费完成 */
            }
        }  else  {
            if (GulPrintTimeCnt > 10) { 
                GulPrintTimeCnt = 0;
                
                if (GbUartRxReply1) {
                    GbUartRxReply1 = 0;
                    uartDrvPutBuf(USART1, UartAckSpdCmdTxBuf, 8);          /* danger 返回速度慢 */ 
                }
                /* uartDrvPutBuf(USART2, UartCMDSpeedTxBuf, 3);    ask for speed frame */ 
                if (uc10msCnt++ > 25) {
                    uc10msCnt = 0;
                    bFlip = !bFlip;
                    
                    if (bFlip) {
                        if (GbSpeedReportEn) {
                            lSetMecVeRpm         = (INT32S)(_GtLowPassVelOneX.qOut + 0.5f);
                            UartAckSpeedTxBuf[4] = (lSetMecVeRpm * (100 / GucPolePairs)) >> 8;
                            UartAckSpeedTxBuf[5] = (lSetMecVeRpm * (100 / GucPolePairs)) & 0x00ff;
                            uartDrvPutBuf(USART1, UartAckSpeedTxBuf, 8);
                            GusTx2PcCnt++;                                                  /* 发送给电脑计数  */ 
                        }
                    } else {
                        INA226_Read(&usVol, &sCur);
                        OLED_PutNumber(0,  OLED_LINE2, usVol / 100.0f, 2, 2, "V", 8, 1);
                        OLED_PutNumber(72, OLED_LINE2, sCur / 1000.0f, 2, 3, "A", 8, 1);
                    }
                } else {
                    OLED_PutNum(40, OLED_LINE3,   GusRxFromPcCnt, 5, 6, 1);                  /* 显示电脑数据帧计数 */
                    OLED_PutNum(40, OLED_LINE3 + LINE_HEIGHT / 2, GusTx2PcCnt, 5, 6, 1);     /* 显示发送计数       */
                }
                eKeyPress = ADKey_Check();
                switch (eKeyPress) {
                    
                case EC11_CW:
                     _GlSetMecVelRpm -= (GsEc11CntCW + 1) << 5;
                     if (_GlSetMecVelRpm < 0l)  _GlSetMecVelRpm = 0l;
                     GsEc11CntCW                  = 0;
                     OLED_PutNum(80,  OLED_LINE3,  _GlSetMecVelRpm,     5,          8, 1);
                     bScreenValFlg = 1;
                     break;
                
                case EC11_CCW:
                    _GlSetMecVelRpm += (GsEc11CntCCW + 1) << 5;
                     if (_GlSetMecVelRpm > 50000l)  _GlSetMecVelRpm = 50000l; 
                    GsEc11CntCCW                 = 0;
                    OLED_PutNum(80,  OLED_LINE3,  _GlSetMecVelRpm,     5,          8, 1);
                    bScreenValFlg = 1;
                    break;
                
                case KEY_UP: 
                    bScreenIDFlg = 0;
                    bEdit_flag   = 0;
                    break;
                
                case EC11_SET:
                    bScreenIDFlg = 0;
                    bEdit_flag   = 0;
                    break;
                    
                default:
                    break;
                }
    //            OLED_PutNum(0,  OLED_LINE3, 1234,  4, 8, 1);
                if (sPwmDutyValue >= 999)  sPwmDutyValue = 999;
                if (sPwmDutyValue <= 0)      sPwmDutyValue = 0;
                PWM_DUTY_SETA(1000 + sPwmDutyValue);
            }
        }
    }
    GsEc11CntCW = 0;
    GsEc11CntCCW = 0;
    PWM_DUTY_SETA(1000);
}
/* end of file */
