/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               bSDP800.h
** Created By  :            Song.wj
** Created Date:            2023-10-13
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

#include "./Component/i2c2.c"
#include "bSDP800.h"
#include "bOLED.h"
#include "bKEY.h"

static uint8_t crc8(uint8_t *ptr, uint8_t len)
{
    uint8_t i;
    uint8_t crc = 0xFF;
    
    while (len--)
    {
        crc ^= *ptr++;
        for (i = 8; i > 0; --i)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    return (crc);
}



//static uint8_t SDP800_DataCrc(uint8_t *ptr, uint8_t len)
//{
//    if (crc8(ptr, len - 1) == *(ptr+len))
//        return 1;
//    else
//        return 0;
//}

static void MeasureMode_Set(uint16_t usMode)
{
    i2cStart();
    i2cWriteByte(SDP800_500_ADDRW);
    i2cWriteByte((uint8_t)(usMode>>8));
    i2cWriteByte((uint8_t)usMode);
    i2cStop();
}

static int16_t SDP800_ReadDiffPres(void)
{
    uint8_t cData[3];
    int16_t sDiffPres;
    INT16S i;
    i2cStart();
    i2cWriteByte(SDP800_500_ADDRR);
    
    cData[0] = i2cReadByte(1);
    cData[1] = i2cReadByte(1);
    cData[2] = i2cReadByte(0);

    i2cStop();
    
//    if (crc8(cData, 2) == cData[2])
//    {
        sDiffPres = (cData[0] << 8) + cData[1];
//    }
    return sDiffPres;
}

static void SDP800_ReadAll(uint8_t *pTemp)
{
    INT16S i;
    i2cStart();
    i2cWriteByte(SDP800_500_ADDRR);
    for (i = 0; i < 8; i++)
    {
        *pTemp = i2cReadByte(1);
        pTemp++;
    }
    *pTemp = i2cReadByte(0);
    i2cStop();
}

void SDP800_Init(void)
{
#ifdef GD32F30X_HD
    /*
     * GPIOÅäÖÃ
     */
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
    MeasureMode_Set(CONTINUOUSMODE3);
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
void SDP800_TEST(uint8_t ucX)
{
    INT16S sBraoAdcRaw;
    uint8_t ul10msCnt = 0;
    uint8_t cData[9];
    int16_t sTemperature, sSclFct, sFiffPre = 0;
    OLED_Fill(0x00);
//    OLED_PutStr(0, OLED_LINE3,     (uint8_t *)"Cali", 6, 1);
//    OLED_PutStr(0, OLED_LINE3 + 1, (uint8_t *)"Raw", 6, 1);

    OLED_Print(0, OLED_LINE0, "ÎÂ¶È:", 1);
    OLED_Print(0, OLED_LINE1, "SclFct:", 1);
    OLED_Print(0, OLED_LINE2, "Ñ¹Á¦:", 1);
    
    SDP800_Init();
    msDelay(100);
    SDP800_ReadAll(cData);
    
    if (crc8(&cData[0], 2) == cData[2])
    {
        sFiffPre = (cData[0] << 8) + cData[1];
    }
    OLED_PutNum(40, OLED_LINE2, sFiffPre, 5, 8, 1);
    
    if (crc8(&cData[3], 2) == cData[5])
    {
        sTemperature = (cData[3] << 8) + cData[4];
    }
    OLED_PutNum(40, OLED_LINE0, sTemperature, 5, 8, 1);
    
    if (crc8(&cData[6], 2) == cData[8])
    {
        sSclFct = (cData[6] << 8) + cData[7];
    }
    OLED_PutNum(56, OLED_LINE1, sSclFct, 5, 8, 1);
//    FLASH_Read(BRAOCALIB_ADDR, (uint16_t*)&_GsBraoAdcOffset, 1);
//    if ((uint8_t)_GsBraoAdcOffset == 0xFF && (uint8_t)(_GsBraoAdcOffset>>8) == 0xFF)
//    {
//        BraoCalibrate();
//    }
//    OLED_PutNum(24, OLED_LINE3, _GsBraoAdcOffset, 4, 6, 1);

#if 1
    while(ADKey_Scan()!=KEY_CANCEL) 
    {
        MeasureMode_Set(0x0006);
        msDelay(20);
        MeasureMode_Set(CONTINUOUSMODE3);
        msDelay(100);
        sFiffPre = SDP800_ReadDiffPres();
//        sFiffPre = 0;
        OLED_PutNum(40, OLED_LINE2, sFiffPre, 5, 8, 1);
        
    }
#else
    while(ADKey_Scan()!=KEY_CANCEL) {
        if (GulPrintTimeCnt > 10) 
        { 
            GulPrintTimeCnt = 0;
            
            if (ul10msCnt++ > 10) 
            {
                ul10msCnt = 0;
                SDP800_ReadDiffPres(sFiffPre);
                OLED_PutNum(40, OLED_LINE2, sFiffPre, 5, 8, 1);
            }
            if (ADKey_Scan() == KEY_DOWN)
            {
//                OLED_PutNum(24, OLED_LINE3, _GsBraoAdcOffset, 4, 6, 1);
//                FLASH_Write(BRAOCALIB_ADDR, (uint16_t*)&_GsBraoAdcOffset, 1);
            }
        }
    }
#endif
}