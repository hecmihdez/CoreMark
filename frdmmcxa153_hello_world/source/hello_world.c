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

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
signed short total_errors = 0;
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    char ch;

    /* Init board hardware. */
    BOARD_InitHardware();

    PRINTF("MCXA153 Coremark.");

//    while (1)
//    {
//        ch = GETCHAR();
//        PUTCHAR(ch);

//        CMHELPER_RunCoremark();
//    }
        while(1)
        {

        	CMHELPER_RunCoremark();

        	PRINTF("LISTO\r\n");
        	ch = GETCHAR();
			PUTCHAR(ch);
        }
}
