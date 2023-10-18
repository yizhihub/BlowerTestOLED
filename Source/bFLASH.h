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

//�û������Լ�����Ҫ����
#define GD32_FLASH_SIZE 512            //��ѡSTM32��FLASH������С(��λΪK)
#define GD32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define GD32_FLASH_BASE 0x08000000 	//GD32 FLASH����ʼ��ַ
//FLASH������ֵ
 

uint16_t FLASH_ReadHalfWord(uint32_t faddr);          //��������  
void FLASH_WriteLenByte(uint32_t WriteAddr, uint32_t DataToWrite, uint16_t Len);    //ָ����ַ��ʼд��ָ�����ȵ�����
uint32_t FLASH_ReadLenByte(uint32_t ReadAddr, uint16_t Len);                        //ָ����ַ��ʼ��ȡָ����������
void FLASH_Write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite);       //��ָ����ַ��ʼд��ָ�����ȵ�����
void FLASH_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);          //��ָ����ַ��ʼ����ָ�����ȵ�����

#endif
