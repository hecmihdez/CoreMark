#include "coremark_helper.h"
#include "fsl_device_registers.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_ostimer.h"
//#include "fsl_tstmr.h"

/*******************************************************************************
* Definitions
*******************************************************************************/
/* The timer used for coremark */
#define CMHELPER_TIMER_BASE            OSTIMER0
#define CMHELPER_TIMER_NUM             0

#define OSTIMER_CLOCK_FREQUENCY_MHZ		(1U)

/* The UART used for debug console */
#define CMHELPER_DEBUG_UART_TYPE       kSerialPort_Uart
#define CMHELPER_DEBUG_UART_INSTANCE   0U
#define CMHELPER_DEBUG_UART_CLK_FREQ   CLOCK_GetFlexcommClkFreq(CMHELPER_DEBUG_UART_INSTANCE)
#define CMHELPER_DEBUG_UART_CLK_ATTACH kFRO48M_to_FLEXCOMM0
#define CMHELPER_DEBUG_UART_BAUDRATE   115200

/*******************************************************************************
* Local Prototypes
*******************************************************************************/
extern void coremark_main(void);

#if (defined(POWER_MEASURE_EN) && (POWER_MEASURE_EN==0))
static void CMHELPER_TimerInit(void);
static void CMHELPER_PrintDebugInfo(void);
#endif  // (POWER_MEASURE_EN==0)
/*******************************************************************************
* Variables
*******************************************************************************/

/*******************************************************************************
* Code
*******************************************************************************/
void SystemPerfMonInit(void)
{  
  
#ifdef MCXN
  /* Reset all the counters */
  CMX_PERFMON0->PMCR[0].PMCR |= (SYSPM_PMCR_RECTR3_MASK | SYSPM_PMCR_RECTR2_MASK | SYSPM_PMCR_RECTR1_MASK);
  
  CMX_PERFMON0->PMCR[0].PMCR = (SYSPM_PMCR_SELEVT3(0x40)  //cac_if - cache instruction fetch
      | SYSPM_PMCR_SELEVT2(0x44)                //cac_if_stalls - stalled cached fetch
      | SYSPM_PMCR_SELEVT1(0x50)  );              //cac_if_miss - cache miss
  
  CMX_PERFMON0->PMCR[0].PMCR |= SYSPM_PMCR_SSC(2);                       // start the counters
#endif
}


void CMHELPER_RunCoremark(void) {    

      /* Core Mark Info */
      CMHELPER_PrintDebugInfo();
  
#if (defined(POWER_MEASURE_EN) && (POWER_MEASURE_EN==0))
        /* Start timer for coremark */
        CMHELPER_TimerInit();
#endif  // (POWER_MEASURE_EN==0)
        coremark_main();

}


#if (defined(POWER_MEASURE_EN) && (POWER_MEASURE_EN==0))
/*
 * Configures the timer for coremark.
 */
static void CMHELPER_TimerInit(void) {

      /* Board pin, clock, debug console init */
//     CLOCK_EnableClock(kCLOCK_Tstmr0);
	OSTIMER_Init(CMHELPER_TIMER_BASE);
}

/*
 * Print debug information regarding MCU settings.
 */
static void CMHELPER_PrintDebugInfo(void) {

    
    PRINTF("\r\nStarting CoreMark.\r\n");

    /*@ToDo: Print the debug info like memory */
}

#endif // (POWER_MEASURE_EN==0)

/* 
 * Gets raw timer count (number of ticks).
 */
uint32_t CMHELPER_TimerGetCount(void) {
#if (defined(POWER_MEASURE_EN) && (POWER_MEASURE_EN==0))
//    return (uint32_t)(TSTMR_ReadTimeStamp(CMHELPER_TIMER_BASE));
    return (uint32_t)(OSTIMER_GetCurrentTimerValue(CMHELPER_TIMER_BASE));
#else
    return 0;
#endif  // (POWER_MEASURE_EN==0)
}

/* 
 * Get the frequency of the Timer clock.
 */
uint32_t CMHELPER_TimerGetClockFreq(void) {
#if (defined(POWER_MEASURE_EN) && (POWER_MEASURE_EN==0))
    return (uint32_t)(OSTIMER_CLOCK_FREQUENCY_MHZ*1000000);//Multiply to 1000000 to obtain the frequency in Hz
#else
    return 1;
#endif  // (POWER_MEASURE_EN==0)
}

#if (defined(POWER_MEASURE_EN) && (POWER_MEASURE_EN==1))
/*
 * This is here so that code can compile and avoid hard faults
 * when debug console is disabled.
 */
int ee_printf(const char *fmt, ...) {
    return 0;
}
#endif  // (POWER_MEASURE_EN==1)
