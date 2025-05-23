#ifndef __bFLASH_H__
#define __bFLASH_H__

#ifdef C51_PLATEFORM
#include "common.h"
#elif defined(GD32F30X_HD)
#include "sys.h"
#include "stm32f10x_flash.h"
#else
#error "NO PLEATEFORM DEFINED!"
#endif

//用户根据自己的需要设置
#define GD32_FLASH_SIZE 512            //所选STM32的FLASH容量大小(单位为K)
#define GD32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define GD32_FLASH_BASE 0x08000000     //GD32 FLASH的起始地址
//FLASH解锁键值
 

uint16_t FLASH_ReadHalfWord(uint32_t faddr);          //读出半字  
void FLASH_WriteLenByte(uint32_t WriteAddr, uint32_t DataToWrite, uint16_t Len);    //指定地址开始写入指定长度的数据
uint32_t FLASH_ReadLenByte(uint32_t ReadAddr, uint16_t Len);                        //指定地址开始读取指定长度数据
void FLASH_Write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite);       //从指定地址开始写入指定长度的数据
void FLASH_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);          //从指定地址开始读出指定长度的数据

#endif
