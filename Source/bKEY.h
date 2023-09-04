
#ifndef _bkey_h_
#define _bkey_h_

#ifdef C51_PLATEFORM
#include "common.h"
#elif defined(GD32F30X_HD)
#include "sys.h"
#include "HwDrv.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#else
#error "NO PLEATEFORM DEFINED!"
#endif


#if	defined(IAP15W413ASDIP28)
sbit INDEPKEY1 = P2^5;
sbit INDEPKEY2 = P2^4;
sbit INDEPKEY3 = P2^3;
sbit INDEPKEY4 = P2^2;
#elif (defined(IAP15W413ASSOP20) || defined(STC15W408ASDIP16))
sbit INDEPKEY1 = P1^1;
sbit INDEPKEY2 = P1^0;
sbit INDEPKEY3 = P3^7;
sbit INDEPKEY4 = P3^6;
#elif defined(GD32F30X_HD)
#define  INDEPKEY1  PCin(0)
#define  INDEPKEY2  PCin(1)
#define  INDEPKEY3  PCin(2)
#define  INDEPKEY4  PCin(3)
#define  EC11_A_R   PAin(0)
#define  EC11_B_R   PAin(1)

#else 
#error NO PLEATEFORM DEFINED!
#endif

enum keyn_e {
    KEY_NONE=0, 
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_CANCEL,
    EC11_CW,
    EC11_CCW,
    EC11_SET,
    KEY_MAX,
};
typedef enum keyn_e KEYn_e;

extern INT16S GsEc11CntCW, GsEc11CntCCW;



void keyInit(void);
void ec11Init(void);

KEYn_e ADKey_Scan(void);
KEYn_e ADKey_Check(void);
KEYn_e ADKey_Check2(void);

extern INT16S GsEc11SetV;
void ec11Scan(INT8U bActionFlg, INT8U ucStepNum);


#endif

