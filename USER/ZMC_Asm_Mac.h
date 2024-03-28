/*********************************************Copyright (c)***********************************************
**                                Guangzhou ZLG MCU Technology Co., Ltd.
**
**                                        http://www.zlgmcu.com
**
**      ������������Ƭ���Ƽ����޹�˾���ṩ�����з�������ּ��Э���ͻ����ٲ�Ʒ���з����ȣ��ڷ�����������ṩ
**  ���κγ����ĵ������Խ����������֧�ֵ����Ϻ���Ϣ���������ο����ͻ���Ȩ��ʹ�û����вο��޸ģ�����˾��
**  �ṩ�κε������ԡ��ɿ��Եȱ�֤�����ڿͻ�ʹ�ù��������κ�ԭ����ɵ��ر�ġ�żȻ�Ļ��ӵ���ʧ������˾��
**  �е��κ����Ρ�
**                                                                        ����������������Ƭ���Ƽ����޹�˾
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               ZMC_Asm_Mac.h
** Last modified date:      2013-03-11
** Last version:            V1.0
** Descriptions:            This file contains macros used by the IAR and KEIL assemblers.
**                          ���ļ�����IAR��KEIL�����ʹ�õĺ궨�塣    
**
**--------------------------------------------------------------------------------------------------------
** Created by:              Steven Zhou
** Created date:            2013-03-11
** Version:                 V1.0
** Descriptions:            The original version ��ʼ�汾
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
