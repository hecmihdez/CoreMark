/*
 * Flash_data.c
 *
 *  Created on: 11 may. 2025
 *      Author: Sergio R
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_debug_console.h"
#include "fsl_romapi.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FLASH_USER_START  		(0x0000C010)
#define FLASH_USER_END   		(0x0001FFFF)

#define FLASH_16BYTE_LENGTH		(16U)

#define FLASH_ERASE_START 		(0x0000C000)
#define FLASH_ERASE_FLAG		(0xC52)
/*******************************************************************************
 * types
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Flash driver Structure */
static flash_config_t s_flashDriver;
static uint32_t FlashData_s_CurrentAddress = FLASH_USER_START;
/*******************************************************************************
 * Code
 ******************************************************************************/
/*
 * @brief Gets called when an error occurs.
 *
 * @details Print error message and trap forever.
 */
static void error_trap(void)
{
    PRINTF("\r\n\r\n\r\n\t---- HALTED DUE TO FLASH ERROR! ----");
    while (1)
    {
    }
}

static bool FlashData_s_bValidateAdrss(uint32_t address, uint32_t length)
{
    return (address >= FLASH_USER_START) && ((address + length) <= FLASH_USER_END);
}

static uint16_t FlashData_s_u16GetEraseState(void)
{
	uint32_t State = 0U;

	FLASH_API->flash_read(&s_flashDriver, FLASH_ERASE_START, (uint8_t *)&State, sizeof(State));

	return State;
}

static void FlashData_s_vEraseSector(void)
{
	status_t status;
	uint32_t erase_flag = FLASH_ERASE_FLAG;

	FLASH_API->flash_erase_sector(&s_flashDriver, FLASH_ERASE_START, 81920, kFLASH_ApiEraseKey);
	status = FLASH_API->flash_verify_erase_sector(&s_flashDriver, FLASH_ERASE_START, 81920);
	if(status != kStatus_Success)
	{
		error_trap();
	}

	FLASH_API->flash_program_phrase(&s_flashDriver, FLASH_ERASE_START, (uint8_t *)&erase_flag, 4);
	status = FLASH_API->flash_verify_program(&s_flashDriver, FLASH_ERASE_START, 4, (const uint8_t *)&erase_flag, NULL, NULL);
	if(status != kStatus_Success)
	{
		error_trap();
	}
}

static bool FlashData_bCheckPage(uint32_t address, uint16_t lenght)
{
	uint32_t data = 0U;
	uint8_t index = 0U;
	bool dataFlag = false;

	while(index < lenght / 4)
	{
		FLASH_API->flash_read(&s_flashDriver, address, (uint8_t *)&data, sizeof(data));
		if(data != 0xFFFFFFFF)
		{
			dataFlag = true;
			break;
		}

		index++;
		address += sizeof(data);
	}

	return dataFlag;
}

void FlashData_vInit(void)
{
	uint16_t erase_state = 0U;
	/* Clean up Flash, Cache driver Structure*/
	memset(&s_flashDriver, 0, sizeof(flash_config_t));

	/* Initialize flash driver */
	if (FLASH_API->flash_init(&s_flashDriver) != kStatus_Success)
	{
		error_trap();
	}

	erase_state = FlashData_s_u16GetEraseState();

	if(erase_state != FLASH_ERASE_FLAG)
	{
		FlashData_s_vEraseSector();
	}
}

void FlashData_vEraseSector(void)
{
	FlashData_s_vEraseSector();
}

uint8_t FlashData_u8WriteData(uint8_t* Buffer, uint16_t lenght)
{
	status_t status = kStatus_FLASH_AddressError;
	bool DataFlag;

	lenght = (lenght + 15) & ~0x0F;
	while(FlashData_s_bValidateAdrss(FlashData_s_CurrentAddress, lenght))
	{
		if(!FlashData_bCheckPage(FlashData_s_CurrentAddress, lenght))
		{
			FLASH_API->flash_program_phrase(&s_flashDriver, FlashData_s_CurrentAddress, Buffer, lenght);
			status = FLASH_API->flash_verify_program(&s_flashDriver, FlashData_s_CurrentAddress, lenght, Buffer, NULL, NULL);
			if (status == kStatus_FLASH_Success)
			{
				FlashData_s_CurrentAddress += lenght;
				break;
			}
		}

		FlashData_s_CurrentAddress += lenght;
	}

	return status;
}

void FlashData_vReadData(uint8_t* buffer, uint16_t lenght, uint32_t address)
{
	FLASH_API->flash_read(&s_flashDriver, address, buffer, lenght);
}
