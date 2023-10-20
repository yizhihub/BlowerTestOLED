/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               bKey.h
** Created By  :            YZ
** Created Date:            2023-10-11
** Last Version:            v1.0 
** Descriptions:            ARM_PLATEFORM
**                          FLASH模拟EEPROM操作，支持地址连续读写
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
#include "bFLASH.h"

#if GD32_FLASH_SIZE < 256
#define GD_SECTOR_SIZE 1024 //字节
#else 
#define GD_SECTOR_SIZE    2048
#endif


/**
********************************************************************************************************
** @nameis FLASH_ReadHalfWord
** @effect 读取指定地址的半字(16位数据)
** @import faddr:读地址(此地址必须为2的倍数!!)
** @export none
** @return 返回值:对应数据.
** @create
** @modify 
*********************************************************************************************************/
uint16_t FLASH_ReadHalfWord(uint32_t faddr)
{
    return *(vu16*)faddr; 
}

#if GD32_FLASH_WREN    //如果使能了写  
/**
********************************************************************************************************
** @nameis FLASH_Write_NoCheck
** @effect 无检查写入
** @import ulWriteAddr:Start Address
**         psBuffer：Data Pointer
**         usNumToWrite:Number to write16
** @export none
** @return none
** @create
** @modify 
*********************************************************************************************************/
void FLASH_Write_NoCheck(uint32_t ulWriteAddr, uint16_t *psBuffer, uint16_t usNumToWrite)   
{                       
    uint16_t i;
    for(i = 0;i < usNumToWrite;i++)
    {
        FLASH_ProgramHalfWord(ulWriteAddr, psBuffer[i]);
        ulWriteAddr += 2;//地址增加2.
    }  
} 

/**
********************************************************************************************************
** @nameis FLASH_Write
** @effect 带检查写入
** @import ulWriteAddr:Start Address
**         psBuffer：Data Pointer
**         usNumToWrite:Number to write16
** @export none
** @return none
** @create
** @modify 
*********************************************************************************************************/
uint16_t GDFLASH_BUF[GD_SECTOR_SIZE/2];//最多是2K字节
void FLASH_Write(uint32_t ulWriteAddr, uint16_t *psBuffer, uint16_t usNumToWrite)    
{
    uint32_t ulSecpos;     //扇区地址
    uint16_t usSecoff;     //扇区内偏移地址(16位字计算)
    uint16_t usSecremain;  //扇区内剩余地址(16位字计算)       
    uint16_t i;    
    uint32_t ulOffaddr;    //去掉0X08000000后的地址
    if (ulWriteAddr < GD32_FLASH_BASE || (ulWriteAddr >= (GD32_FLASH_BASE + 1024*GD32_FLASH_SIZE)))
        return;     //非法地址
    FLASH_Unlock();                                 //解锁
    ulOffaddr = ulWriteAddr - GD32_FLASH_BASE;         //实际偏移地址.
    ulSecpos = ulOffaddr/GD_SECTOR_SIZE;               //扇区地址  0~127 for STM32F103RBT6
    usSecoff = (ulOffaddr%GD_SECTOR_SIZE)/2;           //在扇区内的偏移(2个字节为基本单位.)
    usSecremain = GD_SECTOR_SIZE/2 - usSecoff;         //扇区剩余空间大小   
    if (usNumToWrite <= usSecremain)
        usSecremain = usNumToWrite;                     //不大于该扇区范围
    while(1) 
    {    
        FLASH_Read(ulSecpos*GD_SECTOR_SIZE + GD32_FLASH_BASE,GDFLASH_BUF,GD_SECTOR_SIZE/2);//读出整个扇区的内容
        for (i = 0;i < usSecremain;i++)//校验数据
        {
            if (GDFLASH_BUF[usSecoff+i] != 0xFFFF)
                break;//需要擦除        
        }
        if (i < usSecremain)//需要擦除
        {
            FLASH_ErasePage(ulSecpos * GD_SECTOR_SIZE + GD32_FLASH_BASE);//擦除这个扇区
            for (i = 0;i < usSecremain;i++)//复制
            {
                GDFLASH_BUF[i + usSecoff] = psBuffer[i];      
            }
            FLASH_Write_NoCheck(ulSecpos*GD_SECTOR_SIZE + GD32_FLASH_BASE, GDFLASH_BUF, GD_SECTOR_SIZE/2);//写入整个扇区  
        }
        else 
        {
            FLASH_Write_NoCheck(ulWriteAddr, psBuffer, usSecremain);//写已经擦除了的,直接写入扇区剩余区间.
        }
        if (usNumToWrite == usSecremain)//写入结束了
            break;
        else//写入未结束
        {
            ulSecpos++;                //扇区地址增1
            usSecoff = 0;                //偏移位置为0      
            psBuffer += usSecremain;      //指针偏移
            ulWriteAddr += usSecremain;    //写地址偏移       
            usNumToWrite -= usSecremain;    //字节(16位)数递减
            if (usNumToWrite > (GD_SECTOR_SIZE/2))
                usSecremain = GD_SECTOR_SIZE/2;//下一个扇区还是写不完
            else
                usSecremain = usNumToWrite;//下一个扇区可以写完了
        }     
    }
    FLASH_Lock();//上锁
}

#endif

/**
********************************************************************************************************
** @nameis FLASH_Read
** @effect 从指定地址开始读出指定长度的数据
** @import ulReadAddr:Start Address
**         psBuffer：Data Pointer
**         usNumToRead:Number to read16
** @export none
** @return none
** @create
** @modify 
*********************************************************************************************************/
void FLASH_Read(uint32_t ulReadAddr, uint16_t *psBuffer, uint16_t usNumToRead)       
{
    uint16_t i;
    for(i = 0;i < usNumToRead;i++)
    {
        psBuffer[i] = FLASH_ReadHalfWord(ulReadAddr);//读取2个字节.
        ulReadAddr += 2;//偏移2个字节.
    }
}
