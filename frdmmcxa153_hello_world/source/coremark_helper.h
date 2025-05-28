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

/* Structure to save data in non-volatile memory*/
typedef struct
{
	uint8_t 	StartMsg[8];
	uint32_t 	Temperature;
	uint32_t 	CoreMarkSize;
	uint32_t 	TotalTicks;
	uint32_t 	TotalTime;
	uint32_t	IterationsPerSec;
	uint32_t	Iterations;
	uint16_t 	SeedCrc;
	uint16_t	CrcList;
	uint16_t	CrcMatrix;
	uint16_t 	CrcState;
	uint16_t 	CrcFinal;
	uint16_t 	NumErrors;
	uint8_t		EndMsg[4];
}RESULTS_FLASH;
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
void CMHELPER_RunCoremark(RESULTS_FLASH* test_results);

#endif // __COREMARK_HELPER__
