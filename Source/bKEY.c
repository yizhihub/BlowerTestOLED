/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               bKey.h
** Created By  :            YZ
** Created Date:            2023-06-15
** Last Version:            v1.0 
** Descriptions:            C51_PLATEFORM and ARM_PLATEFORM
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
**--------------------------------------------------------------------------------------------------------
**
*********************************************************************************************************/
#include "bkey.h"

enum ec11_edge_type {
    A_FALLING = 0,
    A_RISING,
    B_FALLING,
    B_RISING,
    AB_NONE
};

typedef enum ec11_edge_type EC11_EDGE;


INT16S GsEc11SetV;
INT16S GsEc11CntCW, GsEc11CntCCW;

/* 
 *  the GD32F303RC motor demo's author maybe stupid, which is hard to comprehend and complex. 
 */ 
//EC11_EDGE  ePreEdge = AB_NONE; 
//EC11_EDGE  eEndEdge = AB_NONE;

/* ec11 input direction */
typedef enum{
    DIRECTION_INCREASE = 1,                                                      /* the direction of ec11 input is increase */
    DIRECTION_DECREASE,                                                          /* the direction of ec11 input is decrease */
}ec11_direction;

#define EC11_STEP 1

/**
********************************************************************************************************
** @nameis keyInit
** @effect key port pin initilize
** @import none
** @export none
** @return void
** @create 2023.06.23 MyEmbeddedSoftware
** @modify 
*********************************************************************************************************/
void keyInit(void)
{
#ifdef GD32F30X_HD
    /*
     * KEY GPIO配置
     */
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU; 
    GPIO_Init(GPIOC,&GPIO_InitStructure);
#elif defined(C51_PLATEFORM)
    
#else
#error NO KEY_INIT FUNCTION    
#endif
}
/**
********************************************************************************************************
** @nameis ec11Init
** @effect ec11 encoder key A B port initialize 
** @import none
** @export none
** @return void
** @create 2023.06.23 MyEmbeddedSoftware
** @modify 
*********************************************************************************************************/
void ec11Init(void)
{
#ifdef GD32F30X_HD
    /*
     * ec11 Independent Key and encoder A B 
     */
    GPIO_InitTypeDef   GPIO_InitStructure;
    EXTI_InitTypeDef   EXIT_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU; 
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU; 
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXIT_InitStructure.EXTI_Line = EXTI_Line0;
    EXIT_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXIT_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXIT_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXIT_InitStructure);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
    EXIT_InitStructure.EXTI_Line = EXTI_Line1;
    EXIT_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXIT_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXIT_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXIT_InitStructure);
    
    EXTI_ClearITPendingBit(EXTI_Line0);
    NVIC_DisableIRQ(EXTI0_IRQn); 
    NVIC_SetPriority(EXTI0_IRQn, 0);
    NVIC_EnableIRQ(EXTI0_IRQn);
    
    EXTI_ClearITPendingBit(EXTI_Line1);
    NVIC_DisableIRQ(EXTI1_IRQn); 
    NVIC_SetPriority(EXTI1_IRQn, 0);
    NVIC_EnableIRQ(EXTI1_IRQn); 

#elif defined(C51_PLATEFORM)
    
#else
#error NO KEY_INIT FUNCTION    
#endif
}




/**
********************************************************************************************************
** @nameis ADKey_Scan
** @effect key port pin initilize
** @import none
** @export none
** @return void
** @create
** @modify 
*********************************************************************************************************/
KEYn_e ADKey_Scan(void)
{
  KEYn_e i=KEY_NONE;
//  ad=Get_ADC10bitResult(0x03); 
//  if(ad>660)                i=KEY_UP;
//  else if(ad>370 && ad<490) i=KEY_DOWN;
//  else if(ad>500 && ad<650) i=KEY_LEFT;
//  else if(ad>240 && ad<350) i=KEY_RIGHT;
//  else if(ad>120 && ad<200) i=KEY_ENTER;
  if      (INDEPKEY1 == 0)    i = KEY_UP;
  else if (INDEPKEY2 == 0)    i = KEY_DOWN;
  else if (INDEPKEY3 == 0)    i = KEY_CANCEL;
  else if (INDEPKEY4 == 0)    i = EC11_SET;
  else                        i = KEY_NONE;
  return i;
}


// 加松手检测 
KEYn_e ADKey_Check(void)
{
    static KEYn_e last_key;
    static uint8_t key_cnt,key_cnt_flag;
    KEYn_e tmp_key,key=KEY_NONE; 

    tmp_key=ADKey_Scan();
    if(tmp_key!=last_key && tmp_key!=KEY_NONE) 
    { 
        key_cnt_flag=1;  // push the key for the first time 
        key_cnt=0;
    }
    else if(key_cnt_flag==1 && tmp_key==last_key)
    {
        key_cnt++;
        if(key_cnt>10)    // dt=5ms  20    dt=20ms  5;   dt= 10ms  10
        {
            key=last_key; // get the fatall value 
            key_cnt=0;
            key_cnt_flag=0;
        }
    }
    /* 
     * asume the ec11 is key by yizhi
     */ 
    else if (GsEc11CntCW)             
    {
        GsEc11CntCW--;
        key = EC11_CW;
    }
    else if (GsEc11CntCCW)
    {
        GsEc11CntCCW--;
        key = EC11_CCW;
    }
    else 
    {
        key=KEY_NONE;
    }
    last_key=tmp_key;
    return key;
}
// 不卡死式  可连续加减 
KEYn_e ADKey_Check2(void)
{
   static KEYn_e last_key;
   static uint8_t key_cnt;
   KEYn_e tmp_key,key=KEY_NONE; 
   
//   tmp_key=ADKey_Scan();
   if(tmp_key!=KEY_NONE && tmp_key==last_key) 
   { 
     key_cnt++;
         if(key_cnt>20)
         {
             key=last_key;
             key_cnt=0;
         }
   }
   else 
   {
     key_cnt=0;  
   }
   last_key=tmp_key;
   return key;
}

/**
********************************************************************************************************
** @nameis ec11Scan
** @effect scan ec11, get the action flag and Step number
** @import none
** @export none
** @return void
** @create
** @modify 
*********************************************************************************************************/
void ec11Scan(INT8U bActionFlg, INT8U ucStepNum)
{
      bActionFlg = bActionFlg;
      ucStepNum  = ucStepNum;
//    static INT8U bALast, bBLast;
    
//    if (b) {
//    
//        
//    }
        
//    bALast = EC11_GDA_R;
//    bBLast = EC11_B_R;
    
    
}


#ifdef GD32F30X_HD
/**
********************************************************************************************************
** @nameis ec11Step
** @effect ec11 click event increse or decrease
** @import usStep;
** @export none
** @return void
** @create
** @modify this func. have abondoned
*********************************************************************************************************/
void ec11Step(INT16S sStep)
{
    GsEc11SetV += sStep;
    if (GsEc11SetV > 1000) {
        GsEc11SetV = 1000;
    }
    if (GsEc11SetV <= 0) {
        GsEc11SetV = 0;
    }
}

/**
********************************************************************************************************
** @nameis EXTI10_15_IRQHandler   STM32 GPIO Interrupt routine
** @effect scan ec11, get the action flag and Step number
** @import none
** @export none
** @return void
** @create
** @modify 
*********************************************************************************************************/
INT16U GsHallTim = 0;
void EXTI0_IRQHandler(void)
{
#if 1
    INT32U ulStep = 0;;
//    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line0);
        LED0_TOG();
        
       /*
        * for (usEc11DlyCnt = 0; usEc11DlyCnt < 5000; usEc11DlyCnt++); 
        */
//        if (EC11_A_R == RESET) {                                  /* there is a falling edge on A signal */
            if (EC11_B_R == SET) {
                if (GulEc11IntervalCnt < 3) {
                    GulEc11IntervalCnt = 0;
                    return;
                }
                ulStep = 500u / GulEc11IntervalCnt;
                if (ulStep <= 4) 
                    ulStep = 1;
                GsEc11CntCCW += ulStep;
                GulEc11IntervalCnt = 0;
            }

//            if (eEndEdge == A_FALLING) {eEndEdge = AB_NONE; return;}
//            if (ePreEdge == B_FALLING) {
//                ePreEdge = AB_NONE;
//                ec11Step(-EC11_STEP);
//                eEndEdge = A_FALLING;
//            } else {
//                ePreEdge = A_FALLING;
//            }
//        } else {
//            if (eEndEdge == A_RISING) {eEndEdge = AB_NONE; return;}
//            if (ePreEdge == B_RISING) {
//                ePreEdge = AB_NONE;
//                ec11Step(-EC11_STEP);
//                eEndEdge = A_RISING;
//            } else {
//                ePreEdge = A_RISING;
//            }
//        }
//    }
#else
    EXTI_ClearITPendingBit(EXTI_Line0);
    GsHallTim = TIM3->CNT;
    TIM3->CNT = 0;
#endif
}
/**
********************************************************************************************************
** @nameis EXTI10_15_IRQHandler   STM32 GPIO Interrupt routine
** @effect scan ec11, get the action flag and Step number
** @import none
** @export none
** @return void
** @create
** @modify 
*********************************************************************************************************/
void EXTI1_IRQHandler(void)
{
    INT32U ulStep;
//    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line1);
       LED1_TOG();
        
       /*
        * for (usEc11DlyCnt = 0; usEc11DlyCnt < 5000; usEc11DlyCnt++); 
        */
//       if (EC11_B_R == RESET) {
           if (EC11_A_R == SET) {
               if (GulEc11IntervalCnt < 3) {
                    GulEc11IntervalCnt = 0;
                    return;
                }
                ulStep = 500u / GulEc11IntervalCnt;
                if (ulStep <= 4) 
                    ulStep = 1;
                GsEc11CntCW += ulStep;
                GulEc11IntervalCnt = 0;
           }
//           if (eEndEdge == B_FALLING) {eEndEdge = AB_NONE; return;}
//           if (ePreEdge == A_FALLING) {
//               ePreEdge = AB_NONE;
//               ec11Step(EC11_STEP);
//               eEndEdge = B_FALLING;
//           } else {
//               ePreEdge = B_FALLING;
//           }
//       } else {
//           if (eEndEdge == B_RISING) {eEndEdge = AB_NONE; return;}
//           if (ePreEdge == A_RISING) {
//               ePreEdge = AB_NONE;
//               ec11Step(EC11_STEP);
//               eEndEdge = B_RISING;
//           } else {
//               ePreEdge = B_RISING;
//           }
//       }
//    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line6) != RESET)  
    {
        EXTI_ClearITPendingBit(EXTI_Line6);
        GsHallTim = TIM3->CNT;
        TIM3->CNT = 0;
    }
    else if (EXTI_GetITStatus(EXTI_Line7) != RESET)  
    {        
        EXTI_ClearITPendingBit(EXTI_Line7);
        GsHallTim = TIM3->CNT;
        TIM3->CNT = 0;
    }
}
#endif
