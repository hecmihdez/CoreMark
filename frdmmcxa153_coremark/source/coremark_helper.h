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
	uint8_t 	StartMsg[8];			/*Caracteres para indicar el inicio de una prueba en memoria.*/
	uint32_t 	Temperature;			/*Lectura del sensor de temperatura de la tarjeta.*/
	uint32_t 	CoreMarkSize;			/*Tamaño de los datos de la prueba.*/
	uint32_t 	TotalTicks;				/*Total de ticks de ejecución de la prueba.*/
	uint32_t 	TotalTime;				/*Total de segundos de ejecución de la prueba.*/
	uint32_t	IterationsPerSec;		/*Iteraciones por segundo.*/
	uint32_t	Iterations;				/*Número de iteraciones de ejecución.*/
	uint16_t 	SeedCrc;
	uint16_t	CrcList;
	uint16_t	CrcMatrix;
	uint16_t 	CrcState;
	uint16_t 	CrcFinal;
	uint16_t 	NumErrors;
	uint8_t		EndMsg[4];				/*Caracteres para indicar el fin de una prueba en memoria.*/
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
