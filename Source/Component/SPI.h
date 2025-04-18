#ifndef _spi_h_
#define _spi_h_
#include "common.h"

#define SPIF  0x80
#define WCOL  0x40

//sbit NRF_CSN=P4^3;
sbit SPI_MOSI=P2^3; 
sbit SPI_MISO=P2^2;
sbit SPI_CLK=P2^1;  

sbit PCS0=P4^3;
sbit PCS1=P2^0;
sbit PCS2=P2^0; // SD_CS 

#define NRF_CSN  SPI_PCS0
#define SD_CSN   SPI_PCS1  



typedef enum SPI_PCSn
{
	SPI_PCS0=0,
	SPI_PCS1,
	SPI_PCS2,
	
}SPI_PCSn_e;


void SPI_Init(void);
uchar SPI_WriteRead(uchar dat);


uchar SPI_Write1(SPI_PCSn_e pcs,uchar add,uchar dat);
uchar SPI_Read1(SPI_PCSn_e pcs,uchar add);
uchar SPI_WriteN(SPI_PCSn_e pcs,uchar add,uchar *dat,uchar N);
uchar SPI_ReadN(SPI_PCSn_e pcs,uchar add,uchar *dat,uchar N);
uchar SPI_Read_Buf(SPI_PCSn_e pcs,uchar reg, uchar *pBuf, uchar uchars);

#endif