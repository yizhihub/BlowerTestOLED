/****************************************Copyright (c)****************************************************
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               bBraometerAD.c
** Last modified Date:      Read Braometer sensor
** Last Version:            V1.0
** Description:             read adc braometer value and transfer to bar
** Created By:             
** Created date:            
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
#include "bBraoAdc.h"
#include "bFLASH.h"
#include "bKey.h"
#include "bOLED.h"

/*********************************************************************************************************
  变量定义
*********************************************************************************************************/
INT16S _GsBraoAdcOffset = 0;


/*********************************************************************************************************
  内部函数声明
*********************************************************************************************************/
const uint16_t  BRAO_Table[] = {
    4086, //0    -55     (i * 5) - 55
    4084, //1    -50
    4082, //2    -45
    4079, //3    -40
    4075, //4    -35
    4069, //5    -30
    4062, //6    -25
    4052, //7    -20
    4040, //8    -15
    4023, //9    -10
    4002, //10    -5
    3976, //11    0
    3942, //12    5
    3901, //13    10
    3851, //14    15
    3790, //15    20
    3717, //16    25
    3632, //17    30
    3533, //18    35
    3421, //19    40
    3295, //20    45
    3155, //21    50
    3004, //22    55
    2843, //23    60
    2674, //24    65
    2500, //25    70
    2323, //26    75
    2147, //27    80
    1974, //28    85
    1807, //29    90
    1647, //30    95
    1495, //31    100
    1354, //32    105
    1223, //33    110
    1102, //34    115
    992 , //35    120
    892 , //36    125
    802 , //37    130
    721 , //38    135
    647 , //39    140
    582 , //40    145
    524 , //41    150
};

/**
*********************************************************************************************************
** @nameis BraoAdcInit
** @effect 
** @import 
** @export 
** @return 
** @create yizhi 2023.05.12
** @modify  
*********************************************************************************************************/
void BraoAdcInit(void)
{
    ADC_InitTypeDef    ADC_InitStruct; 
    GPIO_InitTypeDef   GPIO_InitStruct; 
     
    /* 
     * GD32F103CY   ESC            board ： PA2(ADC01_IN2) is the NTC sample channel 
     * GD32F303RET6 BlowerTestOLED boardV1.0 :  PB0(ADC01_IN8) is the Pressure sampler channel 
     * GD32F303RET6 BlowerTestOLED boardV2.0 :  PC5(ADC01_IN15) is the Pressure sampler channel 
     * GD32F303RET6 BlowerTestOLED board :  PC5(ADC01_IN15) is the pressuer sampler channel
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_5; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* 
     *  ADC1, ADC2 恢复默认值. 这个恢复默认操作必须否则每次上电校准值不一致。
     */
    ADC_DeInit(ADC2); 

    ADC_InitStruct.ADC_Mode               = ADC_Mode_Independent; 
    ADC_InitStruct.ADC_ScanConvMode       = DISABLE; 
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE; 
    ADC_InitStruct.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right; 
    ADC_InitStruct.ADC_NbrOfChannel       = 1; 
    ADC_Init(ADC2, &ADC_InitStruct); 
    
    ADC_Cmd(ADC2, ENABLE); 
    
    ADC_ResetCalibration(ADC2); 
    while(ADC_GetResetCalibrationStatus(ADC2)); 
    ADC_StartCalibration(ADC2); 
    while(ADC_GetCalibrationStatus(ADC2)); 
    
    ADC_RegularChannelConfig(ADC2, ADC_Channel_15, 1, ADC_SampleTime_239Cycles5);
}

/**
*********************************************************************************************************
** @nameis BraoAdcGet
** @effect get regulaer adc value
** @import none
** @export pusResult
** @return 
** @create yizhi 2023.05.12
** @modify  
*********************************************************************************************************/
void BraoAdcGet(int16_t *psResult)
{
    INT8U i;
    FP32  fUbusTemp = 0;
    
    /*
     * 母线电压ADC数据读取
     */
    for (i = 0; i <= 10; i++) {

        ADC_SoftwareStartConvCmd(ADC2, ENABLE); 
        while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC));    
        fUbusTemp += ADC_GetConversionValue(ADC2);                          /* 母线采集为 ADC2的通道4     */
    }
    /*
     * 转换为实际电压表示
     */
//    fUbusTemp = (FP32)(((FP64)fUbusTemp / 4096.0 / 50.0) * 3.3 * fUratio);
    fUbusTemp = ((fUbusTemp + 5.5f) / 11.0f);
    *psResult  = (INT16S)fUbusTemp;                                                                    
}

/**
*********************************************************************************************************
** @nameis LinerInterpolationCalc
** @effect 根据采集的ADC值，查表并且线性插值计算出温度。
** @import uint16类型，采集的ADC数值，0~4095
** @export none
** @return int16类型， -550 ~ 1500表示 -55℃ ~ 150℃ 计算出来的温度值，实际温度*10。
** @create yizhi 2023.05.12
** @modify  
*********************************************************************************************************/
static int16_t LinerInterpolationCalc(uint16_t  usAdc)
{
    int8_t i;
    int16_t sTemp;
    
    if (usAdc > BRAO_Table[0]) {
        return -550;
    }
    if (usAdc <= BRAO_Table[41]) {
        return 1500;
    }
    
    for (i = 0; i < 41; i++) {
        if (usAdc <= BRAO_Table[i] && usAdc > BRAO_Table[i+1]) {
              sTemp = ((i * 5 - 55) + ((float)(BRAO_Table[i] - usAdc) / (BRAO_Table[i] - BRAO_Table[i+1])) * 5.0f) * 10;
              return sTemp;
        }
    }
    while(1);
    //assert(0);
}

/**
*********************************************************************************************************
** @nameis LinerFunctionCalc
** @effect 根据y = kx + b， x为ADC采集值， y为输出物理量。
** @import uint16类型，采集的ADC数值，0~4095
** @export none
** @return float类型，压力值单位 hPa
** @create yizhi 2023.09.2
** @modify  
*********************************************************************************************************/
float _GfLinerBraoK = 0.0187f;
float _GfLinerBrakB = 0; //-8.3273f; 

static float LinerFunctionCalc(int16_t sX)
{
    float fRslt;
    
    fRslt =  (_GfLinerBraoK * sX) + _GfLinerBrakB;
    
    return fRslt;
}

/**
*********************************************************************************************************
** @nameis BraoCalibrate
** @effect 
** @import 
** @export 
** @return 
** @create 
** @modify  
*********************************************************************************************************/
void BraoCalibrate(void)
{
    int8_t  i;
    INT16S  sTmp;
    INT32U  ulSumTmp = 0;

    _GsBraoAdcOffset = 0;
    BraoAdcGet(&sTmp); BraoAdcGet(&sTmp);
    BraoAdcGet(&sTmp); BraoAdcGet(&sTmp);
    BraoAdcGet(&sTmp); BraoAdcGet(&sTmp);
    BraoAdcGet(&sTmp); BraoAdcGet(&sTmp);
    
    for (i = 0; i < 10; i++) {
        BraoAdcGet(&sTmp);
        ulSumTmp += sTmp;
        msDelay(10);
    }
    _GsBraoAdcOffset = (INT16S)((ulSumTmp + 5.0f) / 10.0f); 
    //while(1);
    //assert(0);
}

/**
*********************************************************************************************************
** @nameis BraoGet
** @effect 
** @import 
** @export 
** @return 
** @create 
** @modify  
*********************************************************************************************************/
float BraoGet(void)
{
    INT16S  sAdc;
    float   fBraoHPa;
    
    BraoAdcGet(&sAdc);
    sAdc = sAdc - _GsBraoAdcOffset;
    fBraoHPa = LinerFunctionCalc(sAdc);
    return fBraoHPa;
}

/**
*********************************************************************************************************
** @nameis BraoCalibration
** @effect 
** @import 
** @export 
** @return 
** @create 
** @modify  
*********************************************************************************************************/
void BraoCalibration(uint8_t ucX)
{
    INT16S sBraoAdcRaw;
    uint8_t ul10msCnt = 0;
    OLED_Fill(0x00);
    
    OLED_PutStr(0, OLED_LINE3,     (uint8_t *)"Cali", 6, 1);
    OLED_PutStr(0, OLED_LINE3 + 1, (uint8_t *)"Raw", 6, 1);
    OLED_Print(50, OLED_LINE3, "压力:", 1);
    FLASH_Read(BRAOCALIB_ARRR, (uint16_t*)&_GsBraoAdcOffset, 1);
    if ((uint8_t)_GsBraoAdcOffset == 0xFF && (uint8_t)(_GsBraoAdcOffset>>8) == 0xFF)
    {
    BraoCalibrate();
    }
    OLED_PutNum(24, OLED_LINE3, _GsBraoAdcOffset, 4, 6, 1);
    
    while(ADKey_Scan()!=KEY_CANCEL) {
        if (GulPrintTimeCnt > 10) 
        { 
            GulPrintTimeCnt = 0;
            
            if (ul10msCnt++ > 10) 
            {
                ul10msCnt = 0;
         BraoAdcGet(&sBraoAdcRaw);
         OLED_PutNum(24, OLED_LINE3 + 1, sBraoAdcRaw, 4, 6, 1);
         OLED_PutNumber(86, OLED_LINE3, BraoGet(), 2, 1, "cmH", 6, 1);
    }
            if (ADKey_Scan() == KEY_DOWN)
            {
                BraoCalibrate();
                OLED_PutNum(24, OLED_LINE3, _GsBraoAdcOffset, 4, 6, 1);
                FLASH_Write(BRAOCALIB_ARRR, (uint16_t*)&_GsBraoAdcOffset, 1);
            }
        }
    }
    GsEc11CntCW = 0;
    GsEc11CntCCW = 0;
}

/********************************************************************************************************
  End Of File
********************************************************************************************************/
