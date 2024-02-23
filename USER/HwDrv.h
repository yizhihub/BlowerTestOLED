/*********************************************Copyright (c)***********************************************
**                                 Yuwell Technology Co., Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               HwDrv.h
** Last modified Date:      2023-07-25
** Last Version:            1.0
** Description:             �������ļ�
**
**--------------------------------------------------------------------------------------------------------
** Created By:              liguagndao
** Created date:            2023-07-25
** Version:                 1.0
** Descriptions:            The original version ��ʼ�汾
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
#  LED ���      GD32F103������ PB9  GD32F303���� PD2
**********************************************************************************************************/
#define LED0 PCout(6)     // PA8
#define LED1 PCout(7)     // PD2
#define LED0_OFF()      LED0 = 1// FM3_GPIO->PDOR8 |=  (1 << 1)                   /* LED ��                       */
#define LED0_ON()       LED0 = 0//  FM3_GPIO->PDOR8 &= ~(1 << 1)                   /* LED ��                       */
#define LED0_TOG()      LED0 = !LED0// FM3_GPIO->PDOR8 ^=  (1 << 1)                   /* LED ��ת                     */
#define LED1_OFF()      LED1 = 0// FM3_GPIO->PDOR8 |=  (1 << 1)                   /* LED ��                       */
#define LED1_ON()       LED1 = 1//  FM3_GPIO->PDOR8 &= ~(1 << 1)                   /* LED ��                       */
#define LED1_TOG()      LED1 = !LED1// FM3_GPIO->PDOR8 ^=  (1 << 1)                   /* LED ��ת                     */

/*
 * LEDָʾ�������
 * LED_MODE_ON  �� �ϵ��δ����forѭ��
 * LED_MODE_SLOW�� �����������
 * LED_MODE_FAST�� ��������쳣
 * LED_SPECIAL_1�� Ԥ��
 * LED_SPECIAL_2�� ���δע��ָʾ
 * LED_SPECIAL_3�� Ԥ��    */
#define LED_MODE_ON     0                                               /* LEDģʽ������                */
#define LED_MODE_SLOW   1                                               /* LEDģʽ������ ����1s         */
#define LED_MODE_FAST   2                                               /* LEDģʽ������ ����320ms      */
#define LED_SPECIAL_1   3                                               /* LEDģʽ����һ�Σ�����1��     */
#define LED_SPECIAL_2   4                                               /* LEDģʽ�������Σ�����1��     */
#define LED_SPECIAL_3   5                                               /* LEDģʽ�������Σ�����1��     */
#define LED_MODE_OFF    6                                               /* LEDģʽ, ����               */

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
