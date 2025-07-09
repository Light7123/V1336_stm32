
/**
  ***************************************************************************************************************
  ***************************************************************************************************************
  ***************************************************************************************************************
  File:	      FLASH_SECTOR_F4.c
  Modifier:   ControllersTech.com
  Updated:    27th MAY 2021
  ***************************************************************************************************************
  Copyright (C) 2017 ControllersTech.com
  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.
  ***************************************************************************************************************
*/


#include "FLASH_SECTOR_F4.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
#include "xmlParse.h"
#define flashADDR   0x0801F800


/* DEFINE the SECTORS according to your reference manual
 * STM32F446RE have:-
 *  Sector 0 to Sector 3 each 16KB
 *  Sector 4 as 64KB
 *  Sector 5 to Sector 7 each 128KB
 */

static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < 0x08003FFF) && (Address >= 0x08000000))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < 0x08007FFF) && (Address >= 0x08004000))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < 0x0800BFFF) && (Address >= 0x08008000))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < 0x0800FFFF) && (Address >= 0x0800C000))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < 0x0801FFFF) && (Address >= 0x08010000))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address < 0x0803FFFF) && (Address >= 0x08020000))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address < 0x0805FFFF) && (Address >= 0x08040000))
  {
    sector = FLASH_SECTOR_6;
  }
  else if((Address < 0x0807FFFF) && (Address >= 0x08060000))
  {
    sector = FLASH_SECTOR_7;
  }
  else if((Address < 0x0809FFFF) && (Address >= 0x08080000))
  {
    sector = FLASH_SECTOR_8;
  }
  else if((Address < 0x080BFFFF) && (Address >= 0x080A0000))
  {
    sector = FLASH_SECTOR_9;
  }
  else if((Address < 0x080DFFFF) && (Address >= 0x080C0000))
  {
    sector = FLASH_SECTOR_10;
  }
  else if((Address < 0x080FFFFF) && (Address >= 0x080E0000))
  {
    sector = FLASH_SECTOR_11;
  }
  return sector;
}







uint8_t bytes_temp[4];


void float2Bytes(uint8_t * ftoa_bytes_temp,float float_variable)
{
    union {
      float a;
      uint8_t bytes[4];
    } thing;

    thing.a = float_variable;

    for (uint8_t i = 0; i < 4; i++) {
      ftoa_bytes_temp[i] = thing.bytes[i];
    }

}

float Bytes2float(uint8_t * ftoa_bytes_temp)
{
    union {
      float a;
      uint8_t bytes[4];
    } thing;

    for (uint8_t i = 0; i < 4; i++) {
    	thing.bytes[i] = ftoa_bytes_temp[i];
    }

   float float_variable =  thing.a;
   return float_variable;
}


uint32_t Flash_Write_Data (uint32_t StartSectorAddress, uint32_t *Data, uint16_t numberofwords)
{

	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError;
	int sofar=0;
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGAERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGPERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
	 /* Unlock the Flash to enable the flash control register access *************/
	  HAL_FLASH_Unlock();

	  /* Erase the user Flash area */

	  /* Get the number of sector to erase from 1st sector */

	  uint32_t StartSector = GetSector(StartSectorAddress);
	  uint32_t EndSectorAddress = StartSectorAddress + numberofwords*4;
	  uint32_t EndSector = GetSector(EndSectorAddress);

	  /* Fill EraseInit structure*/
	  EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	  EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	  EraseInitStruct.Sector        = StartSector;
	  EraseInitStruct.NbSectors     = (EndSector - StartSector) + 1;

	  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	     you have to make sure that these data are rewritten before they are accessed during code
	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	     DCRST and ICRST bits in the FLASH_CR register. */
	  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	  {
		 // UARTPrint("Flash erase error\r\n");
		  return HAL_FLASH_GetError ();
	  }

	  /* Program the user Flash area word by word
	    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	   while (sofar<numberofwords)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartSectorAddress, Data[sofar]) == HAL_OK)
	     {
	    	 StartSectorAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
	    	 sofar++;
	     }
	     else
	     {
	       /* Error occurred while writing data in Flash memory*/
	    	// UARTPrint("Flash program error\r\n");
	    	 return HAL_FLASH_GetError ();
	     }
	   }

	  /* Lock the Flash to disable the flash control register access (recommended
	     to protect the FLASH memory against possible unwanted operation) *********/
	  HAL_FLASH_Lock();

	   return 0;
}


void Flash_Read_Data (uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	while (1)
	{

		*RxBuf = *(__IO uint32_t *)StartSectorAddress;
		StartSectorAddress += 4;
		RxBuf++;
		if (!(numberofwords--)) break;
	}
}

void Convert_To_Str (uint32_t *Data, char *Buf)
{
	int numberofbytes = ((strlen((char *)Data)/4) + ((strlen((char *)Data) % 4) != 0)) *4;

	for (int i=0; i<numberofbytes; i++)
	{
		Buf[i] = Data[i/4]>>(8*(i%4));
	}
}


void Flash_Write_NUM (uint32_t StartSectorAddress, float Num)
{

	float2Bytes(bytes_temp, Num);

	Flash_Write_Data (StartSectorAddress, (uint32_t *)bytes_temp, 1);
}


float Flash_Read_NUM (uint32_t StartSectorAddress)
{
	uint8_t buffer[4];
	float value;

	Flash_Read_Data(StartSectorAddress, (uint32_t *)buffer, 1);
	value = Bytes2float(buffer);
	return value;
}
//
//uint8_t writeFlash_device (uint32_t addr,dev data)
//{
//
//	HAL_StatusTypeDef status;
//	uint32_t structureSize = sizeof(dev);
//	FLASH_EraseInitTypeDef FlashErase;
//	uint32_t sectorError = 0;
//
//	__disable_irq();
//	HAL_FLASH_Unlock();
//
//	FlashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
//	FlashErase.NbSectors = 1;
//	FlashErase.Sector = GetSector(addr);
//	FlashErase.VoltageRange = VOLTAGE_RANGE_3;
//	if (HAL_FLASHEx_Erase(&FlashErase, &sectorError) != HAL_OK)
//	{
//		HAL_FLASH_Lock();
//        __enable_irq();
//		return HAL_ERROR;
//	}
//	uint32_t *dataPtr = (uint32_t *)&data;
//	for (uint8_t i = 0; i < structureSize / 4; i++)
//	{
//		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, dataPtr[i]);
//		addr += 4;
//	}
//	__enable_irq();
//	return status;
//}
//
//uint8_t writeFlash_modem (uint32_t addr,modem data)
//{
//
//	HAL_StatusTypeDef status;
//	uint32_t structureSize = sizeof(modem);
//	FLASH_EraseInitTypeDef FlashErase;
//	uint32_t sectorError = 0;
//
//	__disable_irq();
//	HAL_FLASH_Unlock();
//
//	FlashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
//	FlashErase.NbSectors = 1;
//	FlashErase.Sector = GetSector(addr);
//	FlashErase.VoltageRange = VOLTAGE_RANGE_3;
//	if (HAL_FLASHEx_Erase(&FlashErase, &sectorError) != HAL_OK)
//	{
//		HAL_FLASH_Lock();
//        __enable_irq();
//		return HAL_ERROR;
//	}
//	uint32_t *dataPtr = (uint32_t *)&data;
//	for (uint8_t i = 0; i < structureSize / 4; i++)
//	{
//		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, dataPtr[i]);
//		addr += 4;
//	}
//	__enable_irq();
//	return status;
//}
//
//uint8_t writeFlash_modbus (uint32_t addr,modbus_conf data)
//{
//
//	HAL_StatusTypeDef status;
//	uint32_t structureSize = sizeof(modbus_conf);
//	FLASH_EraseInitTypeDef FlashErase;
//	uint32_t sectorError = 0;
//
//	__disable_irq();
//	HAL_FLASH_Unlock();
//
//	FlashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
//	FlashErase.NbSectors = 1;
//	FlashErase.Sector = GetSector(addr);
//	FlashErase.VoltageRange = VOLTAGE_RANGE_3;
//	if (HAL_FLASHEx_Erase(&FlashErase, &sectorError) != HAL_OK)
//	{
//		HAL_FLASH_Lock();
//        __enable_irq();
//		return HAL_ERROR;
//	}
//	uint32_t *dataPtr = (uint32_t *)&data;
//	for (uint8_t i = 0; i < structureSize / 4; i++)
//	{
//		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, dataPtr[i]);
//		addr += 4;
//	}
//	__enable_irq();
//	return status;
//}
//
//void readFlash_dev (uint32_t addr)
//{
//	uint32_t structureSize = sizeof(dev);
//	uint32_t *dataPtr = (uint32_t*) &dv_conf;
//	for (int i = 0; i < structureSize / 4; i++)
//	{
//		dataPtr[i] = *(__IO uint32_t*)addr;
//		addr += 4;
//	}
//}
//
//void readFlash_modem(uint32_t addr)
//{
//	uint32_t structureSize = sizeof(modem);
//	uint32_t *dataPtr = (uint32_t*) &mo_conf;
//	for (int i = 0; i < structureSize / 4; i++)
//	{
//		dataPtr[i] = *(__IO uint32_t*)addr;
//		addr += 4;
//	}
//}
//
//void readFlash_modbus (uint32_t addr)
//{
//	uint32_t structureSize = sizeof(modbus_conf);
//	uint32_t *dataPtr = (uint32_t*) &mb_conf;
//	for (int i = 0; i < structureSize / 4; i++)
//	{
//		dataPtr[i] = *(__IO uint32_t*)addr;
//		addr += 4;
//	}
//}

