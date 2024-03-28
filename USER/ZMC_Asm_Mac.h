/*********************************************Copyright (c)***********************************************
**                                Guangzhou ZLG MCU Technology Co., Ltd.
**
**                                        http://www.zlgmcu.com
**
**      广州周立功单片机科技有限公司所提供的所有服务内容旨在协助客户加速产品的研发进度，在服务过程中所提供
**  的任何程序、文档、测试结果、方案、支持等资料和信息，都仅供参考，客户有权不使用或自行参考修改，本公司不
**  提供任何的完整性、可靠性等保证，若在客户使用过程中因任何原因造成的特别的、偶然的或间接的损失，本公司不
**  承担任何责任。
**                                                                        ——广州周立功单片机科技有限公司
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               ZMC_Asm_Mac.h
** Last modified date:      2013-03-11
** Last version:            V1.0
** Descriptions:            This file contains macros used by the IAR and KEIL assemblers.
**                          本文件包含IAR和KEIL汇编器使用的宏定义。    
**
**--------------------------------------------------------------------------------------------------------
** Created by:              Steven Zhou
** Created date:            2013-03-11
** Version:                 V1.0
** Descriptions:            The original version 初始版本
**--------------------------------------------------------------------------------------------------------
** Modified by:        
** Modified date:      
** Version:            
** Descriptions:       
**                      
*********************************************************************************************************/

#ifndef   __ZMC_ASM_MAC_H
#define   __ZMC_ASM_MAC_H   1


#ifdef __IASMARM__                                                      /* IAR assembler                */
    #define __ZMC_ASM_IMPORT(sLabel)             EXTERN sLabel
    #define __ZMC_ASM_EXPORT(sLabel)               PUBLIC sLabel

    #define __ZMC_ASM_CODE_SECTION(sLabel)         RSEG sLabel: CODE: NOROOT(4)
    
    #define __ZMC_ASM_EQUATE(sLabel, ulValue)   sLabel  EQU  ulValue

    #define __ZMC_ASM_STACK_ALIGN()              
    #define __ZMC_ASM_FILE_END()                END
    #define __ZMC_ASM_SECTION(sLable)              sLable 
    #define __ZMC_ASM_CODE_ALIGN(ucValue)          ALIGNROM ucValue

    #ifdef __ARMVFP__ 
    #define __ZMC_ASM_ARM_FPU                   1u         
    #endif

#elif defined (__CC_ARM)                                                /* KEIL assembler               */
    #define __ZMC_ASM_IMPORT(sLabel)             EXTERN sLabel
    #define __ZMC_ASM_EXPORT(sLabel)               EXPORT sLabel

    #define __ZMC_ASM_CODE_SECTION(sLabel)      AREA    sLabel, CODE, READONLY
    
    #define __ZMC_ASM_EQUATE(sLabel, ulValue)   sLabel  EQU  ulValue

    #define __ZMC_ASM_STACK_ALIGN()                  PRESERVE8
    #define __ZMC_ASM_FILE_END()                END
    #define __ZMC_ASM_SECTION(sLable)              sLable 
    #define __ZMC_ASM_CODE_ALIGN(ucValue)          ALIGN ucValue
 
    #ifdef __TARGET_FPU_VFP 
    #define __ZMC_ASM_FPU                       1u    
    #endif

#else                                                                   /* Compiler not defined         */
   #error  "Compiler not defined!"


#endif                                                                  /* #ifdef __IASMARM__           */


#endif                                                                  /* __ZMC_ASM_MAC_H              */


/*********************************************************************************************************
  End of File
*********************************************************************************************************/
