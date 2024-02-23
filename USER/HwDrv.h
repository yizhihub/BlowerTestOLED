/*********************************************Copyright (c)***********************************************
**                                 Yuwell Technology Co., Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               HwDrv.h
** Last modified Date:      2023-07-25
** Last Version:            1.0
** Description:             主函数文件
**
**--------------------------------------------------------------------------------------------------------
** Created By:              liguagndao
** Created date:            2023-07-25
** Version:                 1.0
** Descriptions:            The original version 初始版本
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#ifndef __HWDRV_H 
#define __HWDRV_H 

#ifdef __cplusplus
extern "C" {
#endif

extern INT32U GulEscReoprtT1msCnt;
extern INT32U GulLapReoprtT1msCnt;
extern INT32U GulPrintTimeCnt;
extern INT32U GulEc11IntervalCnt;


/*********************************************************************************************************
#  LED 相关      GD32F103板子是 PB9  GD32F303板子 PD2
**********************************************************************************************************/
#define LED0 PCout(6)     // PA8
#define LED1 PCout(7)     // PD2
#define LED0_OFF()      LED0 = 1// FM3_GPIO->PDOR8 |=  (1 << 1)                   /* LED 灭                       */
#define LED0_ON()       LED0 = 0//  FM3_GPIO->PDOR8 &= ~(1 << 1)                   /* LED 亮                       */
#define LED0_TOG()      LED0 = !LED0// FM3_GPIO->PDOR8 ^=  (1 << 1)                   /* LED 反转                     */
#define LED1_OFF()      LED1 = 0// FM3_GPIO->PDOR8 |=  (1 << 1)                   /* LED 灭                       */
#define LED1_ON()       LED1 = 1//  FM3_GPIO->PDOR8 &= ~(1 << 1)                   /* LED 亮                       */
#define LED1_TOG()      LED1 = !LED1// FM3_GPIO->PDOR8 ^=  (1 << 1)                   /* LED 反转                     */

/*
 * LED指示结果描述
 * LED_MODE_ON  ： 上电后未进入for循环
 * LED_MODE_SLOW： 电机正常运行
 * LED_MODE_FAST： 电机运行异常
 * LED_SPECIAL_1： 预留
 * LED_SPECIAL_2： 电调未注册指示
 * LED_SPECIAL_3： 预留    */
#define LED_MODE_ON     0                                               /* LED模式，常亮                */
#define LED_MODE_SLOW   1                                               /* LED模式，慢闪 周期1s         */
#define LED_MODE_FAST   2                                               /* LED模式，快闪 周期320ms      */
#define LED_SPECIAL_1   3                                               /* LED模式，闪一次，常灭1秒     */
#define LED_SPECIAL_2   4                                               /* LED模式，闪两次，常灭1秒     */
#define LED_SPECIAL_3   5                                               /* LED模式，闪三次，常灭1秒     */
#define LED_MODE_OFF    6                                               /* LED模式, 常灭               */

#define PWM_DUTY_SETA(usX)   {    \
    TIM_SetCompare1(TIM3, usX);   \
    TIM_SetCompare1(TIM1, usX);   \
    }

#define PWM_PERIOD_SETA(usX)   {    \
    TIM_SetAutoreload(TIM3, usX);   \
    TIM_SetAutoreload(TIM1, usX);   \
    }

//#define BOARD_V1
#define BORAD_V2

void  gpioInit(void);
void  ledInit(void);
void  hwInit(void);
void  timInit(void);
void  msDelay(uint32_t  ulDly);
void  usDelay(uint32_t  ulDly);

#ifdef __cplusplus
}
#endif


#endif


/*********************************************************************************************************
  End Of File 
*********************************************************************************************************/
