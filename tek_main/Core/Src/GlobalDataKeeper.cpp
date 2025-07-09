/*
 * GlobalDataKeeper.cpp
 *
 *  Created on: Nov 28, 2023
 *      Author: 566kb523
 */

#include "GlobalDataKeeper.h"
#include <FLASH_SECTOR_F4.h>
#include "string.h"
#include "time.h"
#include "xmlParse.h"

#define NumofByte 1024
uint32_t num_param_addr=0x08040000;
uint32_t start_modem_adrr=0x08040000+sizeof(uint16_t);
uint32_t start_param_adrr=0x08040000+20*sizeof(ModemStruct)+sizeof(uint16_t);

string broker_adress;

GlobalDataKeeper::GlobalDataKeeper() {

	serialNumber=123;
	//device;
	//modem;

}
void GlobalDataKeeper::create_map(){
	global_var.clear();
	global_var.insert(Mapa::value_type("serialNumber",serialNumber));
	//	global_var.insert(Mapa::value_type("device",device));
	//	global_var.insert(Mapa::value_type("modem",modem));


}
void GlobalDataKeeper::map_to_data(void)
{
	serialNumber = global_var["serialNumber"];
	//	modem = global_var["modem"];
	//	device = global_var["device"];

}

void GlobalDataKeeper::data_to_map(void)
{
	global_var["serialNumber"] = serialNumber;
	//	global_var["modem"] = modem;
	//	global_var["device"] = device;

}


void GlobalDataKeeper::saveData()
{


	//cost1:0;cost2:0;cost3:0;cost4:0;cost5:0;price cost 1:10;price cost 2:10;price cost 3:10;price ob1:50;price ob2:100;price ob3:189;rashodeomer:30;ust1:50;ust2:100;ust3:189;
	data_to_map();

	//	char arr[] = { 'J', 'O', 'U', 'R', 'N', 'A', 'L', 'D', 'E', 'V' };

	std::string dataToFlash_str;
	//std::string add_str = key;


	//UARTPrint("dataToFlash_str save:\r\n");
	//UARTPrint("%s\r\n",dataToFlash_str.c_str());

	for (auto const& x : global_var)
	{
		dataToFlash_str.append(x.first);
		dataToFlash_str.append(";");
		dataToFlash_str.append(std::to_string(x.second));
		dataToFlash_str.append(";");
		//UARTPrint("dataToFlash_str save:\r\n");
		//UARTPrint("%s\r\n",dataToFlash_str.c_str());
	}
	//UARTPrint("GlobalDataKeeper save!\r\n");
	//UARTPrint("%s\r\n",dataToFlash_str.c_str());



	uint8_t dataWR[NumofByte];
	strcpy((char*)dataWR,dataToFlash_str.c_str());



	//запись во внутренню флеш
	numofwords = (dataToFlash_str.size()/4)+((dataToFlash_str.size()%4)!=0); //расчет количества слов в памяти
	uint32_t flashError = Flash_Write_Data(0x08070000 , (uint32_t *)dataToFlash_str.c_str(), numofwords);
	//UARTPrint("flash error code = %u \r\n",flashError);
	// пишем по адресу 0x08070000, потому что вторая банка памяти в режиме защиты от записи
}
void GlobalDataKeeper::readData()
{

	char readFlash_str[1024];

	//чтение из внутренний flash памяти
	uint32_t readFlash[1024];
	Flash_Read_Data(0x08070000 , readFlash, numofwords); //TODO правильно определять numofwords
	Convert_To_Str(readFlash, readFlash_str);

	//*********************************


	if(strstr(readFlash_str,";") != NULL)
	{
		//UARTPrint("GlobalDataKeeper read OK:\r\n");

		//парсим данные из строки в мапу
		char * p;
		//		key = strtok(readFlash_str, ";");//ключ активации лежит первым в памяти
		//		//std::string val = p;
		//		//strcpy(key, val.c_str());
		//		url_Telemetry = strtok(NULL, ";");
		//		UARTPrint("key: %s \n",key.c_str());
		//		UARTPrint("url: %s \n",url_Telemetry.c_str());
		//
		//		p = strtok(NULL, ";");
		while (p != NULL) {
			//UARTPrint("%s\r\n",p);
			std::string dataName = p;
			p = strtok(NULL, ";");
			std::string val = p;
			p = strtok(NULL, ";");
			int value = stoi(val);
			global_var[dataName] = value;
			if(dataName.compare("ust3") == 0)
			{
				break;
			}
		}
	}
	else
	{
		//UARTPrint("GlobalDataKeeper Fail, system default:\r\n");
		//key[0] = 'k'; key[1] = 'e'; key[2] = 'y'; key[3] = '='; key[4] = '1';
	}

	for (auto const& x : global_var)
	{
		//UARTPrint("%s %d\r\n",x.first.c_str(),x.second);
	}
	map_to_data();
}

uint32_t GetSector1(uint32_t Address)
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


uint8_t FlashErase(uint32_t addr)
{
	FLASH_EraseInitTypeDef FlashErase;
	uint32_t sectorError = 0;
	__disable_irq();
	HAL_FLASH_Unlock();
	FlashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
	FlashErase.NbSectors = 1;
	FlashErase.Sector = GetSector1(addr);
	FlashErase.VoltageRange = VOLTAGE_RANGE_1;
	if (HAL_FLASHEx_Erase(&FlashErase, &sectorError) != HAL_OK)
	{
		HAL_FLASH_Lock();
		__enable_irq();
		return HAL_ERROR;
	}
	__enable_irq();
	return HAL_OK;
}

uint8_t Write_Flash_Param(uint32_t addr,DeviceStruct* device)
{
	HAL_StatusTypeDef status;
	uint32_t structureSize = sizeof( DeviceStruct);
	__disable_irq();
	HAL_FLASH_Unlock();
	//	FlashErase(addr);
	uint32_t *dataPtr = (uint32_t *)device;
	for (uint8_t i = 0; i < structureSize; i++)
	{
		uint8_t test=dataPtr[i];
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, device->inf[i]);
		addr++;
	}
	__enable_irq();
	addr=start_param_adrr;
	DeviceStruct *test=(DeviceStruct*)(0x08040000+20*sizeof(ModemStruct)+sizeof(uint16_t));
//	uint8_t *readdataPtr = (uint8_t *)&test;
//	for (int i = 0; i < structureSize; i++)
//	{
//		readdataPtr[i] = *(__IO uint32_t*)addr;
//		addr++;
//	}

	//		if(test==mdm)
	//		{
	//			HAL_Delay(10);
	//		}

	start_param_adrr+=sizeof(DeviceStruct);
	return status;
}

uint8_t Write_Flash_Modem(uint32_t addr, ModemStruct* mdm)
{

	HAL_StatusTypeDef status;
	uint32_t structureSize = sizeof( ModemStruct);
	__disable_irq();
	HAL_FLASH_Unlock();
	//	FlashErase(addr);
	uint32_t *dataPtr = (uint32_t *)mdm;
	for (uint8_t i = 0; i < structureSize; i++)
	{
		uint8_t test=dataPtr[i];
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, mdm->inf[i]);
		addr++;
	}
	__enable_irq();
	addr=start_modem_adrr;
	ModemStruct *test=(ModemStruct*)(0x08040000+sizeof(uint16_t));
	//test[0].
//	ModemStruct test2;
//	uint8_t *readdataPtr = (uint8_t *)&test;
//	for (int i = 0; i < structureSize; i++)
//	{
//		readdataPtr[i] = *(__IO uint32_t*)addr;
//		addr++;
//	}

	//		if(test==mdm)
	//		{
	//			HAL_Delay(10);
	//		}

	start_modem_adrr+=sizeof( ModemStruct);
	return status;
}

DeviceStruct Read_Flash_Param (uint32_t addr)
{
	DeviceStruct dev;
	uint32_t structureSize = sizeof(DeviceStruct);
	uint32_t *dataPtr = (uint32_t *)&dev;
	for (int i = 0; i < structureSize / 4; i++)
	{
		dataPtr[i] = *(__IO uint32_t*)addr;
		addr += 4;
	}
}


ModemStruct Read_Flash_Modem (uint32_t addr)
{
	ModemStruct dev;
	uint32_t structureSize = sizeof(ModemStruct);
	uint32_t *dataPtr = (uint32_t *)&dev;
	for (int i = 0; i < structureSize / 4; i++)
	{
		dataPtr[i] = *(__IO uint32_t*)addr;
		addr += 4;
	}
}
