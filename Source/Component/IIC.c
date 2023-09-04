/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               IIC.c
** Created By  :            yizhi
** Created Date:            2023-03-15
** Last Version:            v1.0 
** Descriptions:            include most hard ware init
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
#include "IIC.h"

static void usDelayOne(void)
{
    uint8_t i;
    for (i = 0; i < 10; i++) {      //  k < 5:  used 4 us @ 12.0000MHz
         ___NOP();                  // k < 4:  used 3.64 @ 12.0000MHz
        }                           // i < 4:  used 0.7~0.8us @ 精英版1.7 72.000MHz
}

void i2cInit(void)
{
    IIC_SCL_1;
    IIC_SDA_1;
    usDelayOne();
}
void i2cStart(void)
{
    IIC_SDA_1;
    IIC_SCL_1 ;
    usDelayOne();
    IIC_SDA_0;
    usDelayOne();
    IIC_SCL_0;                                                /* there might be a delay after pull scl down */
}
void i2cStop(void)
{
    IIC_SDA_0;
    IIC_SCL_1 ;
    usDelayOne();
    IIC_SDA_1;
    usDelayOne();
}

/**
********************************************************************************************************
** @nameis i2cRespons
** @effect waite and get the slave's response signal 
** @import none
** @export none
** @return 0 : slave responed others: slave not responsed
** @create yizhi 2023.03.20
** @modify 
*********************************************************************************************************/
uint8_t i2cRespons()
{
    uint8_t ucTimeOut = 0;
    /**
     * 1. The follow either OLED_SDIN_Set or usDelayOne(a longer delay),which is indispensable
     * 2. a usDelayOne() is enough for SS1306, but SS1312 need two usDealyOne() for stable. @RT1052 plateform
     * 3. reduce a usDelayOne() after IIC_SDA_1.
     */ 
    IIC_SDA_IN;                                                        /* master handover sda's control  */
    IIC_SDA_1;
    usDelayOne();
    IIC_SCL_1;
    
    /**
     *  it takes me 1.5 hour to figureout here can not be place a usDelayOne(), but why ?  2022.05~06
     *  it takes me 0.5 day  to figureout here must place a usDelayOne(), but why ?        2023.05.09
     *  increase while(OLED_SDA_R && (ucTimeOut < 10u)) to judge the reponse signal        2023.05.10
     */
    while(IIC_SDA_R && (ucTimeOut < 5u)) {
        ucTimeOut++;
        usDelayOne(); 
    }
    IIC_SCL_0;                                                         /* after this, slave release sda */
    IIC_SDA_OUT;                                                       /* maseter recontrol sda         */
    //usDelayOne();                                                       /* there might be a delay is no need */
    return ucTimeOut;
}
/**
********************************************************************************************************
** @nameis i2cAnswer
** @effect send ack signal
** @import ucAns:1 应答 0 不应答
** @export none
** @return none
** @create yizhi 2023.03.20
** @modify 
*********************************************************************************************************/
void i2cAnswer(uint8_t ucAns) // 主机应答期间， 1 应答 0 不应答
{
     if(ucAns==1)
     {
         IIC_SDA_0;
         IIC_SDA_OUT;
         usDelayOne();
     }
     else
     {
         IIC_SDA_1;
         IIC_SDA_OUT;
         usDelayOne();
      }
      IIC_SCL_1;
      usDelayOne();// 让slave读取应答信号
      IIC_SCL_0;
}
     
     
/****************************************************
function: send a byte and return reponse status
input:  dat: the byte to send
output: return reponse status
author: yizhi
******************************************************/
uint8_t i2cWriteByte(uint8_t ucData)
{
    uint8_t i,ucTemp;
    ucTemp = ucData;
    
    for (i=0;i<8;i++)
    {
        if (ucTemp & 0x80)
            IIC_SDA_1;
        else
            IIC_SDA_0;
        usDelayOne();
        IIC_SCL_1;
        usDelayOne();
        ucTemp <<= 1;
        IIC_SCL_0;
        
    }
    return i2cRespons();
}
/****************************************************
function: receive a byte and if sent ack or not
input:  ucAns: 1-send ack 0-not send ack
output: received byte
author: yizhi
******************************************************/
uint8_t i2cReadByte(uint8_t ans)
{
    uint8_t ucTemp,i;
    
//    IIC_SCL_0; // is it indispensable ?
    IIC_SDA_IN;
    IIC_SDA_1;
    usDelayOne();                                               /* which is indispensable  */ 
    for(i=0;i<8;i++)
    {
        ucTemp <<= 1;
        IIC_SCL_1;
        usDelayOne();
        if (IIC_SDA_R)
            ucTemp |= 0x01;
        IIC_SCL_0;
        usDelayOne();
    }
    i2cAnswer(ans);
    return ucTemp;
}

/****************************************************
function:写一个字节
input:  ad: machine address
        add: ROM   address;
        dat: the data to write;
output: none;
author: yizhi
******************************************************/
void i2cWriteChar(uint8_t ad,uint8_t add,uint8_t dat)
{
    i2cStart();
    i2cWriteByte(ad);    // 0x3c HMC5883L   0xD0 MPU6050 
    i2cWriteByte(add);
    i2cWriteByte(dat);
    i2cStop();
    
}
/****************************************************
function: 读一个字节
input:  ad: machine address
        add: ROM   address;
output: the readed data;
author: yizhi
******************************************************/
uint8_t i2cReadChar(uint8_t ad,uint8_t add)
{
      
      i2cStart();
      i2cWriteByte(ad);
      i2cWriteByte(add);
      i2cStart();
      i2cWriteByte(ad+0x01); // 先读后写
      return i2cReadByte(0);
    
}

