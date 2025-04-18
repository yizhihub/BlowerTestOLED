/*********************************************Copyright (c)***********************************************
**                                 Yuwell Technology Co., Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               HwDrv.c
** Last modified Date:      2023-07-25
** Last Version:            1.0
** Description:             主函数文件
**
**--------------------------------------------------------------------------------------------------------
** Created By:              liguagndao
** Created date:            2023-07-25
** Version:                 1.0
** Descriptions:            The original version 初始版本
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/
#include "sys.h"
#include "HwDrv.h"


INT32U  GulEscReoprtT1msCnt = 0;
INT32U  GulLapReoprtT1msCnt = 0;
INT32U GulPrintTimeCnt = 0;
INT32U GulEc11IntervalCnt = 0;


extern INT8U   GucT20msFlg;
extern INT32U  GulThrt1MsCnt;
extern INT32U  GulCtnBlockTmsCnt;
extern INT32U  GucUartRecT1msCnt;
extern INT32U  GulDelayT1msCnt;

INT8U __GucLedMode  = LED_MODE_ON;                                      /* LED工作模式                  */

extern INT32U  GulPrintT1msCnt;


/*********************************************************************************************************
** Function name:           watchdog_init
** Descriptions:            看门狗初始化
** input parameters:        time ：看门狗倒计时时间 (0.1ms ~ 409.6ms 最小单位0.1ms)
** output parameters:       none
** Returned value:          none
** Created by:              Cao Zhongyuan
** Created Date:            2017-10-11
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**
*********************************************************************************************************/
void hwdInit(uint16_t usTimeMs)
{
}

/*********************************************************************************************************
** Function name:           watchdog_enable
** Descriptions:            使能看门狗
** input parameters:        none
** output parameters:       none
** Returned value:          none
** Created by:              Cao Zhongyuan
** Created Date:            2017-10-11
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**
*********************************************************************************************************/
void hwdEnable(void)
{
}
/*********************************************************************************************************
** Function name:           watchdog_enable
** Descriptions:            使能看门狗
** input parameters:        none
** output parameters:       none
** Returned value:          none
** Created by:              Cao Zhongyuan
** Created Date:            2017-10-11
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**
*********************************************************************************************************/
void hwdDisable(void)
{
}

/*********************************************************************************************************
** Function name:           sysTickCallBack
** Descriptions:            sysTick 中断回调函数
** input parameters:        none
** output parameters:       none
** Returned value:          none
** Created by:              Fan Linyun
** Created Date:            2014-07-05
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**
*********************************************************************************************************/
//void  SysTick_Handler (void)
//{
//    static  INT32U  GulCnt = 0;
//    
//    GulPrintT1msCnt++; 
//    
//    GtThrt.ulOneMsDelta++;
//    
//    GulThrt1MsCnt++;
//    
//    GulCtnBlockTmsCnt++;      
//    GulSpeedCheckT1msCnt++;
//    GucUartRecT1msCnt++;
//    GulDelayT1msCnt++;  
//    
//    if (++GulCnt >= 20) {
//        GulCnt    = 0;
//        GucT20msFlg = 1;
//    }
//}

/**
********************************************************************************************************
** @nameis TIM2_IRQHandler
** @effect software delay 
** @import ulDly: time to delay 
** @export none
** @return void
** @create yizhi 2023.06.23
** @modify 
*********************************************************************************************************/
 void TIM2_IRQHandler(void) 
 {
      if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
      
          TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
          GulEscReoprtT1msCnt++;
          GulLapReoprtT1msCnt++;
          GulPrintTimeCnt++;
          GulEc11IntervalCnt++;
//          LED1_TOG();
      }
 }
 
 
/**
********************************************************************************************************
** @nameis TIM3_IRQHandler
** @effect software delay 
** @import ulDly: time to delay 
** @export none
** @return void
** @create yizhi 2023.06.23
** @modify 
*********************************************************************************************************/
 INT16U usTim3Cnt, usTim3Ccr1;
 INT32U GusHallSpd;
 extern INT8U  GucPolePair;
 static INT8U  ucHallState, ucHallValue = 0;
 static INT32U ulHallSampSum = 0;
 static INT32U ulHallAngSum[6] = {0};
 static INT32U q32HallAngTab[6] = {HALL_DEFAULT_ANG, HALL_DEFAULT_ANG, HALL_DEFAULT_ANG, HALL_DEFAULT_ANG, HALL_DEFAULT_ANG, HALL_DEFAULT_ANG};
 static INT16U usHallSampCnt = 0;

 
 void HallCalibrationEn(void)
 {
     ucHallState = 1;
     ulHallSampSum = 0;
     memset(ulHallAngSum, 0, sizeof(ulHallAngSum));
 }
 
 void TIM3_IRQHandler(void) 
 {
    int i;
    if(TIM_GetITStatus(TIM3,TIM_IT_CC1)!=RESET)
      {
          TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);  //清除触发中断标志位
          LED0_TOG();
          ucHallValue = MC_FOC_HALL_PU1;
          usTim3Ccr1 = TIM3->CCR1;
          switch (ucHallState)
          {
              case 0: GusHallSpd  = ((long long )q32HallAngTab[ucHallValue-1]*2000000*360) / (usTim3Ccr1*6) / 0xFFFFFFFF / GucPolePair;
                      break;
              
              case 1: if (usHallSampCnt/6 < 100)
                      {
                          ulHallAngSum[ucHallValue-1] += usTim3Ccr1;
                          ulHallSampSum += usTim3Ccr1;
                          usHallSampCnt++;
                      }
                      else
                      {
                          usHallSampCnt = 0;
                          ucHallState = 2;
                      }
                      break;
              
              case 2: for(i = 0; i < 6; i++)
                      {
                          q32HallAngTab[i] = (long long)ulHallAngSum[i] * 0xFFFFFFFF / ulHallSampSum; 
                      }
                      ucHallState = 0;
                      break;
                      
              default:break;
          }
      }
      
      if(TIM_GetITStatus(TIM3, TIM_IT_Update)!=RESET)
      {
          TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除触发中断标志位
          GusHallSpd = 0;
      }
 }
/*********************************************************************************************************
** Function name:           ledInit
** Descriptions:            LED 初始化函数
** input parameters:        none
** output parameters:       none
** Returned value:          none
** Created by:              lgd
** Created Date:             
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**
*********************************************************************************************************/
void  ledInit (void)
{
    /*
     * LED GPIO配置
     */
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOC,&GPIO_InitStructure);
}

/*********************************************************************************************************
** Function name:           ledSet
** Descriptions:            LED 模式设置函数
** input parameters:        none
** output parameters:       none
** Returned value:          none
** Created by:              Hong Haogao
** Created Date:            2015-09-25  
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**
*********************************************************************************************************/
void  ledSet (INT8U  ucLedMode) 
{
    __GucLedMode = ucLedMode; 
}

/*********************************************************************************************************
** Function name:           ledTask
** Descriptions:            LED 任务处理函数  —— 每20ms循环调用
** input parameters:        none
** output parameters:       none
** Returned value:          none
** Created by:              Hong Haogao
** Created Date:            2015-09-25  
**--------------------------------------------------------------------------------------------------------
** Modified by:             Zhou Weiwei
** Modified date:           2016-01-07
**
*********************************************************************************************************/
void  ledTask (void)
{
    static INT32U  GulCnt = 0;
    
    switch (__GucLedMode) {
        
    case LED_MODE_ON:                                                   /* LED模式，常亮                */
        LED1_ON();
        break;
    case LED_MODE_OFF:                                                  /* LED模式, 常灭               */
        LED1_OFF();
        break;    
    case LED_MODE_SLOW:                                                 /* LED模式，慢闪 周期1s         */
        if (++GulCnt >= 25) {
            GulCnt = 0;
            LED1_TOG();
        }
        break;
        
    case LED_MODE_FAST:                                                 /* LED模式，快闪 周期320ms      */
        if (++GulCnt >= 8) {
            GulCnt = 0;
            LED1_TOG();
        }
        break;
 
    case LED_SPECIAL_1:                                                 /* LED模式，闪一次，常灭1秒 ZWW */
        GulCnt ++;
        if (GulCnt <= 5) {
            LED1_ON();
        } else if (GulCnt <= (5 + 75)) {
            LED1_OFF();
        } else {
            GulCnt = 0;
        }
        break;
    
    case LED_SPECIAL_2:                                                 /* LED模式，闪两次，常灭1秒 ZWW */
        GulCnt ++;
        if (GulCnt <= 5) {
            LED1_ON();
        } else if (GulCnt <= 15) {
            LED1_OFF();
        } else if (GulCnt <= 25) {
            LED1_ON();
        } else if (GulCnt <= (25 + 75)) {
            LED1_OFF();
        } else {
            GulCnt = 0;
        }
        break;

    case LED_SPECIAL_3:                                                 /* LED模式，闪三次，常灭1秒 ZWW */
        GulCnt ++;
        if (GulCnt <= 5) {
            LED1_ON();
        } else if (GulCnt <= 15) {
            LED1_OFF();
        } else if (GulCnt <= 20) {
            LED1_ON();
        } else if (GulCnt <= 30) {
            LED1_OFF();
        } else if (GulCnt <= 35) {
            LED1_ON();
        } else if (GulCnt <= (35 + 75)) {
            LED1_OFF();
        } else {
            GulCnt = 0;
        }
        break;
        
    default:
        LED1_ON();
        break;
    }
}

void timInit(void)
{
    TIM_TimeBaseInitTypeDef    tTimeBaseConfig;
    GPIO_InitTypeDef  GPIO_InitStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    /*
     * 初始化一个定时器， 用于产生一个1ms的时间基准
     */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_DeInit(TIM2);
    TIM_InternalClockConfig(TIM2);

    tTimeBaseConfig.TIM_Prescaler     = 72 - 1;                 /* Time Unit  1000ns                         */
    tTimeBaseConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    tTimeBaseConfig.TIM_CounterMode   = TIM_CounterMode_Up; 
    tTimeBaseConfig.TIM_Period        = 1000; 
    tTimeBaseConfig.TIM_RepetitionCounter = 0;                  
    TIM_TimeBaseInit(TIM2, &tTimeBaseConfig);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

    /* 
     *  初始化一个定时器，用于产生一个400Hz， 1ms~2ms的脉冲信号 
     *  测试平台V1.0 使用PA6、测试平台V2.0使用PA8
     */
     
    /*PA8  TIM1*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP; 
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_DeInit(TIM1);
    TIM_InternalClockConfig(TIM1);

    tTimeBaseConfig.TIM_Prescaler     = 72 - 1;                 /* Time Unit  1000ns    */
    tTimeBaseConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    tTimeBaseConfig.TIM_CounterMode   = TIM_CounterMode_Up; 
    tTimeBaseConfig.TIM_Period        = 2500 - 1; 
    tTimeBaseConfig.TIM_RepetitionCounter = 0;                  
    TIM_TimeBaseInit(TIM1, &tTimeBaseConfig);
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

    TIM_CtrlPWMOutputs(TIM1,ENABLE);          //MOE 主输出使能
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
//    TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
    
    TIM_SetCompare1(TIM1,1000);
    
    /* 开启所有定时器 */
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    
    NVIC_DisableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);                                 /* 设置中断优先级               */
    NVIC_EnableIRQ(TIM2_IRQn);

}
void HallGpioInit(void)
{
    GPIO_InitTypeDef   GPIO_InitStructure;
//    EXTI_InitTypeDef   EXIT_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
//    EXIT_InitStructure.EXTI_Line = EXTI_Line0;
//    EXIT_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXIT_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//    EXIT_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXIT_InitStructure);
//    
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);
//    EXIT_InitStructure.EXTI_Line = EXTI_Line6;
//    EXIT_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXIT_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//    EXIT_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXIT_InitStructure);
//    
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource7);
//    EXIT_InitStructure.EXTI_Line = EXTI_Line7;
//    EXIT_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXIT_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//    EXIT_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXIT_InitStructure);
//    
//    EXTI_ClearITPendingBit(EXTI_Line0);
//    NVIC_DisableIRQ(EXTI0_IRQn); 
//    NVIC_SetPriority(EXTI0_IRQn, 0);
//    NVIC_EnableIRQ(EXTI0_IRQn);
//    
//    EXTI_ClearITPendingBit(EXTI_Line6);
//    NVIC_DisableIRQ(EXTI9_5_IRQn);
//    NVIC_SetPriority(EXTI9_5_IRQn, 0);
//    NVIC_EnableIRQ(EXTI9_5_IRQn);
//    
//    EXTI_ClearITPendingBit(EXTI_Line7);
//    NVIC_DisableIRQ(EXTI9_5_IRQn); 
//    NVIC_SetPriority(EXTI9_5_IRQn, 0);
//    NVIC_EnableIRQ(EXTI9_5_IRQn);
    
}
void TIM3HallCapInit(void)
{
    TIM_TimeBaseInitTypeDef  tTimeBaseConfig;
    TIM_ICInitTypeDef        TIM3_ICInitStructure;
    
    HallGpioInit();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM_DeInit(TIM3);
    TIM_InternalClockConfig(TIM3);

    tTimeBaseConfig.TIM_Prescaler     = 36 - 1;                 /* Time Unit  1000ns                         */
    tTimeBaseConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    tTimeBaseConfig.TIM_CounterMode   = TIM_CounterMode_Up; 
    tTimeBaseConfig.TIM_Period        = 0xFFFF; 
    tTimeBaseConfig.TIM_RepetitionCounter = 0;                  
    TIM_TimeBaseInit(TIM3, &tTimeBaseConfig);
    
    TIM3_ICInitStructure.TIM_Channel     = TIM_Channel_1;
    TIM3_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_BothEdge;    //上升沿捕获TIM_ICPolarity_BothEdge
    TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC; //映射到TI1上
    TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;     //配置输入分频,不分频 
    TIM3_ICInitStructure.TIM_ICFilter    = 0x05;//IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(TIM3, &TIM3_ICInitStructure);
//    
//    TIM3_ICInitStructure.TIM_Channel     = TIM_Channel_2;
//    TIM3_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_BothEdge;    //上升沿捕获TIM_ICPolarity_BothEdge
//      TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI; //映射到TI1上
//      TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;     //配置输入分频,不分频 
//      TIM3_ICInitStructure.TIM_ICFilter    = 0x05;//IC1F=0000 配置输入滤波器 不滤波
//    TIM_ICInit(TIM3, &TIM3_ICInitStructure);
//    
//    TIM3_ICInitStructure.TIM_Channel     = TIM_Channel_3;
//    TIM3_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_BothEdge;    //上升沿捕获TIM_ICPolarity_BothEdge
//      TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI; //映射到TI1上
//      TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;     //配置输入分频,不分频 
//      TIM3_ICInitStructure.TIM_ICFilter    = 0x05;//IC1F=0000 配置输入滤波器 不滤波
//    TIM_ICInit(TIM3, &TIM3_ICInitStructure);

    NVIC_DisableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 3);                                 /* 设置中断优先级               */
    NVIC_EnableIRQ(TIM3_IRQn);
    
    TIM_SelectHallSensor(TIM3, ENABLE);
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1F_ED);
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable); 

    TIM_UpdateRequestConfig(TIM3, TIM_UpdateSource_Regular);
    TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_CC1, ENABLE);
    
    TIM_Cmd(TIM3, ENABLE);
    
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update | TIM_IT_CC1);
}


/**
********************************************************************************************************
** @nameis msDelay
** @effect software delay 
** @import ulDly: time to delay 
** @export none
** @return void
** @create yizhi 2023.06.23
** @modify 
*********************************************************************************************************/
/**
 *  74990      @ RT1052 SDK.
 *  74990 * 2  @ RT1052 RamFly
 *  7374       @ LKS08x
 *  8500       @ GD32F103
 *  5000       @ STM32F103
 *  9226       @ GD32F303 
 */
void  msDelay(uint32_t  ulDly)
{
    volatile uint32_t i= 0, j = 0;
    for(j = 0; j < ulDly; j++) 
    {
        for (i = 0; i < (5000); ++i)
        {
            __asm("NOP"); /* delay */
        }
    }
}

void  usDelay(uint32_t  ulDly)
{
    volatile uint32_t i= 0, j = 0;
    for(j = 0; j < ulDly; j++) 
    {
        for (i = 0; i < (8); ++i)
        {
            __asm("NOP"); /* delay */
        }
    }
}

/*********************************************************************************************************
** Function name:           fputc
** Descriptions:            retarget the C library printf function to the USART
** input parameters:        none
** output parameters:       none
** Returned value:          none
** Created by:              lgd 
** Created Date:            2017-10-10
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**
*********************************************************************************************************/
//int fputc(int ch, FILE *f)
//{
//    USART_SendData(USART3, (uint8_t)ch);
//    while(0 == USART_GetFlagStatus(USART3, USART_FLAG_TXE));

//    return ch;
//}

/*********************************************************************************************************
** Function name:           hwInit
** Descriptions:            硬件初始化函数
** input parameters:        none
** output parameters:       none
** Returned value:          none
** Created by:              Hong Haogao
** Created Date:            2015-09-28
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
**
*********************************************************************************************************/
void  hwInit (void)
{
    __disable_irq();                                          /* 全局中断禁能                  */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);// 设置中断优先级分组2
    
    msDelay(50);
//    NVIC_SetPriority(TIM2_IRQn, 3); 
//    NVIC_EnableIRQ(TIM2_IRQn);        
//    NVIC_SetPriority(USART1_IRQn, 4);                                   /* 设置优先级中断               */
//    NVIC_EnableIRQ(USART1_IRQn);  
    

//    SysTick_Config(120000000 / 1000);                                   /* SysTick初始化 1ms中断时间     */
}


/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
