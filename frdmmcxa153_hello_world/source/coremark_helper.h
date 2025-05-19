#ifndef __COREMARK_HELPER__
#define __COREMARK_HELPER__

#include <stddef.h>
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/     
#define POWER_MEASURE_EN      0
#define ITERATIONS            2850

     
#if (defined(POWER_MEASURE_EN) && (POWER_MEASURE_EN==1))
    #define HAS_PRINTF        0
#else
    #define HAS_PRINTF        1
#endif
/*******************************************************************************
 * Variables
 ******************************************************************************/
     
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Gets raw timer count (number of ticks) */
uint32_t CMHELPER_TimerGetCount(void);

/* Get the frequency of the Timer clock*/
uint32_t CMHELPER_TimerGetClockFreq(void);

int ee_printf(const char *fmt, ...);

/* Run coremark*/
void CMHELPER_RunCoremark(void); 

#endif // __COREMARK_HELPER__
