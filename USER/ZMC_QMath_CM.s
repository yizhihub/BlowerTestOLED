;*********************************************Copyright (c)***********************************************
;*                                Guangzhou ZLG MCU Technology Co., Ltd.
;*
;*                                        http://www.zlgmcu.com
;*
;*      广州周立功单片机科技有限公司所提供的所有服务内容旨在协助客户加速产品的研发进度，在服务过程中所提供
;*  的任何程序、文档、测试结果、方案、支持等资料和信息，都仅供参考，客户有权不使用或自行参考修改，本公司不
;*  提供任何的完整性、可靠性等保证，若在客户使用过程中因任何原因造成的特别的、偶然的或间接的损失，本公司不
;*  承担任何责任。
;*                                                                        ——广州周立功单片机科技有限公司
;*
;*--------------File Info---------------------------------------------------------------------------------
;* File name:               ZMC_QMath_CM.s
;* Last modified date:      2013-11-10
;* Last version:            V1.1
;* Descriptions:            The assembly QMath functions that Cortex-M requires. 
;*                          Cortex-M3所需要的QMath汇编函数。 
;*                         
;*--------------------------------------------------------------------------------------------------------
;* Created by:              Steven Zhou
;* Created date:            2013-03-11
;* Version:                 V1.0
;* Descriptions:            The original version 初始版本
;*--------------------------------------------------------------------------------------------------------
;* Modified by:             Fan Linyun
;* Modified date:           2013-11-10
;* Version:                 V1.1
;* Descriptions:            增加除法函数
;*                      
;*********************************************************************************************************

#include "ZMC_Asm_Mac.h"

;*********************************************************************************************************
;  Declarations for the exported functions             
;  输出外部声明                          
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
;** Output parameters:    None 无
;** Returned value:        S0
;** Created by:            Steven Zhou 周绍刚
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
;** Output parameters:  None 无
;** Returned value:     R0 = (R0 / R1) << Qn, Qn format
;** Created by:         Fan Linyun
;** Created Date:       2013.06.24
;**-------------------------------------------------------------------------------------------------------
;** Modified by:        2013.11.15
;** Modified date:      增加函数入口参数 Qn
;**-------------------------------------------------------------------------------------------------------
;*********************************************************************************************************
zmcQnDiv
    PUSH    {R3, R4}
    EOR.W   R4, R1, R0  ; 与商同正负号
    
    CMP     R0, #0      ; R0 = |R0|
    IT      MI
    RSBMI   R0, R0, #0
    
    IT      EQ          ; R1 == 0时，商为0
    BEQ    __ZMC_DIV_END
    
    CMP     R1, #0      ; R1 = |R1|
    IT      MI
    RSBMI   R1, R1, #0
    
    ITT     EQ          ; R1 == 0时，R0 = 0x7FFFFFFF 
    MOVEQ   R0, #0x7FFFFFFF
    BEQ   __ZMC_DIV_SIGN
    
    CLZ     R3, R0      ; R3: R0 前导0的个数
    
    CMP     R3, R2
    BLT     __ZMC_NFA_LT
    LSL     R0, R0, R2
    MOV     R3, #0
    B       __ZMC_NA_DIV_NB
__ZMC_NFA_LT
    LSL     R0, R0, R3      ; R0:扩充为 31位
    RSB     R3, R3, R2      ; R3: 商需要扩充的位数
    
    CLZ     R2, R1          ; R2: R1 前导0的个数
    RSB     R2, R2, #32     ; R2: R1有效位数
    
    CMP     R2, #16
    BLT     __ZMC_NA_DIV_NB
    LSR     R2, R3, #1      ; R1 = R1 << (R3 / 2)
    LSR     R1, R1, R2
    SUB     R3, R3, R2
__ZMC_NA_DIV_NB
    UDIV    R0, R0, R1
    
    CLZ     R1, R0          ; 饱合运算
    CMP     R1, R3  
    ITE     GT
    LSLGT   R0, R0, R3
    MOVLE   R0, #0x7FFFFFFF
__ZMC_DIV_SIGN
    CMP     R4, #0          ; 加上负号
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
