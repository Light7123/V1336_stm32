/*
 * xmlParse.h
 *
 *  Created on: Nov 28, 2023
 *      Author: 566kb523
 */

#ifndef INC_XMLPARSE_H_
#define INC_XMLPARSE_H_
#include <iostream>
#include <regex>

struct DeviceParameters {
	int updateTime;
	std::string group;
	int writeFunc;
	bool subscre;
	std::string dataType;
	std::string alias;
	int readFunc;
	int registerCount;
};

typedef struct
{
	//regaddres, func, pub_delay
	union{
		uint16_t regaddres;
		struct{
			uint8_t regaddresLO;
			uint8_t regaddresHI;
		};
	};
	uint8_t write_func;
	uint8_t func;
	std::string res="wait";
	std::string old_res;
	bool pub=true;
	bool filtred=false;
	uint16_t old_modbud;
	uint8_t fil_count=0;
	uint16_t pub_delay;
	uint32_t last_pub;
	uint16_t group;
	uint8_t type;

} test;

typedef struct
{
	//regaddres, func, pub_delay
	union{
		uint16_t regaddres;
		struct{
			uint8_t regaddresLO;
			uint8_t regaddresHI;
		};
	};
	uint8_t func;
	uint16_t pub_delay;
	uint16_t group;

} dev;

typedef struct
{
	//regaddres, func, pub_delay
	char alias[30];
	uint16_t pub_delay;

} modem1;

typedef struct
{
	//regaddres, func, pub_delay
	uint16_t keep_live;
	char user[30];
	char pass[30];
	char url[50];
	uint16_t speed;
	char config_date[30];

} modbus_conf;

extern test device[733];
extern uint32_t su_time;
//extern dev dv_conf;
//extern modbus_conf mb_conf;
//extern modem mo_conf;
void parseConfigString(std::string config);
typedef struct {
	unsigned short BrokerKeepalive;
	char GUID[16];
	int BottomNetworkSpeed;
	char User[16];
	char Password[16];
	char BrokerAddress[32];
	char ConfigDateAndTime[32];
} ParametersStruct;
#pragma pack(push, 1)
typedef struct {
	union{
		char inf[24];
		struct
		{
			unsigned int update_time_sec;
			char alias[20];
		};
	};
} ModemStruct;

typedef struct {
	union
	{
		char inf[40];
		struct
		{
			char Device;
			unsigned int update_time_sec;
			unsigned short group;
			uint8_t data_type;
			unsigned short address;
			uint8_t  read_func;
		};
	};
} DeviceStruct;
#pragma pack(pop)
int Run(char byte, ParametersStruct* param, ModemStruct* mdm, DeviceStruct* device, bool Clean);
int GetDataType(const char* value, DeviceStruct* device);
void ReadFile(char* f);
#endif /* INC_XMLPARSE_H_ */
