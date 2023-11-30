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

#define NumofByte 1024

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

