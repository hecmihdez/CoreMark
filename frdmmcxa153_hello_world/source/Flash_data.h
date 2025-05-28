/*
 * Flash_Data.h
 *
 *  Created on: 11 may. 2025
 *      Author: Sergio R
 */

#ifndef FLASH_DATA_H_
#define FLASH_DATA_H_

extern void FlashData_vInit(void);
extern void FlashData_vEraseSector(void);
extern uint8_t FlashData_u8WriteData(uint8_t* Buffer, uint16_t lenght);
extern void FlashData_vReadData(uint8_t* buffer, uint16_t lenght, uint32_t address);

#endif /* FLASH_DATA_H_ */
