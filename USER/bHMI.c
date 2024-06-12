/*********************************************Copyright (c)***********************************************
**                                Yuwell MCU Technology Co., Ltd.
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               bHMI.h
** Created By  :            YZ
** Created Date:            2023-06-15
** Last Version:            v1.0 
** Descriptions:            C51_PLATEFORM and ARM_PLATEFORM
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
#include "bHMI.h"
#include "bOLED.h"
#include "bBraoAdc.h"
#include "bINA226.h"
#include "usart.h"
#include "bFLASH.h"
#include "bSDP800.H"
#include "stm32f10x_tim.h"

#include "aUpperCom.h"
//#include "string.h"
//#include "oled.h"
//#include "key.h"
//#include "adc.h"
//#include "NRF24L01.h"
//#include "MPU6050.h"
//#include "HMC5883.h"
//#include "spi.h"
//#include "iic.h"
//#include "SMG.h"
//#include "DS18B20.h"
//#include "DS1302.h" 
//#include "isr.h"

INT8U  GucPolePair;

static uchar MenuItem[9][17];
static uchar NowItem;
static uchar FirstItem;
u8 VerSionStr[24] = {0};
PWM_CTRL_UNION PwmCtrl1, PwmCtrl2;

extern const char GscBuildDate[];
extern const char GscSoftRelease[];
extern const char GscBuildTIme[];

/**
********************************************************************************************************
** @nameis VersionInfoDisp
** @effect Display Version info.
** @import ulPeriodHz    : pwm frequency��unit Hz 
**         ulDeadtimeNs  : dead time��unit ns
**         pfuncIntFault : PWM Fault interrupter func. pointer, input (void *)0  if disable.
**         pfuncIntPwm   : PWM Periodic Interrupter callback func. pointer, if noone insert (void *)0
** @export none
** @return none
** @create yizhi 2023.06.24
** @modify 
*********************************************************************************************************/
uchar UartCmdVerTxBuf[10] = {0x2B, 0xC2, 0x3E};
void VersionInfoDisp(void)
{
    OLED_Fill(0x00);
    GbUartRxDone = 0;
    
    uartDrvPutBuf(USART2, UartCmdVerTxBuf, 3);    /* ask for version frame */ 
    while (ADKey_Scan() != KEY_CANCEL) {
        
        OLED_PutChar(10, OLED_LINE0, 'V', 8, 1);
        OLED_PutChar(18, OLED_LINE0, GscSoftRelease[0], 8, 1);
        OLED_PutChar(26, OLED_LINE0, '.', 8, 1);
        OLED_PutChar(34, OLED_LINE0, GscSoftRelease[1], 8, 1);
        OLED_PutChar(42, OLED_LINE0, '.', 8, 1);
        OLED_PutChar(50, OLED_LINE0, GscSoftRelease[2], 8, 1);
        if (GbUartRxDone) {
            OLED_PutStr(10, OLED_LINE1, VerSionStr,  6, 1);
//            OLED_PutStr(10, OLED_LINE1 + LINE_HEIGHT / 2, "String Null",  6, 1);
        } else {            
//            uartDrvPutBuf(USART2, UartCmdVerTxBuf, 3); 
        }

            
        OLED_PutStr(10, OLED_LINE2, (uint8_t *)GscBuildDate,   6, 1);
        OLED_PutStr(10, OLED_LINE2 + LINE_HEIGHT / 2, (uint8_t *)GscBuildTIme,   6, 1);
        OLED_PutNumber(10, OLED_LINE3, RSHUNT*_GfRsINA226.fdata, 2, 3, "m��", 6, 1);
        msDelay(20);
    }
}

/**
********************************************************************************************************
** @nameis HMI_Draw
** @effect darw the item according need 
** @import none
** @export none
** @return
** @create yizhi 2023.8.1
** @modify 
*********************************************************************************************************/
void HMI_DrawName(uint8_t ucX, uint8_t ucStart, uint8_t str[], uint16_t ucYn)
{
    switch (ucX) {
         
    case 0:
        OLED_Print(0 + ucStart, OLED_LINE1, str, ucYn); // ������ʾ 
        break;
    
    case 1:
        OLED_Print(64 + ucStart, OLED_LINE1, str, ucYn); // ������ʾ    
        break;
    
    case 2:
        OLED_Print(0 + ucStart, OLED_LINE2, str, ucYn); // ������ʾ    
        break;
    
    case 3:
        OLED_Print(64 + ucStart, OLED_LINE2, str, ucYn); // ������ʾ    
        break;
    
    case 4:
        OLED_PutStr(0 + ucStart, OLED_LINE3, str, 6, ucYn); // ������ʾ    
        break;
    case 5:
        OLED_PutStr(0 + ucStart, OLED_LINE3 + (LINE_HEIGHT >> 1), str, 6, ucYn); // ������ʾ    
        break;
    
    default:
        break;
    }
}


/**
********************************************************************************************************
** @nameis HMI_Draw
** @effect darw the item according need 
** @import none
** @export none
** @return
** @create yizhi 2023.8.1
** @modify 
*********************************************************************************************************/
void HMI_DrawNumber(uint8_t ucX, uint8_t ucStart, uint16_t usNumber, PWM_CTRL_UNION *ptPwmCtrl,  uint16_t ucYn)
{
    switch (ucX) {
         
    case 0:
        OLED_PutNumber(0 + ucStart, OLED_LINE1 ,                     usNumber / 10.0f, 2, 1 , " %", 6, ucYn);
        OLED_PutNumber(0 + ucStart, OLED_LINE1 + (LINE_HEIGHT >> 1), ptPwmCtrl->tPwmCtrl.sPwmWidthLow, 4, 0 , "us", 6, ucYn);
        break;
    
    case 1:
        OLED_PutNumber(64 + ucStart, OLED_LINE1, usNumber * 10,  4, 0, "ms", 6, ucYn);
        break;
    
    case 2:
        OLED_PutNumber(0 + ucStart, OLED_LINE2 ,                     usNumber / 10.0f, 2, 1 , " %", 6, ucYn);
        OLED_PutNumber(0 + ucStart, OLED_LINE2 + (LINE_HEIGHT >> 1), ptPwmCtrl->tPwmCtrl.sPwmWidthHigh, 4, 0 , "us", 6, ucYn);
        break;
    
    case 3:
        OLED_PutNumber(64 + ucStart, OLED_LINE2, usNumber * 10, 4, 0, "ms", 6, ucYn);
        break;
    
    case 4:
        OLED_PutNumber(12 + ucStart, OLED_LINE3, usNumber, 2, 0, 0, 6, ucYn);
        break;
    
    case 5:
        OLED_PutNumber(12 + ucStart, OLED_LINE3 + (LINE_HEIGHT >> 1), GucPolePair, 2, 0, 0, 6, ucYn);
        break;
    
    default:
        break;
    }
}
/**
********************************************************************************************************
** @nameis PWM_ProtoUpdate
** @effect update related protocal paramater 
** @import none
** @export none
** @return
** @create yizhi 2023.8.1
** @modify 
*********************************************************************************************************/
void PWM_ProtoUpdate(PWM_CTRL_DATA * ptPwm, INT16U ePwmProto)
{
    switch (ePwmProto) {
        
    case 0u:                                                           /* 400Hz tradiional drone throttle protocol 1~2ms */ 
        ptPwm->sPwmWidthStart   = 1000;
        ptPwm->sPwmWidthEnd     = 2000;
        ptPwm->usPwmModulo      = 2500;
        ptPwm->sPwmWidthRange   = ptPwm->sPwmWidthEnd - ptPwm->sPwmWidthStart;
        ptPwm->sPwmWidthDflt    = 1000;
        break;
    
    case 1u:                                                          /* 300Hz Yuwell throttle protocol 100us~2250us    */
        ptPwm->sPwmWidthStart   = 100;                               /* 100us-310us-420us-2250us-2450us                 */
        ptPwm->sPwmWidthEnd     = 3000;                              /*  2250 -> 2500 -> 3000 for C65 motor             */
        ptPwm->usPwmModulo      = 3333;
        ptPwm->sPwmWidthRange   = ptPwm->sPwmWidthEnd - ptPwm->sPwmWidthStart;;
        ptPwm->sPwmWidthDflt    = 100;
        break;
    
    case 2u:                                                         /* 1000Hz Yuwell throttle protocol 100us~2250us   */
        ptPwm->sPwmWidthStart    = 0;                                /* -50us-80us-100us-900us-950us                   */
        ptPwm->sPwmWidthEnd      = 1000;
        ptPwm->usPwmModulo       = 1000;
        ptPwm->sPwmWidthRange    = ptPwm->sPwmWidthEnd - ptPwm->sPwmWidthStart;;
        ptPwm->sPwmWidthDflt     = 50;
        break;
    
    case 3u:                                                         /* 1000Hz Yuwell throttle protocol 100us~2250us   */
        ptPwm->sPwmWidthStart    = 0;                                /* -50us-80us-100us-900us-950us                   */
        ptPwm->sPwmWidthEnd      = 3333;
        ptPwm->usPwmModulo       = 3333;
        ptPwm->sPwmWidthRange    = ptPwm->sPwmWidthEnd - ptPwm->sPwmWidthStart;;
        ptPwm->sPwmWidthDflt     = 50;
        break;
    
    default:
        break;
    }
}

/**
********************************************************************************************************
** @nameis PWM_KeyCallback
** @effect KeyValue change then do something
** @import Current selected item
** @export none
** @return
** @create yizhi 2023.8.1
** @modify 
*********************************************************************************************************/
void PWM_KeyCallback(PWM_CTRL_UNION *PwmCtrl, uint8_t ucItem)
{
   if (ucItem == 4) {                                                                         /* ���Э�� */ 
        if (PwmCtrl->sPwmCtrl[4] > 3)  PwmCtrl->sPwmCtrl[4] = 3;  /* max ePwmProtocol is 2 */
        PWM_ProtoUpdate(&PwmCtrl->tPwmCtrl, PwmCtrl->tPwmCtrl.ePwmProtocol);
        PWM_PERIOD_SETA(PwmCtrl->tPwmCtrl.usPwmModulo);
        PwmCtrl->tPwmCtrl.sPwmWidthSetting = PwmCtrl->tPwmCtrl.sPwmWidthDflt;
    } else if (ucItem == 0) {                                                                /* �趨��ת��ƽ̨ */
        
        PwmCtrl->tPwmCtrl.sPwmWidthLow  = PwmCtrl->tPwmCtrl.sPwmWidthStart +              /* PwmCtrl->tPwmCtrl.sPwmLowVal;�ӵ���ռ�ձȰٷֱ��޸�Ϊֱ�ӵ��������� */
                                         PwmCtrl->tPwmCtrl.sPwmWidthRange * PwmCtrl->tPwmCtrl.sPwmLowVal /  1000u;
        if (PwmCtrl->tPwmCtrl.sPwmWidthLow < PwmCtrl->tPwmCtrl.sPwmWidthDflt)
            PwmCtrl->tPwmCtrl.sPwmWidthLow = PwmCtrl->tPwmCtrl.sPwmWidthDflt;
        PwmCtrl->tPwmCtrl.sPwmWidthSetting =  PwmCtrl->tPwmCtrl.sPwmWidthLow;
    } else if (ucItem == 2) {                                                              /* �趨��ת��ƽ̨ */
        PwmCtrl->tPwmCtrl.sPwmWidthHigh = PwmCtrl->tPwmCtrl.sPwmHighVal;
        PwmCtrl->tPwmCtrl.sPwmWidthHigh = PwmCtrl->tPwmCtrl.sPwmWidthStart +              /* PwmCtrl->tPwmCtrl.sPwmHighVal;�ӵ���ռ�ձȰٷֱ��޸�Ϊֱ�ӵ��������� */
                                         PwmCtrl->tPwmCtrl.sPwmWidthRange * PwmCtrl->tPwmCtrl.sPwmHighVal /  1000u; 
        if (PwmCtrl->tPwmCtrl.sPwmWidthHigh < PwmCtrl->tPwmCtrl.sPwmWidthDflt)
            PwmCtrl->tPwmCtrl.sPwmWidthHigh = PwmCtrl->tPwmCtrl.sPwmWidthDflt;
        PwmCtrl->tPwmCtrl.sPwmWidthSetting =  PwmCtrl->tPwmCtrl.sPwmWidthHigh;
    } else {
        PwmCtrl->tPwmCtrl.sPwmWidthSetting = PwmCtrl->tPwmCtrl.sPwmWidthDflt;
    }
}
/***********************************************************************************************************
# ��������: DramMenu()
# �������: cMenuItem �˵���Ŀ�ı��Ļ���  num ���˵��ܹ�����Ŀ��  tile: �Ƿ���ʾ����  1 ��ʾ  0 ����ʾ 
# �������: uint8
# ��    ��: ���Ʋ˵���Ŀ
# ��    ����
# ��    ��: 2015/3/8, by yizhi
************************************************************************************************************/
uchar DrawMenu(uchar MenuItem[][17],uchar num,uchar title)  
{
  uchar ii;

  uchar key,title2;
  uchar screen_flag = 1;
  if(title)    
  {        
    title2=0;
    //OLED_PutStr(0, OLED_LINE0, "-TSL1401Send2PC-", 8, 0);
  }
  else 
  {        
    title2=1;
    //OLED_CLS(); ע�ͱ�� AAA
  }
  while(1)
  {
    if(screen_flag)
    {
      screen_flag=0;
      if(num>2)
      {
        for(ii=0;ii<(3+title2);ii++)   //  
        {
          if(NowItem!=ii)
          {
            OLED_Print(0,(ii*2+title*2),MenuItem[FirstItem+ii], 1); // ������ʾ
          }
          else
          {
            OLED_Print(0,(ii*2+title*2),MenuItem[FirstItem+ii], 0); // ������ʾ
          }
        }
      }
      else
      {
        for(ii =0;ii<2;ii++)
        {
          if(NowItem!=ii)
          {
            OLED_Print(0,(ii*2+2),MenuItem[FirstItem+ii], 1);     // ������ʾ 
          }
          else
          {
            OLED_Print(0,(ii*2+2),MenuItem[FirstItem+ii], 0);      // ������ʾ 
          }
        }
        
      }
      while(ADKey_Scan() != KEY_NONE);
    /*��仰���ϱ�Ҫ���ǣ�1���״ν���ò�˵����Ӵ������˳�����
    //�Ӵ˲˵��˳��������˳������߷��أ��Ĵ���������Ϊ KEY_CANCEL���������ﲻ��һ�µĻ����ͻ�ֱ��
    //���������˵���ҳ�˵���2��������仰����˵������¾Ͳ����а������¼����ɿ�����������������
    //Ч���� ��Ȼ����ֻ��һ��˵�������ע�͵���*/
    }
    key = ADKey_Check();
    if(EC11_CW == key)
    {
      GsEc11CntCW = 0;
      // msDelay(10);
      // if(ADKey_Scan(1)==KEY_UP)
      //while(ADKey_Scan()==KEY_UP);
      if(num>2)
      {
          if(NowItem > 0)         NowItem-=1; // ��ǰ�� ����Ŀ
          else if(FirstItem >0)  FirstItem-=1; // ��ǰ�� �ĵ�һ����Ŀ 
          else  // �ѵ������в˵��ĵ�һ����Ŀ
          {
            NowItem=3-title;
            FirstItem=num-4+title;
          }
      }
      else   // �˵�����Ŀ<=2
      {
        if(NowItem>0)   NowItem-=1;
//        else            NowItem =0;
      }
      screen_flag=1;
    }
    if(EC11_CCW == key)
    {
      GsEc11CntCCW = 0;
      // msDelay(10);  // ��ʱ��������
      // if(ADKey_Scan(1)==KEY_UP) 
      //while(ADKey_Scan()==KEY_DOWN);  
      if(num>2)
      {
        
        if(NowItem<2+title2)                  NowItem+=1;   
        else if((FirstItem+3+title2)<num)   FirstItem+=1;  
        else
        {
          FirstItem = 0;
          NowItem   = 0;
        }
      }
      else   // �˵�����Ŀ<=2
      {
        if(NowItem< (num - 1))   NowItem+=1;
//        else            NowItem =0;
      }
      screen_flag=1;
      
    } 
    if(EC11_SET == key)  //  if don't push ENTER or CANCEL  NO Jump out 
    {
      //while(ADKey_Scan()==KEY_IN); 
      usDelay(10);
      return  FirstItem+NowItem;
    }
    if(KEY_UP == key)
    {
      //while(ADKey_Scan()==KEY_BACK) 
        usDelay(10);
        VersionInfoDisp();   //  display the version info.
        return 0xff;
    }
    if(KEY_CANCEL == key)
    {
      //while(ADKey_Scan()==KEY_BACK) 
        usDelay(10);
        return 0xff;
    }
    msDelay(4); // �ѵ������ʱ��ǰ����˼������ʱ��û�м���2016��1��10�ռ��ϡ�
  }
}

/***********************************************************************************************************
*****************************************��Ŀ����***********************************************/
  /*******************************************************************************
# ����ԭ����IMUCalibrate
# �������: void
# �������: void
# ��    ��: ���������ǵ���� �궨���ٶȼ� 
# ��    ����
# ��    �ԣ�I just struggle myself! 
# ��    ��: 2015/5/5, by yizhi
********************************************************************************/
//void IMUCalibrate(void)
//{
//  uint8 i,key;
//  uint16 cali_flag=0;
//  int16 idata temp[50]; // һ�����õ�100�ֽڵ�idata�ռ� 
//    // �˴�����ʱ�任ȡ�ռ������,ÿ��ֻУ׼һ����,������У׼.�����Ƭ��
//    // �ڴ��㹻��Ҳ����һ����У׼������.
//  int16 temp_sum;
//  //IMURaw_s temp;
//  OLED_CLS();
//  OLED_P8x16Str(0,0,"--Gyro_Cali--",1);  
//  while(ADKey_Scan()!=KEY_CANCEL)
//  {
//         MPU6050_Read(); 
//         OLED_P8x16Three(0,2,GyroX);
//     OLED_P8x16Three(40,2,GyroY);
//     OLED_P8x16Three(80,2,GyroZ);
//         OLED_P8x16Three(0,4,GyroX_Bias);
//     OLED_P8x16Three(40,4,GyroY_Bias);
//     OLED_P8x16Three(80,4,GyroZ_Bias);
//      //OLED_Fill(0x00); // used 10.32ms @ AVR168 16.000MHz OLED_WrDat NO delay 
//         if(cali_flag==1)
//     {
//             OLED_P8x16Str(0,6,"               ",1);
//       OLED_P8x16Str(0,6,"Cali",1);
//       msDelay(1000);  // 1s��ʼУ׼    ����������ȶ� 
//       OLED_P8x16Str(32,6,".",1);// ��һ����
//       for(i=0;i<50;i++) // ��ȡ���ݽ׶α��뱣�־�ֹ 
//       {
//          MPU6050_Read(); 
//          temp[i]=GyroX;  
//          msDelay(5);
//       }
//       SortNum(temp,50); // �Բȵ���50�����������  
//             temp_sum=0;
//             for(i=10;i<40;i++)
//       { 
//         temp_sum+=temp[i];
//       }
//             GyroX_Bias+=Float2Int(temp_sum/30.0);// X��������У׼��� 
//             OLED_P8x16Str(40,6,".",1);//�ڶ����� 
//           for(i=0;i<50;i++) // ��ȡ���ݽ׶α��뱣�־�ֹ 
//       {
//          MPU6050_Read(); 
//          temp[i]=GyroY;  
//          msDelay(5);
//       }
//       SortNum(temp,50); // �Բȵ���50�����������  
//             temp_sum=0;
//             for(i=10;i<40;i++)
//       {
//         temp_sum+=temp[i];
//       }
//             GyroY_Bias+=Float2Int(temp_sum/30.0);// Y��������У׼��� 
//           OLED_P8x16Str(48,6,".",1);//�������� 
//           for(i=0;i<50;i++) // ��ȡ���ݽ׶α��뱣�־�ֹ 
//       {
//          MPU6050_Read(); 
//          temp[i]=GyroZ;  
//          msDelay(5);
//       }
//       SortNum(temp,50); // �Բȵ���50�����������  
//             temp_sum=0;
//             for(i=10;i<40;i++)
//       {
//         temp_sum+=temp[i];
//       }
//       GyroZ_Bias+=Float2Int(temp_sum/30.0);// Z��������У׼��� 
//       OLED_P8x16Str(56,6,".",1);// ���ĸ���
//       OLED_P8x16Str(64,6,".",1);
////       OLED_P8x16Str(72,4,".",1);msDelay(100);//��
////       OLED_P8x16Str(80,4,".",1);msDelay(100);//��
////       OLED_P8x16Str(88,4,".",1);msDelay(100);//�� 
//       OLED_P8x16Str(72,6,"OK!",1); 
//       cali_flag=0; // ��������У׼
//       //Update_ParamValue();
//      }
//            key=ADKey_Scan();    
//            if(key==KEY_UP)
//            {
//              while(ADKey_Scan()==KEY_UP);// �˴��������ּ���ԭ����,��ͬ�˰��°������ɿ���ʱ�䲻һ��
//                // �������ɿ��ֵ�ʱ��Ϊ׼,Ŀ�����ð��������ǳ���ȶ�.
//              cali_flag=1;
//             //  redraw_flag=1;
//            }    
//            msDelay(50); 
//    }
//    while(ADKey_Scan()!=0);  // ���а��� ���� �Ϳ������� 
//  Delay_us(10);
//}
/********************The last study************************
#function�NADKey_Cali()
#input:  void
#output: void 
#others: AD���� ADֵУ׼���������У���У׼AD����
#date:  2016-01-10
#author:  yizhi 
***********************************************************/
void ADKey_Cali(void)
{
//     uint ad;
//     OLED_CLS();
//     OLED_P8x16Str(0,0,"AD Value is:",1);
//     while(ADKey_Scan()!=KEY_CANCEL)
//     {
//         ad=Get_ADC10bitResult(0x03); 
//         msDelay(300);
//     OLED_P8x16Num4(0,2,ad);
//   }
}
/*********prepare to lanqiao competition*************************
#function: IR_Test
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: 
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void IR_Test(void)
{
//     uchar quit_flag=0;
//   OLED_CLS();
//     EX1=1; // enable Int1         
//     TMOD&=0xf0; // T0��ʱ�����ں���������
//     AUXR&=~0x80;// 1Tģʽ ��
//   ET0=0;//�ر��ж�
//   TR0=0;//ֹͣ��ʱ��
//     EA=1; 
//     OLED_P8x16Str(0,0,"IR_Docode:",1);
//     IR_OK_Flag=0;
//     
//     while(ADKey_Check()!=KEY_UP) // ʹ�����ϼ���Ϊ�˳���
//     {
//         if(IR_OK_Flag)
//         {
//             LED0_Turn();
//             IR_OK_Flag=0;
//             OLED_HexDisp(0,2,IR_Buf,4);
//         }
//         msDelay(5);
//     }
//     EX1=0;// �ر��ⲿ�ж�
     
}
/*********prepare to lanqiao competition*************************
#function: RTC_Timer
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description: ����� OLEDͬ����ʾ 
#author::  2016-03-16 by'  yizhi 
******************************************************************/
void RTC_Timer()
{ 
    /*
     RTC_Time_s time={0,0,0,0,0,0};
   OLED_CLS();
     Initial_DS1302();
     TMOD&=0x0f;
     AUXR&=~0x40;// 1T
     TH1=63536>>8;
     TL1=63536&0x00ff;
     TF1=0;
     TR1=1;
     ET1=1;
     EA=1;
     DS1302_GetTime(&time);
     OLED_P8x16Str(0,0,"RTC_Timer:",1);
     //time={0,0,0,0,0,0};  // ����������ֵ 
     while(ADKey_Scan()!=KEY_CANCEL)
     {
         //ad=Get_ADC10bitResult(0x03); 
       DS1302_GetTime(&time);
         GPSTime_Disply(0,2,&time);
         UpdateTime(&time); // ˢ�������ʾ�Դ�
         msDelay(990);
     }
   TR0=0;
   ET0=0;
     EA=0; // �رն�ʱ��,�ͷ���Դ
     //OLED_P8x16Num4(0,2,ad);   
    */
}
void DS18B20(void)
{
//      INT16U temp;
//      OLED_CLS();
//      while(Init_DS18B20())
//        {
//            
//             LED0_Turn();
//       OLED_P8x16Str(0,2,"DS18B20 is't connected!!",1);
//            if(KEY_CANCEL==ADKey_Scan()) // ADKey_Check2()
//            {
//                return;
//            }    
//       msDelay(20);            
//        }
//      Init_DS18B20();
//      msDelay(10);
//      OLED_Print(0,0,"�¶�Value:");
//      
//      while(ADKey_Scan()!=KEY_CANCEL)
//        {
//            temp=ReadTemperature();
//            OLED_P16x32Num(1,temp,1); 
//            msDelay(50); // ����Ӹ���ʱ�ȽϺ�   @2016-04-24 ����
//            LED0_Turn();
//        }

}

/********************The last study************************
#function�NMPU6050_Test 
#input:  
#output:  
#others: 
#date:  2016-01-10
#author:  yizhi 
***********************************************************/ 
/*********prepare to lanqiao competition*************************
#function:  HMC_Test();
#input: none
#output: none
#others: refer to IAP15F2K60S2 pdf
#description:
#author::  2016-04-28 by'  yizhi 
******************************************************************/
/**
********************************************************************************************************
** @nameis PWM_KeyCallback
** @effect KeyValue change then do something
** @import Current selected item
** @export none
** @return
** @create yizhi 2023.8.1
** @modify 
*********************************************************************************************************/
extern  INT32U GusHallSpd;
INT32U  GusHallSpdPut;
float  GfLowPassIndex = 0.5;
INT8U  ucTemperature;
INT16U ulVoltage10Mv;
INT16U uleRpm, uleRpmFilter;
INT32U uleErrorCode = 0;
INT16U uleRpmLPFilterArray[20];
INT8U  ucArrayIndex = 0, GucLingORptEn = 1;
INT32U uleRpmFilterSum = 0;
INT16S sPwmDutyValue  = 0;
uchar UartCMDSpeedTxBuf[10] = {0x2B, 0xC1, 0x3F};
uchar MenuValue[7][16];
uchar ucNowItem = 0;
uchar ucFirstItem = 0;
uchar ucTopDisp;
uchar ucEntrysEveryScreen = 0;
uchar ucEntrysTotal       = 0;
uchar bPwmRunning  = 0, bScreenIDFlg = 1, bScreenValFlg = 0, bEdit_flag   = 0;
void BlowerBiLTest(uint8_t ucX)
{
    KEYn_e eKeyPress;
    uint8_t ii = 0, ul10msCnt = 0, bFlip = 0, bErDispFlg = 0;
    INT16U  usVol;
    INT16S  sCur;
    
    GbUartRxDone = 0;
    GucPolePair = 1;
    bPwmRunning  = 0;
    bScreenIDFlg = 1;
    bScreenValFlg = 0,
    bEdit_flag   = 0;
    ucEntrysEveryScreen = 6;
    ucEntrysTotal       = 6;
    INA226_Init();
    BraoCalibrate();
    
    FLASH_Read(PWMCTRL1_ADDR, (uint16_t*)PwmCtrl1.sPwmCtrl, 6);
    if ((uint8_t)PwmCtrl1.sPwmCtrl[0] == 0xFF && (uint8_t)(PwmCtrl1.sPwmCtrl[0]>>8) == 0xFF)
    {
        PwmCtrl1.tPwmCtrl.sPwmLowVal  = 500;
        PwmCtrl1.tPwmCtrl.sPwmLowDur  = 100;
        PwmCtrl1.tPwmCtrl.sPwmHighVal = 999;
        PwmCtrl1.tPwmCtrl.sPwmHighDur = 100;
        PwmCtrl1.tPwmCtrl.ePwmProtocol = 0;
    }
    
    OLED_Fill(0x00);
//        OLED_PutStr(105, OLED_LINE0, (uint8_t *)"ErCd", 6, 1); 
    OLED_PutStr(104, OLED_LINE3, (uint8_t *)"RPM", 6, 1); 
    OLED_PutStr(104, OLED_LINE3 + (LINE_HEIGHT >> 1), (uint8_t *)"RPM", 6, 1); 
    PWM_KeyCallback(&PwmCtrl1, 4);                         /* stimulate Press KeyProtocol KeyUP KEYDOWN update related value */
    PWM_KeyCallback(&PwmCtrl1, 0);
    PWM_KeyCallback(&PwmCtrl1, 2);
    
    while(ADKey_Scan()!=KEY_CANCEL)
    {
       if (GbUartRxDone) {
          if (USART_RX_BUF[0] == 0x2B) {              /* ������LingO���Э�� */
              if (GucUartRxIndex == 8)
              {
                  uleErrorCode = 0;
                  uleRpm        = USART_RX_BUF[3] << 8 | USART_RX_BUF[4];
                  GbUartRxDone =  0;                                     /* ������� */ 
                  uleRpmFilterSum += uleRpm;
                  ucArrayIndex++;
    //              OLED_PutNum(64,  OLED_LINE3 + (LINE_HEIGHT >> 1),  uleRpm,     5,          6, 1);
                  if (ucArrayIndex == 10) {
                      ucArrayIndex = 0;
                      uleRpmFilter = uleRpmFilterSum / 10;
                      uleRpmFilterSum = 0;                      
                     /* OLED_PutNumber(0 , OLED_LINE1, ulVoltage10Mv / 100.0f, 2, 1, "V",  8, 1);
                        OLED_PutNumber(48, OLED_LINE1, ucTemperature,          2, 0, "��", 8, 1); */
    //                  OLED_PutNum(64,  OLED_LINE3 + (LINE_HEIGHT >> 1),  uleRpmFilter * 100,     5,          6, 1);
                      OLED_PutNum(64,  OLED_LINE3 + (LINE_HEIGHT >> 1),  uleRpmFilter,     5,          6, 1);
                  }
              }
              else if (GucUartRxIndex == 6)
              {
                  uleErrorCode  = USART_RX_BUF[3] << 8 | USART_RX_BUF[4];
                  if (uleErrorCode)
                  {
                      bErDispFlg = !bErDispFlg;
                      if (bErDispFlg) {
                          OLED_ClearLine(OLED_LINE0, OLED_LINE0 + (LINE_HEIGHT >> 1), 0x00);
                      } else {
                          OLED_PutStr(0, OLED_LINE0, (uint8_t *)"Error:", 8, 1); 
                          OLED_HexDisp(72, OLED_LINE0, (uint8_t *)&uleErrorCode, 2, 8, 1);
                      }
                  }
              }
          } else if (GucUartRxIndex == 10 && USART_RX_BUF[0] != 0x5A) {              /* �������Э�� */
              GucLingORptEn = 0;
              ucTemperature = USART_RX_BUF[0];
              ulVoltage10Mv = USART_RX_BUF[1] << 8 | USART_RX_BUF[2];
              uleRpm        = USART_RX_BUF[7] << 8 | USART_RX_BUF[8];
              GbUartRxDone =  0;                                     /* ������� */ 
              uleRpmFilterSum += uleRpm;
              ucArrayIndex++;
//              OLED_PutNum(64,  OLED_LINE3 + (LINE_HEIGHT >> 1),  uleRpm*100,     5,          6, 1);
              if (ucArrayIndex == 10) {
                  ucArrayIndex = 0;
                  uleRpmFilter = uleRpmFilterSum / 10;
                  uleRpmFilterSum = 0;                      
                 /* OLED_PutNumber(0 , OLED_LINE1, ulVoltage10Mv / 100.0f, 2, 1, "V",  8, 1);
                    OLED_PutNumber(48, OLED_LINE1, ucTemperature,          2, 0, "��", 8, 1); */
                  OLED_PutNum(64,  OLED_LINE3 + (LINE_HEIGHT >> 1),  uleRpmFilter * 100 / GucPolePair,     5,          6, 1);
//                  OLED_PutNum(64,  OLED_LINE3 + (LINE_HEIGHT >> 1),  uleRpm,     5,          6, 1);
              }
          } else if (GucUartRxIndex == 10 && USART_RX_BUF[0] == 0x5A) {             /* ���޴�Э�� */
              GucLingORptEn = 0;                                                    /* ����LingO���� */
              uleRpm        = USART_RX_BUF[7] << 8 | USART_RX_BUF[6];
              uleErrorCode  = USART_RX_BUF[4] << 8 | USART_RX_BUF[5];
              GbUartRxDone =  0;                                   /* ������� */               
              OLED_PutNum(64,  OLED_LINE3 + (LINE_HEIGHT >> 1),  uleRpm,     5,          6, 1);
              if (uleErrorCode)
              {
                  bErDispFlg = !bErDispFlg;
                  if (bErDispFlg) {
                      OLED_ClearLine(OLED_LINE0, OLED_LINE0 + (LINE_HEIGHT >> 1), 0x00);
                  } else {
                      OLED_PutStr(0, OLED_LINE0, (uint8_t *)"Error:", 8, 1); 
                      OLED_HexDisp(72, OLED_LINE0, (uint8_t *)&uleErrorCode, 2, 8, 1);
                  }
              }
          }else {
              uleRpm = 0;
              GbUartRxDone = 0;                                     /* ֱ��������� */
          }
          memset(USART_RX_BUF, 0, sizeof(USART_RX_BUF));
        }
        if (GulPrintTimeCnt > 10) { 
            GulPrintTimeCnt = 0;
            
            if (ul10msCnt++ > 10) {
                ul10msCnt = 0;
                if (GucLingORptEn == 1)
                    uartDrvPutBuf(USART2, UartCMDSpeedTxBuf, 3);    /* ask for speed frame */ 
                bFlip = !bFlip;
            
                if (bFlip) {
//                    OLED_PutNumber(0, OLED_LINE3 + (LINE_HEIGHT >> 1),  BraoGet(), 2, 1, "cmH", 6, 1); 
                } else if (!uleErrorCode){
                                      
                    INA226_Read(&usVol, &sCur);
                    OLED_PutNumber(0,  OLED_LINE0, usVol / 100.0f, 2, 2, "V", 8, 1);
                    OLED_PutNumber(72, OLED_LINE0, sCur / 1000.0f, 2, 3, "A", 8, 1);
                }
                GusHallSpdPut = (INT32U)(GfLowPassIndex * (float)GusHallSpdPut + (1 - GfLowPassIndex)*(float)GusHallSpd);
                OLED_PutNum(64, OLED_LINE3, GusHallSpdPut, 5, 6, 1);
            }
            if(bScreenIDFlg)
            {
                bScreenIDFlg=0;
                
                strcpy((char*)MenuItem[0], "L:");
                strcpy((char*)MenuItem[1], "T:");
                strcpy((char*)MenuItem[2], "H:");
                strcpy((char*)MenuItem[3], "T:");
                strcpy((char*)MenuItem[4], "Mode:");
                strcpy((char*)MenuItem[5], "Pole:");
                
//                sprintf((char*)MenuValue[0], "%4.1f%%", PwmCtrl1.tPwmCtrl.sPwmLowVal / 10.0f);
//                sprintf((char*)MenuValue[1] ,"%04dms", PwmCtrl1.tPwmCtrl.sPwmLowDur * 10);
//                sprintf((char*)MenuValue[2] ,"%4.1f%%", PwmCtrl1.tPwmCtrl.sPwmHighVal / 10.0f);
//                sprintf((char*)MenuValue[3] ,"%04dms", PwmCtrl1.tPwmCtrl.sPwmHighDur * 10);
//                sprintf((char*)MenuValue[4] ,"%02d", PwmCtrl1.tPwmCtrl.ePwmProtocol);
                
                for(ii=0; ii < ucEntrysEveryScreen; ii++)                                          
                {
                    if((ucNowItem != ii) || bPwmRunning == 1)
                    {   
                        HMI_DrawName(ii, 0, MenuItem[ucFirstItem+ii], 1); // ������ʾ
                    }
                    else
                    {
                        HMI_DrawName(ii, 0, MenuItem[ucFirstItem+ii], 0); // ������ʾ
                    }
                }

                for(ii=0; ii < ucEntrysEveryScreen; ii++)                                          
                {
                      HMI_DrawNumber(ii, 16, PwmCtrl1.sPwmCtrl[ii], &PwmCtrl1, 1); // ������ʾÿһ��Name�µ�Number
                }
            }
            
            if (bScreenValFlg)
            {
                bScreenValFlg = 0;
//                sprintf((char*)MenuValue[0], "%4.1f%%", PwmCtrl1.tPwmCtrl.sPwmLowVal / 10.0f);
//                sprintf((char*)MenuValue[1] ,"%04dms", PwmCtrl1.tPwmCtrl.sPwmLowDur*10);
//                sprintf((char*)MenuValue[2] ,"%4.1f%%", PwmCtrl1.tPwmCtrl.sPwmHighVal / 10.0f);
//                sprintf((char*)MenuValue[3] ,"%04dms", PwmCtrl1.tPwmCtrl.sPwmHighDur*10);
//                sprintf((char*)MenuValue[4] ,"%02d", PwmCtrl1.tPwmCtrl.ePwmProtocol);
                HMI_DrawNumber(ucNowItem, 16, PwmCtrl1.sPwmCtrl[ucFirstItem+ucNowItem], &PwmCtrl1, 0);                 /* ��ǰѡ����Ŀ������ʾ  */

            }
            eKeyPress = ADKey_Check();
            switch (eKeyPress) {
                
            case EC11_CW:

                if (!bEdit_flag) {
                    GsEc11CntCW                  = 0;
                    if(ucNowItem > 0)         ucNowItem   -= 1;               // ��ǰ�� ����Ŀ
                    else if(ucFirstItem > 0)  ucFirstItem -= 1;               // ��ǰ�� �ĵ�һ����Ŀ 
                    else                                                      // �ѵ������в˵��ĵ�һ����Ŀ
                    {
                        ucNowItem   =  ucEntrysEveryScreen - 1;
                        ucFirstItem =  ucEntrysTotal - ucEntrysEveryScreen;                                
                    }
                    bScreenIDFlg = 1;
                    bPwmRunning  = 0;                                   /* once rotate stop pwm run */ 
                } else {
                    PwmCtrl1.sPwmCtrl[ucNowItem] -= (GsEc11CntCW + 1);          /* ��ѡ�е�ֵ���м��� */
                    GsEc11CntCW                  = 0;
                   if (PwmCtrl1.sPwmCtrl[ucNowItem] < 0) PwmCtrl1.sPwmCtrl[ucNowItem] = 0;
                    bScreenValFlg = 1;
                    PWM_KeyCallback(&PwmCtrl1, ucNowItem);
                    if (ucNowItem == 4) {
                        PWM_KeyCallback(&PwmCtrl1, 0);
                        PWM_KeyCallback(&PwmCtrl1, 2);
                        PwmCtrl1.tPwmCtrl.sPwmWidthSetting = PwmCtrl1.tPwmCtrl.sPwmWidthDflt;
                        bScreenIDFlg = 1;
                    } else if (ucNowItem == 5 && GucPolePair > 1) {
                        GucPolePair--;
                    }
                }
                break;
            
            case EC11_CCW:
                if (!bEdit_flag) {
                    GsEc11CntCCW                 = 0;
                    if(ucNowItem < (ucEntrysEveryScreen - 1))                      ucNowItem   += 1;
                    else if((ucFirstItem + ucEntrysEveryScreen) < ucEntrysTotal)   ucFirstItem += 1;
                    else
                    {
                      ucFirstItem = 0;
                      ucNowItem   = 0;
                    }
                    bScreenIDFlg = 1;
                    bPwmRunning  = 0;                                   /* once rotate stop pwm run */ 
                } else {
                    PwmCtrl1.sPwmCtrl[ucNowItem] += (GsEc11CntCCW + 1);       /* ��ѡ�е�ֵ�������� */
                    GsEc11CntCCW                 = 0;
                    if (PwmCtrl1.sPwmCtrl[ucNowItem] > 999) PwmCtrl1.sPwmCtrl[ucNowItem] = 999;
                    bScreenValFlg = 1;
                    PWM_KeyCallback(&PwmCtrl1, ucNowItem);
                    if (ucNowItem == 4) {
                        PWM_KeyCallback(&PwmCtrl1, 0);
                        PWM_KeyCallback(&PwmCtrl1, 2);
                        PwmCtrl1.tPwmCtrl.sPwmWidthSetting = PwmCtrl1.tPwmCtrl.sPwmWidthDflt;
                        bScreenIDFlg = 1;
                    }else if (ucNowItem == 5 && GucPolePair < 4) {
                        GucPolePair++;
                        bScreenIDFlg = 1;
                    }
                }
                break;
            
            case KEY_UP: 
                bPwmRunning = !bPwmRunning;
                bScreenIDFlg = 1;
                bEdit_flag   = 0;
                break;
            case KEY_DOWN: 
                HallCalibrationEn();
                break;
            
            case EC11_SET:
                              if (uleErrorCode)
                                      OLED_ClearLine(OLED_LINE0, OLED_LINE0 + (LINE_HEIGHT >> 1), 0x00);
                if (!bPwmRunning) {
                    FLASH_Write(PWMCTRL1_ADDR, (uint16_t*)PwmCtrl1.sPwmCtrl, 6);
                    bEdit_flag = !bEdit_flag;
                    (bEdit_flag == 1) ? (bScreenValFlg = 1) : (bScreenIDFlg = 1);
                    if (bEdit_flag) 
                        PWM_KeyCallback(&PwmCtrl1, ucNowItem);
                } else {                                                /* enter adjust mode when Pwm is Runing, just like press KEY_CANCEL */
                    bPwmRunning = !bPwmRunning;
                    bScreenIDFlg = 1;
                    bEdit_flag   = 0;
                }
                break;
                
            default:
                break;
            }
//            if (bEdit_flag)
//                sPwmDutyValue = PwmCtrl1.tPwmCtrl.sPwmHighVal;
//            else 
//                sPwmDutyValue = 0;
//            
//            if (sPwmDutyValue >= 999)  sPwmDutyValue = 999;
//            if (sPwmDutyValue <= 0)      sPwmDutyValue = 0;
            
            
            if (bPwmRunning)  {
                PwmCtrl1.tPwmCtrl.usTime10msCnt++;
                if (PwmCtrl1.tPwmCtrl.usTime10msCnt < PwmCtrl1.tPwmCtrl.sPwmLowDur) {
                    PwmCtrl1.tPwmCtrl.sPwmDutyValue = PwmCtrl1.tPwmCtrl.sPwmWidthLow;
                } else if (PwmCtrl1.tPwmCtrl.usTime10msCnt < (PwmCtrl1.tPwmCtrl.sPwmLowDur + PwmCtrl1.tPwmCtrl.sPwmHighDur)) {
                    PwmCtrl1.tPwmCtrl.sPwmDutyValue = PwmCtrl1.tPwmCtrl.sPwmWidthHigh;
                } else {
                    PwmCtrl1.tPwmCtrl.usTime10msCnt = 0;
                }
            } else if (bEdit_flag){                                                    /* set pwm output to default value when not run  */
                PwmCtrl1.tPwmCtrl.usTime10msCnt = 0;
                PwmCtrl1.tPwmCtrl.sPwmDutyValue = PwmCtrl1.tPwmCtrl.sPwmWidthSetting;
            } else {
                PwmCtrl1.tPwmCtrl.usTime10msCnt = 0;
                PwmCtrl1.tPwmCtrl.sPwmDutyValue = PwmCtrl1.tPwmCtrl.sPwmWidthDflt;
            }
//            OLED_PutNum(0,  OLED_LINE3, 1234,  4, 8, 1);
            PWM_DUTY_SETA(PwmCtrl1.tPwmCtrl.sPwmDutyValue);
        }
    }
    FLASH_Write(PWMCTRL1_ADDR, (uint16_t*)PwmCtrl1.sPwmCtrl, 6);
    GsEc11CntCW = 0;
    GsEc11CntCCW = 0;
    PWM_DUTY_SETA(PwmCtrl1.tPwmCtrl.sPwmWidthDflt);
}


void BlowerC60Test(uint8_t ucX)
{
    KEYn_e eKeyPress;
    uint8_t ii = 0, ul10msCnt = 0, bFlip = 0;
    INT16U  usVol;
    INT16S  sCur;
    
    bPwmRunning  = 0;
    bScreenIDFlg = 1;
    bScreenValFlg = 0,
    bEdit_flag   = 0;
    ucEntrysEveryScreen = 4;
    ucEntrysTotal       = 4;
    INA226_Init();
    BraoCalibrate();
    
    OLED_Fill(0x00);
    OLED_Print(22,  OLED_LINE0, (uint8_t *)"C61����", 1);
    OLED_PutStr(104, OLED_LINE3, (uint8_t *)"RPM", 8, 1);
    FLASH_Read(PWMCTRL2_ADDR, (uint16_t*)PwmCtrl2.sPwmCtrl, 6);
    if ((uint8_t)PwmCtrl2.sPwmCtrl[0] == 0xFF && (uint8_t)(PwmCtrl2.sPwmCtrl[0]>>8) == 0xFF)
    {
        PwmCtrl2.tPwmCtrl.sPwmLowVal  = 1001;
        PwmCtrl2.tPwmCtrl.sPwmLowDur  = 100;
        PwmCtrl2.tPwmCtrl.sPwmHighVal = 2001;
        PwmCtrl2.tPwmCtrl.sPwmHighDur = 100;
        PwmCtrl2.tPwmCtrl.ePwmProtocol = 1;
    }
    PWM_KeyCallback(&PwmCtrl2, 4); /// stimulate Press KeyProtocol KeyUP KEYDOWN update related value 
    PWM_KeyCallback(&PwmCtrl2, 0);
    PWM_KeyCallback(&PwmCtrl2, 2);
    
    
    while(ADKey_Scan()!=KEY_CANCEL)
    {
        if (GulPrintTimeCnt > 10) { 
            GulPrintTimeCnt = 0;
         /* uartDrvPutBuf(USART2, UartCMDSpeedTxBuf, 3);    ask for speed frame */
            if (ul10msCnt++ > 10) {
                ul10msCnt = 0;
                bFlip = !bFlip;
            
                if (bFlip) {
                    OLED_PutNumber(0, OLED_LINE3 + (LINE_HEIGHT >> 1),  BraoGet(), 2, 1, "cmH", 6, 1); 
                } else {
                    INA226_Read(&usVol, &sCur);
                    OLED_PutNumber(80, OLED_LINE0, usVol / 100.0f, 2, 2, "V", 6, 1);
                    OLED_PutNumber(80, OLED_LINE0 + (LINE_HEIGHT >> 1), sCur / 1000.0f, 1, 3, "A", 6, 1);
                }
            }
            if(bScreenIDFlg)
            {
                bScreenIDFlg=0;
                
                strcpy((char*)MenuItem[0], "H:");
                strcpy((char*)MenuItem[1], "T:");
                strcpy((char*)MenuItem[2], "L:");
                strcpy((char*)MenuItem[3], "T:");
                strcpy((char*)MenuItem[4], "P:");
                
                sprintf((char*)MenuValue[0], "%04dus", PwmCtrl2.tPwmCtrl.sPwmWidthLow);
                sprintf((char*)MenuValue[1] ,"%04dms", PwmCtrl2.tPwmCtrl.sPwmLowDur * 10);
                sprintf((char*)MenuValue[2] ,"%04dus", PwmCtrl2.tPwmCtrl.sPwmWidthHigh);
                sprintf((char*)MenuValue[3] ,"%04dms", PwmCtrl2.tPwmCtrl.sPwmHighDur * 10);
                sprintf((char*)MenuValue[4] ,"%02d", PwmCtrl2.tPwmCtrl.ePwmProtocol);
                
                for(ii=0; ii < ucEntrysEveryScreen; ii++)                                          
                {
                    if((ucNowItem != ii) || bPwmRunning == 1)
                    {   
                        HMI_DrawName(ii, 0, MenuItem[ucFirstItem+ii], 1); // ������ʾ
                    }
                    else
                    {
                        HMI_DrawName(ii, 0, MenuItem[ucFirstItem+ii], 0); // ������ʾ
                    }
                }

                for(ii=0; ii < ucEntrysEveryScreen; ii++)                                          
                {
                      HMI_DrawNumber(ii, 16, PwmCtrl2.sPwmCtrl[ucFirstItem+ii], &PwmCtrl2, 1); // ������ʾ
                }
            }
            
            if (bScreenValFlg)
            {
                bScreenValFlg = 0;
                sprintf((char*)MenuValue[0], "%04dus", PwmCtrl2.tPwmCtrl.sPwmWidthLow);
                sprintf((char*)MenuValue[1] ,"%04dms", PwmCtrl2.tPwmCtrl.sPwmLowDur * 10);
                sprintf((char*)MenuValue[2] ,"%04dus", PwmCtrl2.tPwmCtrl.sPwmWidthHigh);
                sprintf((char*)MenuValue[3] ,"%04dms", PwmCtrl2.tPwmCtrl.sPwmHighDur * 10);
                sprintf((char*)MenuValue[4] ,"%02d", PwmCtrl2.tPwmCtrl.ePwmProtocol);
                HMI_DrawNumber(ucNowItem, 16,  PwmCtrl2.sPwmCtrl[ucFirstItem+ucNowItem],&PwmCtrl2, 0); // ������ʾ

            }
            eKeyPress = ADKey_Check();
            switch (eKeyPress) {
                
            case EC11_CW:

                if (!bEdit_flag) {
                    GsEc11CntCW                  = 0;
                    if(ucNowItem > 0)         ucNowItem   -= 1;               // ��ǰ�� ����Ŀ
                    else if(ucFirstItem > 0)  ucFirstItem -= 1;               // ��ǰ�� �ĵ�һ����Ŀ 
                    else                                                      // �ѵ������в˵��ĵ�һ����Ŀ
                    {
                        ucNowItem   =  ucEntrysEveryScreen - 1;
                        ucFirstItem =  ucEntrysTotal - ucEntrysEveryScreen;                                
                    }
                    bScreenIDFlg = 1;
                    bPwmRunning  = 0;                                   /* once rotate stop pwm run */ 
                } else {
                    PwmCtrl2.sPwmCtrl[ucNowItem] -= (GsEc11CntCW + 1);
                    GsEc11CntCW                  = 0;
                   if (PwmCtrl2.sPwmCtrl[ucNowItem] < 0) PwmCtrl2.sPwmCtrl[ucNowItem] = 0;
                    bScreenValFlg = 1;
                    PWM_KeyCallback(&PwmCtrl2, ucNowItem);
                }
                break;
            
            case EC11_CCW:
                if (!bEdit_flag) {
                    GsEc11CntCCW                 = 0;
                    if(ucNowItem < (ucEntrysEveryScreen - 1))                      ucNowItem   += 1;
                    else if((ucFirstItem + ucEntrysEveryScreen) < ucEntrysTotal)   ucFirstItem += 1;
                    else
                    {
                      ucFirstItem = 0;
                      ucNowItem   = 0;
                    }
                    bScreenIDFlg = 1;
                    bPwmRunning  = 0;                                   /* once rotate stop pwm run */ 
                } else {
                    PwmCtrl2.sPwmCtrl[ucNowItem] += (GsEc11CntCCW + 1);
                    GsEc11CntCCW                 = 0;
                    if (ucNowItem == 0 || ucNowItem == 2) {                                 /* ucNowItem is 0 or 2   */
                        if (PwmCtrl2.sPwmCtrl[ucNowItem] > 3000) PwmCtrl2.sPwmCtrl[ucNowItem] = 3000;
                    } else {
                        if (PwmCtrl2.sPwmCtrl[ucNowItem] > 500) PwmCtrl2.sPwmCtrl[ucNowItem] = 500;
                    }
                    bScreenValFlg = 1;
                    PWM_KeyCallback(&PwmCtrl2, ucNowItem);
                }
                break;
            
            case KEY_UP: 
                bPwmRunning = !bPwmRunning;
                bScreenIDFlg = 1;
                bEdit_flag   = 0;
                break;
            
            case EC11_SET:
                if (!bPwmRunning) {
                    FLASH_Write(PWMCTRL2_ADDR, (uint16_t*)PwmCtrl2.sPwmCtrl, 6);
                    bEdit_flag = !bEdit_flag;
                    (bEdit_flag == 1) ? (bScreenValFlg = 1) : (bScreenIDFlg = 1);
                    if (bEdit_flag) 
                        PWM_KeyCallback(&PwmCtrl2, ucNowItem);
                } else {                                                /* enter adjust mode when Pwm is Runing, just like press KEY_CANCEL */
                    bPwmRunning = !bPwmRunning;
                    bScreenIDFlg = 1;
                    bEdit_flag   = 0;
                }
                break;
                
            default:
                break;
            }
            if (bEdit_flag)
                sPwmDutyValue = PwmCtrl2.tPwmCtrl.sPwmHighVal;
            else 
                sPwmDutyValue = 0;
            
            if (sPwmDutyValue >= 999)  sPwmDutyValue = 999;
            if (sPwmDutyValue <= 0)      sPwmDutyValue = 0;
            
            
            if (bPwmRunning)  {
                PwmCtrl2.tPwmCtrl.usTime10msCnt++;
                if (PwmCtrl2.tPwmCtrl.usTime10msCnt < PwmCtrl2.tPwmCtrl.sPwmLowDur) {
                    PwmCtrl2.tPwmCtrl.sPwmDutyValue = PwmCtrl2.tPwmCtrl.sPwmWidthLow;
                } else if (PwmCtrl2.tPwmCtrl.usTime10msCnt < (PwmCtrl2.tPwmCtrl.sPwmLowDur + PwmCtrl2.tPwmCtrl.sPwmHighDur)) {
                    PwmCtrl2.tPwmCtrl.sPwmDutyValue = PwmCtrl2.tPwmCtrl.sPwmWidthHigh;
                } else {
                    PwmCtrl2.tPwmCtrl.usTime10msCnt = 0;
                }
            } else if (bEdit_flag){                                                    /* set pwm output to default value when not run  */
                PwmCtrl2.tPwmCtrl.usTime10msCnt = 0;
                PwmCtrl2.tPwmCtrl.sPwmDutyValue = PwmCtrl2.tPwmCtrl.sPwmWidthSetting;
            } else {
                PwmCtrl2.tPwmCtrl.usTime10msCnt = 0;
                PwmCtrl2.tPwmCtrl.sPwmDutyValue = PwmCtrl2.tPwmCtrl.sPwmWidthDflt;
            }
            
            PWM_DUTY_SETA(PwmCtrl2.tPwmCtrl.sPwmDutyValue);
        }
    }
    FLASH_Write(PWMCTRL2_ADDR, (uint16_t*)PwmCtrl2.sPwmCtrl, 6);
    GsEc11CntCW = 0;
    GsEc11CntCCW = 0;
}

/***********************************************************************************************************
# Function: Menu_Display()
# Input: void
# Output: void
# Description: 
# Others:
# Date  : 2015/3/8, by yizhi
************************************************************************************************************/
void Menu_Display(void)
{
    uchar sel;
    FirstItem = 0;
    NowItem   = 0;
//    TopDisp  =1;

    while(1)  // ���˵�
    { 
        PWM_DUTY_SETA(0);
        strcpy((char*)MenuItem[0] ,"1:BlowerBiLTest ");     //BlowerBiLTest 
        strcpy((char*)MenuItem[1] ,"2:Blower-NONAME ");    //BlowerC60Test
        strcpy((char*)MenuItem[2] ,"3:INA226Test    ");
        strcpy((char*)MenuItem[3] ,"4:SDP800Test    ");
        strcpy((char*)MenuItem[4] ,"5:BraoCalibrate ");
        strcpy((char*)MenuItem[5] ,"6:CXD 7054      ");
        strcpy((char*)MenuItem[6] ,"7:BOREASA C68S1 ");
        strcpy((char*)MenuItem[7] ,"8:Nidec TF029B  ");
        strcpy((char*)MenuItem[8] ,"9:Todolist_Test ");
        
        OLED_Fill(0x00);
//        OLED_Print(8, OLED_LINE0, "��ѡ�����ͺţ�", 1);
        
        sel=DrawMenu(MenuItem,8,0); 
        msDelay(5);
        usDelay(5);
        switch (sel)  {
            
        case 0:
            BlowerBiLTest(sel);
            //UpperComCXD7054(sel);
            break;

        case 1:
            BlowerC60Test(sel);
           //UpperComBFC68S1(sel);
            break;

        case 2:
            INA226Test(sel);
            break;

        case 3:
            SDP800_TEST(sel);
            break;
        
        case 4:
            BraoCalibration(sel);
            break;

        case 5:
            UpperComCXD7054(sel);
            break;
        
        case 6:
            UpperComBFC68S1(sel);
            break;

        case 7:
            UpperComNidecTF029B(sel);
            break;

         default :
              break;
         
        }
    }
}
/*********************2016��1���ٴε��Բ˵������ܽ�*******************************/ 
/*
1��һ�η�������NRF_test�˵�������NOName����Ŀ��Ϊ��д��Ӧ��Ŀ������������о�����Ļ
��˸һ�£��������˵��У��ҵ��δ��������Ŀ���˵�����˸һ�£������˰��Сʱ�������ҳ�����Ϊ
��NRF-test�����˵������ı���λ�ã�����ҳ�˵������˵���δ���������λ�ã��ڳ������ҷ���ִ����
OLED_CLS() [����AAAע�ͱ�ʶ]������ע�͵���ע�ͺ�����õ�������Ժ��������������󣬿��ܾ���
��������ִ����OLED_CLS(),��������ִ��ʱ��ϳ�������������
2����HMI�ļ�������һ�������˵���NRF_Test����һ������ҳ�˵��µ���������ADCalibra�����ж����˵�
��ѭ����������while(!quit_flag)��ʽ  �������˵��µ�����������ҳ�˵��µ�����������ѭ������Ϊ
while(ADKey_Scan()!=KEY_CANCEL)�� 
*/



