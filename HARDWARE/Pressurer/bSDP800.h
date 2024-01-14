#ifndef _SDP800_H_
#define _SDP800_H_

/*
 * slection between arm or 51 for const data 
 */
#ifdef ARM_PLATEFORM
#define CONST_DATA  const
#define ___NOP()     __asm("NOP")
#else
#define CONST_DATA  code
#define ___NOP()     _nop_()
#endif

/*
 * slection platfrom header files 
 */
#if defined(STM32F10X_HD) || defined(GD32F30X_HD)
    #include "sys.h"
    #include "delay.h"
    #include "HwDrv.h"
#elif defined(STM32F10X_MD)
    #include "ZMD_Includes.h"
#elif defined(RT1052)
    #include "fsl_common.h"
    #include "fsl_snvs_hp.h"
#elif defined(LKS32MC08x)
    #include "basic.h"
    #include "hardware_config.h"
#else
    #include "common.h"
#endif


#define SDP800_500_ADDRW    (0x25 << 1)
#define SDP800_500_ADDRR    (0x25 << 1) + 1

#define CONTINUOUSMODE1     0x3603
#define CONTINUOUSMODE2     0x3608
#define CONTINUOUSMODE3     0x3615
#define CONTINUOUSMODE4     0x361E
#define CONTINUOUSMODESTOP  0x3ff9

void SDP800_TEST(uint8_t ucX);
#endif
