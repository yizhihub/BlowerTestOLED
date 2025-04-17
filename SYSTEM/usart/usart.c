#include "sys.h"
#include "usart.h"
#include "HwDrv.h"
//////////////////////////////////////////////////////////////////////////////////      
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"                    //ucos 使用      
#endif
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化           
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
//////////////////////////////////////////////////////////////////////////////////       
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB      
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
    int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
    x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
    while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
    return ch;
}
#endif 

 
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误       
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART1_RX_BUF[USART_REC_LEN];
extern u8 VerSionStr[24];
//接收状态
//bit15，    接收完成标志
//bit14，    接收到0x0d
//bit13~0，    接收到的有效字节数目
u8 GbUartRxDone, GbUartRxDone1  = 1;       //接收状态标记     
u8 GucUartRxIndex = 0;
u8 GucUartRxCnt   = 0;
u8 GbUartRxCheck   = 0;       //接收状态标记     
u8 GucUartRxIndex1 = 0;
  
void uart_init(u32 bound)
{
    
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    
    /* 
     *  USART1 初始化   PA9   PA10
     */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    //使能USART1，GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
  
    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
    //USART1_RX      GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);    //根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

    USART_InitStructure.USART_BaudRate = 19200;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);//开启串口接受中断
    USART_Cmd(USART1, ENABLE);                    //使能串口1 
    
    /*
     *  USART2 初始化 
     */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    //使能USART1，GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;// PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);    //根据指定的参数初始化VIC寄存器

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//开启串口接受中断
    USART_Cmd(USART2, ENABLE);                    //使能串口2
    
    
}


void USART1_IRQHandler(void)                    //串口1中断服务程序
{
    u8 Res;
    if(USART1->SR & USART_SR_ORE)                                 /* ORE中断                             */
    {
        u8 com_data = USART1->DR;
    }
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res = USART_ReceiveData(USART1);              /* 每隔开大概32ms收到一次遥测报文  */ 
        
        if (GbUartRxDone1 == 0) {                     /* 等待消费完成 */
        
            if (GulLapReoprtT1msCnt > 5)                      /* 间隔超过5ms认为是新的报文     */
            {
                GucUartRxIndex1 = 0;
            }
            USART1_RX_BUF[GucUartRxIndex1++] = Res; 
            
            if (GucUartRxIndex1 == 8) {
                GbUartRxDone1 = 1;                    /* 生产完毕 */ 
            }
        }
        GulLapReoprtT1msCnt = 0;
    }
} 


void USART2_IRQHandler(void)                    //串口1中断服务程序
{
    u8 Res;
    
    if(USART2->SR & USART_SR_ORE)                                 /* ORE中断                             */
    {
        u8 com_data = USART2->DR;
    }
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) 
    {
//        USART_ClearITPendingBit(USART2, USART_IT_IDLE);
        USART_ReceiveData(USART2);
        if (GbUartRxDone == 0) {                     /* 等待消费完成 */
            switch (USART_RX_BUF[0])
            {
                case 0x5A:
                    if (CheckXor_calc(USART_RX_BUF, GucUartRxCnt - 1) != USART_RX_BUF[GucUartRxCnt - 1])
                        break;
                    if (GucUartRxCnt == 24) 
                    {
                        memcpy(VerSionStr, &USART_RX_BUF[4], 18);
                        GbUartRxDone = 1;                                    /* 大无创固件号生产完毕 */ 
                    }
                    else if (GucUartRxCnt == 10) {
                        GbUartRxDone = 1;                    /* 大无创错误转速帧生产完毕 */ 
                    }
                break;
                
                case 0x2B:
                    if (GucUartRxCnt == 10 && crc8_calc(USART_RX_BUF, 0, GucUartRxCnt - 1) ==  USART_RX_BUF[GucUartRxCnt - 1])
                    {
                        GbUartRxDone = 4;                   /* 方波电调协议2B开头的转速帧生产完毕 */
                        break;
                    }
                    if (checksum_calc(USART_RX_BUF, 1, GucUartRxCnt - 2) !=  USART_RX_BUF[GucUartRxCnt - 1])
                        break;
                    if (GucUartRxCnt == 10)
                    {
                        GbUartRxDone = 2;                    /* 新协议转速帧生产完毕 */ 
                    }
                    else if (GucUartRxCnt == 8) 
                    {
                        GbUartRxDone = 3;                    /*三代机转速帧生产完毕 */ 
                    }
                    else if (GucUartRxCnt == 6) 
                    {
                        GbUartRxDone = 2;                    /*新协议&三代机错误帧生产完毕 */ 
                    }
                    else if (GucUartRxCnt == 38) 
                    {
                        memcpy(VerSionStr, &USART_RX_BUF[12], 18);
                        GbUartRxDone = 2;                                    /* 新协议固件号生产完毕 */ 
                    }
                    else if (GucUartRxCnt == 22) 
                    {
                        memcpy(VerSionStr, &USART_RX_BUF[3], 18);
                        GbUartRxDone = 3;                                    /* 三代机固件号生产完毕 */ 
                    }
                break;
                
                default:
                    if (GucUartRxCnt == 10 && crc8_calc(USART_RX_BUF, 0, GucUartRxCnt - 1) ==  USART_RX_BUF[GucUartRxCnt - 1])
                    {
                        GbUartRxDone = 4;                    /* 方波电调转速帧生产完毕 */ 
                    }
                break;
            }
            
            GucUartRxIndex = GucUartRxCnt;
            
        }
        
        GucUartRxCnt   = 0;
        GbUartRxCheck  = 0;
    }
    
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res = USART_ReceiveData(USART2);              /* 每隔开大概32ms收到一次遥测报文  */ 
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
** Descriptions:            发送一个数据
** Input parameters:        ucData ： 待发送的一个数据
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

/*定义 CRC8(计算规则 0x1D)的查表的表格*/
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
* * crc_table[256]表中参数是{0...255}中每个元素与OX1D异或的结果******
* * CRC8 校验子程序 0x1D(x8+x4+x3+x2+1) * * * * * * * * * * * *
* * 参数 1，uint8_t *data：需要计算的数据 * * * * * * * * * * *
* * 参数 1，uint16_t len：需要计算的数据字节长度 * * * * * * * *
* * 返回值，uint8_t crc8：计算出的 CRC 值 * * * * * * * * * * *
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
