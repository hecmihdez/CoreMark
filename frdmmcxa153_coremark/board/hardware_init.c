/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "fsl_clock.h"
#include "fsl_reset.h"
#include "board.h"
#include <stdbool.h>
/*${header:end}*/

/*${function:start}*/
void BOARD_InitHardware(void)
{
    /* Release peripheral reset */
    RESET_ReleasePeripheralReset(kADC0_RST_SHIFT_RSTn);

    /* Attach peripheral clock */
    CLOCK_SetClockDiv(kCLOCK_DivADC0, 1u);
    CLOCK_AttachClk(kFRO12M_to_ADC0);

    CLOCK_SetClockDiv(kCLOCK_DivLPTMR0, 1u);
    CLOCK_AttachClk(kFRO12M_to_LPTMR0);

	CLOCK_AttachClk(kCLK_1M_to_OSTIMER);

	CLOCK_SetupFRO16KClocking(kCLKE_16K_SYSTEM | kCLKE_16K_COREMAIN);

	BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
}
/*${function:end}*/
