#ifndef _bOLED_H
#define _bOLED_H

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
    #include "HwDrv.h"
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

/*
 * slection among CHIP_SSD1306,CHIP_SH1106,CHIP_SSD1309, CHIP_SSD1312(i2c), CHIP_SSD1322, CHIP_SSD1327, CHIP_SSD1331,CHIP_SSD1351
 * CHIP_ST7735, CHIP_ST7789V2,。 
 * 注释： SSD1309是 2.42寸大屏，分辨率12864
 *        SSD1322和SSD1327是灰度，前者是64x256,后者是128x128。
 *        SSD1331和SSD1351是彩屏，前者是96x64，后者是128x128。
 */

#if  defined(CHIP_SSD1331) || defined(CHIP_SSD1351) || defined(CHIP_SSD1322) || defined(CHIP_SSD1327) || defined(CHIP_ST7735) || defined(CHIP_ST7789V2)
#define OLED_COLOR
#endif

#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0x7F 
#define OLED_WIDTH 128
#define OLED_HIGH  64
#define LINE_HEIGHT 2

/*
 * 显示屏尺寸调整
 */
#if defined(CHIP_SSD1331)
    #undef OLED_WIDTH
    #undef LINE_HEIGHT
    #define OLED_WIDTH 96
    #define LINE_HEIGHT 16
    #define OLED_LINE_MAX  3*LINE_HEIGHT
#elif defined(CHIP_SSD1351) || defined(CHIP_SSD1327)
    #undef OLED_HIGH
    #undef LINE_HEIGHT
    #define OLED_HIGH   128
    #define LINE_HEIGHT 16
    #define OLED_LINE_MAX  7*LINE_HEIGHT
#elif defined(CHIP_ST7735)
    #undef OLED_WIDTH
    #undef OLED_HIGH
    #undef LINE_HEIGHT
    #define OLED_WIDTH   160
    #define OLED_HIGH    80
    #define LINE_HEIGHT  16
    #define OLED_LINE_MAX  4*LINE_HEIGHT
#elif defined(CHIP_SSD1322)                                             /* 基于目前的列行式字模 该IC仅支持竖屏显示 64x256  */
    #undef OLED_WIDTH
    #undef OLED_HIGH
    #undef LINE_HEIGHT
    #define OLED_WIDTH   64
    #define OLED_HIGH    256
    #define LINE_HEIGHT  16
    #define OLED_LINE_MAX  15*LINE_HEIGHT
#elif defined(CHIP_ST7789V2)
    #undef OLED_WIDTH
    #undef OLED_HIGH
    #undef LINE_HEIGHT
    #define OLED_WIDTH   240
    #define OLED_HIGH    280
    #define LINE_HEIGHT  16
    #define OLED_LINE_MAX  16*LINE_HEIGHT
#else
    #define OLED_LINE_MAX  3*LINE_HEIGHT
#endif


#define OLED_LINE0 0*LINE_HEIGHT
#define OLED_LINE1 1*LINE_HEIGHT
#define OLED_LINE2 2*LINE_HEIGHT
#define OLED_LINE3 3*LINE_HEIGHT
#define OLED_LINE4 4*LINE_HEIGHT
#define OLED_LINE5 5*LINE_HEIGHT
#define OLED_LINE6 6*LINE_HEIGHT
#define OLED_LINE7 7*LINE_HEIGHT
#define OLED_LINE8 8*LINE_HEIGHT
#define OLED_LINE9 9*LINE_HEIGHT
#define OLED_LINE10 10*LINE_HEIGHT
#define OLED_LINE11 11*LINE_HEIGHT
#define OLED_LINE12 12*LINE_HEIGHT
#define OLED_LINE13 13*LINE_HEIGHT
#define OLED_LINE14 14*LINE_HEIGHT
#define OLED_LINE15 15*LINE_HEIGHT

/*
 * other alternative feature
 */
#define FEATURE_F6x8
#define FEATURE_F8x16
#define FEATURE_F16x32
#define FEATURE_HANZI
//#define FEATURE_BMP

typedef struct yizhi_RTC_Time_s
{
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char week;
    unsigned char date;
    unsigned char month;
    unsigned char year;                                                 /*  ture year=2000+year;        */  
}RTC_Time_s;

#ifdef C51_PLATEFORM

    #if defined(IAP15W413ASDIP16)
    sbit OLED_SCL= P1^2;                                                  /* 15W408AS  DIP16 Right 倒置*/
    sbit OLED_SDA= P1^3;
    sbit OLED_RST= P1^4;
    sbit OLED_DC = P1^5;
    sbit OLED_CS = P5^4;
    #elif defined(IAP15W413ASDIP28)
    sbit OLED_SCL= P2^6;                                                  /* 15W408AS  DIP28   倒置       */
    sbit OLED_SDA= P2^7;
    sbit OLED_RST= P1^0;
    sbit OLED_DC = P1^1;
    sbit OLED_CS = P1^2;
    sbit LCD_BLK = P1^3;
    #elif defined(STC15W408ASDIP16)
    sbit OLED_SCL= P5^4;                                                    /* 15W408AS  DIP16 Left 正置 */
    sbit OLED_SDA= P1^5;
    sbit OLED_RST= P1^4;
    sbit OLED_DC = P1^3;
    sbit OLED_CS = P1^2;
	#elif defined(IAP15W413ASSOP20)
	sbit OLED_SCL= P1^2;                                                    /* 15W408AS  SOP20 Left 正置 */
    sbit OLED_SDA= P1^3;
    sbit OLED_RST= P1^4;
    sbit OLED_DC = P1^5;
    sbit OLED_CS = P1^6;
	sbit LCD_BLK = P1^7;
    #endif
    
	#define LCD_BLK_0    LCD_BLK = 0
    #define LCD_BLK_1    LCD_BLK = 1
    #define OLED_SCL_0    OLED_SCL = 0
    #define OLED_SCL_1    OLED_SCL = 1
    #define OLED_SDA_0    OLED_SDA = 0
    #define OLED_SDA_1    OLED_SDA = 1
    #define OLED_RST_0    OLED_RST = 0
    #define OLED_RST_1    OLED_RST = 1
    #define OLED_DC_0     OLED_DC = 0
    #define OLED_DC_1     OLED_DC = 1
    #define OLED_CS_0     OLED_CS = 0
    #define OLED_CS_1     OLED_CS = 1
    #define OLED_SDA_R    OLED_SDA
    #define OLED_SDA_IN      ;
    #define OLED_SDA_OUT     ;
    
#elif defined(STM32F10X_HD) || defined(STM32F10X_MD)

    #ifdef  OLED_SPI
    
        #ifdef GD32F30X_HD
        #define OLED_SCL  PBout(3)
        #define OLED_SDA  PBout(5)
        #define OLED_RST  PCout(8)            /* 在MINISTM32上直接接到了STM32的复位脚！*/
        #define OLED_DC   PCout(9) 
        #define OLED_CS   PAout(15)
        #define OLED_SCL_0    GPIOB->BRR  = 1 << 3// //OLED_SCL = 0
        #define OLED_SCL_1    GPIOB->BSRR = 1 << 3// //OLED_SCL = 1
        #define OLED_SDA_0    GPIOB->BRR  = 1 << 5// //OLED_SDA = 0
        #define OLED_SDA_1    GPIOB->BSRR = 1 << 5// //OLED_SDA = 1
        #define OLED_RST_0    GPIOC->BRR  = 1 << 8// //OLED_RST = 0
        #define OLED_RST_1    GPIOC->BSRR = 1 << 8// //OLED_RST = 1
        #define OLED_DC_0     GPIOC->BRR  = 1 << 9// //OLED_DC  = 0
        #define OLED_DC_1     GPIOC->BSRR = 1 << 9// //OLED_DC  = 1
        #define OLED_CS_0     GPIOA->BRR  = 1 << 15// //OLED_CS  = 0
        #define OLED_CS_1     GPIOA->BSRR = 1 << 15// //OLED_CS  = 1      /* 改为BRR BSRR寄存器 5.5958ms -> 4.4171ms  */
        #else
        #define OLED_SCL  PCout(6)
        #define OLED_SDA  PBout(0)
        #define OLED_RST  PBout(2)            /* 在MINISTM32上直接接到了STM32的复位脚！*/
        #define OLED_DC   PBout(4) 
        #define OLED_CS   PBout(6)
        #define OLED_SCL_0    GPIOC->BRR  = 1 << 6// //OLED_SCL = 0
        #define OLED_SCL_1    GPIOC->BSRR = 1 << 6// //OLED_SCL = 1
        #define OLED_SDA_0    GPIOB->BRR  = 1 << 0// //OLED_SDA = 0
        #define OLED_SDA_1    GPIOB->BSRR = 1 << 0// //OLED_SDA = 1
        #define OLED_RST_0    GPIOB->BRR  = 1 << 2// //OLED_RST = 0
        #define OLED_RST_1    GPIOB->BSRR = 1 << 2// //OLED_RST = 1
        #define OLED_DC_0     GPIOB->BRR  = 1 << 4// //OLED_DC  = 0
        #define OLED_DC_1     GPIOB->BSRR = 1 << 4// //OLED_DC  = 1
        #define OLED_CS_0     GPIOB->BRR  = 1 << 6// //OLED_CS  = 0
        #define OLED_CS_1     GPIOB->BSRR = 1 << 6// //OLED_CS  = 1      /* 改为BRR BSRR寄存器 5.5958ms -> 4.4171ms  */
        #endif
    #if defined(CHIP_ST7735) || defined(CHIP_ST7789V2) 
    #define LCD_BLK_0     GPIOB->BRR  = 1 << 8
    #define LCD_BLK_1     GPIOB->BSRR = 1 << 8
    #endif
    
    #elif   defined(OLED_I2C)   
#if  1
    #define OLED_SCL         PAout(13)
    #define OLED_SDA         PAout(14)
    #define OLED_SDA_R       PAin(14)
    #define OLED_SDA_IN      {GPIOA->CRH &= 0XF0FFFFFF; GPIOA->CRH |= 8 << 24;}
    #define OLED_SDA_OUT     {GPIOA->CRH &= 0XF0FFFFFF; GPIOA->CRH |= 3 << 24;}
#else
    #define OLED_SCL         PBout(6)
    #define OLED_SDA         PBout(7)
    #define OLED_SDA_R       PBin(7)
    #define OLED_SDA_IN      {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 8 << 28;}
    #define OLED_SDA_OUT     {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 3 << 28;}

#endif
    #define OLED_SCL_0   OLED_SCL = 0 //GPIOA->BRR  = 1 << 13
    #define OLED_SCL_1   OLED_SCL = 1 //GPIOA->BSRR = 1 << 13
    #define OLED_SDA_0   OLED_SDA = 0 //GPIOA->BRR  = 1 << 14
    #define OLED_SDA_1   OLED_SDA = 1 //GPIOA->BSRR = 1 << 14
    #define OLED_CMD  0
    #define OLED_DATA 1
    
    #else 
    
    #error "NO OLED INTERFACE DEFINED"
    
    #endif   /* #ifdef OLED_SPI */
#elif defined(LKS32MC08x)
    #ifdef  OLED_SPI
    
    #elif   defined(OLED_I2C)   
#if  1
    #define OLED_SDA_R       (GPIO0->PDI & BIT7)
    #define OLED_SDA_IN      {GPIO0->POE &= ~BIT7; GPIO0->PIE |= BIT7;}
    #define OLED_SDA_OUT     {GPIO0->PIE &= ~BIT7; GPIO0->POE |= BIT7;}
#else
    #define OLED_SCL         PBout(6)
    #define OLED_SDA         PBout(7)
    #define OLED_SDA_R       PBin(7)
    #define OLED_SDA_IN      {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 8 << 28;}
    #define OLED_SDA_OUT     {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= 3 << 28;}

#endif
    #define OLED_SCL_0    GPIO0->PDO &= ~BIT6
    #define OLED_SCL_1    GPIO0->PDO |=  BIT6
    #define OLED_SDA_0    GPIO0->PDO &= ~BIT7
    #define OLED_SDA_1    GPIO0->PDO |=  BIT7
    #define OLED_CMD  0
    #define OLED_DATA 1
    
    #else 
    
    #error "NO OLED INTERFACE DEFINED"
    
    #endif   /* #ifdef OLED_SPI */

#elif defined(RT1052)

    #ifdef OLED_SPI
    
    /* OLED Pin Group 1*/
//    #define OLED_SCL_0    GPIO5->DR  &= ~(1 << 2)
//    #define OLED_SCL_1    GPIO5->DR  |=  (1 << 2)
//    #define OLED_SDA_0    GPIO3->DR  &= ~(1 << 4)
//    #define OLED_SDA_1    GPIO3->DR  |= (1 << 4)
//    #define OLED_RST_0    GPIO3->DR  &= ~(1 << 2)
//    #define OLED_RST_1    GPIO3->DR  |= (1 << 2)
//    #define OLED_DC_0     GPIO2->DR  &= ~(1 << 30)
//    #define OLED_DC_1     GPIO2->DR  |= (1 << 30)
//    #define OLED_CS_0     GPIO1->DR  &= ~(1 << 20)
//    #define OLED_CS_1     GPIO1->DR  |= (1 << 20)
    /* OLED Pin Group 2*/
    #define OLED_SCL_0    GPIO5->DR  &= ~(1 << 1)
    #define OLED_SCL_1    GPIO5->DR  |=  (1 << 1)
    #define OLED_SDA_0    GPIO3->DR  &= ~(1 << 5)
    #define OLED_SDA_1    GPIO3->DR  |= (1 << 5)
    #define OLED_RST_0    GPIO3->DR  &= ~(1 << 3)
    #define OLED_RST_1    GPIO3->DR  |= (1 << 3)
    #define OLED_DC_0     GPIO2->DR  &= ~(1 << 31)
    #define OLED_DC_1     GPIO2->DR  |= (1 << 31)
    #define OLED_CS_0     GPIO1->DR  &= ~(1 << 21)
    #define OLED_CS_1     GPIO1->DR  |= (1 << 21)
    
    #elif defined(OLED_I2C)
    
    /* OLED I2C Pin Group 1*/
    #define OLED_SCL_0      GPIO4->DR &= ~(1 << 12u)
    #define OLED_SCL_1      GPIO4->DR |= (1 << 12u)
    #define OLED_SDA_0      GPIO4->DR &= ~(1 << 11u)
    #define OLED_SDA_1      GPIO4->DR |= (1 << 11u)
    #define OLED_SDA_IN     GPIO4->GDIR &= ~(1 << 11u)
    #define OLED_SDA_OUT    GPIO4->GDIR |= (1 << 11u)
    #define OLED_SDA_R      (GPIO4->DR & (1 << 11u))
    
    #else 
    
    #error "NO OLED INTERFACE DEFINED"

    #endif   /* #ifdef OLED_SPI */
#endif

void OLED_WrDat(unsigned char dat);
void OLED_WrCmd(unsigned char cmd);
void OLED_Init(void);
void OLED_Fill(unsigned int usData);
void OLED_ClearLine(uint8_t ucStartDine, uint8_t ucEndDine,unsigned int usData);
void OLED_PutPixel(uint8_t x,uint8_t y);
void OLED_DrawBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);
/* 汉字显示 */
void OLED_PutHan(uint8_t x,uint8_t y,uint8_t ch[], uint16_t ucYn);
void OLED_Print(uint8_t x, uint8_t y, uint8_t ch[], uint16_t ucYn);
/* 字符显示 */
void OLED_PutChar(uint8_t x,uint8_t y,uint8_t wan, uint8_t ucSize, uint16_t ucYn);
void OLED_PutStr(uint8_t x,uint8_t y,uint8_t ch[], uint8_t ucSize, uint16_t ucYn);
/* 数值显示 */
void OLED_PutNum(uint8_t x,uint8_t y,int m, uint8_t ucLen, uint8_t ucSize, uint16_t ucYn);
void OLED_PutNumber(uint8_t x,uint8_t y,float m, uint8_t M, uint8_t N, char* pUnit, uint8_t ucSize, uint16_t ucYn);

/* 特殊数值显示 */
void OLED_P16x32Num(uint8_t p,int num,uint8_t unit);// 特大字体
void OLED_HexDisp(uint8_t x,uint8_t y,uint8_t *dat,uint8_t N, uint8_t ucSize, uint16_t ucYn);
void OLED_P16x32Time(uint8_t p, RTC_Time_s *ptTime);//大字体时钟显示
void OLED_PutTime(uint8_t x,uint8_t y,RTC_Time_s * time, uint8_t ucSize, uint16_t ucYn);//小字体时钟显示;
void timeClockStep(RTC_Time_s *ptTime);

extern uint16_t BACK_COLOR;
extern uint16_t FRONT_COLOR;
//颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
#endif

