#ifndef _bINA226_H_
#define _bINA226_H_

#include "stm32f10x.h"
#include "sys.h"
#include "HwDrv.h"

#define INA226_DEV_ADDR     0x80

#define CONFIG_REG_ADDR     0x00
#define VSHUNT_REG_ADDR     0x01
#define VBUS_REG_ADDR       0x02
#define POWER_REG_ADDR      0x03
#define CURRENT_REG_ADDR    0x04
#define CALIBRT_REG_ADDR    0x05

/* 
 * 下面校准数值按照电子负载输出2A，进行校准，比如设置电子负载输入2A，INA226显示2.01A，则此处应填写 1.005f 
 *  // #1 1.011    #2 1.008   #3  1.009  #4 1.0035 
 */
#define CALIBRATION_INDEX   (1.009f)
#define RSHUNT              (10)  //mΩ
#define MAX_CURRENT         (15) //A

#define VSHUNT_LSB          (2.5)   //μV
#define VBUS_LSB            (1.25)  //mV
#define CURRENT_LSB         (1)  //(MAX_CURRENT*1000) >> 15 mA
#define POWER_LSB           (CURRENT_LSB*25)

extern FloatToHex _GfRsINA226;

void INA226_Init(void);
void INA226_Config(uint16_t config_data);
void Write_Calibrt_reg(uint16_t cal);
float Read_VShunt(void);
float Read_VBus(void);
float Read_Power(void);
float Read_Current(void);
void INA226_Read(INT16U *pusVol10mv, INT16S *psCur1mA);
void INA226Test(uint8_t ucX);
#endif
