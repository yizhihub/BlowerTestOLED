#include "spi.h"
//#define SPIF  0x80
//#define WCOL  0x40


/************To make helicopter***************************
#function： 读写一个字节，spi协议，读写是同时进行的
#input:  the data be writed
#output:  the data read 
#others: poll style(查询方式)， spi最低层
#date:  2014-08-24
#author:  yizhi 
***********************************************************/

uchar SPI_WriteRead(uchar dat)
{
	  //PCS2=0;
    SPDAT=dat;
	  while(!(SPSTAT&SPIF));
	  SPSTAT=0xC0; 
	 // PCS2=1;
	  return SPDAT;
}
/************To make helicopter***************************
#function：spi initialize 
#input: none
#output: none
#others: refer to IAP pdf
#date:  2014-08-24
#author:  yizhi 
***********************************************************/
void SPI_Init(void)
{
	 AUXR1&=~(1<<3); //  SPI_S1=3     SPI_S0=2  
	 AUXR1|=(1<<2); // set P2.4 P2.3 P2.2 P2.1  interface 
	 SPDAT=0;  // clear buffer 
	 SPSTAT=0xC0; // clear the spi flag;
	 SPCTL=0xD0;// SSIG  SPEN DORD MSTR CPOL CPHA SPR1 SPR0  //Master  MSB frist send  baud:sys/4   
	 IE2&=~(1<<2); // close the interrupt
}
/************To make helicopter***************************
#function： 在指定的地址， 写一个字节， 同时返回一个状态字
#input: add：Register address dat: data
#output: status( normaly useless)
#others:  adapted to NRF24L01,other spi machine may be not compatibile
#date:  2014-08-24
#author:  yizhi 
***********************************************************/
 uchar SPI_Write1(SPI_PCSn_e pcs,uchar add,uchar dat)
{
	  uchar status;
   // CSN=0;	
	 // SPI_SS(ss,1);// 遇到多片选时，想到的第一种方法。
	  switch(pcs)   // 第二种方法
	  {
		case 0: PCS0=0; break;
	  case 1: PCS1=0; break;
	  case 2: PCS2=0; break;
	  default:        break;
    }
	  //while(SPI_MISO); // waite for CC2500  CC2500 SPI协议 专用
	  SPDAT=add;  // write address
	  while(!(SPSTAT&SPIF));
		SPSTAT=0xC0;
	  status=SPDAT;
	  SPDAT=dat;  // write data
	  while(!(SPSTAT&SPIF));
		SPSTAT=0xC0;
		switch(pcs)
	  {
		case 0: PCS0=1; break; 
	  case 1: PCS1=1; break;
	  case 2: PCS2=1; break;
		default:       break;
    }
	  return status;
}
/************To make helicopter***************************
#function：指定地址读一个字节
#input: add：register address 
#output: the data read
#others: adapted to NRF24L01,other spi machine may be not compatibile
#date:  2014-08-24
#author:  yizhi 
***********************************************************/
uchar SPI_Read1(SPI_PCSn_e pcs,uchar add)
{
	 uchar temp;
	
	// CSN=0;  //拉从机的SS
	switch(pcs)   
	{
	case 0: PCS0=0; break;
	case 1: PCS1=0; break;
	case 2: PCS2=0; break;
	default:         break;
	}
	 //while(SPI_MISO);
	 SPDAT=add;
	 while(!(SPSTAT&SPIF));
	 SPSTAT=0xC0;  // 清标志位
	
	 SPDAT=0; 
	 while(!(SPSTAT&SPIF));
	 SPSTAT=0xC0;
	 temp=SPDAT;  //读寄存器的数据 
	switch(pcs)   
	{
	case 0: PCS0=1; break;
	case 1: PCS1=1; break;
	case 2: PCS2=1; break;
	default:         break;
	}       
   return temp;
}
/************To make helicopter***************************
#function：从指定的地址开始 连续写N个字节，
#input: add： the destination address  dat: the point of data serial 
#output: status(normal useless)
#others: adapted to NRF24L01,other spi machine may be not compatibile
#date:  2014-08-24
#author:  yizhi 
***********************************************************/

uchar SPI_WriteN(SPI_PCSn_e pcs,uchar add,uchar *dat,uchar N)
{
	 uchar status,i;
  switch(pcs)   
	{
	case 0: PCS0=0; break;
	case 1: PCS1=0; break;
	case 2: PCS2=0; break;
	default:         break;
	}  
	 SPDAT=add;
	 while(!(SPSTAT&SPIF));
	 SPSTAT=0xC0;
	 status=SPDAT;
	 for(i=0;i<N;i++)
	 {
		 SPDAT=*dat++; // SPDAT=dat[i];
	   while(!(SPSTAT&SPIF));
	   SPSTAT=0xC0; 
   }
  switch(pcs)   
	{
	case 0: PCS0=1; break;
	case 1: PCS1=1; break;
	case 2: PCS2=1; break;
	default:         break;
	}  
	 return status;
}
/************To make helicopter***************************
#function：从指定的地址开始 连续读取N  个字节，
#input: add： the destination address  dat: the point of data serial 
#output: status(normal useless)
#others: adapted to NRF24L01,other spi machine may be not compatibile
#date:  2014-08-24
#author:  yizhi 
***********************************************************/
uchar SPI_ReadN(SPI_PCSn_e pcs,uchar add,uchar *dat,uchar N)
{
	 uchar status,i;
  switch(pcs)   
	{
	case 0: PCS0=0; break;
	case 1: PCS1=0; break;
	case 2: PCS2=0; break;
	default:         break;
	}  
	 SPDAT=add;
	 while(!(SPSTAT&SPIF));
	 SPSTAT=0xC0;
	 status=SPDAT;
	 for(i=0;i<N;i++)
	 {
		SPDAT=0xff;  
	  while(!(SPSTAT&SPIF));
	  SPSTAT=0xC0;
    dat[i]=SPDAT;  
 	 }
		switch(pcs)   
	{
	case 0: PCS0=1; break;
	case 1: PCS1=1; break;
	case 2: PCS2=1; break;
	default:         break;
	}  
	 return status;	 
}



uchar SPI_Read_Buf(SPI_PCSn_e pcs,uchar reg, uchar *pBuf, uchar uchars)
{
	 
	 uchar status,uchar_ctr;
	 switch(pcs)   
	 {
	 case 0: PCS0=0; break;
	 case 1: PCS1=0; break;
	 case 2: PCS2=0; break;
 	 default:         break;
	 }  
	 
	 status = SPI_WriteRead(reg);   
	 for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
	 pBuf[uchar_ctr]=SPI_WriteRead(0xFF);
	 
	 switch(pcs)   
	 {
	 case 0: PCS0=1; break;
	 case 1: PCS1=1; break;
	 case 2: PCS2=1; break;
	 default:         break;
	 }        
	 return(status);              
}