;*********************************************Copyright (c)***********************************************
;*                                Guangzhou ZLG MCU Technology Co., Ltd.
;*
;*                                        http://www.zlgmcu.com
;*
;*      ������������Ƭ���Ƽ����޹�˾���ṩ�����з�������ּ��Э���ͻ����ٲ�Ʒ���з����ȣ��ڷ�����������ṩ
;*  ���κγ����ĵ������Խ����������֧�ֵ����Ϻ���Ϣ���������ο����ͻ���Ȩ��ʹ�û����вο��޸ģ�����˾��
;*  �ṩ�κε������ԡ��ɿ��Եȱ�֤�����ڿͻ�ʹ�ù��������κ�ԭ����ɵ��ر�ġ�żȻ�Ļ��ӵ���ʧ������˾��
;*  �е��κ����Ρ�
;*                                                                        ����������������Ƭ���Ƽ����޹�˾
;*
;*--------------File Info---------------------------------------------------------------------------------
;* File name:               ZMC_QMath_CM.s
;* Last modified date:      2013-11-10
;* Last version:            V1.1
;* Descriptions:            The assembly QMath functions that Cortex-M requires. 
;*                          Cortex-M3����Ҫ��QMath��ຯ���� 
;*                         
;*--------------------------------------------------------------------------------------------------------
;* Created by:              Steven Zhou
;* Created date:            2013-03-11
;* Version:                 V1.0
;* Descriptions:            The original version ��ʼ�汾
;*--------------------------------------------------------------------------------------------------------
;* Modified by:             Fan Linyun
;* Modified date:           2013-11-10
;* Version:                 V1.1
;* Descriptions:            ���ӳ�������
;*                      
;*********************************************************************************************************

#include "ZMC_Asm_Mac.h"

;*********************************************************************************************************
;  Declarations for the exported functions             
;  ����ⲿ����                          
;*********************************************************************************************************
#ifdef __ZMC_ASM_CORTEX_M4
    __ZMC_ASM_EXPORT(zmcQFloatSqrt)
#endif   
    
    __ZMC_ASM_EXPORT(zmcQnDiv)
    
             
    __ZMC_ASM_STACK_ALIGN()                                                ; 8-byte alignment of the stack
    __ZMC_ASM_CODE_SECTION(ZMC_QMath_CM)
    __ZMC_ASM_SECTION(THUMB)                                            ; Set to Thumb instruction seg.


;*********************************************************************************************************
;** Function name:      zmcQFloatSqrt
;** Descriptions:        Floating-point Square Root.
;** Input parameters:    S0:
;** Output parameters:    None ��
;** Returned value:        S0
;** Created by:            Steven Zhou ���ܸ�
;** Created Date:        2011.10.31
;**-------------------------------------------------------------------------------------------------------
;** Modified by:         
;** Modified date:       
;**-------------------------------------------------------------------------------------------------------
;*********************************************************************************************************
#ifdef __ZMC_ASM_CORTEX_M4
zmcQFloatSqrt
    VSQRT.F32  S0, S0
    BX         LR
#endif


;*********************************************************************************************************
;** Function name:      zmcQnDiv
;** Descriptions:       Divides two Q numbers, without rounding
;                       ZMC_Q(qA/qB) = ((qA << na) / (qB >> nb)) << nc
;                       na, nb, nc is unfix var.
;** Input parameters:   R0: Hold the value to be divided
;**                     R1: Hold the value to divide
;**                     R2: Hold the value of Qn format 
;** Output parameters:  None ��
;** Returned value:     R0 = (R0 / R1) << Qn, Qn format
;** Created by:         Fan Linyun
;** Created Date:       2013.06.24
;**-------------------------------------------------------------------------------------------------------
;** Modified by:        2013.11.15
;** Modified date:      ���Ӻ�����ڲ��� Qn
;**-------------------------------------------------------------------------------------------------------
;*********************************************************************************************************
zmcQnDiv
    PUSH    {R3, R4}
    EOR.W   R4, R1, R0  ; ����ͬ������
    
    CMP     R0, #0      ; R0 = |R0|
    IT      MI
    RSBMI   R0, R0, #0
    
    IT      EQ          ; R1 == 0ʱ����Ϊ0
    BEQ    __ZMC_DIV_END
    
    CMP     R1, #0      ; R1 = |R1|
    IT      MI
    RSBMI   R1, R1, #0
    
    ITT     EQ          ; R1 == 0ʱ��R0 = 0x7FFFFFFF 
    MOVEQ   R0, #0x7FFFFFFF
    BEQ   __ZMC_DIV_SIGN
    
    CLZ     R3, R0      ; R3: R0 ǰ��0�ĸ���
    
    CMP     R3, R2
    BLT     __ZMC_NFA_LT
    LSL     R0, R0, R2
    MOV     R3, #0
    B       __ZMC_NA_DIV_NB
__ZMC_NFA_LT
    LSL     R0, R0, R3      ; R0:����Ϊ 31λ
    RSB     R3, R3, R2      ; R3: ����Ҫ�����λ��
    
    CLZ     R2, R1          ; R2: R1 ǰ��0�ĸ���
    RSB     R2, R2, #32     ; R2: R1��Чλ��
    
    CMP     R2, #16
    BLT     __ZMC_NA_DIV_NB
    LSR     R2, R3, #1      ; R1 = R1 << (R3 / 2)
    LSR     R1, R1, R2
    SUB     R3, R3, R2
__ZMC_NA_DIV_NB
    UDIV    R0, R0, R1
    
    CLZ     R1, R0          ; ��������
    CMP     R1, R3  
    ITE     GT
    LSLGT   R0, R0, R3
    MOVLE   R0, #0x7FFFFFFF
__ZMC_DIV_SIGN
    CMP     R4, #0          ; ���ϸ���
    IT      MI
    RSBMI   R0, R0, #0
__ZMC_DIV_END
    POP     {R3, R4}
    BX      LR
    

    __ZMC_ASM_CODE_ALIGN(4)
    __ZMC_ASM_FILE_END() 


;*********************************************************************************************************
;  End of File 
;*********************************************************************************************************
