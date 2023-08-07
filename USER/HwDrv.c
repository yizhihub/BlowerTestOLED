/*********************************************Copyright (c)***********************************************
**                                 Yuwell Technology Co., Ltd.
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               HwDrv.c
** Last modified Date:      2023-07-25
** Last Version:            1.0
** Description:             �������ļ�
**
**--------------------------------------------------------------------------------------------------------
** Created By:              liguagndao
** Created date:            2023-07-25
** Version:                 1.0
** Descriptions:            The original version ��ʼ�汾
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


INT32U GulEscReoprtT1msCnt = 0;
INT32U GulPrintTimeCnt = 0;
INT32U GulEc11IntervalCnt = 0;


extern INT8U   GucT20msFlg;
extern INT32U  GulThrt1MsCnt;
extern INT32U  GulCtnBlockTmsCnt;
extern INT32U  GucUartRecT1msCnt;
extern INT32U  GulDelayT1msCnt;

INT8U __GucLedMode  = LED_MODE_ON;                                      /* LED����ģʽ                  */

extern INT32U  GulPrintT1msCnt;


/*********************************************************************************************************
** Function name:           watchdog_init
** Descriptions:            ���Ź���ʼ��
** input parameters:        time �����Ź�����ʱʱ�� (0.1ms ~ 409.6ms ��С��λ0.1ms)
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
** Descriptions:            ʹ�ܿ��Ź�
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
** Descriptions:            ʹ�ܿ��Ź�
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
** Descriptions:            sysTick �жϻص�����
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
 void TIM3_IRQHandler(void) 
 {
      if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
      
          TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//          GulEscReoprtT1msCnt++;
//          GulPrintTimeCnt++;
//          LED0_TOG();
      }
 }

/*********************************************************************************************************
** Function name:           ledInit
** Descriptions:            LED ��ʼ������
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
     * LED GPIO����
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
** Descriptions:            LED ģʽ���ú���
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
** Descriptions:            LED ��������  ���� ÿ20msѭ������
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
        
    case LED_MODE_ON:                                                   /* LEDģʽ������                */
        LED1_ON();
        break;
    case LED_MODE_OFF:                                                  /* LEDģʽ, ����               */
        LED1_OFF();
        break;    
    case LED_MODE_SLOW:                                                 /* LEDģʽ������ ����1s         */
        if (++GulCnt >= 25) {
            GulCnt = 0;
            LED1_TOG();
        }
        break;
        
    case LED_MODE_FAST:                                                 /* LEDģʽ������ ����320ms      */
        if (++GulCnt >= 8) {
            GulCnt = 0;
            LED1_TOG();
        }
        break;
 
    case LED_SPECIAL_1:                                                 /* LEDģʽ����һ�Σ�����1�� ZWW */
        GulCnt ++;
        if (GulCnt <= 5) {
            LED1_ON();
        } else if (GulCnt <= (5 + 75)) {
            LED1_OFF();
        } else {
            GulCnt = 0;
        }
        break;
    
    case LED_SPECIAL_2:                                                 /* LEDģʽ�������Σ�����1�� ZWW */
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

    case LED_SPECIAL_3:                                                 /* LEDģʽ�������Σ�����1�� ZWW */
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
     * ��ʼ��һ����ʱ���� ���ڲ���һ��1ms��ʱ���׼
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
     *  ��ʼ��һ����ʱ�������ڲ���һ��400Hz�� 1ms~2ms�������ź� 
     */
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP; 
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_DeInit(TIM3);
    TIM_InternalClockConfig(TIM3);

    tTimeBaseConfig.TIM_Prescaler     = 72 - 1;                 /* Time Unit  1000ns                         */
    tTimeBaseConfig.TIM_ClockDivision = TIM_CKD_DIV1;
    tTimeBaseConfig.TIM_CounterMode   = TIM_CounterMode_Up; 
    tTimeBaseConfig.TIM_Period        = 2500; 
    tTimeBaseConfig.TIM_RepetitionCounter = 0;                  
    TIM_TimeBaseInit(TIM3, &tTimeBaseConfig);
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
   
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
    
    
    TIM_SetCompare1(TIM3,1000);
    
    NVIC_DisableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 1);                                 /* �����ж����ȼ�               */
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_DisableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn, 2);                                 /* �����ж����ȼ�               */
    NVIC_EnableIRQ(TIM3_IRQn);
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
        for (i = 0; i < (9226); ++i)
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
** Descriptions:            Ӳ����ʼ������
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
    __disable_irq();                                          /* ȫ���жϽ���                  */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);// �����ж����ȼ�����2
    
    msDelay(50);
//    NVIC_SetPriority(TIM2_IRQn, 3); 
//    NVIC_EnableIRQ(TIM2_IRQn);        
//    NVIC_SetPriority(USART1_IRQn, 4);                                   /* �������ȼ��ж�               */
//    NVIC_EnableIRQ(USART1_IRQn);  
    

//    SysTick_Config(120000000 / 1000);                                   /* SysTick��ʼ�� 1ms�ж�ʱ��     */
}


/*********************************************************************************************************
  End Of File
*********************************************************************************************************/
