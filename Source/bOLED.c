/*********************************************************************************************************
*                          Copyright (c), 2002-2024, yuwell Co. , Ltd
*    ___                             _   _               ____               ___           _
*   |_ _|_ __  _ __   _____   ____ _| |_(_) ___  _ __   |  _ \ ___  ___    |_ _|_ __  ___| |_
*    | || '_ \| '_ \ / _ \ \ / / _` | __| |/ _ \| '_ \  | |_) / _ \/ __|    | || '_ \/ __| __|
*    | || | | | | | | (_) \ V / (_| | |_| | (_) | | | | |  _ <  __/\__ \_   | || | | \__ \ |_ _
*   |___|_| |_|_| |_|\___/ \_/ \__,_|\__|_|\___/|_| |_| |_| \_\___||___(_) |___|_| |_|___/\__(_)
*
*-------------------------------------------Description---------------------------------------------------
* @file    Application.h
* @author  yizhi
* @date    2025-05-07
* @version v1.0
* @brief   OLED组件实现。
*-------------------------------------------Histroy-------------------------------------------------------
* @history
* <author>           <time>                 <desc>
* liguangdao       2025-05-07                v1.0   初始版本
*
*---------------------------------------------------------------------------------------------------------
 @verbatim
 ##variables type perfix
   pfn(pointer of function), v(void), b(bool), c(char), n(unsigned char),s(short),w(unsigned short),i(int)
   l(long), y(unsigned long), f(float), d(double), e(enum), t(struct), u(union), q(Q quality), r(string),
   ll(long long)
 ##variables typedef
   char, S8(signed char), U8(unsigned char), S16(short), U16(unsigned short), S32(int), U32(unsigned int)
   F32(float), F64(double)

 @endverbatim
*********************************************************************************************************/

/* Includes --------------------------------------------------------------------------------------------*/
#include "boled.h"
#include "boled_font.h"

/* Private typedef -------------------------------------------------------------------------------------*/

/* Private define --------------------------------------------------------------------------------------*/

/* Private macro ---------------------------------------------------------------------------------------*/

/* Private variables -----------------------------------------------------------------------------------*/

/* Global variables ------------------------------------------------------------------------------------*/
uint16_t BACK_COLOR = BLACK;
uint16_t FRONT_COLOR = YELLOW;
/* External extern variables ---------------------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------------------------*/


/*-----------------------------------static function start----------------------------------------------*/
#ifdef OLED_I2C
void usDelayOne()
{
    uint8_t i;
    for (i = 0; i < 2; i++) {
         ___NOP();
    }
}

/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{

    OLED_SCL_1 ;
    OLED_SDA_1;
    usDelayOne();
    OLED_SDA_0;
    usDelayOne();
    OLED_SCL_0;                                                      /* there might be a delay */
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
    OLED_SCL_1 ;
    OLED_SDA_0;
    usDelayOne();
    OLED_SDA_1;
    usDelayOne();

}

void IIC_Respons()
{
    uint8_t ucTimeOut = 0;
    /**
     * 1. The follow either OLED_SDIN_Set or usDelayOne(a longer delay),which is indispensable
     * 2. a usDelayOne() is enough for SS1306, but SS1312 need two usDealyOne() for stable. @RT1052 plateform
     * 3. reduce a usDelayOne() after OLED_SDA_1.
     */ 
    OLED_SDA_IN;                                                        /* master handover sda's control  */
    OLED_SDA_1;
    usDelayOne();
    OLED_SCL_1;
    
    /**
     *  it takes me 1.5 hour to figureout here can not be place a usDelayOne(), but why ?  2022.05~06
     *  it takes me 0.5 day  to figureout here must place a usDelayOne(), but why ?        2023.05.09
     *  increase while(OLED_SDA_R && (ucTimeOut < 10u)) to judge the reponse signal        2023.05.10
     */
    while(OLED_SDA_R && (ucTimeOut < 10u)) {
        ucTimeOut++;
        usDelayOne(); 
    }
    OLED_SCL_0;                                                         /* after this, slave release sda */
    OLED_SDA_OUT;                                                       /* maseter recontrol sda         */
    usDelayOne();                                                       /* there might be a delay */
}


void IIC_Answer(uint8_t ucAns) // 主机应答期间， 1 应答 0 不应答
{
     if(ucAns==1)
     {
         OLED_SDA_0;
         usDelayOne();
     }
     else
     {
         OLED_SDA_1;
         usDelayOne();
      }
      OLED_SCL_1;
      usDelayOne();// 等待slave读取应答信号
      OLED_SCL_0;
}

/**********************************************
// IIC Write byte
**********************************************/

void IIC_WriteByte(unsigned char IIC_Byte)
{
    unsigned char i;
    unsigned char m,da;
    da=IIC_Byte;
    OLED_SCL_0;
    for(i=0;i<8;i++)
    {
        m=da;
        m=m&0x80;
        if(m==0x80) {
            OLED_SDA_1;
        } else {
            OLED_SDA_0;
        }
        usDelayOne();
        da=da<<1;
        OLED_SCL_1;
        usDelayOne();
        OLED_SCL_0;
    }
}

/*********************************************************************************************************
** Function name: Read_IIC_Byte()
** Descriptions : thsi is just for SS13xx register
** Input para.  : none
**
** Output para. : the value read out
** Returned Val :
** Created by   : 2022/5/19 星期四, by yizhi
** Created Date :
**--------------------------------------------------------------------------------------------------------
** Modified     :
**
*********************************************************************************************************/
uint8_t IIC_ReadByte(void)
{
    uint8_t ucData = 0;
    uint8_t i;

    // OLED_SCL_0; // is it indispensable ?
    OLED_SDA_IN;
    usDelayOne();

    for(i=0;i<8;i++)
    {
        ucData <<= 1;
        OLED_SCL_1;
        usDelayOne();

        if(OLED_SDA_R)
            ucData |= 0x01;

        OLED_SCL_0;
        usDelayOne();
    }
    OLED_SDA_OUT;
    usDelayOne();
    return ucData;
}
/*********************************************************************************************************
** Function name: Read_IIC_Data()   ——> OLED_ReadChar
** Descriptions : --
** Input para.  :
**
** Output para. :
** Returned Val :
** Created by   : 2022/5/20 星期五,
** Created Date :
**--------------------------------------------------------------------------------------------------------
** Modified     :
**
*********************************************************************************************************/
uint8_t OLED_ReadChar(uint8_t ucReg)
{
    IIC_Start();
    IIC_WriteByte(0x78);
    IIC_Respons();
    IIC_WriteByte(ucReg);
    IIC_Respons();
    IIC_Start();
    IIC_WriteByte(0x78 + 0x01);
    IIC_Respons();
    return IIC_ReadByte();
}

/**********************************************
// IIC Write Command
**********************************************/
void I2C_WrCmd(unsigned char IIC_Command)
{
    IIC_Start();
    IIC_WriteByte(0x78);            //Slave address,SA0=0
    IIC_Respons();
    IIC_WriteByte(0x00);            //write command
    IIC_Respons();
    IIC_WriteByte(IIC_Command);
    IIC_Respons();
    IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void I2C_WrDat(unsigned char IIC_Data)
{
    IIC_Start();
    IIC_WriteByte(0x78);            //D/C#=0; R/W#=0
    IIC_Respons();
    IIC_WriteByte(0x40);            //write data
    IIC_Respons();
    IIC_WriteByte(IIC_Data);
    IIC_Respons();
    IIC_Stop();
}
/**********************************************
// IIC Write Multi Data   pucData: 写入数据的起始地址 ucLen 
**********************************************/
void I2C_WrDatN(CONST_DATA unsigned char *pucData, unsigned char ucLen, unsigned char ucY)
{
    uint8_t i;
    IIC_Start();
    IIC_WriteByte(0x78);            //D/C#=0; R/W#=0
    IIC_Respons();
    IIC_WriteByte(0x40);            //write data
    IIC_Respons();
    for (i = 0; i < ucLen; i++) {
        IIC_WriteByte(ucY ? (*pucData++) : ~(*pucData++));
        IIC_Respons();
    }
    IIC_Stop();
}
void OLED_WrByte(unsigned dat,unsigned cmd)
{
    if(cmd) {
        I2C_WrDat(dat);
    } else {
        I2C_WrCmd(dat);
    }
}
#define OLED_CMD  0
#define OLED_DATA 1
#endif

#ifdef OLED_SPI
void SPI_WrDat(unsigned char dat)
{
  unsigned char i=8;
//  OLED_CS_0;
//  OLED_DC_1;
//  __NOP();__NOP();__NOP();__NOP(); // yanshi
//  OLED_SCL_0;;
//  __NOP();__NOP();__NOP();__NOP(); // yanshi
  while(i--)
  {
    OLED_SCL_0;
//    __NOP();__NOP();// yanshi
    if(dat&0x80)            // 注意此处若为51单片机，采用CY位的方法速度略有提升。f = 340KHz -> 390KHz @12MHz
      OLED_SDA_1;
    else
      OLED_SDA_0;
//    __NOP();__NOP();// yanshi
    OLED_SCL_1;
//    __NOP();__NOP();// yanshi
    dat<<=1;
  }
//  OLED_CS_1;
}
void SPI_WrCmd(unsigned char cmd)
{
  unsigned char i;
//  OLED_CS_0;
  OLED_DC_0;
//  __NOP();__NOP();__NOP();__NOP(); // yanshi
//  OLED_SCL_0;;
//  __NOP();__NOP();__NOP();__NOP();// yanshi
  for(i=0;i<8;i++)
  {
    OLED_SCL_0;
//   __NOP();__NOP();// yanshi 
   if(cmd&0x80)
      OLED_SDA_1;
   else
      OLED_SDA_0;
//   __NOP();__NOP();// yanshi 
   OLED_SCL_1;
//   __NOP();__NOP();// yanshi   
   cmd<<=1;
  }
  OLED_DC_1;
//  OLED_CS_1;
}
#endif



void OLED_WrCmd(unsigned char ucVal)
{
#if   defined(OLED_I2C)
    I2C_WrCmd(ucVal);
#elif defined(OLED_SPI)
    SPI_WrCmd(ucVal);
#else 
    #error "NO OLED INTERFACE DEFINED"
#endif
}
void OLED_WrDat(unsigned char ucVal)
{
#if   defined(OLED_I2C)
     I2C_WrDat(ucVal);
#elif defined(OLED_SPI)
     SPI_WrDat(ucVal);
#else 
    #error "NO OLED INTERFACE DEFINED"
#endif
}
/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
#ifdef OLED_COLOR
void LCD_WrWord(uint16_t color)
{
    OLED_WrDat(color>>8);
    OLED_WrDat(color);
}
/**
*********************************************************************************************************
** @nameis OLED_Address_Set
** @effect 设置耍写窗口
** @import x1 起始列 y1 起始行 x2 终止列 y2 终止行  例如 0, 0, 15, 31 设置一个 水平宽16竖直高为32的显示窗口
** @export none
** @return none
** @create yizhi 2023.03.27
** @modify yizhi 2023.05.15 正式明确输入参数的含义，注意如果改变了屏幕的显示方向，赋值寄存器需要对应修改 
*********************************************************************************************************/
void OLED_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
#if defined(CHIP_SSD1331)
    OLED_WrCmd(0x15);//列地址设置
    OLED_WrCmd(x1);
    OLED_WrCmd(x2);
    OLED_WrCmd(0x75);//行地址设置
    OLED_WrCmd(y1);
    OLED_WrCmd(y2);
#elif  defined(CHIP_SSD1351)
    OLED_WrCmd(0x15);//列地址设置
    OLED_WrDat(x1);
    OLED_WrDat(x2);
    OLED_WrCmd(0x75);//行地址设置
    OLED_WrDat(y1);
    OLED_WrDat(y2);
    OLED_WrCmd(0x5c);
#elif defined(CHIP_ST7735)
    OLED_WrCmd(0x2a);//列地址设置
    LCD_WrWord(y1+26);  
    LCD_WrWord(y2+26);
    OLED_WrCmd(0x2b);//行地址设置
    LCD_WrWord(x1+1);
    LCD_WrWord(x2+1);
    OLED_WrCmd(0x2c);//储存器写
#elif defined(CHIP_ST7789V2)
    OLED_WrCmd(0x2a);//列地址设置
    LCD_WrWord(x1);  
    LCD_WrWord(x2);
    OLED_WrCmd(0x2b);//行地址设置
    LCD_WrWord(y1+20);
    LCD_WrWord(y2+20);
    OLED_WrCmd(0x2c);//储存器写
#elif defined(CHIP_SSD1322)                                             /* 基于目前的列行式字模 该IC仅支持竖屏显示 64x256  */
    y1  = y1 >> 2;                                                      /* 需要确保y1和(y2 + 1)可以被2整除                 */
    y2  = (((y2 + 1) >> 2) - 1);
    OLED_WrCmd(0x15);
    OLED_WrDat(0x1C + y1);
    OLED_WrDat(0x1C + y2);
    OLED_WrCmd(0x75);
    OLED_WrDat(x1);
    OLED_WrDat(x2);
    OLED_WrCmd(0x5c);
#elif defined(CHIP_SSD1327)
    x1  = x1 >> 1;                                                      /* 需要确保x1和(x2 + 1)可以被2整除                 */
    x2  = (((x2 + 1) >> 1) - 1);
    OLED_WrCmd(0x15);
    OLED_WrCmd(x1);
    OLED_WrCmd(x2);
    OLED_WrCmd(0x75);
    OLED_WrCmd(y1);
    OLED_WrCmd(y2);
#endif
}
#else 
void OLED_Set_Pos(unsigned char x, unsigned char y)
{ 
    #ifdef CHIP_SH1106
    x += 2;
    #endif
    OLED_WrCmd(0xb0+y);
    OLED_WrCmd(x&0x0f);
    OLED_WrCmd(((x&0xf0)>>4)|0x10);
} 
void OLED_PutPixel(uint8_t x,uint8_t y)
{
    OLED_WrCmd(0xb0+7-y/8);   //0xb0+0~7表示页0~页7?
    #ifdef CHIP_SH1106    //0x00+0~16表示将128列分成16组其地址在某组中的第几列
    OLED_WrCmd(0x02 + x%16);
    #else 
    OLED_WrCmd(0x00 + x%16);
    #endif  
    OLED_WrCmd(0x10 + x/16);     //0x10+0~16表示将128列分成16组其地址所在第几组
    OLED_WrDat(0x80>>(y%8));
}
#endif

/*-----------------------------------static function end------------------------------------------------*/

#if defined(OLED_COLOR) 
void LCD_DrawPoint(uint8_t x, uint8_t y, uint16_t color)
{
    OLED_Address_Set(x,y,x,y);
    OLED_WrDat(color>>8);
    OLED_WrDat(color);
}
#endif

void OLED_Fill(unsigned int usData)
{
    unsigned short y,x;

#if defined(CHIP_SSD1331) || defined(CHIP_SSD1351) || defined(CHIP_ST7735) || defined(CHIP_ST7789V2)
    OLED_Address_Set(0,0,OLED_WIDTH - 1,OLED_HIGH-1);
    for(x=0; x < OLED_HIGH; x++)
    {
        for (y=0; y < OLED_WIDTH; y++)
        {
            OLED_WrDat(usData >> 8);
            OLED_WrDat(usData);
        }
    }
#elif defined(CHIP_SSD1322) || defined(CHIP_SSD1327)
    OLED_Address_Set(0,0,OLED_WIDTH - 1,OLED_HIGH - 1);
    for(x=0; x < (OLED_WIDTH >> 1); x++)                                /* 每次外循环写2竖列， 需要循环64次    */ 
    {
        for (y=0; y < OLED_HIGH; y++)                                   /* 写这两竖列需要循环128次             */
        {
            OLED_WrDat(usData);                                         /* 高四位为第一个点灰度，低四位为第二个*/
        }
    }
#else
    for(y=0; y< (OLED_LINE_MAX + LINE_HEIGHT); y++)
    {
        OLED_WrCmd(0xb0+y);  //设置页地址（0~7）
        #ifdef CHIP_SH1106   //设置显示位置—列低地址
        OLED_WrCmd(0x02);
        #else 
        OLED_WrCmd(0x00);
        #endif
        OLED_WrCmd(0x10);    //设置显示位置—列高地址  
        for(x=0;x<OLED_WIDTH;x++)
            OLED_WrDat(usData);
    }
#endif
}

void OLED_ClearLine(uint8_t ucStartDine, uint8_t ucEndDine,unsigned int usData)
{
    unsigned short y,x;

#if defined(CHIP_SSD1331) || defined(CHIP_SSD1351) || defined(CHIP_ST7735) || defined(CHIP_ST7789V2)
    OLED_Address_Set(0,0,OLED_WIDTH - 1,OLED_HIGH-1);
    for(x=0; x < OLED_HIGH; x++)
    {
        for (y=0; y < OLED_WIDTH; y++)
        {
            OLED_WrDat(usData >> 8);
            OLED_WrDat(usData);
        }
    }
#elif defined(CHIP_SSD1322) || defined(CHIP_SSD1327)
    OLED_Address_Set(0,0,OLED_WIDTH - 1,OLED_HIGH - 1);
    for(x=0; x < (OLED_WIDTH >> 1); x++)                                /* 每次外循环写2竖列， 需要循环64次    */ 
    {
        for (y=0; y < OLED_HIGH; y++)                                   /* 写这两竖列需要循环128次             */
        {
            OLED_WrDat(usData);                                         /* 高四位为第一个点灰度，低四位为第二个*/
        }
    }
#else
    for(y = ucStartDine; y< (ucEndDine + LINE_HEIGHT/2); y++)
    {
        OLED_WrCmd(0xb0+y);  //设置页地址（0~7）
        #ifdef CHIP_SH1106   //设置显示位置—列低地址
        OLED_WrCmd(0x02);
        #else 
        OLED_WrCmd(0x00);
        #endif
        OLED_WrCmd(0x10);    //设置显示位置—列高地址  
        for(x = 0;x < OLED_WIDTH;x++)
            OLED_WrDat(usData);
    }
#endif
}
//初始化
void OLED_Init(void)        
{
#ifdef  STM32F10X_HD
    #ifdef GD32F30X_TEST_PLATFORM
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE );

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);           /* JTAG-DP 失能 + SW-DP使能 */
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_5);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_8 | GPIO_Pin_9);
    #elif defined(MiniSTIM32V3)
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE );

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);           /* JTAG-DP disable + SW-DP enable */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_6);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_8);
    #else
    #error "Please defined OLED interface platform"
    #endif
#endif
    
#ifdef OLED_SPI
    OLED_RST_0;                          /* por */
    msDelay(10);
    OLED_RST_1;
    msDelay(10);
    OLED_SCL_0;                          /* default is low for accelerate        */
    OLED_CS_0;                           /* default is select  for accelerate    */
    OLED_DC_1;                           /* dafault is date    for accelerate  6.35ms->5.38ms */
#endif
#ifdef CHIP_SSD1331
    {
    OLED_WrCmd(0xAE);
        
    /*
     * set Set Re-map & Data Format 
     * A[0]: A[0]=0, Horizontal address increment A[0]=1, Vertical address increment
     */
    OLED_WrCmd(0xA0);
    OLED_WrCmd(0x73);
    OLED_WrCmd(0xA1);
    OLED_WrCmd(0x00);
    OLED_WrCmd(0xA2);
    OLED_WrCmd(0x00);
    OLED_WrCmd(0xA4);
    OLED_WrCmd(0xA8);
    OLED_WrCmd(0x3F);
    OLED_WrCmd(0xAD);
    OLED_WrCmd(0x8E);
    OLED_WrCmd(0xB0);
    OLED_WrCmd(0x0B);
    OLED_WrCmd(0xB1);
    OLED_WrCmd(0x31);
    OLED_WrCmd(0xB3);
    OLED_WrCmd(0xF0);
    OLED_WrCmd(0x8A);
    OLED_WrCmd(0x64);
    OLED_WrCmd(0x8B);
    OLED_WrCmd(0x78);
    OLED_WrCmd(0x8C);
    OLED_WrCmd(0x64);
    OLED_WrCmd(0xBB);
    OLED_WrCmd(0x3A);
    OLED_WrCmd(0xBE);
    OLED_WrCmd(0x3E);
    OLED_WrCmd(0x87);
    OLED_WrCmd(0x06);
    OLED_WrCmd(0x81);
    OLED_WrCmd(0x91);
    OLED_WrCmd(0x82);
    OLED_WrCmd(0x50);
    OLED_WrCmd(0x83);
    OLED_WrCmd(0x7D);
    OLED_WrCmd(0xAF);
    OLED_Fill(0x00);}
#elif defined(CHIP_SSD1351)
    {
    OLED_WrCmd(0xFD); // set comand lockDriver IC interface is unlocked from entering command
    OLED_WrDat(0x12); // Unlock Driver IC (0x12/0x16/0xB0/0xB1)  0x12 => Driver IC interface is unlocked from entering command.
    OLED_WrCmd(0xFD);
    OLED_WrDat(0xB1); // Unlock All Commands (0x12/0x16/0xB0/0xB1)
    OLED_WrCmd(0xAE); // Display Off (Sleep Mode On)
    OLED_WrCmd(0xB3);
    OLED_WrDat(0xF1); // Set Clock as 90 Frames/Sec
    OLED_WrCmd(0xCA);
    OLED_WrDat(0x7F); // 1/128 Duty (0x0F~0x7F)
    OLED_WrCmd(0xA2);
    OLED_WrDat(0x00); // Shift Mapping RAM Counter (0x00~0x7F)
    OLED_WrCmd(0xA1);
    OLED_WrDat(0x00); // Set Mapping RAM Display Start Line (0x00~0x7F)
    OLED_WrCmd(0xA0);
    OLED_WrDat(0x75); // 0x74 Horizontal Address Increment mode ******************* 0x75 vertical address increment mode
    OLED_WrCmd(0xB5);
    OLED_WrDat(0x00); // Disable GPIO Pins Input
    OLED_WrCmd(0xAB);
    OLED_WrDat(0x01); // Enable Internal VDD Regulator
    OLED_WrCmd(0xB4);
    OLED_WrDat(0xA0); 
    OLED_WrDat(0xB5);
    OLED_WrDat(0x55); // Enable External VSL
    OLED_WrCmd(0xC1);
    OLED_WrDat(0xC8); // Set Contrast of Color A (Red)
    OLED_WrDat(0x80); // Set Contrast of Color B (Green)
    OLED_WrDat(0xC8); // Set Contrast of Color C (Blue)
    OLED_WrCmd(0xC7); 
    OLED_WrDat(0x0F); // Master Contrast Current Control  Default => 0x0F (Maximum)
    /* Set_Gray_Scale_Table() */
    OLED_WrCmd(0xB1); 
    OLED_WrDat(0x32); // Set Phase 1 as 5 Clocks & Phase 2 as 3 Clocks
    OLED_WrCmd(0xBB);
    OLED_WrDat(0x17); // Set Pre-Charge Voltage Level as 0.50*VCC
    OLED_WrCmd(0xB2);
    OLED_WrDat(0xA4); //  Enhance Display Performance
    OLED_WrDat(0x00);
    OLED_WrDat(0x00);
    OLED_WrCmd(0xB6);
    OLED_WrDat(0x01); // Set Second Pre-Charge Period as 1 Clock
    OLED_WrCmd(0xBE);
    OLED_WrDat(0x05); // Set Common Pins Deselect Voltage Level as 0.82*VCC
    OLED_WrCmd(0xA6); // Normal Display
                    //     0xA4 (0x00) => Entire Display Off, All Pixels Turn Off
                    //     0xA5 (0x01) => Entire Display On, All Pixels Turn On at GS Level 63
                    //     0xA6 (0x02) => Normal Display
                    //     0xA7 (0x03) => Inverse Display
    OLED_WrCmd(0xAF);
    OLED_Fill(0x00);}
#elif defined(CHIP_ST7735)
    {
    LCD_BLK_1;//打开背光
    OLED_WrCmd(0x11);     //Sleep out
    msDelay(120);                //Delay 120ms
    OLED_WrCmd(0xB1);     //Normal mode
    OLED_WrDat(0x05);   
    OLED_WrDat(0x3C);   
    OLED_WrDat(0x3C);   
    OLED_WrCmd(0xB2);     //Idle mode
    OLED_WrDat(0x05);   
    OLED_WrDat(0x3C);   
    OLED_WrDat(0x3C);   
    OLED_WrCmd(0xB3);     //Partial mode
    OLED_WrDat(0x05);   
    OLED_WrDat(0x3C);   
    OLED_WrDat(0x3C);   
    OLED_WrDat(0x05);   
    OLED_WrDat(0x3C);   
    OLED_WrDat(0x3C);   
    OLED_WrCmd(0xB4);     //Dot inversion
    OLED_WrDat(0x03);   
    OLED_WrCmd(0xC0);     //AVDD GVDD
    OLED_WrDat(0xAB);   
    OLED_WrDat(0x0B);   
    OLED_WrDat(0x04);   
    OLED_WrCmd(0xC1);     //VGH VGL
    OLED_WrDat(0xC5);   //C0
    OLED_WrCmd(0xC2);     //Normal Mode
    OLED_WrDat(0x0D);   
    OLED_WrDat(0x00);   
    OLED_WrCmd(0xC3);     //Idle
    OLED_WrDat(0x8D);   
    OLED_WrDat(0x6A);   
    OLED_WrCmd(0xC4);     //Partial+Full
    OLED_WrDat(0x8D);   
    OLED_WrDat(0xEE);   
    OLED_WrCmd(0xC5);     //VCOM
    OLED_WrDat(0x0F);   
    OLED_WrCmd(0xE0);     //positive gamma
    OLED_WrDat(0x07);   
    OLED_WrDat(0x0E);   
    OLED_WrDat(0x08);   
    OLED_WrDat(0x07);   
    OLED_WrDat(0x10);   
    OLED_WrDat(0x07);   
    OLED_WrDat(0x02);   
    OLED_WrDat(0x07);   
    OLED_WrDat(0x09);   
    OLED_WrDat(0x0F);   
    OLED_WrDat(0x25);   
    OLED_WrDat(0x36);   
    OLED_WrDat(0x00);   
    OLED_WrDat(0x08);   
    OLED_WrDat(0x04);   
    OLED_WrDat(0x10);   
    OLED_WrCmd(0xE1);     //negative gamma
    OLED_WrDat(0x0A);   
    OLED_WrDat(0x0D);   
    OLED_WrDat(0x08);   
    OLED_WrDat(0x07);   
    OLED_WrDat(0x0F);   
    OLED_WrDat(0x07);   
    OLED_WrDat(0x02);   
    OLED_WrDat(0x07);   
    OLED_WrDat(0x09);   
    OLED_WrDat(0x0F);   
    OLED_WrDat(0x25);   
    OLED_WrDat(0x35);   
    OLED_WrDat(0x00);   
    OLED_WrDat(0x09);   
    OLED_WrDat(0x04);   
    OLED_WrDat(0x10);
         
    OLED_WrCmd(0xFC);    
    OLED_WrDat(0x80);  
        
    OLED_WrCmd(0x3A);     
    OLED_WrDat(0x55);   
    OLED_WrCmd(0x36);
    /* 
     * OLED_WrDat(0x08); Vertical
     * OLED_WrDat(0xC8); Vertical
     * OLED_WrDat(0x78); Landscope 正置
       OLED_WrDat(0xA8); Landscope 倒置*/
    OLED_WrDat(0x88);     //Landscope   0x48 正置 0x88 倒置
    OLED_WrCmd(0x21);     //Display inversion
    OLED_WrCmd(0x29);     //Display on
//    OLED_WrCmd(0x2A);     //Set Column Address
//    OLED_WrDat(0x00);   
//    OLED_WrDat(0x1A);     //26  
//    OLED_WrDat(0x00);   
//    OLED_WrDat(0x69);     //105 
//    OLED_WrCmd(0x2B);     //Set Page Address
//    OLED_WrDat(0x00);   
//    OLED_WrDat(0x01);    //1
//    OLED_WrDat(0x00);   
//    OLED_WrDat(0xA0);    //160
//    OLED_WrCmd(0x2C);
    }
#elif defined(CHIP_ST7789V2)
    {
    LCD_BLK_1;//打开背光
  msDelay(10);
    
    //************* Start Initial Sequence **********//
    OLED_WrCmd(0x11); //Sleep out 
    msDelay(120);              //Delay 120ms 
    //************* Start Initial Sequence **********// 
    OLED_WrCmd(0x36);
    OLED_WrDat(0x00);


    OLED_WrCmd(0x3A);            
    OLED_WrDat(0x05);

    OLED_WrCmd(0xB2);            
    OLED_WrDat(0x0C);
    OLED_WrDat(0x0C); 
    OLED_WrDat(0x00); 
    OLED_WrDat(0x33); 
    OLED_WrDat(0x33);             

    OLED_WrCmd(0xB7);            
    OLED_WrDat(0x35);

    OLED_WrCmd(0xBB);            
    OLED_WrDat(0x32); //Vcom=1.35V
                    
    OLED_WrCmd(0xC2);
    OLED_WrDat(0x01);

    OLED_WrCmd(0xC3);            
    OLED_WrDat(0x15); //GVDD=4.8V  颜色深度
                
    OLED_WrCmd(0xC4);            
    OLED_WrDat(0x20); //VDV, 0x20:0v

    OLED_WrCmd(0xC6);            
    OLED_WrDat(0x0F); //0x0F:60Hz            

    OLED_WrCmd(0xD0);            
    OLED_WrDat(0xA4);
    OLED_WrDat(0xA1); 

    OLED_WrCmd(0xE0);
    OLED_WrDat(0xD0);   
    OLED_WrDat(0x08);   
    OLED_WrDat(0x0E);   
    OLED_WrDat(0x09);   
    OLED_WrDat(0x09);   
    OLED_WrDat(0x05);   
    OLED_WrDat(0x31);   
    OLED_WrDat(0x33);   
    OLED_WrDat(0x48);   
    OLED_WrDat(0x17);   
    OLED_WrDat(0x14);   
    OLED_WrDat(0x15);   
    OLED_WrDat(0x31);   
    OLED_WrDat(0x34);   

    OLED_WrCmd(0xE1);     
    OLED_WrDat(0xD0);   
    OLED_WrDat(0x08);   
    OLED_WrDat(0x0E);   
    OLED_WrDat(0x09);   
    OLED_WrDat(0x09);   
    OLED_WrDat(0x15);   
    OLED_WrDat(0x31);   
    OLED_WrDat(0x33);   
    OLED_WrDat(0x48);   
    OLED_WrDat(0x17);   
    OLED_WrDat(0x14);   
    OLED_WrDat(0x15);   
    OLED_WrDat(0x31);   
    OLED_WrDat(0x34);
    OLED_WrCmd(0x21); 

    OLED_WrCmd(0x29);
    }
#elif defined(CHIP_SSD1322)
    {
    OLED_WrCmd(0xfd);  /*Command Lock*/ 
    OLED_WrDat(0x12);

    OLED_WrCmd(0xae); //Sleep In 

    OLED_WrCmd(0xb3);    //Set Display Clock Divide Ratio/Oscillator Frequency 
    OLED_WrDat(0x91);

    OLED_WrCmd(0xca);    //Set Multiplex Ratio 
    OLED_WrDat(0x3f);

    OLED_WrCmd(0xa2);    //Set Display Offset 
    OLED_WrDat(0x00);    //

    OLED_WrCmd(0xa1);    //Set Display Start Line 
    OLED_WrDat(0x00);    //

    OLED_WrCmd(0xa0);    //Set Re-Map $ Dual COM Line Mode
    if(0)
    {
    OLED_WrDat(0x14);  // 0x14 = 0b00010100, A2 = 1(enable Nibble Re-map) A4 = 1(scan from COM[N-1] to COM[0], where N is the Multiplex ratio) A[5] = 0
    }
    else
    {
    /* 
     * 0 0 A5 A4 0 A2 A1 A0  // A0 Horizaontal or vertial  A2 = 1(enable Nibble RE-map) 
     * 0 0  0 0  0  1  0  1  // A4 COM0~COM[N-1] 反序   A1 = 1(Enable Column Address Re-map)
     */
    OLED_WrDat(0x12);         // 0x12 正置竖屏  0x10 倒置竖屏
    }

    OLED_WrCmd(0xB5);    //Set GPIO 
    OLED_WrDat(0x00);

    OLED_WrCmd(0xab);    //Function Selection
    OLED_WrDat(0x01);    //

    OLED_WrCmd(0xb4);    //Enable External VSL 
    OLED_WrDat(0xa0);    //
    OLED_WrDat(0xfd);    //

    OLED_WrCmd(0xc1);    //Set Contrast Current
    OLED_WrDat(0xff);    

    OLED_WrCmd(0xc7);    //Master Contrast Current Control
    OLED_WrDat(0x0f);    //

    OLED_WrCmd(0xb9);    //Select Default Linear Gray Scale Table 

    OLED_WrCmd(0xb1);    //Set Phase Length
    OLED_WrDat(0xe2);    

    OLED_WrCmd(0xd1);    //Enhance Driving Scheme Capability 
    OLED_WrDat(0x82);    
    OLED_WrDat(0x20);    

    OLED_WrCmd(0xbb);    //Set Pre-Charge Voltage 
    OLED_WrDat(0x1f);    

    OLED_WrCmd(0xb6);    //Set Second Pre-Charge Period 
    OLED_WrDat(0x08);    

    OLED_WrCmd(0xbe);    //Set VCOMH Deselect Level 
    OLED_WrDat(0x07);        

    OLED_WrCmd(0xa6);    //Set Display Mode
    OLED_Fill(0x00);   //Clear Screen
    OLED_WrCmd(0xaf);    //Sleep Out        
    
    }
#elif defined(CHIP_SSD1327)
    {
    OLED_WrCmd(0xae);//Set display off
    OLED_WrCmd(0xa0);//Set re-map
    OLED_WrCmd(0x55); // 0x66: 倒置
    OLED_WrCmd(0xa1);//Set display start line
    OLED_WrCmd(0x00);
    OLED_WrCmd(0xa2);//Set display offset
    OLED_WrCmd(0x00);
    OLED_WrCmd(0xa4);//Normal Display
    OLED_WrCmd(0xa8);//Set multiplex ratio
    OLED_WrCmd(0x7f);
    OLED_WrCmd(0xab);//Function Selection A
    OLED_WrCmd(0x01);//Enable internal VDD regulator
    OLED_WrCmd(0x81);//Set contrast
    OLED_WrCmd(0x77);
    OLED_WrCmd(0xb1);//Set Phase Length
    OLED_WrCmd(0x31);
    OLED_WrCmd(0xb3);//Set Front Clock Divider /Oscillator Frequency
    OLED_WrCmd(0xb1);
    OLED_WrCmd(0xb5);//
    OLED_WrCmd(0x03);//0X03 enable
    OLED_WrCmd(0xb6);//Set Second pre-charge Period
    OLED_WrCmd(0x0d);
    OLED_WrCmd(0xbc);//Set Pre-charge voltage
    OLED_WrCmd(0x07);
    OLED_WrCmd(0xbe);//Set VCOMH
    OLED_WrCmd(0x07);
    OLED_WrCmd(0xd5);//Function Selection B
    OLED_WrCmd(0x02);//Enable second pre-charge
    OLED_Fill(0x00); //OLED_Clear(0,0,128,128,0x00);
    OLED_WrCmd(0xaf);//Display on  
    }
#elif defined(CHIP_SSD1312)
    {
    #define USE_HORIZONTAL 90
    OLED_WrByte(0xAE,OLED_CMD);//--turn off oled panel

    OLED_WrByte(0x00,OLED_CMD);    /*set lower column address*/
    OLED_WrByte(0x10,OLED_CMD);    /*set higher column address*/

    OLED_WrByte(0xB0,OLED_CMD);    /*set page address*/

    OLED_WrByte(0x81,OLED_CMD);    /*contract control*/
    OLED_WrByte(0x5f,OLED_CMD);    /*128*/

    if(USE_HORIZONTAL==0)
    {
        OLED_WrByte(0x20,OLED_CMD);    /* Set Memory addressing mode (0x20/0x21) */
        OLED_WrByte(0x09,OLED_CMD);     /* 0x09 */

        OLED_WrByte(0xA1,OLED_CMD);    /*set segment remap  0XA1 */
        OLED_WrByte(0xC8,OLED_CMD);    /*Com scan direction   0Xc8  */
    }
    else if(USE_HORIZONTAL==90)
    {
        OLED_WrByte(0x20,OLED_CMD);    /* Set Memory addressing mode (0x20/0x21) */
        OLED_WrByte(0x02,OLED_CMD);     /* 0x02 */

        OLED_WrByte(0xA1,OLED_CMD);    /*set segment remap  0XA1 */
        OLED_WrByte(0xC0,OLED_CMD);    /*Com scan direction   0Xc0  */
    }
    else if(USE_HORIZONTAL==180)
    {
    OLED_WrByte(0x20,OLED_CMD);    /* Set Memory addressing mode (0x20/0x21) */
    OLED_WrByte(0x09,OLED_CMD);     /* 0x09 */

    OLED_WrByte(0xA0,OLED_CMD);    /*set segment remap  0XA0 */
    OLED_WrByte(0xC0,OLED_CMD);    /*Com scan direction   0Xc0  */
    }
    else if(USE_HORIZONTAL==270)
    {
    OLED_WrByte(0x20,OLED_CMD);    /* Set Memory addressing mode (0x20/0x21) */
    OLED_WrByte(0x02,OLED_CMD);     /* 0x02 */

    OLED_WrByte(0xA0,OLED_CMD);    /*set segment remap  0XA0 */
    OLED_WrByte(0xC8,OLED_CMD);    /*Com scan direction   0Xc8  */
    }

    OLED_WrByte(0xA4,OLED_CMD);    /*Disable Entire Display On (0xA4/0xA5)*/

    OLED_WrByte(0xA6,OLED_CMD);    /*normal / reverse*/

    OLED_WrByte(0xA8,OLED_CMD);    /*multiplex ratio*/
    OLED_WrByte(0x3F,OLED_CMD);    /*duty = 1/64*/

    OLED_WrByte(0xD3,OLED_CMD);    /*set display offset*/
    OLED_WrByte(0x00,OLED_CMD);    /*   0x20   */

    OLED_WrByte(0xD5,OLED_CMD);    /*set osc division*/
    OLED_WrByte(0x80,OLED_CMD);

    OLED_WrByte(0xD9,OLED_CMD);    /*set pre-charge period*/
    OLED_WrByte(0x22,OLED_CMD);

    OLED_WrByte(0xDA,OLED_CMD);    /* Set SEG Pins Hardware Configuration */
    OLED_WrByte(0x10,OLED_CMD);

    OLED_WrByte(0xdb,OLED_CMD);    /*set vcomh*/
    OLED_WrByte(0x30,OLED_CMD);

    OLED_WrByte(0x8d,OLED_CMD);    /*set charge pump enable*/
    OLED_WrByte(0x72,OLED_CMD);    /* 0x12:7.5V; 0x52:8V;  0x72:9V;  0x92:10V */
    OLED_Fill(0x00);
    OLED_WrByte(0xAF,OLED_CMD);
    }
#else 
    {//从上电到下面开始初始化要有足够的时间，即等待RC复位完毕   
    OLED_WrCmd(0xFD);// 待验证新增  SSD1306pass
    OLED_WrCmd(0x12);// 待验证新增  SSD1306pass
    OLED_WrCmd(0xae);//--turn off oled panel
    OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_WrCmd(0xA0);// 待验证新增  SSD1306pass
   
    OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
    OLED_WrCmd(0x3f);//--1/64 duty  12864: 0x3f  12832: 0x1f
    OLED_WrCmd(0xd3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WrCmd(0x00);//-not offset    
    OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)

//    OLED_WrCmd(0xa6);//--set normal display  待验证可删 SSD1306pass 

    OLED_WrCmd(0xa0);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WrCmd(0xc0);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
        
    OLED_WrCmd(0x81);//--set contrast control register
    OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
    
    OLED_WrCmd(0xda);//--set com pins hardware configuration
    OLED_WrCmd(0x12);//--12864: 0x12  12832: 0x02
#ifdef CHIP_SSD1306   
    OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    
    OLED_WrCmd(0xd9);//--set pre-charge period
    OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    
    OLED_WrCmd(0xdb);//--set vcomh
    OLED_WrCmd(0x40);//Set VCOM Deselect Level
    
    OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
    OLED_WrCmd(0x14);//--set(0x10) disable
#elif defined(CHIP_SSD1309)
    OLED_WrCmd(0xD9);//--set pre-charge period
    OLED_WrCmd(0x82);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WrCmd(0xDB);//--set vcomh
    OLED_WrCmd(0x34);//Set VCOM Deselect Level
#elif defined(CHIP_SH1106)
    OLED_WrCmd(0xad);
    OLED_WrCmd(0x8b);
    OLED_WrCmd(0x33);
    OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
    OLED_WrCmd(0x14);//--set(0x10) disable
#else 
    #error "NO OLED CHIP SELECTED"
#endif
    //OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    //OLED_WrCmd(0x02);//
    OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5) //验证新增  SSD1306pass
    OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7)  //验证新增  SSD1306pass
    OLED_Fill(0x00);  //初始清屏
    OLED_WrCmd(0xaf);//--turn on oled panel
    OLED_Set_Pos(0,0);}
#endif   /* NOT SSD1331 */  
}

///* m^n */  
uint32_t mypow(uint8_t m,uint8_t n)
{
    uint32_t result=1;     
    while(n--)result*=m;    
    return result;
}
/**
*********************************************************************************************************
** @nameis OLED_PutChar
** @effect 写一个字符
** @import x 显示的列坐标 y 显示的行坐标 wan 待写字符 ucYn 字体颜色(单色OLED屏幕0为反显，1为正常显示)
** @export none
** @return none
** @create yizhi 2023.03.27
** @modify  
*********************************************************************************************************/
void OLED_PutChar(u8 x,u8 y,u8 wan, u8 ucSize, uint16_t ucYn)
{   
#if defined(OLED_COLOR)
    u8 pos,t,ucHeight;
    u32 ulTemp[2];
#else
    u8 i, j, ucTemp;
    CONST_DATA uint8_t *pucTemp; 
    ucTemp = ucTemp;
    pucTemp = pucTemp;
#endif
    
    if(x > (OLED_WIDTH - ucSize))   return; 
    if (ucSize <= 8) {
        wan = wan-' ';
    } else if (ucSize == 16){
        if (wan >= '0' && wan <= '9') {
            wan = wan - '0';
        } else if (wan >= 'A' && wan <= 'Z') {
            wan = wan - 'A' + 22;                                   /* 大写 A-Z */
        } else if (wan == 127)  {                                   /* 摄氏度   */
            wan = 10;
        } else if (wan == 128) {                                    /* 摄氏度   */
            wan = 11;
        } else if (wan == 'a') {
            wan = 12;
        } else if (wan == 'c') {
            wan = 13;
        } else if (wan == 'k') {
            wan = 14;
        } else if (wan == 'm') {
            wan = 15;
        } else if (wan == 'z') {
            wan = 16;
        } else if (wan == '.') {
            wan = 17;
        } else if (wan == '-') {
            wan = 18;
        } else if (wan == ':') {
            wan = 19;
        } else if (wan == ' ') {
            wan = 20;
        } else if (wan == '/') {
            wan = 21;
        } else if (wan == 129) {
            wan = 'Z' - 'A' + 22 + 1;
        } else if (wan == 130) {
            wan = 'Z' - 'A' + 22 + 2;
        } else {
            return;                                                 /* 不支持显示的字符 */
        }
    }  else {
        return;                                                     /* 不支持的字号     */
    }
#ifdef OLED_COLOR
    OLED_Address_Set(x, y, x + ucSize - 1, y + (ucSize << 1 & 0xF8) - 1);/* set refresh windows                  */
    for(pos = 0; pos < ucSize; pos++)                                   /* ucSize also represent character width*/
    {
        if (ucSize == 6) {
     #ifdef FEATURE_F6x8
            ulTemp[0] = F6x8[wan][pos];                                    /* 调用0608字体  */ 
            #ifdef CHIP_SSD1327
            pos++;
            ulTemp[1] = F6x8[wan][pos];                                    /* 调用0608字体  */ 
            #endif
     #else   
            ulTemp[0] = 0xCC;                                             /* 显示斑马纹 */
     #endif 
            ucHeight = 8;                                               /* ucHeight represent character Height */
        } else if (ucSize == 8) {
    #ifdef  FEATURE_F8x16
            ulTemp[0] = (F8X16[wan][pos + 8] << 8) | F8X16[wan][pos]; /* 调用1608字体 */
            #ifdef CHIP_SSD1327
            pos++;
            ulTemp[1] = (F8X16[wan][pos + 8] << 8) | F8X16[wan][pos]; /* 调用1608字体 */
            #endif
    #else 
            ulTemp[0] = 0xCCCC;                                            /* 显示斑马纹 */
    #endif
            ucHeight = 16;                                              /* ucHeight represent character Height */
        } else if (ucSize == 16) {
    #ifdef  FEATURE_F16x32
            ulTemp[0] = (F16X32[wan][pos + 48] << 24) | (F16X32[wan][pos + 32] << 16) |
                        (F16X32[wan][pos + 16] << 8 ) | (F16X32[wan][pos]);/* 调用3216字体           */
            #ifdef CHIP_SSD1327
            pos++;
            ulTemp[1] = (F16X32[wan][pos + 48] << 24) | (F16X32[wan][pos + 32] << 16) |
                        (F16X32[wan][pos + 16] << 8 ) | (F16X32[wan][pos]);/* 调用3216字体           */
            #endif
            
    #else 
            ulTemp[0] = 0xCCCCCCCC;                                        /* 显示斑马纹 */
    #endif
            ucHeight = 32;                                              /* ucHeight represent character Height */
        } else {
            return;
        }
        for (t = 0; t < ucHeight; t++)
        {
    #if defined(CHIP_SSD1331) || defined(CHIP_SSD1351) ||defined(CHIP_ST7735) || defined(CHIP_ST7789V2)
            if (ulTemp[0] & 0x01) {
                OLED_WrDat(ucYn>>8); OLED_WrDat(ucYn);//LCD_WrWord(ucYn);
            } else {
                OLED_WrDat(BACK_COLOR>>8); OLED_WrDat(BACK_COLOR);//LCD_WrWord(BACK_COLOR);
            }
            ulTemp[0] >>= 1;
    #elif defined(CHIP_SSD1322)
            t += 3;
            ucYn = 0;
            for (x = 0; x < 4; x++) {
                
                ucYn >>= 4;
                if (ulTemp[0] & 0x01) {
                    ucYn |= 0xA000;                                     /* 0x8000　高四位控制显示亮度　        */
                }
                ulTemp[0] >>= 1; 
            }
            OLED_WrDat(ucYn >> 8); 
            OLED_WrDat(ucYn);
     #elif defined(CHIP_SSD1327)
            ucYn = 0x00;                                                   
            if (ulTemp[0] & 0x01)  ucYn  = 0xF0;
            if (ulTemp[1] & 0x01)  ucYn |= 0x0F;
            ulTemp[0] >>= 1;
            ulTemp[1] >>= 1;
            OLED_WrDat(ucYn);
     #else
            #error "NO CHIP DEFINED!"
     #endif
        }
    }

#else
    for (j = 0; j < (ucSize >> 2); j++) {
        OLED_Set_Pos(x, y + j);
        for(i=0; i < ucSize; i++) {
            if (ucSize == 6) {
    #ifdef  FEATURE_F6x8
                pucTemp = &F6x8[wan][i];
    #else        
                ucTemp = 0xCC;
    #endif
            } else if (ucSize == 8) {
    #ifdef  FEATURE_F8x16
                pucTemp = &F8X16[wan][i + (ucSize * j)];
    #else 
                ucTemp = 0xCC;
    #endif
            } else if (ucSize == 16) {
    #ifdef      FEATURE_F16x32
                pucTemp = &F16X32[wan][i + (ucSize * j)];/* 调用3216字体           */
    #else 
                ucTemp = 0xCC;                                        /* 显示斑马纹 */
    #endif
            } else {
                return;
            }
    #ifdef  OLED_I2C                                                   /* I2C支持多字节写 */
            I2C_WrDatN(pucTemp, ucSize, ucYn);
            i += ucSize;
    #else
            ucYn ? OLED_WrDat(*pucTemp) : OLED_WrDat(~(*pucTemp));
    #endif
        }
    }
#endif
}
/**
*********************************************************************************************************
** @nameis OLED_PutStr
** @effect 写入一组标准ASCII字符串
** @import x 显示的列坐标 y 显示的行坐标 ch[] 显示字符串 ucSize 字体大小 ucYn 字体颜色(单色OLED屏幕0为反显，1为正常显示)
** @export none
** @return none
** @create yizhi 2023.03.19
** @modify  
*********************************************************************************************************/
void OLED_PutStr(uint8_t x,uint8_t y,uint8_t ch[], uint8_t ucSize, uint16_t ucYn)
{
    uint8_t j=0, wan;
    
    if (ucYn == 1) ucYn = FRONT_COLOR;
    while (ch[j]!='\0')
    {    
        wan = ch[j];
        if(x > (OLED_WIDTH - ucSize))
        {
            x=0;
#ifdef      OLED_COLOR
            y +=  (ucSize << 1) & 0xF8;  /*  y += (ucSize == 6 ? (LINE_HEIGHT >> 1) : LINE_HEIGHT); which is faster ? */
#else 
            y += ((ucSize & 0x08) ? 2 : 1);   
#endif
            if (y > OLED_LINE_MAX)       // 此处有一个待处理的BUG，若显示字体为6号，最后一行书写不了。
                return;
        }
        
       /* 
        * 特殊单位符号处理 ℃=0xA1E6  
        */
        switch (wan) {
            
            case 0xA1:
                wan = 127;                                                      /* 摄氏度                       */
                break;
            case 0xE6:
                wan = 128;                                                      /* 摄氏度                       */
                break;
            case 0xA6:
                wan = 129;                                                      /* 欧姆                         */
                break;
            case 0xB8:
                wan = 130;                                                      /* 欧姆                         */
                break;
            default:
                break;
        }
        OLED_PutChar(x, y, wan, ucSize, ucYn);
        x += ucSize;
        j++; 
    }
}

/**
*********************************************************************************************************
** @nameis OLED_P8x16Num
** @effect 显示N位整数 从最低位开始写
** @import x 显示的列坐标 y 显示的行坐标 m 待显示数值 ucLen 位数 ucYn 字体颜色(单色OLED屏幕0为反显，1为正常显示)
** @export none
** @return none
** @create yizhi 2023.03.19
** @modify  
*********************************************************************************************************/
void OLED_PutNum(uint8_t x,uint8_t y,int m, uint8_t ucLen, uint8_t ucSize, uint16_t ucYn)
{
    int8_t i, ucNegative = 0;
    if (ucYn == 1) ucYn = FRONT_COLOR;
    /*if (m < 0) {
        m = -m;
        OLED_PutChar(x, y, '-', ucYn);
        x += 8;
    } else {
        OLED_PutChar(x, y, ' ', ucYn);
        x += 8;
    }
    ucWei[4] = m%10;
    ucWei[3] = m%100/10;
    ucWei[2] = m%1000/100;
    ucWei[1] = m/1000%10;
    ucWei[0] = m/10000;
    
    for (i = (5 - ucLen); i < 5; i++) {
        OLED_PutChar(x, y, '0' + ucWei[i], ucYn);
        x += 8;
    } */
    if (m < 0) {
        m = -m;
        ucNegative = 1;
    }
    i = ucLen - 1;
    if (m) {
        while(m) {
            OLED_PutChar(x + i-- * ucSize, y, '0' + m % 10, ucSize, ucYn);
            m /= 10;
            if (i < 0) break;
        }
    }  else {
        OLED_PutChar(x + i-- * ucSize, y, '0', ucSize, ucYn);
    }
    if (ucNegative && i >= 0)
    {
        OLED_PutChar(x + i-- * ucSize, y, '-', ucSize, ucYn); 
    }
    while(i >= 0) {
        OLED_PutChar(x + i-- * ucSize, y, ' ', ucSize, ucYn);          /* 整数高位灭零 */ 
    }
}
/**
********************************************************************************************************
** @nameis OLED_PutNumber
** @effect 显示00000.0000 的小数, 若表示为M.N，此处M表示整数位数，N表示小数位数，比如圆周率以1.2格式显示
**         为3.14，若N为0则不显示小数点。若M=5 N=2则表示的整数部分最大范围-9999~65535，小数部分0.01~0.99
** @import x 显示的列坐标 y 显示的行坐标 m 待显示数值 M 1~5整数位数 N 0~2小数位数 pUnit 单位(ASSIC字符，
**         特别地 0为不显示单位) ucYn 字体颜色(对于单色OLED屏幕0为反显，1为正常显示)
** @export none
** @return none
** @create yizhi 2023.03.19
** @modify 
*********************************************************************************************************/
void OLED_PutNumber(uint8_t x,uint8_t y,float m, uint8_t M, uint8_t N, char* pUnit, uint8_t ucSize, uint16_t ucYn)
{
#if    0          //高位灭零  负号位置固定 小数点浮动 单位浮动
    uint8_t subshi,subbai;
    uint8_t length=0,x_temp;
    uint16_t inte,temp;//deci;
    static  uint8_t ucLastOCCupy;
    
    if (ucYn == 1) ucYn = FRONT_COLOR;
    if(m<0)
    {
        OLED_PutChar(x, y, '-', ucYn);
        m=-m;
        x+=8;
    } 
    else
    {
        OLED_PutChar(x,y,' ', ucYn); x+=8; 
    }
   inte=(int16_t)m;  // 取出整数部分 
   temp=inte;

    x_temp=x; 
    while(temp)  //看整数部分有几位 
    {
        temp /= 10;
        length ++;
        x_temp+=8;
    }
    if(length)
    {
        length--;
        while(inte)
        {
            OLED_PutChar(x+8*(length--),y,'0'+inte%10, ucYn);                         /* 先写整数部分的个位 */
            inte/=10;
        }
        x=x_temp;
    }
    else //整数部分为零  这就用不到x_temp 了
    {
      OLED_PutChar(x,y,'0', ucYn); x+=8;
    }

    if (ucFracNum > 0) {
        OLED_PutChar(x,y,'.', ucYn); x+=8; // 写小数点，小数点跟随前面的整数位数，位置浮动。
        subshi=(uint16_t)(m*10)%10;
        OLED_PutChar(x,y,'0'+subshi, ucYn); x+=8; // 显示十分位
    }
    if (ucFracNum == 2) {
        subbai=(uint16_t)(m*100)%10; 
        OLED_PutChar(x,y,'0'+subbai, ucYn); x+=8; // 显示百分位
    }
    switch (ucUnit) {
    
    case 0:                                // 摄氏度 95 + 32  96 + 32
        OLED_PutChar(x,y,'F', ucYn); x+=8;
        OLED_PutChar(x,y,'P', ucYn); x+=8;
        OLED_PutChar(x,y,'S', ucYn); x+=8;
        break;
    
    case 1:
        OLED_PutChar(x,y,'V', ucYn); x+=8;
        break;
    
    case 2:
        OLED_PutChar(x,y,'A', ucYn); x+=8;
        break;
    
    case 3:
        OLED_PutChar(x,y,'M', ucYn); x+=8;
        break;

    case 4:
        OLED_PutChar(x,y,'%', ucYn); x+=8;
        break;

    default :
        break;   
    }

    /*
     * 灭掉前一次数值显示的末尾
     */
    x_temp = x;
    while(x < ucLastOCCupy) {
        OLED_PutChar(x,y,' ', ucYn); x+=8;
    }
    ucLastOCCupy = x_temp;

#else   // 固定小数点(整数部分右对齐)  高位灭零  负号位置跟随整数 单位位置固定
    uint16_t      m_int; 
    int8_t   i=0, negative=0;
    uint8_t  subshi, x_decimal;

    if (ucYn == 1) ucYn = FRONT_COLOR;
    if (m < 0) {
        m = -m;
        negative=1; 
    }
    x_decimal = x + M * ucSize;    
    if (N != 0) {
        OLED_PutChar(x_decimal,y,'.', ucSize, ucYn); // 小数点位置固定   
        x_decimal += ucSize;
        i = 1;
        while(N--) {                                                    /* 显示所有的小数位 */
            subshi =  (uint16_t)(m * mypow(10, i++)) % 10;              /* 如果N>4 需要将此处uint16_t 更改为 uint32_t */
            OLED_PutChar(x_decimal, y, '0' + subshi, ucSize, ucYn);
            x_decimal += ucSize;
        }
        if (pUnit)
            OLED_PutStr(x_decimal, y, (uint8_t *)pUnit, ucSize, ucYn);        /*  写单位  */
    } else {
        if (pUnit)
            OLED_PutStr(x_decimal, y, (uint8_t *)pUnit, ucSize, ucYn);        /*  写单位  */
    }

    m_int = (uint16_t)m;
    i = M - 1;
    if (m_int) {
        while (m_int) {
                if (i == 0 && m_int >= 10) {
                    OLED_PutChar(x + i-- * ucSize, y, 'A'+ (m_int - 10), ucSize, ucYn); /* 位数不够用，最高显示位扩展到26个大写字母  */
                } else {
                    OLED_PutChar(x + i-- * ucSize, y, '0'+ m_int%10, ucSize, ucYn);     /* 先写整数部分的个位 */
                }
            m_int /= 10; 
            if (i < 0) break;
        }
    } else {
        OLED_PutChar(x + i-- * ucSize, y, '0', ucSize, ucYn);
    }
    if (negative && i >= 0)
    {
        OLED_PutChar(x + i-- * ucSize, y, '-', ucSize, ucYn); 
    }
    while(i >= 0) {
        OLED_PutChar(x + i-- * ucSize, y, ' ', ucSize, ucYn);          /* 整数高位灭零 */ 
    }
#endif 
/******ALienTek 数字灭零显示方式*****************************／
    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10; //  从整数的最高位开始输出  
        if(enshow==0&&t<(len-1))         // 当enshow为0时说明还遇到第一个非0最高整数位
        {
            if(temp==0)
            {
                LCD_ShowChar(x+(size/2)*t,y,' ',size,0);//灭零吧 yizhi 
                continue; // 本次循环立即结束 
            }
            else 
            {
              enshow=1; // 遇到第一个非零的最高速，之后无需进行灭零判断。
            }
              
        }
         LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
    }　*/
    /**********************野火 数字显示灭零方式**************************
        while(temp)  //  这个算法我当时想了好久，野火一下子就解决了, 厉害了，比正点原子更佳，野火从个位开始输出。 yizhi 
    {// 得到num的长度
        temp /= 10;
        length ++;
    }
    
    while( num ) // 野火用 while 用的溜呀 
    {
        /／ 从个位开始显示 
        LCD_DispChar((x+STR_WIDTH*(length--)-STR_WIDTH), y, (num%10)+'0', color);
        num /= 10;
    }     */
}
/*********prepare to lanqiao competition*************************
#function: 16进制显示uint8数组,中间加上'-'分隔符, 若形参dat传入的是一个整形指针注意大小端。
#input: 显示位置 x y 显示数据dat 显示个数N
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-03-21 by yizhi 
******************************************************************/
void OLED_HexDisp(uint8_t x,uint8_t y,uint8_t *dat,uint8_t N, uint8_t ucSize, uint16_t ucYn)
{
    uint8_t temp,i;
    if (ucYn == 1) ucYn = FRONT_COLOR; 
    
    OLED_PutChar(x,y,'0', ucSize, ucYn);  x += ucSize;
    OLED_PutChar(x,y,'x', ucSize, ucYn);  x += ucSize;
    
    for(i=0;i<N;i++)
    {
        temp = dat[i];//数组下标方式访问指针数据
        temp = dat[i] >> 4; 
        if(temp<10)
            OLED_PutChar(x,y,'0'+temp, ucSize, ucYn);
        else
            OLED_PutChar(x,y,'A'+temp-10, ucSize, ucYn);
        temp = dat[i] & 0x0f;
        x += ucSize;
        if(temp<10)
            OLED_PutChar(x,y,'0'+temp, ucSize, ucYn);
        else
            OLED_PutChar(x,y,'A'+temp-10, ucSize, ucYn);
        x+=ucSize;
    }
}
/**
***********************************************************************
* @file  OLED_PutTime() 
* @author   yizhi OLED显示时间临时存放位置
* @version 
* @date    
* @brief expect for the motify the define Don't forget the RCC_APB2PeriphClockCmd
*********************************************************************
*/
void OLED_PutTime(uint8_t x,uint8_t y,RTC_Time_s * time, uint8_t ucSize, uint16_t ucYn)
{
    if (ucYn == 1) ucYn = FRONT_COLOR;
// OLED_PutChar(x,y,time->year%100/10+0x30); x+=8;
// OLED_PutChar(x,y,time->year%10+0x30);     x+=8;
// OLED_PutChar(x,y,'-'); x+=8;
// OLED_PutChar(x,y,time->month%100/10+0x30);x+=8;
// OLED_PutChar(x,y,time->month%10+0x30);    x+=8;
// OLED_PutChar(x,y,'-'); x+=8;
// OLED_PutChar(x,y,time->date%100/10+0x30); x+=8;
// OLED_PutChar(x,y,time->date%10+0x30);     x+=8; 
// OLED_PutChar(x,y,' '); x+=8; 
// OLED_PutChar(x,y,time->hour%100/10+0x30); x+=8;
// OLED_PutChar(x,y,time->hour%10+0x30);     x+=8; 
// OLED_PutChar(x,y,':'); x+=8;
    OLED_PutChar(x,y,time->minute%100/10+0x30, ucSize, ucYn); x += ucSize;
    OLED_PutChar(x,y,time->minute%10+0x30, ucSize, ucYn);     x += ucSize; 
    OLED_PutChar(x,y,':', ucSize, ucYn);                      x += ucSize;
    OLED_PutChar(x,y,time->second%100/10+0x30, ucSize, ucYn); x += ucSize;
    OLED_PutChar(x,y,time->second%10+0x30, ucSize, ucYn);          
}

#ifdef FEATURE_HANZI

/*********************************************************************************
# 函数名称: OLED_PutChinese(uint8_t x,uint8_t y,uint8_t ch[])
# 输入参数: x，从哪一列开始输出 y，从哪一行开始输出,字模存放的书组ch[][],uCYn 汉字颜色
# 输出参数: void
# 功    能: 索引形式输出汉字, 字模为14x16(这样使得12864OLED每行能够多显示一个汉字)
# 其    他：找不到字模输出一方块, 需要使用取模软件取模 
# 作    者: 2016/1/21,by yizhi
*********************************************************************************/
void OLED_PutHan(uint8_t x,uint8_t y,uint8_t ch[], uint16_t ucYn)
{
    uint8_t wm=0,ii = 0;
    uint16_t adder=0; 
    #ifdef OLED_COLOR
        uint8_t i;
        uint16_t usTemp[2];
    #endif

    while(ch[ii] != '\0') //挨个输出每个汉字
    {
        wm = 0;
        adder = 0;
        while(F14x16_Idx[wm] > 127) // >127不是字符 yizhi
        {
              if(F14x16_Idx[wm] == ch[ii])
              {
                      if(F14x16_Idx[wm + 1] == ch[ii + 1])
                      {
                              adder = wm * 14;
                              break;
                      }
              }
              wm += 2;
        }  // 挨个（遍历）找出对应字模  yizhi  

        if(x > (OLED_WIDTH - 8))
        {
            x=0;
#ifdef      OLED_COLOR
            y +=  (8 << 1) & 0xF8;  /*  y += (ucSize == 6 ? (LINE_HEIGHT >> 1) : LINE_HEIGHT); which is faster ? */
#else 
            y += ((8 & 0x80) ? 2 : 1);
#endif
            if (y > OLED_LINE_MAX)       // 此处有一个待处理的BUG，若显示字体为6号，最后一行书写不了。
                return;
        }
#ifdef OLED_COLOR
        OLED_Address_Set(x, y, x + 14 - 1, y + (16) - 1);
        for(wm = 0;wm < 14;wm++)                       /* 每个汉字需要扫描14列 */
        {
            usTemp[0] = (F14x16[adder + wm + 14] << 8) | F14x16[adder + wm];  /* 得到要扫描的第N列数据 每列数据包含16个点 */
            #ifdef CHIP_SSD1327
            wm++;
            usTemp[1] = (F14x16[adder + wm + 14] << 8) | F14x16[adder + wm];  /* 得到要扫描的第N列数据 每列数据包含16个点 */
            #endif
            for (i = 0; i < 16; i++)
            {
        #if defined(CHIP_SSD1331) || defined(CHIP_SSD1351) ||defined(CHIP_ST7735) || defined(CHIP_ST7789V2)
                if (usTemp[0] & 0x01) {
                    OLED_WrDat(ucYn>>8); OLED_WrDat(ucYn);//LCD_WrWord(ucYn);
                } else {
                    OLED_WrDat(BACK_COLOR>>8); OLED_WrDat(BACK_COLOR);//LCD_WrWord(BACK_COLOR);
                }
                usTemp[0] >>= 1;
        #elif defined(CHIP_SSD1322)
                i += 3;
                ucYn = 0;
                for (x = 0; x < 4; x++) {
                    
                    ucYn >>= 4;
                    if (usTemp[0] & 0x01) {
                        ucYn |= 0x8000;                                     /* 0x8000　高四位控制显示亮度　        */
                    }
                    usTemp[0] >>= 1; 
                }
                OLED_WrDat(ucYn >> 8); 
                OLED_WrDat(ucYn);
        #elif defined(CHIP_SSD1327)
                ucYn = 0x00;                                                   
                if (usTemp[0] & 0x01)  ucYn  = 0xF0;
                if (usTemp[1] & 0x01)  ucYn |= 0x0F;
                usTemp[0] >>= 1;
                usTemp[1] >>= 1;
                OLED_WrDat(ucYn);
        #else
            #error "NO CHIP DEFINED !"
        #endif
            }
        }
#else
        OLED_Set_Pos(x , y);
        for(wm = 0;wm < 14;wm++)
        {
            ucYn ? OLED_WrDat(F14x16[adder++]) : OLED_WrDat(~F14x16[adder++]);
        }      
        OLED_Set_Pos(x,y + 1); 
        for(wm = 0;wm < 14;wm++)          
        {
            ucYn ? OLED_WrDat(F14x16[adder++]) : OLED_WrDat(~F14x16[adder++]);
        }

#endif
        x += 14;
        ii += 2;
    }
}
/**
********************************************************************************************************
** @nameis OLED_PutNumber
** @effect 输出汉字和字符混合字符串
** @import x 显示的列坐标 y 显示的行坐标 ch[] 包含汉字和字符的字符串 ucYn 字体颜色(对于单色OLED屏幕0为反显，1为正常显示)
** @export none
** @return none
** @create yizhi 2023.03.19
** @modify 
*********************************************************************************************************/
void OLED_Print(uint8_t x,uint8_t y,uint8_t ch[], uint16_t ucYn)
{
    uint8_t ch2[3];
    uint8_t ii=0;
    while(ch[ii] != '\0')
    {
        if(ch[ii] > 127)
        {
            if (x > (OLED_WIDTH - 14))
            {
                x=0;
    #ifdef      OLED_COLOR
                y +=  (8 << 1) & 0xF8;  /*  y += (ucSize == 6 ? (LINE_HEIGHT >> 1) : LINE_HEIGHT); which is faster ? */
    #else 
                y += ((8 & 0x08) ? 2 : 1);   
    #endif
                if (y > OLED_LINE_MAX)       // 此处有一个待处理的BUG，若显示字体为6号，最后一行书写不了。
                    return;
            }
            ch2[0] = ch[ii];
            ch2[1] = ch[ii + 1];
            ch2[2] = '\0';            //汉字为两个字节
            OLED_PutHan(x , y, ch2, ucYn);    //显示汉字
            x += 14;
            ii += 2;
        }
        else
        {
            if (x > (OLED_WIDTH - 8))
            {
                x=0;
    #ifdef      OLED_COLOR
                y +=  (8 << 1) & 0xF8;  /*  y += (ucSize == 6 ? (LINE_HEIGHT >> 1) : LINE_HEIGHT); which is faster ? */
    #else 
                y += ((8 & 0x08) ? 2 : 1);   
    #endif
                if (y > OLED_LINE_MAX)       // 此处有一个待处理的BUG，若显示字体为6号，最后一行书写不了。
                    return;
            }
            ch2[0] = ch[ii];    
            OLED_PutChar(x, y , ch2[0], 8, ucYn);    //显示字母
            x += 8;
            ii+= 1;
        }
    }
}
#endif


#ifdef FEATURE_F16x32

/*
* p: 显示的位置 ，只有两行  第一行 0 第二行 1
 *num： 显示的数据，末位有一小数点。  
 *uint： 数据的单位 uint16_t= 0 no sybmol 1温度2（V）3（A）4（cm） 
*/
//void OLED_P16x32Num(uint8_t p,int num,uint8_t unit)  
//{
//  uint8_t ge=0,shi=0,bai=0,qian=0, wan = 0;
//  uint8_t i=0;
//  uint8_t x=0,y=0;
//  //uint8_t pos=x;
// 
//  if(p==1)  y=4;
//  if (num < 0) num = 0;
//  
//  ge  =num%10;
//  shi =num%100/10;
//  bai =num%1000/100;
//  qian=num%10000/1000;
//  wan =num/10000;
//  
// 
// 
//  if(wan!=0)
//  {
//    OLED_Set_Pos(x,y);                  //write "wan"
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[wan*64+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[wan*64+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[wan*64+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[wan*64+i+48]); 
//    x+=16;
//  }
//  if(qian!=0 || wan != 0)
//  {
//    OLED_Set_Pos(x,y);                  //write "qian"
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[qian*64+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[qian*64+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[qian*64+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[qian*64+i+48]); 
//    x+=16;
//  }
//  
//  if(bai!=0 || qian!=0 || wan != 0)
//  {
//     OLED_Set_Pos(x,y);                  //write "bai"
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[bai*64+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[bai*64+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[bai*64+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[bai*64+i+48]); 
//    x+=16;
//  }
//  OLED_Set_Pos(x,y);                  //write "shi"
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[shi*64+i]);
//  OLED_Set_Pos(x,y+1);
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[shi*64+i+16]); 
//  OLED_Set_Pos(x,y+2);
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[shi*64+i+32]); 
//  OLED_Set_Pos(x,y+3);
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[shi*64+i+48]); 
//  x+=16;
//  
//  OLED_Set_Pos(x,y);                  //write "dot"
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[18*64+i]);
//  OLED_Set_Pos(x,y+1);
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[18*64+i+16]); 
//  OLED_Set_Pos(x,y+2);
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[18*64+i+32]); 
//  OLED_Set_Pos(x,y+3);
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[18*64+i+48]); 
//  x+=16;
//  OLED_Set_Pos(x,y);                  //write "ge"
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge*64+i]);
//  OLED_Set_Pos(x,y+1);
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge*64+i+16]); 
//  OLED_Set_Pos(x,y+2);
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge*64+i+32]); 
//  OLED_Set_Pos(x,y+3);
//  for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge*64+i+48]); 
//  x+=16;
//  switch(unit)
//  {
//    
//  case 0 : break;
//  case 1 :
//    OLED_Set_Pos(x,y);                  //write 'C
//    for(i=0;i<32;i++)
//      OLED_WrDat(F16X32[10*64+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<32;i++)
//      OLED_WrDat(F16X32[10*64+i+32]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<32;i++)
//      OLED_WrDat(F16X32[10*64+i+64]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<32;i++)
//      OLED_WrDat(F16X32[10*64+i+96]); 
//    x+=32;//x+=16; 
//    break;
//  case 2 :
//    OLED_Set_Pos(x,y);                  //write V
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[12*64+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[12*64+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[12*64+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[12*64+i+48]); 
//    x+=16;
//    break;
//  case 3 :
//    OLED_Set_Pos(x,y);                  //write A
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[14*64+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[14*64+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[14*64+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[14*64+i+48]); 
//    x+=16;
//    break;
//  case 4 :
//    OLED_Set_Pos(x,y);                  //write cm
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[16*64+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[16*64+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[16*64+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[16*64+i+48]);
//    x+=16;
//    
//    OLED_Set_Pos(x,y);                 
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[16*64+i+64]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[16*64+i+80]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[16*64+i+96]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//      OLED_WrDat(F16X32[16*64+i+112]); 
//    x+=16;
//    
//    break;

//  }
//  while(x<112)  // 空余位清零 
//  {
//    OLED_Set_Pos(x,y);                 
//    for(i=0;i<16;i++)
//      OLED_WrDat(0x00);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//      OLED_WrDat(0x00); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//      OLED_WrDat(0x00); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//      OLED_WrDat(0x00); 
//    x+=16;
//  }
//}

/*
 * p : 0 写在第一行，1 写在第二行   OLED 12864屏幕就只有两行 
 */
//void OLED_P16x32Time(uint8_t p, RTC_Time_s *ptTime)
//{
//    uint16_t ge=0;
//    uint8_t i=0;
//    uint8_t x=0,y=0;
//    //uint8_t pos=x;
//    
//    if(p==1)  y=4;
//    ge = (ptTime->hour / 10)*64;
//    OLED_Set_Pos(x,y);                 
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+48]); 
//    x+=16;

//    ge = (ptTime->hour % 10)*64;
//    OLED_Set_Pos(x,y);                 
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+48]); 
//    x+=16;
//    
//    OLED_Set_Pos(x,y);                  //write "dot"
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[20*64+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[20*64+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[20*64+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[20*64+i+48]); 
//    x+=16;
//    
//    ge = (ptTime->minute / 10)*64;
//    OLED_Set_Pos(x,y);                 
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+48]); 
//    x+=16;

//    ge = (ptTime->minute % 10)*64;
//    OLED_Set_Pos(x,y);                 
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+48]); 
//    x+=16;
//    
//    
//    OLED_Set_Pos(x,y);                  //write "dot"
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[20*64+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[20*64+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[20*64+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[20*64+i+48]); 
//    x+=16;
//    
//    ge = (ptTime->second / 10)*64;
//    OLED_Set_Pos(x,y);                 
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+48]); 
//    x+=16;

//    ge = (ptTime->second % 10)*64;
//    OLED_Set_Pos(x,y);                 
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i]);
//    OLED_Set_Pos(x,y+1);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+16]); 
//    OLED_Set_Pos(x,y+2);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+32]); 
//    OLED_Set_Pos(x,y+3);
//    for(i=0;i<16;i++)
//    OLED_WrDat(F16X32[ge+i+48]); 
//    x+=16;
//     
//    while(x<112)  // 空余位清零 
//    {
//        OLED_Set_Pos(x,y);                 
//        for(i=0;i<16;i++)
//        OLED_WrDat(0x00);
//        OLED_Set_Pos(x,y+1);
//        for(i=0;i<16;i++)
//        OLED_WrDat(0x00); 
//        OLED_Set_Pos(x,y+2);
//        for(i=0;i<16;i++)
//        OLED_WrDat(0x00); 
//        OLED_Set_Pos(x,y+3);
//        for(i=0;i<16;i++)
//        OLED_WrDat(0x00); 
//        x+=16;
//    }
//}
#endif

    /*
     * stiumulate the clock
     */
void timeClockStep(RTC_Time_s *ptTime)
{
     if (++ptTime->second == 60) {
       if (++ptTime->minute == 60) { 
           if (++ptTime->hour   == 24) {
               ptTime->date++;   
               ptTime->hour   = 0;
           }
           ptTime->minute = 0;
       }               
       ptTime->second = 0;
    }
}



#ifdef FEATURE_BMP
//==============================================================
//函数名： void Draw_BMP(uint8_t x,uint8_t y)
//功能描述：显示BMP图片128×64
//参数：起始点坐标(x,y),x的范围0～127，y为页的范围0～7
//返回：无
//==============================================================
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1)
{     
    unsigned int  ii=0;
    uint8_t x,y;
    
   // if(y1%8==0) y=y1/8;      
   // else y=y1/8+1;
    for(y=y0;y<=y1;y++)
    {
        OLED_Set_Pos(x0,y);                
        for(x=x0;x<x1;x++)
        {      
            OLED_WrDat(Scale1_8x128[ii++]);            
        }
    }
//    for(i=0;i<128;i++)
//    {
//             
//  }
}
#endif


