#include "sys.h"
#include "usart.h"
#include "HwDrv.h"
//////////////////////////////////////////////////////////////////////////////////      
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"                    //ucos ʹ��      
#endif
//////////////////////////////////////////////////////////////////////////////////     
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��           
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
//////////////////////////////////////////////////////////////////////////////////       
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB      
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
    int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
    x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
    while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
    USART2->DR = (u8) ch;      
    return ch;
}
#endif 

 
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���       
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART1_RX_BUF[USART_REC_LEN];
extern u8 VerSionStr[24];
//����״̬
//bit15��    ������ɱ�־
//bit14��    ���յ�0x0d
//bit13~0��    ���յ�����Ч�ֽ���Ŀ
u8 GbUartRxDone   = 1;       //����״̬���     
u8 GucUartRxIndex = 0;
u8 GucUartRxCnt   = 0;
u8 GbUartRxDone1   = 0;       //����״̬���     
u8 GucUartRxIndex1 = 0;
  
void uart_init(u32 bound)
{
    
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    
    /* 
     *  USART1 ��ʼ��   PA9   PA10
     */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    //ʹ��USART1��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
  
    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
    //USART1_RX      GPIOA.10��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);    //����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

    USART_InitStructure.USART_BaudRate = 19200;//���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);//�������ڽ����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
    
    /*
     *  USART2 ��ʼ�� 
     */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    //ʹ��USART1��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;// PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);    //����ָ���Ĳ�����ʼ��VIC�Ĵ���

    USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2
    
    
}

void USART1_IRQHandler(void)                    //����1�жϷ������
{
    u8 Res;
    if(USART1->SR & USART_SR_ORE)                                 /* ORE�ж�                             */
    {
        u8 com_data = USART1->DR;
    }
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        Res = USART_ReceiveData(USART1);              /* ÿ�������32ms�յ�һ��ң�ⱨ��  */ 
        
        if (GbUartRxDone1 == 0) {                     /* �ȴ�������� */
        
            if (GulLapReoprtT1msCnt > 5)                      /* �������5ms��Ϊ���µı���     */
            {
                GucUartRxIndex1 = 0;
            }
            USART1_RX_BUF[GucUartRxIndex1++] = Res; 
            
            if (GucUartRxIndex1 == 8) {
                GbUartRxDone1 = 1;                    /* ������� */ 
            }
        }
        GulLapReoprtT1msCnt = 0;
    }
} 


void USART2_IRQHandler(void)                    //����1�жϷ������
{
    u8 Res;
    
    if(USART2->SR & USART_SR_ORE)                                 /* ORE�ж�                             */
    {
        u8 com_data = USART2->DR;
    }
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) 
    {
//        USART_ClearITPendingBit(USART2, USART_IT_IDLE);
        USART_ReceiveData(USART2);
        if (GbUartRxDone == 0) {                     /* �ȴ�������� */
            if (GucUartRxCnt == 24 && USART_RX_BUF[0] == 0x5A) {
                memcpy(VerSionStr, &USART_RX_BUF[4], 18);
                GbUartRxDone = 1;                                    /* ���޴��̼���������� */ 
            }
            else if (GucUartRxCnt == 22 && USART_RX_BUF[0] == 0x2B) {
                memcpy(VerSionStr, &USART_RX_BUF[3], 18);
                GbUartRxDone = 1;                                    /* �������̼���������� */ 
            }
            else if (GucUartRxCnt == 8) {
                GbUartRxDone = 1;                    /* ����������ת��֡������� */ 
            }
            else if (GucUartRxCnt == 10) {
                GbUartRxDone = 1;                    /* ���޴�����ת��֡������� */ 
            }
//            else {
//                GbUartRxDone = 1;
//            }
            GucUartRxIndex = GucUartRxCnt;
            
        }
        memset(USART_RX_BUF, 0, sizeof(USART_RX_BUF));
        GucUartRxCnt   = 0;
    }
    
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        Res = USART_ReceiveData(USART2);              /* ÿ�������32ms�յ�һ��ң�ⱨ��  */ 
        USART_RX_BUF[GucUartRxCnt++] = Res;   //GucUartRxCnt
        if (GucUartRxCnt >= USART_REC_LEN)
            GucUartRxCnt = 0;
    }
} 
    
/*********************************************************************************************************
** Function name:           usartSendData
** Descriptions:            ����һ������
** Input parameters:        ucData �� �����͵�һ������
** Output parameters:       none
** Returned value:          none
** Created by:              lgd
** Created date:            
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
*********************************************************************************************************/
void  uartDrvPutChar (USART_TypeDef* USARTx, INT8U ucData)
{
    while (!USART_GetFlagStatus(USARTx, USART_FLAG_TXE)) {
    }
    USART_SendData(USARTx, (uint16_t)ucData);

}

INT8U uartDrvPutBuf(USART_TypeDef* USARTx, const INT8U *pucTxBuff, INT32U ulTxSize) 
{
    while(ulTxSize--) {
        uartDrvPutChar(USARTx, *pucTxBuff++);
    }
    return 0;      
}
 
#endif
