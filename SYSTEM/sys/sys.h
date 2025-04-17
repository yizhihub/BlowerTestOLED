#ifndef __SYS_H
#define __SYS_H    
#include "stm32f10x.h"
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//此为数据保存区地址（FLSAH最后一个bank）
#define PWMCTRL1_ADDR   0x0803F000
#define PWMCTRL2_ADDR   0x0803F00C
#define INA226_ADDR     0x0803F00E
#define BRAOCALIB_ADDR  0x0803F012
#define MOTORCHOS_ADDR  0X0803F014

/*********************************************************************************************************
  通用数据类型重定义
*********************************************************************************************************/
typedef unsigned char           BOOL;                                   /* 布尔变量                     */
typedef unsigned char           INT8U;                                  /* 无符号8位整型变量            */
typedef signed   char           INT8S;                                  /* 有符号8位整型变量            */
typedef unsigned short          INT16U;                                 /* 无符号16位整型变量           */
typedef signed   short          INT16S;                                 /* 有符号16位整型变量           */
typedef unsigned long           INT32U;                                 /* 无符号32位整型变量           */
typedef signed   long           INT32S;                                 /* 有符号32位整型变量           */
typedef unsigned long long      INT64U;                                 /* 无符号64位整型变量           */
typedef signed long long        INT64S;                                 /* 有符号64位整型变量           */
typedef float                   FP32;                                   /* 单精度浮点数（32位长度）     */
typedef double                  FP64;                                   /* 双精度浮点数（64位长度）     */
typedef unsigned char           uchar;

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

typedef union floattohex
{
    float fdata;
    uint8_t hdata[4];
} FloatToHex;

//以下为汇编函数
void WFI_SET(void);     //执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void); //开启所有中断
void MSR_MSP(u32 addr); //设置堆栈地址

#endif
