/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               i2c1.c
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

/*
 * slection between arm or 51 for const data 
 */
#ifdef ARM_PLATEFORM
#define CONST_DATA  const
#define ___NOP()     __asm("NOP")
#else
#define CONST_DATA  code
#define ___NOP()     _nop_()
#endif

/*
 * slection platfrom header files 
 */
#if defined(STM32F10X_HD)
    #include "sys.h"
    #include "delay.h"
//      #include "hard_ware.h"
#elif defined(STM32F10X_MD)
    #include "ZMD_Includes.h"
#elif defined(RT1052)
    #include "fsl_common.h"
    #include "fsl_snvs_hp.h"
#elif defined(LKS32MC08x)
    #include "basic.h"
    #include "hardware_config.h"
#else
    #include "common.h"
#endif


#if defined(STM32F103VE)                                                
    #define IIC1_SCL         PDout(3)                                    /* 精英主板V.7  CAMERA接口*/ 
    #define IIC1_SDA         PGout(13)
    #define IIC1_SDA_R       PGin(13)
    #define IIC1_SCL_R       PDin(3)
    #define IIC1_SDA_IN      {GPIOG->CRH &= ~(0xFu << (13 - 8) * 4); GPIOG->CRH |= 8u << (13 - 8) * 4;}
    #define IIC1_SDA_OUT     {GPIOG->CRH &= ~(0xFu << (13 - 8) * 4); GPIOG->CRH |= 3u << (13 - 8) * 4;}
    #define IIC1_SCL_IN      {GPIOD->CRL &= ~(0xFu << (3) * 4); GPIOD->CRL |= 8u << (3) * 4;}
    #define IIC1_SCL_OUT     {GPIOD->CRL &= ~(0xFu << (3) * 4); GPIOD->CRL |= 3u << (3) * 4;}
    
//    #define IIC1_SCL         PBout(11)                                   /* 精英主板V.7  ATK MODULE接口*/
//    #define IIC1_SDA         PBout(10)
//    #define IIC1_SDA_R       PBin(10)
//    #define IIC1_SCL_R       PBin(11)
//    #define IIC1_SDA_IN      {GPIOB->CRH &= ~(0xFu << (10 - 8) * 4); GPIOB->CRH |= 8u << (10 - 8) * 4;}
//    #define IIC1_SDA_OUT     {GPIOB->CRH &= ~(0xFu << (10 - 8) * 4); GPIOB->CRH |= 3u << (10 - 8) * 4;}
//    #define IIC1_SCL_IN      {GPIOB->CRH &= ~(0xFu << (11 - 8) * 4); GPIOB->CRH |= 8u << (11 - 8) * 4;}
//    #define IIC1_SCL_OUT     {GPIOB->CRH &= ~(0xFu << (11 - 8) * 4); GPIOB->CRH |= 3u << (11 - 8) * 4;}
#elif defined(STM32F10X_MD)
    #define IIC1_SCL          PBout(6)
    #define IIC1_SDA          PBout(7)
    #define IIC1_SDA_R       PBin(7)
    #define IIC1_SDA_IN      {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 8 << 28;}
    #define IIC1_SDA_OUT     {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 3 << 28;}
#elif defined(GD32F303RE)
    #define IIC1_SCL         PBout(10)                                    /* 风机测试平台 */ 
    #define IIC1_SDA         PBout(11)
    #define IIC1_SCL_R       PBin(10)
    #define IIC1_SDA_R       PBin(11)
    #define IIC1_SCL_IN      {GPIOB->CRH &= ~(0xFu << (10 - 8) * 4); GPIOB->CRH |= 8u << (10 - 8) * 4;}
    #define IIC1_SCL_OUT     {GPIOB->CRH &= ~(0xFu << (10 - 8) * 4); GPIOB->CRH |= 3u << (10 - 8) * 4;}
    #define IIC1_SDA_IN      {GPIOB->CRH &= ~(0xFu << (11 - 8) * 4); GPIOB->CRH |= 8u << (11 - 8) * 4;}
    #define IIC1_SDA_OUT     {GPIOB->CRH &= ~(0xFu << (11 - 8) * 4); GPIOB->CRH |= 3u << (11 - 8) * 4;}


#else
    sbit IIC1_SDA = P1^0;
    sbit IIC1_SCL = P1^1;
    #define IIC1_SDA_R   IIC1_SDA
    #define IIC1_SDA_IN
    #define IIC1_SDA_OUT
#endif

#define IIC1_SCL_0    IIC1_SCL = 0
#define IIC1_SCL_1    IIC1_SCL = 1
#define IIC1_SDA_0    IIC1_SDA = 0
#define IIC1_SDA_1    IIC1_SDA = 1
    
    

static void usDelayOne(void)
{
    uint8_t i;
    for (i = 0; i < 10; i++) {      //  k < 5:  used 4 us @ 12.0000MHz
         ___NOP();                  // k < 4:  used 3.64 @ 12.0000MHz
        }                           // i < 4:  used 0.7~0.8us @ 精英版1.7 72.000MHz
}

static void i2cInit(void)
{
    IIC1_SCL_1;
    IIC1_SDA_1;
    usDelayOne();
}
static void i2cStart(void)
{
    IIC1_SDA_1;
    IIC1_SCL_1 ;
    usDelayOne();
    IIC1_SDA_0;
    usDelayOne();
    IIC1_SCL_0;                                                /* there might be a delay after pull scl down */
}
static void i2cStop(void)
{
    IIC1_SDA_0;
    IIC1_SCL_1 ;
    usDelayOne();
    IIC1_SDA_1;
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
static uint8_t i2cRespons()
{
    uint8_t ucTimeOut = 0;
    /**
     * 1. The follow either OLED_SDIN_Set or usDelayOne(a longer delay),which is indispensable
     * 2. a usDelayOne() is enough for SS1306, but SS1312 need two usDealyOne() for stable. @RT1052 plateform
     * 3. reduce a usDelayOne() after IIC1_SDA_1.
     */ 
    IIC1_SDA_IN;                                                        /* master handover sda's control  */
    IIC1_SDA_1;
    usDelayOne();
    IIC1_SCL_1;
    
    /**
     *  it takes me 1.5 hour to figureout here can not be place a usDelayOne(), but why ?  2022.05~06
     *  it takes me 0.5 day  to figureout here must place a usDelayOne(), but why ?        2023.05.09
     *  increase while(OLED_SDA_R && (ucTimeOut < 10u)) to judge the reponse signal        2023.05.10
     */
    while(IIC1_SDA_R && (ucTimeOut < 5u)) {
        ucTimeOut++;
        usDelayOne(); 
    }
    IIC1_SCL_0;                                                         /* after this, slave release sda */
    IIC1_SDA_OUT;                                                       /* maseter recontrol sda         */
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
static void i2cAnswer(uint8_t ucAns) // 主机应答期间， 1 应答 0 不应答
{
     if(ucAns==1)
     {
         IIC1_SDA_0;
         IIC1_SDA_OUT;
         usDelayOne();
     }
     else
     {
         IIC1_SDA_1;
         IIC1_SDA_OUT;
         usDelayOne();
      }
      IIC1_SCL_1;
      usDelayOne();// 让slave读取应答信号
      IIC1_SCL_0;
}
     
     
/****************************************************
function: send a byte and return reponse status
input:  dat: the byte to send
output: return reponse status
author: yizhi
******************************************************/
static uint8_t i2cWriteByte(uint8_t ucData)
{
    uint8_t i,ucTemp;
    ucTemp = ucData;
    
    for (i=0;i<8;i++)
    {
        if (ucTemp & 0x80)
            IIC1_SDA_1;
        else
            IIC1_SDA_0;
        usDelayOne();
        IIC1_SCL_1;
        usDelayOne();
        ucTemp <<= 1;
        IIC1_SCL_0;
        
    }
    return i2cRespons();
}
/****************************************************
function: receive a byte and if sent ack or not
input:  ucAns: 1-send ack 0-not send ack
output: received byte
author: yizhi
******************************************************/
static uint8_t i2cReadByte(uint8_t ans)
{
    uint8_t ucTemp,i;
    
//    IIC1_SCL_0; // is it indispensable ?
    IIC1_SDA_IN;
    IIC1_SDA_1;
    usDelayOne();                                               /* which is indispensable  */ 
    for(i=0;i<8;i++)
    {
        ucTemp <<= 1;
        IIC1_SCL_1;
        usDelayOne();
        if (IIC1_SDA_R)
            ucTemp |= 0x01;
        IIC1_SCL_0;
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
static void i2cWriteChar(uint8_t ad,uint8_t add,uint8_t dat)
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
static uint8_t i2cReadChar(uint8_t ad,uint8_t add)
{
      
      i2cStart();
      i2cWriteByte(ad);
      i2cWriteByte(add);
      i2cStart();
      i2cWriteByte(ad+0x01); // 先读后写
      return i2cReadByte(0);
    
}

