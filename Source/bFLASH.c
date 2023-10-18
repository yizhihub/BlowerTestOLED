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
**                          FLASHģ��EEPROM������֧�ֵ�ַ������д
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
#define GD_SECTOR_SIZE 1024 //�ֽ�
#else 
#define GD_SECTOR_SIZE    2048
#endif


/**
********************************************************************************************************
** @nameis FLASH_ReadHalfWord
** @effect ��ȡָ����ַ�İ���(16λ����)
** @import faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
** @export none
** @return ����ֵ:��Ӧ����.
** @create
** @modify 
*********************************************************************************************************/
uint16_t FLASH_ReadHalfWord(uint32_t faddr)
{
    return *(vu16*)faddr; 
}

#if GD32_FLASH_WREN    //���ʹ����д  
/**
********************************************************************************************************
** @nameis FLASH_Write_NoCheck
** @effect �޼��д��
** @import ulWriteAddr:Start Address
**         psBuffer��Data Pointer
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
        ulWriteAddr += 2;//��ַ����2.
    }  
} 

/**
********************************************************************************************************
** @nameis FLASH_Write
** @effect �����д��
** @import ulWriteAddr:Start Address
**         psBuffer��Data Pointer
**         usNumToWrite:Number to write16
** @export none
** @return none
** @create
** @modify 
*********************************************************************************************************/
uint16_t GDFLASH_BUF[GD_SECTOR_SIZE/2];//�����2K�ֽ�
void FLASH_Write(uint32_t ulWriteAddr, uint16_t *psBuffer, uint16_t usNumToWrite)    
{
    uint32_t ulSecpos;     //������ַ
    uint16_t usSecoff;     //������ƫ�Ƶ�ַ(16λ�ּ���)
    uint16_t usSecremain;  //������ʣ���ַ(16λ�ּ���)       
    uint16_t i;    
    uint32_t ulOffaddr;    //ȥ��0X08000000��ĵ�ַ
    if (ulWriteAddr < GD32_FLASH_BASE || (ulWriteAddr >= (GD32_FLASH_BASE + 1024*GD32_FLASH_SIZE)))
        return;     //�Ƿ���ַ
    FLASH_Unlock();                                 //����
    ulOffaddr = ulWriteAddr - GD32_FLASH_BASE;         //ʵ��ƫ�Ƶ�ַ.
    ulSecpos = ulOffaddr/GD_SECTOR_SIZE;               //������ַ  0~127 for STM32F103RBT6
    usSecoff = (ulOffaddr%GD_SECTOR_SIZE)/2;           //�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
    usSecremain = GD_SECTOR_SIZE/2 - usSecoff;         //����ʣ��ռ��С   
    if (usNumToWrite <= usSecremain)
        usSecremain = usNumToWrite;                     //�����ڸ�������Χ
    while(1) 
    {    
        FLASH_Read(ulSecpos*GD_SECTOR_SIZE + GD32_FLASH_BASE,GDFLASH_BUF,GD_SECTOR_SIZE/2);//������������������
        for (i = 0;i < usSecremain;i++)//У������
        {
            if (GDFLASH_BUF[usSecoff+i] != 0xFFFF)
                break;//��Ҫ����        
        }
        if (i < usSecremain)//��Ҫ����
        {
            FLASH_ErasePage(ulSecpos * GD_SECTOR_SIZE + GD32_FLASH_BASE);//�����������
            for (i = 0;i < usSecremain;i++)//����
            {
                GDFLASH_BUF[i + usSecoff] = psBuffer[i];      
            }
            FLASH_Write_NoCheck(ulSecpos*GD_SECTOR_SIZE + GD32_FLASH_BASE, GDFLASH_BUF, GD_SECTOR_SIZE/2);//д����������  
        }
        else 
        {
            FLASH_Write_NoCheck(ulWriteAddr, psBuffer, usSecremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
        }
        if (usNumToWrite == usSecremain)//д�������
            break;
        else//д��δ����
        {
            ulSecpos++;                //������ַ��1
            usSecoff = 0;                //ƫ��λ��Ϊ0      
            psBuffer += usSecremain;      //ָ��ƫ��
            ulWriteAddr += usSecremain;    //д��ַƫ��       
            usNumToWrite -= usSecremain;    //�ֽ�(16λ)���ݼ�
            if (usNumToWrite > (GD_SECTOR_SIZE/2))
                usSecremain = GD_SECTOR_SIZE/2;//��һ����������д����
            else
                usSecremain = usNumToWrite;//��һ����������д����
        }     
    }
    FLASH_Lock();//����
}

#endif

/**
********************************************************************************************************
** @nameis FLASH_Read
** @effect ��ָ����ַ��ʼ����ָ�����ȵ�����
** @import ulReadAddr:Start Address
**         psBuffer��Data Pointer
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
        psBuffer[i] = FLASH_ReadHalfWord(ulReadAddr);//��ȡ2���ֽ�.
        ulReadAddr += 2;//ƫ��2���ֽ�.
    }
}
