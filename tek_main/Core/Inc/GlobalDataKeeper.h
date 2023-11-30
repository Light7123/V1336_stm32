/*
 * GlobalDataKeeper.h
 *
 *  Created on: Nov 28, 2023
 *      Author: 566kb523
 */

#ifndef INC_GLOBALDATAKEEPER_H_
#define INC_GLOBALDATAKEEPER_H_

#include "map"
#include "string"
#include "main.h"
#include "stm32f4xx_hal.h"
#include <vector>
#include "time.h"
using namespace std;


typedef struct
{
	uint8_t address;
	uint16_t register_adress;
	int num_register;
	int pull_mode;
	int pull_interval;
	int data_type;
	string topic;
	string user;
	string password;
	string apn;
} device_conf;


typedef struct
{
	string name;
	string model;
	string imei;
	time_t time;

} modem_conf;

class GlobalDataKeeper {
public:
	GlobalDataKeeper(); 			// заполнение полей ниже дефолтными параметрами

	void create_map(void);			// создание карты для более удобных чтения/записи с флешки
	void map_to_data(void);
	void data_to_map(void);
//
	void saveData();
	void readData();

	// "Внешние" переменные. Их можно вводить, они выводятся на флешку
	int serialNumber;
//	vector<device_conf> device;
//	modem_conf modem;

	map <string, int> global_var;	// map для хранения пар "человекопонятное_название - переменная"

private:
	int numofwords;
};
typedef std::map<string, int> Mapa;




#endif /* INC_GLOBALDATAKEEPER_H_ */
