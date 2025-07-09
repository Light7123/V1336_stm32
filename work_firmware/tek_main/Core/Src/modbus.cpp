
#include "main.h"
#include "modbus.h"
#include "modbus_crc.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <strings.h>
#include "cmsis_os.h"

using namespace std;

uint8_t RxData_modbus[150];
uint8_t TxData_modbus[50];
char RxData_ch[150];
char TxData_ch[150];
char TxData_lcd[40];
void sendData_rs485 (uint8_t *data, int size)
{
	string str12;
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart6, data, size, 100);
//	HAL_UART_Transmit(&huart3, (uint8_t*)"To_modbus: ", 11,50);
//	for(int ii=0;ii<100;ii++)
//	{
//		sprintf(TxData_lcd,"%02x ",data[ii]);
//		str12+=TxData_lcd;
//	}
//	HAL_UART_Transmit(&huart3,(uint8_t*)str12.c_str(), 21,100);
//	HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8,  GPIO_PIN_RESET);
}
uint16_t calculateCRC(const uint8_t* data, size_t length) {
	uint16_t crc = 0xFF;

	for (size_t i = 0; i < length; ++i) {
		crc ^= data[i];
		for (size_t j = 0; j < 8; j++) {
			if (crc & 0x0001) {
				crc = (crc >> 1) ^ 0xA001;
			} else {
				crc = crc >> 1;
			}
		}
	}

	return crc;
}
int write_modbus(uint8_t slave_address,uint8_t func, uint8_t register_high,uint8_t register_low, int num_reg, uint8_t* data )
{
	int j;

	TxData_modbus[0] = slave_address;  // slave address
	TxData_modbus[1] = func;


	TxData_modbus[2] = register_high;  // Register address high
	TxData_modbus[3] = register_low;  // Register address low
	//The coil address will be 00000000 00000000 = 0 + 40001 = 40001

	TxData_modbus[4] = 0;  // no. of Registers high
	TxData_modbus[5] = num_reg;  // no. of Registers low
	// Total no. of Registers = 00000000 00000011 = 3 Registers

	TxData_modbus[6] = num_reg*2; // Byte count (3 Registers would need 6 bytes (2 bytes per register))

	int i=0;
	for(i=0; i<num_reg*2; i++)
	{
		TxData_modbus[i+7] = data[i];  // Data High for first Register
	}


	uint16_t crc = crc16_modbus(TxData_modbus, 7+(num_reg*2));
	TxData_modbus[i+7] = crc&0xFF;   // CRC LOW
	TxData_modbus[i+8] = (crc>>8)&0xFF;  // CRC HIGH

	sendData_rs485(TxData_modbus,i+10);
	//HAL_UART_Transmit(&huart6, TxData_modbus, 32, 400);
	std::string str1;
	std::string str2;
	for(int i=0;i<12;i++)
	{
		sprintf(RxData_ch,"%02x ",TxData_modbus[i]);
		str1+=RxData_ch;
	}
	//  sprintf(TxData_lcd,"%02x TX",TxData_modbus);
	for(int i=0;i<8;i+=1)
	{

		HAL_UARTEx_ReceiveToIdle_IT(&huart6, RxData_modbus, 100);
		osDelay(200);
		for( j=0;j<32;j++)
			if(RxData_modbus[j]!=0&&RxData_modbus[j+1]!=0)
			{
				return 1;
				break;
			}
			else
			{
				return 0;
				break;
			}
	}
	return 0;
}

string read_modbus(uint8_t slave_address,uint8_t register_high,uint8_t register_low,uint8_t func, int num_reg)
{
	int j;
	//memset(RxData_modbus,'\0',sizeof(RxData_modbus));
	TxData_modbus[0] = slave_address;  // slave address
	TxData_modbus[1] = func;  // Function code for Read Input Registers

	TxData_modbus[2] = register_high;
	TxData_modbus[3] = register_low;
	//The Register address will be 00000000 00000001 = 1 +30001 = 30002

	TxData_modbus[4] = 0;
	TxData_modbus[5] = num_reg;
	// no of registers to read will be 00000000 00000101 = 5 Registers = 10 Bytes

	uint16_t crc =crc16_modbus(TxData_modbus, 6);
	TxData_modbus[6] = crc&0xFF;   // CRC HIGH
	TxData_modbus[7] = (crc>>8)&0xFF;  // CRC LOW

	sendData_rs485(TxData_modbus,8);
	//HAL_UART_Transmit(&huart5, TxData_modbus, 32, 400);
	std::string str1;
	std::string str2;
	//		for(int i=0;i<8;i++)
	//		{
	//			sprintf(TxData_ch,"%02x ",TxData_modbus[i]);
	//			str1+=TxData_ch;
	//		}
	//	  sprintf(TxData_lcd,"%02x TX",TxData_modbus);

	//glcd_puts((char*)str1.c_str(), 0, 0);
	//HAL_Delay(100);
	string str12;
	for(int i=0;i<8;i+=1)
	{

		HAL_UARTEx_ReceiveToIdle_IT(&huart6, RxData_modbus, 100);
		osDelay(50);
		for( j=0;j<32;j++)
			if(RxData_modbus[j]!=0&&RxData_modbus[j+1]!=0)
			{
				uint16_t res=(RxData_modbus[3]<<8)+RxData_modbus[4];
				if(res!=512)
					{
//						HAL_UART_Transmit(&huart3,(uint8_t*)"From_modbus: ", 13,50);
//						for(int ii=0;ii<100;ii++)
//						{
//							sprintf(TxData_lcd,"%02x ",RxData_modbus[ii]);
//							str12+=TxData_lcd;
//						}
//						HAL_UART_Transmit(&huart3,(uint8_t*)str12.c_str(), 21,100);
//						HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);
						sprintf(RxData_ch,"%d",res);
						str2+=RxData_ch;
						memset(RxData_modbus,0,150);
						if(read_modbus_count<9)
						{
							read_modbus_count++;
						}
						else
						{
							//mqtt_init("999999/Modem/MB_ConnStat", "OK","");
							read_modbus_count=0;
						}
					}
				break;
			}
	}



	return str2;
}

