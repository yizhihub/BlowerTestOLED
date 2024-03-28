#ifndef __DELAY_H
#define __DELAY_H                
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ʹ��SysTick����ͨ����ģʽ���ӳٽ��й����ʺ�STM32F10xϵ�У�
//����usDelay,msDelay
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2010/1/1
//�汾��V1.8
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.2�޸�˵��
//�������ж��е��ó�����ѭ���Ĵ���
//��ֹ��ʱ��׼ȷ,����do while�ṹ!
//V1.3�޸�˵��
//�����˶�UCOSII��ʱ��֧��.
//���ʹ��ucosII,delay_init���Զ�����SYSTICK��ֵ,ʹ֮��ucos��TICKS_PER_SEC��Ӧ.
//msDelay��usDelayҲ���������ucos�ĸ���.
//usDelay������ucos��ʹ��,����׼ȷ�Ⱥܸ�,����Ҫ����û��ռ�ö���Ķ�ʱ��.
//msDelay��ucos��,���Ե���OSTimeDly����,��δ����ucosʱ,������usDelayʵ��,�Ӷ�׼ȷ��ʱ
//����������ʼ������,��������ucos֮��msDelay������ʱ�ĳ���,ѡ��OSTimeDlyʵ�ֻ���usDelayʵ��.
//V1.4�޸�˵�� 20110929
//�޸���ʹ��ucos,����ucosδ������ʱ��,msDelay���ж��޷���Ӧ��bug.
//V1.5�޸�˵�� 20120902
//��usDelay����ucos��������ֹ����ucos���usDelay��ִ�У����ܵ��µ���ʱ��׼��
//V1.6�޸�˵�� 20150109
//��msDelay����OSLockNesting�жϡ�
//V1.7�޸�˵�� 20150319
//�޸�OS֧�ַ�ʽ,��֧������OS(������UCOSII��UCOSIII,����������OS������֧��)
//���:delay_osrunning/delay_ostickspersec/delay_osintnesting�����궨��
//���:delay_osschedlock/delay_osschedunlock/delay_ostimedly��������
//V1.8�޸�˵�� 20150519
//����UCOSIII֧��ʱ��2��bug��
//delay_tickspersec��Ϊ��delay_ostickspersec
//delay_intnesting��Ϊ��delay_osintnesting
////////////////////////////////////////////////////////////////////////////////// 
     
void delay_init(void);
void ms_Delay(u16 nms);
void us_Delay(u32 nus);

#endif





























