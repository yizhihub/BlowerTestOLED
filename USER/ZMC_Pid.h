/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               ZMC_PID.h
** Last modified Date:      2012.01.05
** Last Version:            v1.0
** Description:             PID computation.  PID计算。
** 
**--------------------------------------------------------------------------------------------------------
** Created By:              Steven Zhou 周绍刚
** Created date:            2012.01.05
** Version:                 v1.0
** Descriptions:            The original version 初始版本
**
*********************************************************************************************************/

#ifndef __ZMC_PID_H
#define __ZMC_PID_H

#ifdef __cplusplus
extern "C" {
#endif

struct zmc_pid_data{
    unsigned long ulQn;                                                 /* Q size, 1 ~ 30               */
      
    ZQ            qRef;                                                 /* Input: Reference input       */
    ZQ            qErr;                                                 /* Error                        */
    ZQ            qKp;                                                  /* Proportional gain            */
    ZQ            qUp;                                                  /* Proportional output          */
    ZQ            qUi;                                                  /* Integral output              */
    ZQ            qUd;                                                  /* Derivative output            */
    ZQ            qOutPreSat;                                           /* Pre-saturated output         */
    ZQ            qOutMax;                                              /* Maximum output               */
    ZQ            qOutMin;                                              /* Minimum output               */
    ZQ            qOut;                                                 /* PID output                   */
    ZQ            qSatErr;                                              /* Saturated difference         */
    ZQ            qKi;                                                  /* Integral gain                */
    ZQ            qKc;                                                  /* Integral correction gain     */
    ZQ            qKd;                                                  /* Derivative gain              */
    ZQ            qUp1;                                                 /* Previous proportional output */
};
typedef struct zmc_pid_data   ZMC_PID_DATA;


/*********************************************************************************************************
  Default initalizer for the PID structure.                 
*********************************************************************************************************/ 
#define ZMC_PID_DATA_DEF {(ZMC_QMATH_SIZE),                                                                \
                          0,                                                                             \
                          0,                                                                             \
                          ZMC_Q(1.3),                                                                    \
                          0,                                                                             \
                          0,                                                                             \
                          0,                                                                             \
                          0,                                                                             \
                          ZMC_Q(1),                                                                      \
                          ZMC_Q(-1),                                                                     \
                          0,                                                                             \
                          0,                                                                             \
                          ZMC_Q(0.02),                                                                   \
                          ZMC_Q(0.5),                                                                    \
                          ZMC_Q(1.05),                                                                   \
                          0,                                                                             \
                          }

/*********************************************************************************************************
  P Macro Definition  
*********************************************************************************************************/
#define ZMC_P(ptPid, qFdb)                                                                              \
        (ptPid)->qErr       = (ptPid)->qRef - (qFdb);                   /* Compute the error           */\
        (ptPid)->qUp        = ZMC_QNMPY( (ptPid)->qKp, (ptPid)->qErr,    (ptPid)->ulQn );                \
                                                                        /* Proportional output         */\
        (ptPid)->qOut       = ZMC_QSAT( (ptPid)->qUp, (ptPid)->qOutMax, (ptPid)->qOutMin );       \
                                                                        /* Saturate the output         */\
   
/*********************************************************************************************************
  PI Macro Definition  
*********************************************************************************************************/
//#define ZMC_PI(ptPid, qFdb)                                                                              \
//        (ptPid)->qErr       = (ptPid)->qRef - (qFdb);                   /* Compute the error           */\
//        (ptPid)->qUp        = ZMC_QNMPY( (ptPid)->qKp, (ptPid)->qErr,    (ptPid)->ulQn );                \
//                                                                        /* Proportional output         */\
//        (ptPid)->qUi       += ZMC_QNMPY( (ptPid)->qKi, (ptPid)->qUp,     (ptPid)->ulQn ) +               \
//                              ZMC_QNMPY( (ptPid)->qKc, (ptPid)->qSatErr, (ptPid)->ulQn );                \
//                                                                        /* Integral output             */\
//        (ptPid)->qOutPreSat = (ptPid)->qUp + (ptPid)->qUi;              /* Pre-saturated output        */\
//        (ptPid)->qOut       = ZMC_QSAT( (ptPid)->qOutPreSat, (ptPid)->qOutMax, (ptPid)->qOutMin );       \
//                                                                        /* Saturate the output         */\
//        (ptPid)->qSatErr    = (ptPid)->qOut - (ptPid)->qOutPreSat;      /* Saturate difference         */
        
#define ZMC_PI(ptPid, qFdb)                                                                              \
        (ptPid)->qErr       = (ptPid)->qRef - (qFdb);                   /* Compute the error           */\
        (ptPid)->qUp        = ZMC_QNMPY( (ptPid)->qKp, (ptPid)->qErr,    ZMC_QMATH_SIZE );                \
                                                                        /* Proportional output         */\
        (ptPid)->qUi       += ZMC_QNMPY( (ptPid)->qKi, (ptPid)->qUp,     ZMC_QMATH_SIZE ) +               \
                              ZMC_QNMPY( (ptPid)->qKc, (ptPid)->qSatErr, ZMC_QMATH_SIZE );                \
                                                                        /* Integral output             */\
        (ptPid)->qOutPreSat = (ptPid)->qUp + (ptPid)->qUi;              /* Pre-saturated output        */\
        (ptPid)->qOut       = ZMC_QSAT( (ptPid)->qOutPreSat, (ptPid)->qOutMax, (ptPid)->qOutMin );       \
                                                                        /* Saturate the output         */\
        (ptPid)->qSatErr    = (ptPid)->qOut - (ptPid)->qOutPreSat;      /* Saturate difference         */
/*********************************************************************************************************
  PID Macro Definition  
*********************************************************************************************************/
#define ZMC_PID(ptPid, qFdb)                                                                             \
	(ptPid)->qErr 	    = (ptPid)->qRef - (qFdb);                   /* Compute the error           */\
	(ptPid)->qUp  	    = ZMC_QNMPY( (ptPid)->qKp, (ptPid)->qErr,    (ptPid)->ulQn );                \
                                                                        /* Proportional output 	       */\
        (ptPid)->qUi       += ZMC_QNMPY( (ptPid)->qKi, (ptPid)->qUp,     (ptPid)->ulQn ) +               \
                              ZMC_QNMPY( (ptPid)->qKc, (ptPid)->qSatErr, (ptPid)->ulQn );	         \
		                                                        /* Integral output             */\
	(ptPid)->qUd        = ZMC_QNMPY( (ptPid)->qKd, ((ptPid)->qUp - (ptPid)->qUp1), (ptPid)->ulQn );  \
                                                                        /* Derivative output           */\
        (ptPid)->qOutPreSat = (ptPid)->qUp + (ptPid)->qUi + (ptPid)->qUd;                                \
                                                                        /* Pre-saturated output        */\
	(ptPid)->qOut       = ZMC_QSAT( (ptPid)->qOutPreSat, (ptPid)->qOutMax, (ptPid)->qOutMin);        \
                                                                        /* Saturate the output	       */\
        (ptPid)->qSatErr    = (ptPid)->qOut - (ptPid)->qOutPreSat;      /* Saturate difference         */\
        (ptPid)->qUp1       = (ptPid)->qUp;	


#ifdef __cplusplus
}
#endif

#endif                                                                  /* __ZMC_PID_H                  */

/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
