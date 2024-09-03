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
u8 GbUartRxDone, GbUartRxDone1  = 1;       //����״̬���     
u8 GucUartRxIndex = 0;
u8 GucUartRxCnt   = 0;
u8 GbUartRxCheck   = 0;       //����״̬���     
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
            switch (USART_RX_BUF[0])
            {
                case 0x5A:
                    if (CheckXor_calc(USART_RX_BUF, GucUartRxCnt - 1) != USART_RX_BUF[GucUartRxCnt - 1])
                        break;
                    if (GucUartRxCnt == 24) 
                    {
                        memcpy(VerSionStr, &USART_RX_BUF[4], 18);
                        GbUartRxDone = 1;                                    /* ���޴��̼���������� */ 
                    }
                    else if (GucUartRxCnt == 10) {
                        GbUartRxDone = 1;                    /* ���޴�����ת��֡������� */ 
                    }
                break;
                
                case 0x2B:
                    if (GucUartRxCnt == 10 && crc8_calc(USART_RX_BUF, 0, GucUartRxCnt - 1) ==  USART_RX_BUF[GucUartRxCnt - 1])
                    {
                        GbUartRxDone = 4;                   /* �������Э��2B��ͷ��ת��֡������� */
                        break;
                    }
                    if (checksum_calc(USART_RX_BUF, 1, GucUartRxCnt - 2) !=  USART_RX_BUF[GucUartRxCnt - 1])
                        break;
                    if (GucUartRxCnt == 10)
                    {
                        GbUartRxDone = 2;                    /* ��Э��ת��֡������� */ 
                    }
                    else if (GucUartRxCnt == 8) 
                    {
                        GbUartRxDone = 3;                    /*������ת��֡������� */ 
                    }
                    else if (GucUartRxCnt == 6) 
                    {
                        GbUartRxDone = 2;                    /*��Э��&����������֡������� */ 
                    }
                    else if (GucUartRxCnt == 38) 
                    {
                        memcpy(VerSionStr, &USART_RX_BUF[12], 18);
                        GbUartRxDone = 2;                                    /* ��Э��̼���������� */ 
                    }
                    else if (GucUartRxCnt == 22) 
                    {
                        memcpy(VerSionStr, &USART_RX_BUF[3], 18);
                        GbUartRxDone = 3;                                    /* �������̼���������� */ 
                    }
                break;
                
                default:
                    if (GucUartRxCnt == 10 && crc8_calc(USART_RX_BUF, 0, GucUartRxCnt - 1) ==  USART_RX_BUF[GucUartRxCnt - 1])
                    {
                        GbUartRxDone = 4;                    /* �������ת��֡������� */ 
                    }
                break;
            }
            
            GucUartRxIndex = GucUartRxCnt;
            
        }
        
        GucUartRxCnt   = 0;
        GbUartRxCheck  = 0;
    }
    
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        Res = USART_ReceiveData(USART2);              /* ÿ�������32ms�յ�һ��ң�ⱨ��  */ 
        if (Res == 0x2B)
        {
            GbUartRxCheck ^= 1;
            if (!GbUartRxCheck)
                return;
        }
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

uint8_t checksum_calc(uint8_t * data_ptr, uint8_t first_num, uint8_t len)
{
    int i;
    uint8_t sum = 0;
    for (i = first_num; i <= len; i++)
    {
        sum += data_ptr[i];
    }
    return 256 - sum;
}

/*���� CRC8(������� 0x1D)�Ĳ��ı��*/
const uint8_t crc_table[256] ={
 0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
 0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
 0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
 0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
 0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
 0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
 0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
 0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
 0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
 0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
 0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
 0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
 0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
 0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
 0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
 0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
 0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
 0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
 0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
 0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
 0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
 0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
 0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
 0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
 0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
 0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
 0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
 0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
 0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
 0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
 0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
 0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * crc_table[256]���в�����{0...255}��ÿ��Ԫ����OX1D���Ľ��******
* * CRC8 У���ӳ��� 0x1D(x8+x4+x3+x2+1) * * * * * * * * * * * *
* * ���� 1��uint8_t *data����Ҫ��������� * * * * * * * * * * *
* * ���� 1��uint16_t len����Ҫ����������ֽڳ��� * * * * * * * *
* * ����ֵ��uint8_t crc8��������� CRC ֵ * * * * * * * * * * *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8_t crc8_calc(uint8_t *data, uint8_t first_num, uint16_t len)
{
    int i;
    uint8_t crc8 = 0x00;
    for (i = first_num; i <= len; i++)
    {
        crc8 = crc_table[crc8 ^ data[i]];
    }
    return crc8;
}

uint8_t CheckXor_calc(uint8_t *Buffer, uint16_t NByte)
{                                               
    uint8_t usTemp=0;

    while(NByte-- >0)
    {
      usTemp ^=(*Buffer);
      Buffer++;
    }

    return(usTemp);
}
