#ifndef _bBraoAdc_H
#define _bBraoAdc_H

/*
 * slection between arm or 51 for const data 
 */
#ifdef ARM_PLATEFORM
#define CONST_DATA  const
#define ___NOP()     __asm("NOP")
#else
#define CONST_DATA  code
#define ___NOP()     _nop_()
#endif

/*
 * slection platfrom header files 
 */
#if defined(STM32F10X_HD) || defined(GD32F30X_HD)
    #include "sys.h"
    #include "delay.h"
    #include "HwDrv.h"
    #include "stm32f10x_gpio.h"
    #include "stm32f10x_exti.h"
    #include "stm32f10x_adc.h"
#elif defined(STM32F10X_MD)
    #include "ZMD_Includes.h"
#elif defined(RT1052)
    #include "fsl_common.h"
    #include "fsl_snvs_hp.h"
#elif defined(LKS32MC08x)
    #include "basic.h"
    #include "hardware_config.h"
#else
    #include "common.h"
#endif

extern INT16S _GsBraoAdcOffset;

void  BraoAdcInit(void);
void  BraoAdcGet(int16_t *psResult);
void  BraoCalibrate(void);
float BraoGet(void);
void  BraoCalibration(uint8_t ucX);

#endif 
