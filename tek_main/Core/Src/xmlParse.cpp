#include <iostream>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "xmlParse.h"
#include "main.h"
#include "GlobalDataKeeper.h"


test device[733];
uint32_t su_time=0;
//dev dv_conf;
//modbus_conf mb_conf;
//modem mo_conf;
ParametersStruct parameters;
ModemStruct modem[10];
DeviceStruct variable[10];

bool configBegin = false;
bool paramBegin = false;
bool modemBegin = false;
bool devicesBegin = false;
bool deviceBegin = false;

int device_char;
char base_group[32];
unsigned short group_count = 0;

void ReadFile(char* f) {
	Run(0, NULL, NULL, NULL, 1);
	int variableindex = 0;
	int modemindex = 0;
	char ch;
	int i=0;
	while ((ch = f[i]) != EOF) {
		if(!configBegin&&ch=='\0')
		{
			ch='\t';
		}
		switch (Run(ch, &parameters, &modem[1], &variable[1], 0)) {
		case 2:

			modemindex++;
			i++;
			break;
		case 3:
			variableindex++;
			i++;
			break;
		default:
			i++;
			break;
		}
		if(i>=55000)
		{
			break;
		}
	}
}

void ReadParameters(char* str, ParametersStruct* param) {
	char value[32];
	if (strstr(str, "<BrokerKeepalive")) {
		char* start = strchr(str, '"') + 1;
		char* end = strrchr(str, '"');
		int len = end - start;
		if(len>2)
		{
		snprintf(value, sizeof(value), "%.*s", len, start);
		param->BrokerKeepalive = atoi(value);
		}
	}
	if (strstr(str, "<GUID")) {
		char* start = strchr(str, '"') + 1;
		char* end = strrchr(str, '"');
		int len = end - start;
		if(len>2)
		{
		strncpy(param->GUID,start,len);
		}
	}
	if (strstr(str, "<BottomNetworkSpeed")) {
		char* start = strchr(str, '"') + 1;
		char* end = strrchr(str, '"');
		int len = end - start;
		snprintf(value, sizeof(value), "%.*s", len, start);
		if(len>2)
		{
		param->BottomNetworkSpeed = atoi(value);
		}
	}
	if (strstr(str, "<User")) {
		char* start = strchr(str, '"') + 1;
		char* end = strrchr(str, '"');
		int len = end - start;
		{
		strncpy(param->User,  start, len);
		}
	}
	if (strstr(str, "<Password")) {
		char* start = strchr(str, '"') + 1;
		char* end = strrchr(str, '"');
		int len = end - start;
		if(len>2)
		{
		strncpy(param->Password, start, len);
		}
	}
	if (strstr(str, "<BrokerAddress")) {
		char* start = strchr(str, '"') + 1;
		char* end = strrchr(str, '"');
		int len = end - start;
		if(len>2)
		{
		strncpy(param->BrokerAddress,  start, len);
		}
	}
	if (strstr(str, "<ConfigDateAndTime")) {
		char* start = strchr(str, '"') + 1;
		char* end = strrchr(str, '"');
		int len = end - start;
		if(len>2)
		{
		strncpy(param->ConfigDateAndTime, start, len);
		}
	}
}

int ReadModem(const char* str, ModemStruct* mdm) {
	char value[32];
	if (strstr(str, "<Variable")) {
		char* alias = strchr(str, '/') + 1;
		char* end_alias;
		if (alias) {
			end_alias = strchr(alias, '"');
			if (end_alias) {
				strncpy(mdm->alias, alias, end_alias - alias);
				char* update = strchr(end_alias + 1, '"');
				char* end_update;
				if (update) {
					end_update = strchr(update + 1, '"');
					if (end_update) {
						snprintf(value, sizeof(value), "%.*s", (short)(end_update - update - 1), update + 1);
						mdm->update_time_sec = atoi(value);
					}
				}
			}
		}
		Write_Flash_Modem(start_modem_adrr, mdm);
		return 2;
	}
}
int counter_device=0;
int ReadDevices(const char* str, DeviceStruct* device) {
	char value[32];

	if (strstr(str, "<Device")) {
		deviceBegin = true;
		char* address = strstr(str, "address");
		if (address) {
			char* start = strchr(address, '"');
			if (start) {
				start++;
				device_char = atoi(start);
			}
		}
	}
	if (deviceBegin) {
		if (strstr(str, "<Variable")) {
			device->Device = device_char;

			char* update = strstr(str, "update_time_sec");
			char* end;
			if (update) {
				char* start = strchr(update, '"');
				if (start) {
					end = strchr(start + 1, '"');
					if (end) {
						snprintf(value, sizeof(value), "%.*s", (int)(end - start - 1), start + 1);
						device->update_time_sec = atoi(value);
					}
				}
			}

			char* group = strstr(str, "group");
			if (group) {
				char* start = strchr(group, '"');
				if (start) {
					end = strchr(start + 1, '"');
					if (end) {
						if (base_group == NULL) {
							snprintf(base_group, sizeof(base_group), "%.*s", (short)(end - start - 1), start + 1);
							device->group = group_count;
						}
						else
						{
							snprintf(value, sizeof(value), "%.*s", (short)(end - start - 1), start + 1);
							if (strcmp(value, base_group) != 0) {
								group_count++;
							}
							device->group = group_count;
							//strcpy_s(base_group, sizeof(value), value);
						}
					}
				}
			}

			char* data = strstr(str, "data_type");
			if (data) {
				char* start = strchr(data, '"');
				char* end;
				if (start) {
					end = strchr(start + 1, '"');
					if (end) {
						snprintf(value, sizeof(value), "%.*s", (short)(end - start - 1), start + 1);
						GetDataType(value, device);
					}
				}
			}

			char* address = strstr(str, "address");
			if (address) {
				char* start = strchr(address, '"');
				char* end;
				if (start) {
					end = strchr(start + 1, '"');
					if (end) {
						snprintf(value, sizeof(value), "%.*s", (short)(end - start - 1), start + 1);
						device->address = atoi(value);
					}
				}
			}

			char* read = strstr(str, "read_func");
			if (read) {
				char* start = strchr(read, '"');
				if (start) {
					start++;
					device->read_func = atoi(start);
				}
			}
			Write_Flash_Param(start_param_adrr, device);
			return 3;
		}
	}
	if (strstr(str, "</Device>")) {
		deviceBegin = false;

		counter++;
	}
}

int GetDataType(const char* value, DeviceStruct* device) {
	if (strstr(value, "uint16")) {
		device->data_type = 0;
		return 3;
	}
	if (strstr(value, "uint32")) {
		device->data_type = 1;
		return 3;
	}
	if (strstr(value, "int16")) {
		device->data_type = 2;
		return 3;
	}
	if (strstr(value, "float")) {
		device->data_type = 3;
		return 3;
	}
	if (strstr(value, "date_irz500")) {
		device->data_type = 4;
		return 3;
	}
	if (strstr(value, "time_irz500")) {
		device->data_type = 5;
		return 3;
	}
	if (strstr(value, "timer_irz500")) {
		device->data_type = 6;
		return 3;
	}
}

int Run(char byte, ParametersStruct* param, ModemStruct* mdm, DeviceStruct* device, bool Clean)
{
	static char str[400];
	static unsigned short index = 0;
	if (Clean) {
		index = 0;
	}
	else
	{
		str[index++] = byte;
		if (byte == 0x0A) {
			str[index - 1] = '\0';
			index = 0;
			//char value[32];
			if (strstr(str, "<Configuration>")) {
				FlashErase(start_modem_adrr);
				configBegin = true;
			}
			if (configBegin) {
				if (strstr(str, "<Parameters")) {
					paramBegin = true;
				}
				else if (paramBegin) {
					ReadParameters(str, param);
				}
				if (strstr(str, "</Parameters") /*&& paramBegin*/) {
					paramBegin = false;
					return 1;
				}
				else if (strstr(str, "<Modem>")) {
					modemBegin = true;
				}
				else if (modemBegin) {
					if (ReadModem(str, mdm)) {
						return 2;
					}
				}
				if (strstr(str, "</Modem>")) {

					modemBegin = false;
				}
				if (strstr(str, "<Devices>")) {
					modemBegin = false;
					devicesBegin = true;
				}
				else if (devicesBegin) {
					if (ReadDevices(str, device)) {
						return 3;
					}
				}
				if (strstr(str, "</Devices>")) {
					devicesBegin = false;
				}
			}
			memset(str,9,400);
		}
		if (strstr(str, "</Configuration>")) {
			configBegin = false;
			puts("Configuration end");
		}

	}
	return 0;
}





