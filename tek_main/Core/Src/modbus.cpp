
#include "main.h"
#include "modbus.h"
#include "modbus_crc.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <strings.h>


using namespace std;

uint8_t RxData_modbus[150];
uint8_t TxData_modbus[50];
char RxData_ch[150];
char TxData_ch[150];

void sendData_rs485 (uint8_t *data, int size)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart6, data, size, 100);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8,  GPIO_PIN_RESET);
}

void write_modbus(uint8_t slave_address,uint8_t register_high,uint8_t register_low, int num_reg, uint8_t* data )
{
	int j;

			TxData_modbus[0] = slave_address;  // slave address
			TxData_modbus[1] = 0x10;


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


			uint16_t crc = crc16_modbus(TxData_modbus, 9);
			TxData_modbus[i+8] = crc&0xFF;   // CRC LOW
			TxData_modbus[i+9] = (crc>>8)&0xFF;  // CRC HIGH

			sendData_rs485(TxData_modbus,i+10);
			//HAL_UART_Transmit(&huart6, TxData_modbus, 32, 400);
			std::string str1;
			std::string str2;
//			for(int i=0;i<8;i++)
//			{
//				sprintf(TxData_lcd,"%02x ",TxData_modbus[i]);
//				str1+=TxData_lcd;
//			}
			//  sprintf(TxData_lcd,"%02x TX",TxData_modbus);
			for(int i=0;i<8;i+=1)
			{

				HAL_UARTEx_ReceiveToIdle_IT(&huart6, RxData_modbus, 100);
				HAL_Delay(200);
				for( j=0;j<32;j++)
					if(RxData_modbus[j]!=0&&RxData_modbus[j+1]!=0)
						break;
			}


}

string read_modbus(uint8_t slave_address,uint8_t register_high,uint8_t register_low, int num_reg)
{
	int j;
	TxData_modbus[0] = slave_address;  // slave address
	TxData_modbus[1] = 0x03;  // Function code for Read Input Registers

	TxData_modbus[2] = register_high;
	TxData_modbus[3] = register_low;
	//The Register address will be 00000000 00000001 = 1 +30001 = 30002

	TxData_modbus[4] = 0;
	TxData_modbus[5] = num_reg;
	// no of registers to read will be 00000000 00000101 = 5 Registers = 10 Bytes

	uint16_t crc = crc16_modbus(TxData_modbus, 6);
	TxData_modbus[6] = crc&0xFF;   // CRC LOW
	TxData_modbus[7] = (crc>>8)&0xFF;  // CRC HIGH

	sendData_rs485(TxData_modbus,8);
	//HAL_UART_Transmit(&huart5, TxData_modbus, 32, 400);
	std::string str1;
	std::string str2;
//	for(int i=0;i<8;i++)
//	{
//		sprintf(TxData_lcd,"%02x ",TxData_modbus[i]);
//		str1+=TxData_lcd;
//	}
	//  sprintf(TxData_lcd,"%02x TX",TxData_modbus);

	//glcd_puts((char*)str1.c_str(), 0, 0);
	for(int i=0;i<8;i+=1)
	{

		HAL_UARTEx_ReceiveToIdle_IT(&huart6, RxData_modbus, 100);
		HAL_Delay(200);
		for( j=0;j<32;j++)
			if(RxData_modbus[j]!=0&&RxData_modbus[j+1]!=0)
				break;
	}

	//sendData_modbus(RxData_modbus, 32);
	//HAL_UART_Transmit(&huart5, RxData_modbus, 32, 400);
	for(int i=j;i<j+32;i++)
	{
		sprintf(RxData_ch,"%02x ",RxData_modbus[i]);
		str2+=RxData_ch;
	}
	return str2;
}

