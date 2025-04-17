/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               bHMI.h
** Created By  :            YZ
** Created Date:            2023-06-15
** Last Version:            v1.0 
** Descriptions:            C51_PLATEFORM and ARM_PLATEFORM
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
#ifndef _hmi_h
#define _hmi_h

#ifdef ARM_PLATEFORM

#include "sys.h"
#include "bkey.h"
#include "HwDrv.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h" 

#elif defined(C51_PLATEFORM)

#include "common.h"

#else 

#error NO PLATEFORM DEFINED 

#endif


enum pwm_type_data {
    PWM_300HZ = 0,
    PMW_1KHZ,
    PWM_DRONE, 
};

typedef enum pwm_type_data PWM_TYPE_DATA;

    
struct pwm_ctrl_data {
    INT16S sPwmLowVal;    /* 比例 0 ~ 1000 */ 
    INT16S sPwmLowDur;
    INT16S sPwmHighVal;   /* 比例 0 ~ 1000 */  
    INT16S sPwmHighDur;
    INT16S ePwmProtocol;
    INT16S usPwmModulo;
    INT16S usTime10msCnt;
    INT16S sPwmWidthStart;
    INT16S sPwmWidthEnd;
    INT16S sPwmWidthRange;
    INT16S sPwmWidthDflt;
    INT16S sPwmWidthHigh;   /* 实际脉宽时间 */
    INT16S sPwmWidthLow;    /* 实际脉宽时间 */
    INT16S sPwmWidthSetting;
    INT16S sPwmDutyValue;
};
typedef struct pwm_ctrl_data PWM_CTRL_DATA;

union  pwm_ctrl_union { 
    INT16S sPwmCtrl[6];
    PWM_CTRL_DATA tPwmCtrl; 
};
typedef union pwm_ctrl_union PWM_CTRL_UNION;


extern PWM_CTRL_UNION PwmCtrl;


void Menu_Display(void);
void HMI_DrawName(uint8_t ucX, uint8_t ucStart, uint8_t str[], uint16_t ucYn);
void HMI_DrawNumber(uint8_t ucX, uint8_t ucStart, uint16_t usNumber, PWM_CTRL_UNION *ptPwmCtrl,  uint16_t ucYn);
void PWM_ProtoUpdate(PWM_CTRL_DATA * ptPwm, INT16U ePwmProto);
void PWM_KeyCallback(PWM_CTRL_UNION *PwmCtrl, uint8_t ucItem);





#endif

