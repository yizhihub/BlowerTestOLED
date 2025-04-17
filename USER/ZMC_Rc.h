/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               ZMC_Rc.h
** Last modified Date:      2013-08-15
** Last Version:            1.1
** Description:             ramp contrl
** 
**--------------------------------------------------------------------------------------------------------
** Created By:              Fan Linyun
** Created date:            2012-02-15
** Version:                 1.0
** Descriptions:            The original version 初始版本
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Fan Linyun
** Modified date:           2013-08-15
** Version:                 1.1
** Descriptions:            增加ramp上升下降增量为两个独立变量
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#ifndef __ZMC_RC_H
#define __ZMC_RC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
  RAMP Controller structure                  
*********************************************************************************************************/
struct zmc_rc_data{  
    ZQ  qTarg;                                                          /* target valuse                */
    ZQ  qStepUp;                                                        /* increase valuse at every exe */
    ZQ  qStepUp1;
    ZQ  qStepUp2;
    ZQ  qStepDown;                                                      /* decrease valuse at every exe */
    ZQ  qOut;                                                           /* output valuse                */
} ;
typedef struct zmc_rc_data  ZMC_RC_DATA;

/*********************************************************************************************************
  RAMP Controller Macro Definition           
*********************************************************************************************************/
#define ZMC_RC(ptRc) {                                                  /* RAMP Controller Macro        */\
    if (((ptRc)->qTarg) > (ptRc)->qOut) {                               /* need increase valuse         */\
        (ptRc)->qOut += (ptRc)->qStepUp;                                                               \
        if ((ptRc)->qOut > (ptRc)->qTarg) {                                                               \
            (ptRc)->qOut = (ptRc)->qTarg;                                                                 \
        }                                                                                                 \
    } else if ((ptRc)->qTarg < (ptRc)->qOut) {                          /* need decrease valuse         */\
        (ptRc)->qOut -= (ptRc)->qStepDown;                                                               \
        if ((ptRc)->qOut < (ptRc)->qTarg) {                                                               \
            (ptRc)->qOut = (ptRc)->qTarg;                                                                 \
        }                                                                                                 \
    }                                                                                                     \
}


#ifdef __cplusplus
}
#endif

#endif                                                                  /* __ZMC_PID_H                  */

/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
