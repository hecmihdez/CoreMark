/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"
#include "coremark_helper.h"
#include "temperature.h"
#include "Flash_data.h"
#include "fsl_lptmr.h"
#include "MCXA153_benchmark_cfg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LPTMR_BASE    		 LPTMR0
#define LPTMR_IRQn    		 LPTMR0_IRQn
#define LPTMR_TIMER_HANDLER  LPTMR0_IRQHandler

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static RESULTS_FLASH CoreMark_Results = {"**START*",0,0,0,0,0,0,0,0,0,0,0,0,"END*"};
static bool CoreMark_RUN = false;
static int8_t saveFlag = 0;
signed short total_errors = 0;
/*******************************************************************************
 * Code
 ******************************************************************************/

void LPTMR_TIMER_HANDLER(void)
{
    LPTMR_ClearStatusFlags(LPTMR_BASE, kLPTMR_TimerCompareFlag);
    CoreMark_RUN = true;
}


/*!
 * @brief Main function
 */
int main(void)
{
    lptmr_config_t lptmrConfig;

    /* Init board hardware. */
    BOARD_InitHardware();

    FlashData_vInit();

#if CLEAN_MEMORY
    FlashData_vEraseSector();
    while(1);
#else

    temperature_init();

    PRINTF("MCXA153 Coremark.\r\n");

    /* Configure LPTMR */
    /*
     * lptmrConfig.timerMode = kLPTMR_TimerModeTimeCounter;
     * lptmrConfig.pinSelect = kLPTMR_PinSelectInput_0;
     * lptmrConfig.pinPolarity = kLPTMR_PinPolarityActiveHigh;
     * lptmrConfig.enableFreeRunning = false;
     * lptmrConfig.bypassPrescaler = true;
     * lptmrConfig.prescalerClockSource = kLPTMR_PrescalerClock_1;
     * lptmrConfig.value = kLPTMR_Prescale_Glitch_0;
     */
    /* Note: the input clock source for prescaler clock must be enabled and attached in advance with configuration in SYSCON or SCG */
    LPTMR_GetDefaultConfig(&lptmrConfig);

    lptmrConfig.bypassPrescaler = false;
    lptmrConfig.value = kLPTMR_Prescale_Glitch_13;

    /* Initialize the LPTMR */
    LPTMR_Init(LPTMR_BASE, &lptmrConfig);

    /* Set timer period.
     * Note : the parameter "ticks" of LPTMR_SetTimerPeriod should be equal or greater than 1.
     */
    LPTMR_SetTimerPeriod(LPTMR_BASE, EXECUTION_TIME_FREQ);

    /* Enable timer interrupt */
    LPTMR_EnableInterrupts(LPTMR_BASE, kLPTMR_TimerInterruptEnable);

    /* Enable at the NVIC */
    EnableIRQ(LPTMR_IRQn);

    LPTMR_StartTimer(LPTMR_BASE);

	while(1)
	{
		if((CoreMark_RUN) && (saveFlag == 0))
		{
			GPIO_PinWrite(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, LOGIC_LED_OFF);
			GPIO_PinWrite(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_GPIO_PIN, LOGIC_LED_ON);
			PRINTF("Ejecutando\r\n");

			(void)temperature_getValue();
			CMHELPER_RunCoremark(&CoreMark_Results);
			CoreMark_Results.Temperature = temperature_getValue();

			saveFlag = FlashData_u8WriteData((uint8_t *)&CoreMark_Results, sizeof(CoreMark_Results));

			CoreMark_RUN = false;

			PRINTF("Prueba finalizada\r\n");
		}
		else if (saveFlag != 0)
		{
			PRINTF("Memoria Llena\r\n");
			GPIO_PinWrite(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_GPIO_PIN, LOGIC_LED_OFF);
			GPIO_PinWrite(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, LOGIC_LED_ON);
		}
	}
#endif
}
