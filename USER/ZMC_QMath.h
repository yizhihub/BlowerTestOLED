/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               ZMC_QMath.h
** Last modified Date:      2012.01.05
** Last Version:            1.0
** Description:             The header file of QMath. QMath的头文件。
** 
**--------------------------------------------------------------------------------------------------------
** Created By:              Steven Zhou 周绍刚
** Created date:            2012.01.05
** Version:                 1.0
** Descriptions:            The original version 初始版本
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Description:             
**
*********************************************************************************************************/

#ifndef __ZMC_QMATH_H
#define __ZMC_QMATH_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include <math.h>
#include "ZMC_QMath_Cfg.h"
#include "sys.h"
      
      
/*********************************************************************************************************
  Constants definitions  
  常量定义。
*********************************************************************************************************/
#define  ZMC_QMATH_32BIT    0u
#define  ZMC_QMATH_16BIT    1u  
#define  ZMC_QMATH_FLOAT    2u  
  
#define  ZMC_FPI            (3.1415926535897932384626433832795f)
#define  ZMC_QPI            ( ZMC_Q(3.1415926535897932384626433832795) )  

  
/*********************************************************************************************************
  Congfig file Checking 
  配置文件检查
*********************************************************************************************************/
#if (ZMC_QMATH_TYPE != ZMC_QMATH_32BIT) && \
    (ZMC_QMATH_TYPE != ZMC_QMATH_16BIT) && \
    (ZMC_QMATH_TYPE != ZMC_QMATH_FLOAT) 
#error "ZMC_QMath_Cfg.h, ZMC_QMATH_TYPE setting error"
#endif

#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT)                                 /* 32bit fixed-point            */
#if (ZMC_QMATH_SIZE < 0u) || (ZMC_QMATH_SIZE > 30u) 
#error "ZMC_QMath_Cfg.h, for 32bit fixed-point, 0u <= ZMC_QMATH_SIZE <= 30u"
#endif
#endif

#if (ZMC_QMATH_TYPE == ZMC_QMATH_16BIT)                                 /* 16bit fixed-point            */
#if (ZMC_QMATH_SIZE < 0u) || (ZMC_QMATH_SIZE > 15u) 
#error "ZMC_QMath_Cfg.h, for 16bit fixed-point, 0u <= ZMC_QMATH_SIZE <= 15u"
#endif
#endif
      
      
/*********************************************************************************************************
  Keywords for compilers
  编译器的关键字
*********************************************************************************************************/  
#if  defined ( __CC_ARM )
    #define ZMC_INLINE         __inline                                 /* inline keyword for Keil      */

#elif defined ( __ICCARM__ )
    #define ZMC_INLINE         inline                                   /* inline keyword for IAR       */
                                                                        /* Only available in High opti. */
#endif  

  
/*********************************************************************************************************
  Date types(Compiler specific)  数据类型（和编译器相关）                
*********************************************************************************************************/
#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT)                                 /* 32bit fixed-point            */
typedef   signed  long    ZQ;
typedef   signed  long    ZQ30;
typedef   signed  long    ZQ29;
typedef   signed  long    ZQ28;
typedef   signed  long    ZQ27;
typedef   signed  long    ZQ26;
typedef   signed  long    ZQ25;
typedef   signed  long    ZQ24;
typedef   signed  long    ZQ23;
typedef   signed  long    ZQ22;
typedef   signed  long    ZQ21;
typedef   signed  long    ZQ20;
typedef   signed  long    ZQ19;
typedef   signed  long    ZQ18;
typedef   signed  long    ZQ17;
typedef   signed  long    ZQ16;
typedef   signed  long    ZQ15;
typedef   signed  long    ZQ14;
typedef   signed  long    ZQ13;
typedef   signed  long    ZQ12;
typedef   signed  long    ZQ11;
typedef   signed  long    ZQ10;
typedef   signed  long    ZQ9;
typedef   signed  long    ZQ8;
typedef   signed  long    ZQ7;
typedef   signed  long    ZQ6;
typedef   signed  long    ZQ5;
typedef   signed  long    ZQ4;
typedef   signed  long    ZQ3;
typedef   signed  long    ZQ2;
typedef   signed  long    ZQ1;
typedef   signed  long    ZQ0;

#elif (ZMC_QMATH_TYPE == ZMC_QMATH_16BIT)                               /* 16bit fixed-point            */
typedef   signed  short   ZQ;
typedef   signed  short   ZQ15;
typedef   signed  short   ZQ14;
typedef   signed  short   ZQ13;
typedef   signed  short   ZQ12;
typedef   signed  short   ZQ11;
typedef   signed  short   ZQ10;
typedef   signed  short   ZQ9;
typedef   signed  short   ZQ8;
typedef   signed  short   ZQ7;
typedef   signed  short   ZQ6;
typedef   signed  short   ZQ5;
typedef   signed  short   ZQ4;
typedef   signed  short   ZQ3;
typedef   signed  short   ZQ2;
typedef   signed  short   ZQ1;
typedef   signed  short   ZQ0;

#elif (ZMC_QMATH_TYPE == ZMC_QMATH_FLOAT)                               /* floating-point               */
typedef    float   ZQ;
typedef    float   ZQ30;
typedef    float   ZQ29;
typedef    float   ZQ28;
typedef    float   ZQ27;
typedef    float   ZQ26;
typedef    float   ZQ25;
typedef    float   ZQ24;
typedef    float   ZQ23;
typedef    float   ZQ22;
typedef    float   ZQ21;
typedef    float   ZQ20;
typedef    float   ZQ19;
typedef    float   ZQ18;
typedef    float   ZQ17;
typedef    float   ZQ16;
typedef    float   ZQ15;
typedef    float   ZQ14;
typedef    float   ZQ13;
typedef    float   ZQ12;
typedef    float   ZQ11;
typedef    float   ZQ10;
typedef    float   ZQ9;
typedef    float   ZQ8;
typedef    float   ZQ7;
typedef    float   ZQ6;
typedef    float   ZQ5;
typedef    float   ZQ4;
typedef    float   ZQ3;
typedef    float   ZQ2;
typedef    float   ZQ1;
typedef    float   ZQ0;
#endif


/*********************************************************************************************************
  Convert a value into Q format. 
  将一个值转成Q格式。                 
*********************************************************************************************************/
#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT)                                 /* 32bit fixed-point            */
#define ZMC_Q30(fA)             ( (ZQ30)((fA) * (1 << 30u)) )
#define ZMC_Q29(fA)             ( (ZQ29)((fA) * (1 << 29u)) )
#define ZMC_Q28(fA)             ( (ZQ28)((fA) * (1 << 28u)) )
#define ZMC_Q27(fA)             ( (ZQ27)((fA) * (1 << 27u)) )
#define ZMC_Q26(fA)             ( (ZQ26)((fA) * (1 << 26u)) )
#define ZMC_Q25(fA)             ( (ZQ25)((fA) * (1 << 25u)) )
#define ZMC_Q24(fA)             ( (ZQ24)((fA) * (1 << 24u)) )
#define ZMC_Q23(fA)             ( (ZQ23)((fA) * (1 << 23u)) )
#define ZMC_Q22(fA)             ( (ZQ22)((fA) * (1 << 22u)) )
#define ZMC_Q21(fA)             ( (ZQ21)((fA) * (1 << 21u)) )
#define ZMC_Q20(fA)             ( (ZQ20)((fA) * (1 << 20u)) )
#define ZMC_Q19(fA)             ( (ZQ19)((fA) * (1 << 19u)) )
#define ZMC_Q18(fA)             ( (ZQ18)((fA) * (1 << 18u)) )
#define ZMC_Q17(fA)             ( (ZQ17)((fA) * (1 << 17u)) )
#define ZMC_Q16(fA)             ( (ZQ16)((fA) * (1 << 16u)) )
#endif

#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT) || (ZMC_QMATH_TYPE == ZMC_QMATH_16BIT)                  
#define ZMC_Q15(fA)             ( (ZQ15)((fA) * (1 << 15u)) )          /* 32bit and 16bit fixed-point  */
#define ZMC_Q14(fA)             ( (ZQ14)((fA) * (1 << 14u)) )
#define ZMC_Q13(fA)             ( (ZQ13)((fA) * (1 << 13u)) )
#define ZMC_Q12(fA)             ( (ZQ12)((fA) * (1 << 12u)) )
#define ZMC_Q11(fA)             ( (ZQ11)((fA) * (1 << 11u)) )
#define ZMC_Q10(fA)             ( (ZQ10)((fA) * (1 << 10u)) )
#define ZMC_Q9(fA)              ( (ZQ9) ((fA) * (1 << 9u))  )
#define ZMC_Q8(fA)              ( (ZQ8) ((fA) * (1 << 8u))  )
#define ZMC_Q7(fA)              ( (ZQ7) ((fA) * (1 << 7u))  )
#define ZMC_Q6(fA)              ( (ZQ6) ((fA) * (1 << 6u))  )
#define ZMC_Q5(fA)              ( (ZQ5) ((fA) * (1 << 5u))  )
#define ZMC_Q4(fA)              ( (ZQ4) ((fA) * (1 << 4u))  )
#define ZMC_Q3(fA)              ( (ZQ3) ((fA) * (1 << 3u))  )
#define ZMC_Q2(fA)              ( (ZQ2) ((fA) * (1 << 2u))  )
#define ZMC_Q1(fA)              ( (ZQ1) ((fA) * (1 << 1u))  )
#define ZMC_Q0(fA)              ( (ZQ0) (fA)                 )
#endif

#if (ZMC_QMATH_TYPE == ZMC_QMATH_FLOAT)                                 /* floating-point               */
#define ZMC_Q30(fA)             (fA)
#define ZMC_Q29(fA)             (fA)
#define ZMC_Q28(fA)             (fA)
#define ZMC_Q27(fA)             (fA)
#define ZMC_Q26(fA)             (fA)
#define ZMC_Q25(fA)             (fA)
#define ZMC_Q24(fA)             (fA)
#define ZMC_Q23(fA)             (fA)
#define ZMC_Q22(fA)             (fA)
#define ZMC_Q21(fA)             (fA)
#define ZMC_Q20(fA)             (fA)
#define ZMC_Q19(fA)             (fA)
#define ZMC_Q18(fA)             (fA)
#define ZMC_Q17(fA)             (fA)
#define ZMC_Q16(fA)             (fA)
#define ZMC_Q15(fA)             (fA)
#define ZMC_Q14(fA)             (fA)
#define ZMC_Q13(fA)             (fA)
#define ZMC_Q12(fA)             (fA)
#define ZMC_Q11(fA)             (fA)
#define ZMC_Q10(fA)             (fA)
#define ZMC_Q9(fA)              (fA)
#define ZMC_Q8(fA)              (fA)
#define ZMC_Q7(fA)              (fA)
#define ZMC_Q6(fA)              (fA)
#define ZMC_Q5(fA)              (fA)
#define ZMC_Q4(fA)              (fA)
#define ZMC_Q3(fA)              (fA)
#define ZMC_Q2(fA)              (fA)
#define ZMC_Q1(fA)              (fA)
#define ZMC_Q0(fA)              (fA)
#endif

#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT) || (ZMC_QMATH_TYPE == ZMC_QMATH_FLOAT)              
#if (ZMC_QMATH_SIZE == 30)                                              /* 32bit fixed-point & floating */
#define ZMC_Q(fA)               ZMC_Q30(fA)
#endif
#if (ZMC_QMATH_SIZE == 29)
#define ZMC_Q(fA)               ZMC_Q29(fA)
#endif
#if (ZMC_QMATH_SIZE == 28)
#define ZMC_Q(fA)               ZMC_Q28(fA)
#endif
#if (ZMC_QMATH_SIZE == 27)
#define ZMC_Q(fA)               ZMC_Q27(fA)
#endif
#if (ZMC_QMATH_SIZE == 26)
#define ZMC_Q(fA)               ZMC_Q26(fA)
#endif
#if (ZMC_QMATH_SIZE == 25)
#define ZMC_Q(fA)               ZMC_Q25(fA)
#endif
#if (ZMC_QMATH_SIZE == 24)
#define ZMC_Q(fA)               ZMC_Q24(fA)
#endif
#if (ZMC_QMATH_SIZE == 23)
#define ZMC_Q(fA)               ZMC_Q23(fA)
#endif
#if (ZMC_QMATH_SIZE == 22)
#define ZMC_Q(fA)               ZMC_Q22(fA)
#endif
#if (ZMC_QMATH_SIZE == 21)
#define ZMC_Q(fA)               ZMC_Q21(fA)
#endif
#if (ZMC_QMATH_SIZE == 20)
#define ZMC_Q(fA)               ZMC_Q20(fA)
#endif
#if (ZMC_QMATH_SIZE == 19)
#define ZMC_Q(fA)               ZMC_Q19(fA)
#endif
#if (ZMC_QMATH_SIZE == 18)
#define ZMC_Q(fA)               ZMC_Q18(fA)
#endif
#if (ZMC_QMATH_SIZE == 17)
#define ZMC_Q(fA)               ZMC_Q17(fA)
#endif
#if (ZMC_QMATH_SIZE == 16)
#define ZMC_Q(fA)               ZMC_Q16(fA)
#endif
#endif                                                                  /* 32bit fixed-point & floating */

#if (ZMC_QMATH_SIZE == 15)                                              /* All types                    */
#define ZMC_Q(fA)               ZMC_Q15(fA)
#endif
#if (ZMC_QMATH_SIZE == 14)
#define ZMC_Q(fA)               ZMC_Q14(fA)
#endif
#if (ZMC_QMATH_SIZE == 13)
#define ZMC_Q(fA)               ZMC_Q13(fA)
#endif
#if (ZMC_QMATH_SIZE == 12)
#define ZMC_Q(fA)               ZMC_Q12(fA)
#endif
#if (ZMC_QMATH_SIZE == 11)
#define ZMC_Q(fA)               ZMC_Q11(fA)
#endif
#if (ZMC_QMATH_SIZE == 10)
#define ZMC_Q(fA)               ZMC_Q10(fA)
#endif
#if (ZMC_QMATH_SIZE == 9)
#define ZMC_Q(fA)               ZMC_Q9(fA)
#endif
#if (ZMC_QMATH_SIZE == 8)
#define ZMC_Q(fA)               ZMC_Q8(fA)
#endif
#if (ZMC_QMATH_SIZE == 7)
#define ZMC_Q(fA)               ZMC_Q7(fA)
#endif
#if (ZMC_QMATH_SIZE == 6)
#define ZMC_Q(fA)               ZMC_Q6(fA)
#endif
#if (ZMC_QMATH_SIZE == 5)
#define ZMC_Q(fA)               ZMC_Q5(fA)
#endif
#if (ZMC_QMATH_SIZE == 4)
#define ZMC_Q(fA)               ZMC_Q4(fA)
#endif
#if (ZMC_QMATH_SIZE == 3)
#define ZMC_Q(fA)               ZMC_Q3(fA)
#endif
#if (ZMC_QMATH_SIZE == 2)
#define ZMC_Q(fA)               ZMC_Q2(fA)
#endif
#if (ZMC_QMATH_SIZE == 1)
#define ZMC_Q(fA)               ZMC_Q1(fA)
#endif  


/*********************************************************************************************************
  Converts a value between the global Q format and a specified Q format.
  将一个数值在全局Q格式和指定Q格式之间转换。
*********************************************************************************************************/  
#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT)                                 /* 32bit fixed-point            */
#define   ZMC_Q_Q30(qA)     ((ZQ30)(qA) << (30 - ZMC_QMATH_SIZE))
#define   ZMC_Q30_Q(q30A)   ((ZQ)(q30A) >> (30 - ZMC_QMATH_SIZE))

#if (ZMC_QMATH_SIZE >= 29u)
#define   ZMC_Q_Q29(qA)     ((ZQ29)(qA) >> (ZMC_QMATH_SIZE - 29u))
#define   ZMC_Q29_Q(q29A)   ((ZQ)(q29A) << (ZMC_QMATH_SIZE - 29u))
#else
#define   ZMC_Q_Q29(qA)     ((ZQ29)(qA) << (29u - ZMC_QMATH_SIZE))
#define   ZMC_Q29_Q(q29A)   ((ZQ)(q29A) >> (29u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 28u)
#define   ZMC_Q_Q28(qA)     ((ZQ28)(qA) >> (ZMC_QMATH_SIZE - 28u))
#define   ZMC_Q28_Q(q28A)   ((ZQ)(q28A) << (ZMC_QMATH_SIZE - 28u))
#else
#define   ZMC_Q_Q28(qA)     ((ZQ28)(qA) << (28u - ZMC_QMATH_SIZE))
#define   ZMC_Q28_Q(q28A)   ((ZQ)(q28A) >> (28u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 27u)
#define   ZMC_Q_Q27(qA)     ((ZQ27)(qA) >> (ZMC_QMATH_SIZE - 27u))
#define   ZMC_Q27_Q(q27A)   ((ZQ)(q27A) << (ZMC_QMATH_SIZE - 27u))
#else
#define   ZMC_Q_Q27(qA)     ((ZQ27)(qA) << (27u - ZMC_QMATH_SIZE))
#define   ZMC_Q27_Q(q27A)   ((ZQ)(q27A) >> (27u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 26u)
#define   ZMC_Q_Q26(qA)     ((ZQ26)(qA) >> (ZMC_QMATH_SIZE - 26u))
#define   ZMC_Q26_Q(q26A)   ((ZQ)(q26A) << (ZMC_QMATH_SIZE - 26u))
#else
#define   ZMC_Q_Q26(qA)     ((ZQ26)(qA) << (26u - ZMC_QMATH_SIZE))
#define   ZMC_Q26_Q(q26A)   ((ZQ)(q26A) >> (26u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 25u)
#define   ZMC_Q_Q25(qA)     ((ZQ25)(qA) >> (ZMC_QMATH_SIZE - 25u))
#define   ZMC_Q25_Q(q25A)   ((ZQ)(q25A) << (ZMC_QMATH_SIZE - 25u))
#else
#define   ZMC_Q_Q25(qA)     ((ZQ25)(qA) << (25u - ZMC_QMATH_SIZE))
#define   ZMC_Q25_Q(q25A)   ((ZQ)(q25A) >> (25u - ZMC_QMATH_SIZE))
#endif 
  
#if (ZMC_QMATH_SIZE >= 24u)
#define   ZMC_Q_Q24(qA)     ((ZQ24)(qA) >> (ZMC_QMATH_SIZE - 24u))
#define   ZMC_Q24_Q(q24A)   ((ZQ)(q24A) << (ZMC_QMATH_SIZE - 24u))
#else
#define   ZMC_Q_Q24(qA)     ((ZQ24)(qA) << (24u - ZMC_QMATH_SIZE))
#define   ZMC_Q24_Q(q24A)   ((ZQ)(q24A) >> (24u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 23u)
#define   ZMC_Q_Q23(qA)     ((ZQ23)(qA) >> (ZMC_QMATH_SIZE - 23u))
#define   ZMC_Q23_Q(q23A)   ((ZQ)(q23A) << (ZMC_QMATH_SIZE - 23u))
#else
#define   ZMC_Q_Q23(qA)     ((ZQ23)(qA) << (23u - ZMC_QMATH_SIZE))
#define   ZMC_Q23_Q(q23A)   ((ZQ)(q23A) >> (23u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 22u)
#define   ZMC_Q_Q22(qA)     ((ZQ22)(qA) >> (ZMC_QMATH_SIZE - 22u))
#define   ZMC_Q22_Q(q22A)   ((ZQ)(q22A) << (ZMC_QMATH_SIZE - 22u))
#else
#define   ZMC_Q_Q22(qA)     ((ZQ22)(qA) << (22u - ZMC_QMATH_SIZE))
#define   ZMC_Q22_Q(q22A)   ((ZQ)(q22A) >> (22u - ZMC_QMATH_SIZE))
#endif
  
#if (ZMC_QMATH_SIZE >= 21u)
#define   ZMC_Q_Q21(qA)     ((ZQ21)(qA) >> (ZMC_QMATH_SIZE - 21u))
#define   ZMC_Q21_Q(q21A)   ((ZQ)(q21A) << (ZMC_QMATH_SIZE - 21u))
#else
#define   ZMC_Q_Q21(qA)     ((ZQ21)(qA) << (21u - ZMC_QMATH_SIZE))
#define   ZMC_Q21_Q(q21A)   ((ZQ)(q21A) >> (21u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 20u)
#define   ZMC_Q_Q20(qA)     ((ZQ20)(qA) >> (ZMC_QMATH_SIZE - 20u))
#define   ZMC_Q20_Q(q20A)   ((ZQ)(q20A) << (ZMC_QMATH_SIZE - 20u))
#else
#define   ZMC_Q_Q20(qA)     ((ZQ20)(qA) << (20u - ZMC_QMATH_SIZE))
#define   ZMC_Q20_Q(q20A)   ((ZQ)(q20A) >> (20u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 19u)
#define   ZMC_Q_Q19(qA)     ((ZQ19)(qA) >> (ZMC_QMATH_SIZE - 19u))
#define   ZMC_Q19_Q(q19A)   ((ZQ)(q19A) << (ZMC_QMATH_SIZE - 19u))
#else
#define   ZMC_Q_Q19(qA)     ((ZQ19)(qA) << (19u - ZMC_QMATH_SIZE))
#define   ZMC_Q19_Q(q19A)   ((ZQ)(q19A) >> (19u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 18u)
#define   ZMC_Q_Q18(qA)     ((ZQ18)(qA) >> (ZMC_QMATH_SIZE - 18u))
#define   ZMC_Q18_Q(q18A)   ((ZQ)(q18A) << (ZMC_QMATH_SIZE - 18u))
#else
#define   ZMC_Q_Q18(qA)     ((ZQ18)(qA) << (18u - ZMC_QMATH_SIZE))
#define   ZMC_Q18_Q(q18A)   ((ZQ)(q18A) >> (18u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 17u)
#define   ZMC_Q_Q17(qA)     ((ZQ17)(qA) >> (ZMC_QMATH_SIZE - 17u))
#define   ZMC_Q17_Q(q17A)   ((ZQ)(q17A) << (ZMC_QMATH_SIZE - 17u))
#else
#define   ZMC_Q_Q17(qA)     ((ZQ17)(qA) << (17u - ZMC_QMATH_SIZE))
#define   ZMC_Q17_Q(q17A)   ((ZQ)(q17A) >> (17u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 16u)
#define   ZMC_Q_Q16(qA)     ((ZQ16)(qA) >> (ZMC_QMATH_SIZE - 16u))
#define   ZMC_Q16_Q(q16A)   ((ZQ)(q16A) << (ZMC_QMATH_SIZE - 16u))
#else
#define   ZMC_Q_Q16(qA)     ((ZQ16)(qA) << (16u - ZMC_QMATH_SIZE))
#define   ZMC_Q16_Q(q16A)   ((ZQ)(q16A) >> (16u - ZMC_QMATH_SIZE))
#endif
#endif

#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT) || (ZMC_QMATH_TYPE == ZMC_QMATH_16BIT)                 
#if (ZMC_QMATH_SIZE >= 15u)     /* 32bit and 16bit fixed-point  */
#define   ZMC_Q_Q15(qA)     ((ZQ15)(qA) >> (ZMC_QMATH_SIZE - 15u))
#define   ZMC_Q15_Q(q15A)   ((ZQ)(q15A) << (ZMC_QMATH_SIZE - 15u))
#else
#define   ZMC_Q_Q15(qA)     ((ZQ15)(qA) << (15u - ZMC_QMATH_SIZE))
#define   ZMC_Q15_Q(q15A)   ((ZQ)(q15A) >> (15u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 14u)
#define   ZMC_Q_Q14(qA)     ((ZQ14)(qA) >> (ZMC_QMATH_SIZE - 14u))
#define   ZMC_Q14_Q(q14A)   ((ZQ)(q14A) << (ZMC_QMATH_SIZE - 14u))
#else
#define   ZMC_Q_Q14(qA)     ((ZQ14)(qA) << (14u - ZMC_QMATH_SIZE))
#define   ZMC_Q14_Q(q14A)   ((ZQ)(q14A) >> (14u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 13u)
#define   ZMC_Q_Q13(qA)     ((ZQ13)(qA) >> (ZMC_QMATH_SIZE - 13u))
#define   ZMC_Q13_Q(q13A)   ((ZQ)(q13A) << (ZMC_QMATH_SIZE - 13u))
#else
#define   ZMC_Q_Q13(qA)     ((ZQ13)(qA) << (13u - ZMC_QMATH_SIZE))
#define   ZMC_Q13_Q(q13A)   ((ZQ)(q13A) >> (13u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 12u)
#define   ZMC_Q_Q12(qA)     ((ZQ12)(qA) >> (ZMC_QMATH_SIZE - 12u))
#define   ZMC_Q12_Q(q12A)   ((ZQ)(q12A) << (ZMC_QMATH_SIZE - 12u))
#else
#define   ZMC_Q_Q12(qA)     ((ZQ12)(qA) << (12u - ZMC_QMATH_SIZE))
#define   ZMC_Q12_Q(q12A)   ((ZQ)(q12A) >> (12u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 11u)
#define   ZMC_Q_Q11(qA)     ((ZQ11)(qA) >> (ZMC_QMATH_SIZE - 11u))
#define   ZMC_Q11_Q(q11A)   ((ZQ)(q11A) << (ZMC_QMATH_SIZE - 11u))
#else
#define   ZMC_Q_Q11(qA)     ((ZQ11)(qA) << (11u - ZMC_QMATH_SIZE))
#define   ZMC_Q11_Q(q11A)   ((ZQ)(q11A) >> (11u - ZMC_QMATH_SIZE))
#endif

#if (ZMC_QMATH_SIZE >= 10u)
#define   ZMC_Q_Q10(qA)     ((ZQ10)(qA) >> (ZMC_QMATH_SIZE - 10u))
#define   ZMC_Q10_Q(q10A)   ((ZQ)(q10A) << (ZMC_QMATH_SIZE - 10u))
#else
#define   ZMC_Q_Q10(qA)     ((ZQ10)(qA) << (10u - ZMC_QMATH_SIZE))
#define   ZMC_Q10_Q(q10A)   ((ZQ)(q10A) >> (10u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 9u)
#define   ZMC_Q_Q9(qA)      ((ZQ9)(qA) >> (ZMC_QMATH_SIZE - 9u))
#define   ZMC_Q9_Q(q9A)     ((ZQ)(q9A) << (ZMC_QMATH_SIZE - 9u))
#else
#define   ZMC_Q_Q9(qA)      ((ZQ9) (qA) << (9u - ZMC_QMATH_SIZE))
#define   ZMC_Q9_Q(q9A)     ((ZQ) (q9A) >> (9u - ZMC_QMATH_SIZE))
#endif  
  
#if (ZMC_QMATH_SIZE >= 8u)
#define   ZMC_Q_Q8(qA)      ((ZQ8)(qA) >> (ZMC_QMATH_SIZE - 8u))
#define   ZMC_Q8_Q(q8A)     ((ZQ)(q8A) << (ZMC_QMATH_SIZE - 8u))
#else
#define   ZMC_Q_Q8(qA)      ((ZQ8)(qA) << (8u - ZMC_QMATH_SIZE))
#define   ZMC_Q8_Q(q8A)     ((ZQ)(q8A) >> (8u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 7u)
#define   ZMC_Q_Q7(qA)      ((ZQ7)(qA) >> (ZMC_QMATH_SIZE - 7u))
#define   ZMC_Q7_Q(q7A)     ((ZQ)(q7A) << (ZMC_QMATH_SIZE - 7u))
#else
#define   ZMC_Q_Q7(qA)      ((ZQ7)(qA) << (7u - ZMC_QMATH_SIZE))
#define   ZMC_Q7_Q(q7A)     ((ZQ)(q7A) >> (7u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 6u)
#define   ZMC_Q_Q6(qA)      (ZQ6)(qA) >> (ZMC_QMATH_SIZE - 6u))
#define   ZMC_Q6_Q(q6A)     ((ZQ)(q6A) << (ZMC_QMATH_SIZE - 6u))
#else
#define   ZMC_Q_Q6(qA)      ((ZQ6)(qA) << (6u - ZMC_QMATH_SIZE))
#define   ZMC_Q6_Q(q6A)     ((ZQ)(q6A) >> (6u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 5u)
#define   ZMC_Q_Q5(qA)      ((ZQ5) (qA) >> (ZMC_QMATH_SIZE - 5u))
#define   ZMC_Q5_Q(q5A)     ((ZQ) (q5A) << (ZMC_QMATH_SIZE - 5u))
#else
#define   ZMC_Q_Q5(qA)      ((ZQ5)(qA) << (5u - ZMC_QMATH_SIZE))
#define   ZMC_Q5_Q(q5A)     ((ZQ)(q5A) >> (5u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 4u)
#define   ZMC_Q_Q4(qA)      ((ZQ4)(qA) >> (ZMC_QMATH_SIZE - 4u))
#define   ZMC_Q4_Q(q4A)     ((ZQ)(q4A) << (ZMC_QMATH_SIZE - 4u))
#else
#define   ZMC_Q_Q4(qA)      ((ZQ4)(qA) << (4u - ZMC_QMATH_SIZE))
#define   ZMC_Q4_Q(q4A)     ((ZQ)(q4A) >> (4u - ZMC_QMATH_SIZE))
#endif  
  
#if (ZMC_QMATH_SIZE >= 3u)
#define   ZMC_Q_Q3(qA)      ((ZQ3)(qA) >> (ZMC_QMATH_SIZE - 3u))
#define   ZMC_Q3_Q(q3A)     ((ZQ)(q3A) << (ZMC_QMATH_SIZE - 3u))
#else
#define   ZMC_Q_Q3(qA)      ((ZQ3)(qA) << (3u - ZMC_QMATH_SIZE))
#define   ZMC_Q3_Q(q3A)     ((ZQ)(q3A) >> (3u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 2u)
#define   ZMC_Q_Q2(qA)      ((ZQ2)(qA) >> (ZMC_QMATH_SIZE - 2u))
#define   ZMC_Q2_Q(q2A)     ((ZQ)(q2A) << (ZMC_QMATH_SIZE - 2u))
#else
#define   ZMC_Q_Q2(qA)      ((ZQ2)(qA) << (2u - ZMC_QMATH_SIZE))
#define   ZMC_Q2_Q(q2A)     ((ZQ)(q2A) >> (2u - ZMC_QMATH_SIZE))
#endif  

#if (ZMC_QMATH_SIZE >= 1u)
#define   ZMC_Q_Q1(qA)      ((ZQ1)(qA) >> (ZMC_QMATH_SIZE - 1u))
#define   ZMC_Q1_Q(q1A)     ((ZQ)(q1A) << (ZMC_QMATH_SIZE - 1u))
#else
#define   ZMC_Q_Q1(qA)      ((ZQ1)(qA) << (1u - ZMC_QMATH_SIZE))
#define   ZMC_Q1_Q(q1A)     ((ZQ)(q1A) >> (1u - ZMC_QMATH_SIZE))
#endif  
#endif

#if (ZMC_QMATH_TYPE == ZMC_QMATH_FLOAT)                                 /* floating-point               */
#define   ZMC_Q_Q30(qA)     ((ZQ30)(qA))
#define   ZMC_Q30_Q(q30A)   ((ZQ)(q30A))

#define   ZMC_Q_Q29(qA)     ((ZQ29)(qA))
#define   ZMC_Q29_Q(q29A)   ((ZQ)(q29A))

#define   ZMC_Q_Q28(qA)     ((ZQ28)(qA))
#define   ZMC_Q28_Q(q28A)   ((ZQ)(q28A))

#define   ZMC_Q_Q27(qA)     ((ZQ27)(qA))
#define   ZMC_Q27_Q(q27A)   ((ZQ)(q27A))

#define   ZMC_Q_Q26(qA)     ((ZQ26)(qA))
#define   ZMC_Q26_Q(q26A)   ((ZQ)(q26A))

#define   ZMC_Q_Q25(qA)     ((ZQ25)(qA))
#define   ZMC_Q25_Q(q25A)   ((ZQ)(q25A))

#define   ZMC_Q_Q24(qA)     ((ZQ24)(qA))
#define   ZMC_Q24_Q(q24A)   ((ZQ)(q24A))

#define   ZMC_Q_Q23(qA)     ((ZQ23)(qA))
#define   ZMC_Q23_Q(q23A)   ((ZQ)(q23A))

#define   ZMC_Q_Q22(qA)     ((ZQ22)(qA))
#define   ZMC_Q22_Q(q22A)   ((ZQ)(q22A))

#define   ZMC_Q_Q21(qA)     ((ZQ21)(qA))
#define   ZMC_Q21_Q(q21A)   ((ZQ)(q21A))

#define   ZMC_Q_Q20(qA)     ((ZQ20)(qA))
#define   ZMC_Q20_Q(q20A)   ((ZQ)(q20A))

#define   ZMC_Q_Q19(qA)     ((ZQ19)(qA))
#define   ZMC_Q19_Q(q19A)   ((ZQ)(q19A))

#define   ZMC_Q_Q18(qA)     ((ZQ18)(qA))
#define   ZMC_Q18_Q(q18A)   ((ZQ)(q18A))

#define   ZMC_Q_Q17(qA)     ((ZQ17)(qA))
#define   ZMC_Q17_Q(q17A)   ((ZQ)(q17A))

#define   ZMC_Q_Q16(qA)     ((ZQ16)(qA))
#define   ZMC_Q16_Q(q16A)   ((ZQ)(q16A))

#define   ZMC_Q_Q15(qA)     ((ZQ15)(qA))
#define   ZMC_Q15_Q(q15A)   ((ZQ)(q15A))

#define   ZMC_Q_Q14(qA)     ((ZQ14)(qA))
#define   ZMC_Q14_Q(q14A)   ((ZQ)(q14A))

#define   ZMC_Q_Q13(qA)     ((ZQ13)(qA))
#define   ZMC_Q13_Q(q13A)   ((ZQ)(q13A))

#define   ZMC_Q_Q12(qA)     ((ZQ12)(qA))
#define   ZMC_Q12_Q(q12A)   ((ZQ)(q12A))

#define   ZMC_Q_Q11(qA)     ((ZQ11)(qA))
#define   ZMC_Q11_Q(q11A)   ((ZQ)(q11A))

#define   ZMC_Q_Q10(qA)     ((ZQ10)(qA))
#define   ZMC_Q10_Q(q10A)   ((ZQ)(q10A))

#define   ZMC_Q_Q9(qA)      ((ZQ9)(qA))
#define   ZMC_Q9_Q(q9A)     ((ZQ)(q9A))

#define   ZMC_Q_Q8(qA)      ((ZQ8)(qA))
#define   ZMC_Q8_Q(q8A)     ((ZQ)(q8A))

#define   ZMC_Q_Q7(qA)      ((ZQ7)(qA))
#define   ZMC_Q7_Q(q7A)     ((ZQ)(q7A))

#define   ZMC_Q_Q6(qA)      ((ZQ6)(qA))
#define   ZMC_Q6_Q(q6A)     ((ZQ)(q6A))

#define   ZMC_Q_Q5(qA)      ((ZQ5)(qA))
#define   ZMC_Q5_Q(q5A)     ((ZQ)(q5A))

#define   ZMC_Q_Q4(qA)      ((ZQ4)(qA))
#define   ZMC_Q4_Q(q4A)     ((ZQ)(q4A))

#define   ZMC_Q_Q3(qA)      ((ZQ3)(qA))
#define   ZMC_Q3_Q(q3A)     ((ZQ)(q3A))

#define   ZMC_Q_Q2(qA)      ((ZQ2)(qA))
#define   ZMC_Q2_Q(q2A)     ((ZQ)(q2A))

#define   ZMC_Q_Q1(qA)      ((ZQ1)(qA))
#define   ZMC_Q1_Q(q1A)     ((ZQ)(q1A))

#endif


/*********************************************************************************************************
  Multiplies two Q numbers.
  两个Q值数相乘
*********************************************************************************************************/
#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT)
#define ZMC_QNMPY(qA, qB, ucN)          ( (ZQ)(((signed long long)(qA) * (qB)) >> (ucN)) )
                                                                        /* (qA * qB) >> ucN             */
#define ZMC_Q15MPY(qA, qB, ucN)          ( (ZQ)(((signed long)(qA) * (qB)) >> (ucN)) )
    
#define ZMC_QMPY(qA, qB)                ZMC_QNMPY(qA, qB, ZMC_QMATH_SIZE)
                                                                        /* (qA * qB) >> ZMC_QMATH_SIZE  */

#define ZMC_QNMAM(qA, qB, qC, qD, ucN)  ( (ZQ)(((signed long long)(qA) * (qB) +                         \
                                                (signed long long)(qC) * (qD)) >> (ucN)) )
                                                                        /* (qA * qB + qC *qD) >> ucN    */

#define ZMC_QMAM(qA, qB, qC, qD)        ZMC_QNMAM(qA, qB, qC, qD, ZMC_QMATH_SIZE)
                                                                        /* (qA*qB+qC*qD)>>ZMC_QMATH_SIZE*/

#define ZMC_QNMSM(qA, qB, qC, qD, ucN)  ZMC_QNMAM(qA, qB, qC, -(qD), ucN)
                                                                        /* (qA * qB - qC * qD) >> ucN   */

#define ZMC_QMSM(qA, qB, qC, qD)        ZMC_QNMAM(qA, qB, qC, -(qD), ZMC_QMATH_SIZE)
                                                                        /* (qA*qB-qC*qD)>>ZMC_QMATH_SIZE*/

#elif (ZMC_QMATH_TYPE == ZMC_QMATH_16BIT)                                                       
#define ZMC_QNMPY(qA, qB, ucN)          ( (ZQ)(((signed long)(qA) * (qB)) >> (ucN)) )
                                                                        /* (qA * qB) >> ucN             */

#define ZMC_QMPY(qA, qB)                ZMC_QNMPY(qA, qB, ZMC_QMATH_SIZE)
                                                                        /* (qA * qB) >> ZMC_QMATH_SIZE  */

#define ZMC_QNMAM(qA, qB, qC, qD, ucN)  ( (ZQ)(((signed long)(qA) * (qB) +                              \
                                                (signed long)(qC) * (qD)) >> (ucN)) )
                                                                        /* (qA * qB + qC *qD) >> ucN    */

#define ZMC_QMAM(qA, qB, qC, qD)        ZMC_QNMAM(qA, qB, qC, qD, ZMC_QMATH_SIZE)
                                                                        /* (qA*qB+qC*qD)>>ZMC_QMATH_SIZE*/

#define ZMC_QNMSM(qA, qB, qC, qD, ucN)  ( (ZQ)(((signed long)(qA) * (qB) - (signed long)(qC) * (qD)) >> (ucN)) )
                                                                        /* (qA * qB - qC * qD) >> ucN   */

#define ZMC_QMSM(qA, qB, qC, qD)        ZMC_QNMSM(qA, qB, qC, qD, ZMC_QMATH_SIZE)
                                                                        /* (qA*qB-qC*qD)>>ZMC_QMATH_SIZE*/

#elif (ZMC_QMATH_TYPE == ZMC_QMATH_FLOAT)
#define ZMC_QNMPY(qA, qB, ucN)          ( (qA) * (qB) )
#define ZMC_QMPY(qA, qB)                ( (qA) * (qB) )

#define ZMC_QNMAM(qA, qB, qC, qD, ucN)  ( (qA) * (qB) + (qC) * (qD) )
#define ZMC_QMAM(qA, qB, qC, qD)        ( (qA) * (qB) + (qC) * (qD) )

#define ZMC_QNMSM(qA, qB, qC, qD, ucN)  ( (qA) * (qB) - (qC) * (qD) )
#define ZMC_QMSM(qA, qB, qC, qD)        ( (qA) * (qB) - (qC) * (qD) )

#endif



/*********************************************************************************************************
  Divides two Q numbers, without rounding
  两个Q值数相除
*********************************************************************************************************/
#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT)
extern ZQ zmcQnDiv(ZQ qA, ZQ qB, unsigned char ucN);
#define ZMC_QNDIV(qA, qB, ucN)          ((long long)qA << ucN) / qB;      /*      zmcQnDiv(qA, qB, ucN);      */
#define ZMC_QDIV(qA, qB)                ZMC_QNDIV(qA, qB, ZMC_QMATH_SIZE)
                                                                        /* (qA << ZMC_QMATH_SIZE) / qB  */

#elif (ZMC_QMATH_TYPE == ZMC_QMATH_16BIT)                             

static  ZMC_INLINE  ZQ  zmcQnDiv (ZQ qA, ZQ qB, unsigned char ucN)
{
    signed long lTmp;
    if (qB == 0) {
        if (qA > 0) {
            return (32767);
        } else if (qA < 0) {
            return (-32768);
        } else {
            return (0);
        }
    }
    
    lTmp =(signed long)((signed long)qA* (1 << (ucN)) / (qB));
    if (lTmp > 32767) {
        return (32767);
    } else if (lTmp < -32768) {
        return (-32768);
    } else {
        return ((ZQ)lTmp);
    }
}
#define ZMC_QNDIV(qA, qB, ucN)          zmcQnDiv(qA, qB, ucN);          /* (qA << ucN) / qB             */
#define ZMC_QDIV(qA, qB)                ZMC_QNDIV(qA, qB, ZMC_QMATH_SIZE)
                                                                        /* (qA << ZMC_QMATH_SIZE) / qB  */

#elif (ZMC_QMATH_TYPE == ZMC_QMATH_FLOAT)                             
#define ZMC_QNDIV(qA, qB, ucN)          ( (qA) / (qB) ) 
#define ZMC_QDIV(qA, qB)                ( (qA) / (qB) ) 

#endif

/*********************************************************************************************************
  Saturates a Q value in a given range.
  将一个Q值数限定在给定范围内。
*********************************************************************************************************/
#define ZMC_QSAT(qA, qPos, qNeg)        ( ((qA) > (qPos))  ?                                             \
                                        (qPos)           :                                               \
                                        (((qA) < (qNeg)) ? (qNeg) : (qA)) )


/*********************************************************************************************************
  Compute the square root of a Q value 计算整数的平方根
  计算Q值数的平方根   ZMC_INLINE  Aug08
*********************************************************************************************************/
static  __inline  long  zmcQ32Sqrt (unsigned long  ulValue)
{
    unsigned long  ulRem, ulRoot, ulIdx;

    ulRem  = 0;                                                            /* Init. the rem. and root to 0.*/
    ulRoot = 0;                                                         /* 把余数和根复位为0            */

    for (ulIdx = 0; ulIdx < 16; ulIdx++) {                              /* Loop over the 16bits in root */
                                                                        /* 查看根中的16位数。           */
        /*
         *  Shift the root up by a bit to make room for the new bit that is about to be computed.
         *  把根的一位向上移，留出空位计算。
         */
        ulRoot <<= 1;

        /*  
         *  Get two more bits from the input into the remainder.
         *  从输出参数中再取两个位，放到余数中
         */
        ulRem = ((ulRem << 2) + (ulValue >> 30));
        ulValue <<= 2;

        /*  
         *  Make the test root be 2n + 1.
         *  使测试的根为2n+1
         */
        ulRoot++;

        /*
         *  See if the root is greater than the remainder.
         *  判断根是否大于余数
         */
        if(ulRoot <= ulRem)
        {
            /* 
             *  Subtract the test root from the remainder.
             *  从余数中减去测试的根
             */
            ulRem -= ulRoot;

            /*
             *  Increment the root, setting the second LSB.
             *  根加一，设置第二个LSB
             */
            ulRoot++;
        }
        else
        {
            /*
             *  The root is greater than the remainder, so the new bit of the root is actually zero.
             *  根大于余数，所以根的新位实际上为0
             */
            ulRoot--;
        }
    }

    /*
     *  Return the computed root. 
     *  返回算好的根
     */
    return(ulRoot >> 1);
}


static __inline  unsigned  short  zmcQ16Sqrt (unsigned short  usValue)
{
    unsigned long  ulRem, ulRoot, ulIdx;

    ulRem  = 0;                                                         /* Init. the rem. and root to 0.*/
    ulRoot = 0;                                                         /* 把余数和根复位为0            */

    for (ulIdx = 0; ulIdx < 8; ulIdx++) {                               /* Loop over the 16bits in root */
                                                                        /* 查看根中的16位数。           */
        /*
         *  Shift the root up by a bit to make room for the new bit that is about to be computed.
         *  把根的一位向上移，留出空位计算。
         */
        ulRoot <<= 1;

        /*  
         *  Get two more bits from the input into the remainder.
         *  从输出参数中再取两个位，放到余数中
         */
        ulRem = ((ulRem << 2) + (usValue >> 14));
        usValue <<= 2;

        /*  
         *  Make the test root be 2n + 1.
         *  使测试的根为2n+1
         */
        ulRoot++;

        /*
         *  See if the root is greater than the remainder.
         *  判断根是否大于余数
         */
        if(ulRoot <= ulRem)
        {
            /* 
             *  Subtract the test root from the remainder.
             *  从余数中减去测试的根
             */
            ulRem -= ulRoot;

            /*
             *  Increment the root, setting the second LSB.
             *  根加一，设置第二个LSB
             */
            ulRoot++;
        }
        else
        {
            /*
             *  The root is greater than the remainder, so the new bit of the root is actually zero.
             *  根大于余数，所以根的新位实际上为0
             */
            ulRoot--;
        }
    }

    /*
     *  Return the computed root. 
     *  返回算好的根
     */
    return(ulRoot >> 1);
}


#if (ZMC_QMATH_TYPE == ZMC_QMATH_32BIT)
#define ZMC_QSQRT(qA)   zmcQ32Sqrt((unsigned long)(qA)) 

#elif (ZMC_QMATH_TYPE == ZMC_QMATH_16BIT)
#define ZMC_QSQRT(qA)   zmcQ16Sqrt((unsigned short)(qA))

#elif (ZMC_QMATH_TYPE == ZMC_QMATH_FLOAT)
#ifdef __ZMC_ASM_FPU
float   zmcQFloatSqrt(float  fA);
#define ZMC_QSQRT(qA)   zmcQFloatSqrt((float)(qA));
#else
#define ZMC_QSQRT(qA)   (float)sqrt((float)(qA));
#endif

#endif



#ifdef __cplusplus
    }
#endif

#endif                                                                  /* __ZMC_QMATH_H                */
