/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               ZMC_QMath_Cfg.h
** Last modified Date:      2012.01.05
** Last Version:            1.0
** Description:             The config file of QMath. QMath的配置文件。
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

#ifndef __ZMC_QMATH_CFG_H
#define __ZMC_QMATH_CFG_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************************************************************************************************
  QMath Type
  0 for 32bit fixed-point, 1 for 16bit fixed-point, 2 for floating-point
*********************************************************************************************************/    
#define  ZMC_QMATH_TYPE     2u                                     

  
/*********************************************************************************************************
  Default size for all QMath functions. 
  For 32bit fixed-point, 0 <= ZMC_QMATH_SIZE <= 30. 
  For 16bit fixed-point, 0 <= ZMC_QMATH_SIZE <= 15. 
  For floating-point,   ZMC_QMATH_SIZE is meaningless, can be set to any value.
*********************************************************************************************************/  
#define  ZMC_QMATH_SIZE     0u
                                                                  

#ifdef __cplusplus
    }
#endif

#endif                                         /* __ZMC_QMATH_CFG_H            */
