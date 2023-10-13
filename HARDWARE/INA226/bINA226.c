/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               INA226.h
** Created By  :            Song.wj
** Created Date:            2023-09-01
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

//#include "./Component/iiC.h"
#include "./Component/i2c1.c"
#include "bINA226.h"
#include "bFLASH.h"
#include "bKey.h"
#include "bOLED.h"


FloatToHex _GfRsINA226;
float _GfRsINA226Fresh = 0.0;
/**
********************************************************************************************************
** @nameis INA226_init
** @effect INA226 pin initilize & config mode and calibration
** @import none
** @export none
** @return void
** @create
** @modify 
*********************************************************************************************************/
void INA226_Init(void)
{
    uint16_t  ulCalibrateValue = 0;
#ifdef GD32F30X_HD
    /*
     * KEY GPIO配置
     */
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;                
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
    i2cInit();
    INA226_Config(0x4727);                                        /*  64 averages 1.1ms convert time */
    
    FLASH_Read(INA226_ADDR, (uint16_t*)&_GfRsINA226.hdata[0], 2);
    if (_GfRsINA226.hdata[0] == 0xFF && _GfRsINA226.hdata[1] == 0xFF)
    {
        _GfRsINA226.fdata = CALIBRATION_INDEX;
    }
    if (_GfRsINA226Fresh)
    {
        if (_GfRsINA226Fresh != _GfRsINA226.fdata)
            _GfRsINA226.fdata = _GfRsINA226Fresh;
    }
    ulCalibrateValue = (5120.0f / (CURRENT_LSB * RSHUNT * _GfRsINA226.fdata));
    
    Write_Calibrt_reg(ulCalibrateValue);
}

/**
********************************************************************************************************
** @nameis INA226_config
** @effect config INA226 number of averages, VBus/VShunt conversion time and Mode
** @import none
** @export none
** @return void
** @create
** @modify 
*********************************************************************************************************/
void INA226_Config(uint16_t config_data)
{
    uint8_t temp = 0;
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR);    
    i2cWriteByte(CONFIG_REG_ADDR);
    
    temp = (uint8_t)(config_data >> 8);
    i2cWriteByte(temp);
    
    temp = (uint8_t)(config_data & 0x00FF);
    i2cWriteByte(temp);
    
    i2cStop();
}

/**
********************************************************************************************************
** @nameis Write_Calibrt_reg
** @effect config INA226 Calibration index(Cal = 0.00512/(Current_LSB*Rshunt))
** @import none
** @export none
** @return void
** @create
** @modify 
*********************************************************************************************************/
void Write_Calibrt_reg(uint16_t cal)
{
    uint8_t temp = 0;
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR);    
    i2cWriteByte(CALIBRT_REG_ADDR);
    
    temp = (uint8_t)(cal >> 8);
    i2cWriteByte(temp);
    
    temp = (uint8_t)(cal & 0x00FF);
    i2cWriteByte(temp);
    
    i2cStop();
}

/**
********************************************************************************************************
** @nameis Read_VShunt
** @effect Read VShunt value (mV)
** @import none
** @export none
** @return float
** @create
** @modify 
*********************************************************************************************************/
float Read_VShunt(void)
{
    uint16_t voltage;
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR);
    i2cWriteByte(VSHUNT_REG_ADDR);
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR+0x01); // 先读后写
    voltage = i2cReadByte(1);
    voltage <<= 8;
    voltage |= i2cReadByte(0);
    i2cStop();
    return (voltage * VSHUNT_LSB / 1000);
}

/**
********************************************************************************************************
** @nameis Read_VBus
** @effect Read VBus value (V)
** @import none
** @export none
** @return float
** @create
** @modify 
*********************************************************************************************************/
float Read_VBus(void)
{
    uint16_t voltage;
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR);
    i2cWriteByte(VBUS_REG_ADDR);
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR+0x01); // 先读后写
    voltage = i2cReadByte(1);
    voltage <<= 8;
    voltage |= i2cReadByte(0);
    i2cStop();
    return (voltage * VBUS_LSB / 1000);
}

/**
********************************************************************************************************
** @nameis Read_Power
** @effect Read Power value (W)
** @import none
** @export none
** @return float
** @create
** @modify 
*********************************************************************************************************/
float Read_Power(void)
{
    uint16_t power;
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR);
    i2cWriteByte(POWER_REG_ADDR);
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR+0x01); // 先读后写
    power = i2cReadByte(1);
    power <<= 8;
    power |= i2cReadByte(0);
    i2cStop();
    return (power * POWER_LSB / 1000);
}

/**
********************************************************************************************************
** @nameis Read_Current
** @effect Read Current value (A)
** @import none
** @export none
** @return float
** @create
** @modify 
*********************************************************************************************************/
float Read_Current(void)
{
    uint16_t current;
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR);
    i2cWriteByte(CURRENT_REG_ADDR);
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR+0x01); // 先读后写
    current = i2cReadByte(1);
    current <<= 8;
    current |= i2cReadByte(0);
    i2cStop();
    return (current * CURRENT_LSB / 1000);
}

/**
********************************************************************************************************
** @nameis INA226_Read
** @effect Read Vbus and Current value(10mv, 1mA)
** @import none
** @export pusVol10mv : Vbus(10mV), Current(1mA)
** @return void
** @create 
** @modify 
*********************************************************************************************************/
void INA226_Read(INT16U *pusVol10mv, INT16S *psCur1mA)
{
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR);
    i2cWriteByte(VBUS_REG_ADDR);
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR+0x01); // 先读后写
    *pusVol10mv = i2cReadByte(1);
    *pusVol10mv <<= 8;
    *pusVol10mv |= i2cReadByte(0);
    i2cStop();
    *pusVol10mv >>= 3;

    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR);
    i2cWriteByte(CURRENT_REG_ADDR);
    i2cStart();
    i2cWriteByte(INA226_DEV_ADDR+0x01); // 先读后写
    *psCur1mA = i2cReadByte(1);
    *psCur1mA <<= 8;
    *psCur1mA |= i2cReadByte(0);
    i2cStop();
}

/**
********************************************************************************************************
** @nameis INA226Test
** @effect Read Vbus and Current value(10mv, 1mA)
** @import none
** @export pusVol10mv : Vbus(10mV), Current(1mA)
** @return void
** @create 
** @modify 
*********************************************************************************************************/
static uchar bScreenIDFlg = 1, bScreenValFlg = 0, bEdit_flag   = 0;
void INA226Test(uint8_t ucX)
{
    KEYn_e eKeyPress;
    INT16U usVol;
    INT16S sCur;
    uint8_t ul10msCnt = 0;
    OLED_Fill(0x00);
    
    INA226_Init();
    OLED_PutStr(0, OLED_LINE1, "CalibInd", 8, 1);
    OLED_PutNumber(72, OLED_LINE1, _GfRsINA226.fdata, 2, 4, 0, 8, 1);
    while(ADKey_Scan()!=KEY_CANCEL) 
    {
        if (GulPrintTimeCnt > 10) 
        { 
            GulPrintTimeCnt = 0;
            
            if (ul10msCnt++ > 10) 
            {
                ul10msCnt = 0;
                INA226_Read(&usVol, &sCur);
                OLED_PutNumber(0,  OLED_LINE0, usVol / 100.0f, 2, 2, "V", 8, 1);
                OLED_PutNumber(72, OLED_LINE0, sCur / 1000.0f, 2, 3, "A", 8, 1);
            }
            if (bScreenValFlg)
            {
                bScreenValFlg = 0;
                OLED_PutNumber(72, OLED_LINE1, _GfRsINA226.fdata, 2, 4, 0, 8, 0);
            }
            eKeyPress = ADKey_Check();
            switch (eKeyPress) 
            {
                case EC11_CW:
                    if (!bEdit_flag)
                    {
                        GsEc11CntCW = 0;
                    }
                    else
                    {
                        _GfRsINA226.fdata *= 10000;
                        _GfRsINA226.fdata -= (GsEc11CntCW + 1);
                        if(_GfRsINA226.fdata < 0)
                            _GfRsINA226.fdata = 0;
                        _GfRsINA226.fdata /= 10000;
                        _GfRsINA226Fresh = _GfRsINA226.fdata;
                        GsEc11CntCW = 0;
                        bScreenValFlg = 1;
                    }
                    break;
                case EC11_CCW:
                    if (!bEdit_flag)
                    {
                        GsEc11CntCCW = 0;
                    }
                    else
                    {
                        _GfRsINA226.fdata *= 10000;
                        _GfRsINA226.fdata += (GsEc11CntCCW + 1);
                        _GfRsINA226.fdata /= 10000;
                        _GfRsINA226Fresh = _GfRsINA226.fdata;
                        GsEc11CntCCW = 0;
                        bScreenValFlg = 1;
                    }
                    break;
                case EC11_SET:
                    if (bEdit_flag)
                    {
                        OLED_PutNumber(72, OLED_LINE1, _GfRsINA226.fdata, 2, 4, 0, 8, 1);
                        bEdit_flag   = 0;
                        INA226_Init();
                    }
                    else
                    {
                        bScreenValFlg = 1;
                        bEdit_flag   = 1;
                    }break;
                default:
                    break;
            }
        }
    }
    FLASH_Write(INA226_ADDR, (uint16_t*)&_GfRsINA226.hdata[0], 2);
    GsEc11CntCW = 0;
    GsEc11CntCCW = 0;
}
