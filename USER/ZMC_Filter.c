/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               ZMC_Filter.c
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
#include "ZMC_QMath_cfg.h"
#include "ZMC_QMath.h"
#include "ZMC_Filter.h"


/*********************************************************************************************************
** Function name:       zmcLowPassInit
** Descriptions:        Initialization of a low pass filter
** Input parameters:    ptLp:    A pointer to a low pass filter, 
**                      fTsmpS:  Sample period, unit second 
**                      fFcutHz: Cut of frequency, unit Hz
**                      ulQn:    Q size of the filter parameters                            
** Output parameters:   None
** Returned value:      1:       ptLp is a null pointer
**                      0:       Initialize successfully 
*********************************************************************************************************/   
INT8U  zmcLowPassInit (ZMC_LOWPASS_DATA  *ptLp, 
                       FP32               fTsmpS, 
                       FP32               fFcutHz,
                       INT32U             ulQn)
{
    if ( (ptLp == (ZMC_LOWPASS_DATA *) 0) ||
         (fTsmpS  <= 0u)                  ||
         (fFcutHz <= 0u)                  ||   
         (ulQn    >= 31u) ) {
        return (1u);        
    }
        
    /*
     *  qKa =  1 / (1 + 2 * ZMC_FPI * fTsmpS * fFcutHz)   
     */
    ptLp->qKa  = (ZQ)( (FP32)(1u << ulQn) / (1 + 2 * ZMC_FPI * fTsmpS * fFcutHz) ); 
    
    /*
     *  qKb = 1 - ptLp->qKa
     */
    ptLp->qKb  = (ZQ)(1u << ulQn) - ptLp->qKa;                                   
    
    ptLp->ulQn = ulQn;    
    ptLp->qOut = 0u;        
    ptLp->q15Out = 0u;
    ptLp->qIn    = 0u;
    
    return (0u);        
}

/*********************************************************************************************************
** Function name:       ZMC_LOW_PASS
** Descriptions:        execute of a low pass filter
** Input parameters:    ptLp:    A pointer to a low pass filter, 
**                      fTsmpS:  Sample period, unit second 
**                      fFcutHz: Cut of frequency, unit Hz
**                      ulQn:    Q size of the filter parameters                            
** Output parameters:   None
** Returned value:      1:       ptLp is a null pointer
**                      0:       Initialize successfully 
*********************************************************************************************************/   
//void  ZMC_LOW_PASS(ZMC_LOWPASS_DATA  *ptLp, INT32S qIn)
//{
//        (ptLp)->qIn  = qIn;
//        (ptLp)->qOut = ZMC_QNMPY( (ptLp)->qKa, (ptLp)->qOut, ZMC_QMATH_SIZE) +                     \
//                       ZMC_QNMPY( (ptLp)->qKb, (qIn),        ZMC_QMATH_SIZE);
////       (ptLp)->qOut = ZMC_QNMAM((ptLp)->qKa, (ptLp)->qOut, (ptLp)->qKb, (qIn), ZMC_QMATH_SIZE);
//}

//void  ZMC_LOW_PASSONE(ZMC_LOWPASS_DATA  *ptLp, INT32S qIn)
//{
////    if (ptLp->ulQn == 14u) {
////        qIn = qIn >> 10;
////        ptLp->qOut = (ptLp->qKa * ptLp->qOut + ptLp->qKb * qIn) >> 4u;
////       
////    } else {
////        (ptLp)->qOut = ZMC_QNMPY( (ptLp)->qKa, (ptLp)->qOut, ZMC_QMATH_SIZE) +                     \
////                       ZMC_QNMPY( (ptLp)->qKb, (qIn),        ZMC_QMATH_SIZE);
//          (ptLp)->qIn  = qIn;
//          if (qIn < 0) qIn = 0;
//          ptLp->q15In  = qIn >> 8;
//          ptLp->q15Out = (((INT32U)ptLp->qKa * ptLp->q15Out) + ((INT32U)ptLp->qKb * ptLp->q15In) + 32768) >> 16;
//          ptLp->qOut   = (ptLp->q15Out) << 8;
////          ptLp->qOut   = qIn;
////          if (ptLp->q15Out != ptLp->q15OutLst) {
////              ptLp->bLpfDone = 0;
////          } else {
////              ptLp->bLpfDone = 1;
////          }
//}




/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
