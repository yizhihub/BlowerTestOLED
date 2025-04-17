/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               ZMC_Filter.h
** Last modified Date:      2012.01.05
** Last Version:            v1.0
** Description:             Filters 滤波器
** 
**--------------------------------------------------------------------------------------------------------
** Created By:              Steven Zhou 周绍刚
** Created date:            2012.01.05
** Version:                 v1.0
** Descriptions:            The original version 初始版本
**
*********************************************************************************************************/

#ifndef __ZMC_FILTER_H
#define __ZMC_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************************************************************************************************
  Lowpass filter structure                  
*********************************************************************************************************/  
struct zmc_lowpass_data {
    ZQ      qKa;
    ZQ      qKb; 
           
    ZQ      qOut; 
    INT32U  q15Out;
    INT32U  q15OutLst;
    INT32U  q15In;
    ZQ      qIn;
    
    INT32U  ulQn; 
    INT8U   bLpfDone;
};
typedef struct zmc_lowpass_data ZMC_LOWPASS_DATA;   


/*********************************************************************************************************
  Default initalizer for the lowpass filter.                 
*********************************************************************************************************/ 
#define ZMC_LOWPASS_DATA_DEF {0, 0, 0 ,0}


/*********************************************************************************************************
  Lowpass filter Macro Definition  
  qOut = qKa * qOut + qKb * qIn            
*********************************************************************************************************/
#define ZMC_LOW_PASS(ptLp, qInput)                                                                          \
        (ptLp)->qIn = (qInput); \
        (ptLp)->qOut = ZMC_QNMPY( (ptLp)->qKa, (ptLp)->qOut, (INT8U)(ptLp)->ulQn ) +                     \
                       ZMC_QNMPY( (ptLp)->qKb, (ptLp)->qIn,  (INT8U)(ptLp)->ulQn );

//#define ZMC_LOW_PASSONE(ptLp, qIn)                                                                          \
//        (ptLp)->qOut = ZMC_QNMPY( (ptLp)->qKa, (ptLp)->qOut, ZMC_QMATH_SIZE) +                     \
//                       ZMC_QNMPY( (ptLp)->qKb, (qIn),        ZMC_QMATH_SIZE);

#define ZMC_LOW_PASSONE(ptLp, qInput)                                                                          \
          if (qInput < 0) qInput = 0;\
          (ptLp)->q15In  = qInput >> 8;\
          (ptLp)->q15Out = (((INT32U)(ptLp)->qKa * (ptLp)->q15Out) + ((INT32U)(ptLp)->qKb * (ptLp)->q15In) + 32768) >> 16;\
          (ptLp)->qOut   = (ptLp)->q15Out << 8;

#define ZMC_LOW_PASSQ16(ptLp, qInput)                                                                          \
          (ptLp)->q15In  = qInput;\
          (ptLp)->q15Out = (((INT32U)(ptLp)->qKa * (ptLp)->q15Out) + ((INT32U)(ptLp)->qKb * (ptLp)->q15In) + 32768) >> 16;\
          (ptLp)->qOut   = (ptLp)->q15Out << 8;

INT8U  zmcLowPassInit(ZMC_LOWPASS_DATA  *ptLp, 
                      FP32               fTsmpS, 
                      FP32               fFcutHz,
                      INT32U             ulQn);

//void  ZMC_LOW_PASS(ZMC_LOWPASS_DATA  *ptLp, INT32S qIn);
//void  ZMC_LOW_PASSONE(ZMC_LOWPASS_DATA  *ptLp, INT32S qIn);

#ifdef __cplusplus
}
#endif

#endif                                                                  /* __ZMC_FILTER_H               */
