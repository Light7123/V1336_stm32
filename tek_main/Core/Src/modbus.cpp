
#include "main.h"
#include "modbus.h"
#include "modbus_crc.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <strings.h>
#include "cmsis_os.h"
#include "xmlParse.h"
#include "UnixTime.h"

using namespace std;

uint8_t RxData_modbus[300];
uint8_t TxData_modbus[50];
char RxData_ch[150];
char TxData_ch[150];
char TxData_lcd[40];
char TxData_mb[40];
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
	//	HAL_UART_Transmit(&huart3,(uint8_t*)str12.c_str(), 40,100);
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
int last=0;
int write_modbus(uint8_t slave_address,uint8_t func, uint8_t register_high,uint8_t register_low, int num_reg, uint8_t* data )
{
	int j;
	int z;

	for(int i=0;i<733;i++)
	{
		if(device[i].regaddresLO==register_low&&device[i].regaddresHI==register_high)
		{
			z=i;
			device[i].pub=true;
			if(i<=730)
			{
				if(device[i].pub_delay<30)
				{
					counter=i;
				}
				else
				{
					slow_counter=i;
				}
			}
		}
	}
	if(device[z].write_func==5)
	{
		counter=570;
		slow_counter=0;
		read_modbus_count=570;
		TxData_modbus[0] = slave_address;  // slave address
		TxData_modbus[1] = device[z].write_func;


		TxData_modbus[2] = register_high;  // Register address high
		TxData_modbus[3] = register_low;  // Register address low
		TxData_modbus[4] = data[0];  // Data High for first Register
		TxData_modbus[5] = data[1];  // Data High for first Register
		uint16_t crc = crc16_modbus(TxData_modbus, 6);
		TxData_modbus[6] = crc&0xFF;   // CRC LOW
		TxData_modbus[7] = (crc>>8)&0xFF;  // CRC HIGH

	}
	else
	{
		TxData_modbus[0] = slave_address;  // slave address
		TxData_modbus[1] = device[z].write_func;


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
	}
	sendData_rs485(TxData_modbus,20);
	//HAL_UART_Transmit(&huart6, TxData_modbus, 32, 400);
	std::string str1;
	std::string str2;
	for(int i=0;i<12;i++)
	{
		sprintf(RxData_ch,"%02x ",TxData_modbus[i]);
		str1+=RxData_ch;
	}
	memset(TxData_modbus,0,50);
	//  sprintf(TxData_lcd,"%02x TX",TxData_modbus);
	for(int i=0;i<8;i+=1)
	{

		HAL_UARTEx_ReceiveToIdle_IT(&huart6, RxData_modbus, 100);
		osDelay(50);
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
string modbus_to_unix(string data1,string data2,string data3)
{
	int rmd;
	int rhy;
	int rms;
	uint8_t month_su;
	uint8_t day_su;
	uint8_t hour_su;
	uint8_t year_su;
	uint8_t minute_su;
	uint8_t second_su;
	char RxData_ch[150];
	std::string str2;
	rmd=atoi(data1.c_str());
	std::stringstream ss1;
	uint16_t add1;
	ss1 << std::hex << rmd;
	ss1 >> add1;
	month_su=(add1>>8)&0xFF;
	day_su=add1&0xFF;
	rhy=atoi(data2.c_str());
	std::stringstream ss2;
	uint16_t add2;
	ss2 << std::hex << rhy;
	ss2 >> add2;
	hour_su=(add2>>8)&0xFF;
	year_su=add2&0xFF;
	rms=atoi(data3.c_str());
	std::stringstream ss3;
	uint16_t add3;
	ss3 << std::hex << rms;
	ss3 >> add3;
	minute_su=add3&0xFF;
	second_su=(add3>>8)&0xFF;
	year=year_su+2000;
	month=month_su;
	day=day_su;
	hour=hour_su;
	minute=minute_su;
	second=second_su;
	sprintf(RxData_ch,"%d",getUnix());
	str2=RxData_ch;
	return str2;
}
uint32_t old_unix_su=0;
uint32_t old_unix_start=0;
uint32_t old_unix_stop=0;
string read_single(uint8_t slave_address,uint8_t register_high,uint8_t register_low,uint8_t func)
{
	int j;
	//memset(RxData_modbus,'\0',sizeof(RxData_modbus));
	TxData_modbus[0] = slave_address;  // slave address
	TxData_modbus[1] = func;  // Function code for Read Input Registers

	TxData_modbus[2] = register_high;
	TxData_modbus[3] = register_low;
	//The Register address will be 00000000 00000001 = 1 +30001 = 30002

	TxData_modbus[4] = 0;
	TxData_modbus[5] = 1;
	// no of registers to read will be 00000000 00000101 = 5 Registers = 10 Bytes

	uint16_t crc =crc16_modbus(TxData_modbus, 6);
	TxData_modbus[6] = crc&0xFF;   // CRC HIGH
	TxData_modbus[7] = (crc>>8)&0xFF;  // CRC LOW

	sendData_rs485(TxData_modbus,8);
	//HAL_UART_Transmit(&huart5, TxData_modbus, 32, 400);
	std::string str1;
	std::string str2="err";
	//		for(int i=0;i<8;i++)
	//		{
	//			sprintf(TxData_ch,"%02x ",TxData_modbus[i]);
	//			str1+=TxData_ch;
	//		}
	//	  sprintf(TxData_lcd,"%02x TX",TxData_modbus);

	//glcd_puts((char*)str1.c_str(), 0, 0);
	//HAL_Delay(100);
	string str12;
	bool fl=false;
	for(int i=0;i<4;i+=1)
	{
		osDelay(70);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, RxData_modbus, 300);
		uint16_t crc1=crc16_modbus(RxData_modbus,3+1*2);
		uint16_t crc2=(RxData_modbus[4+1*2]<<8)+(RxData_modbus[3+1*2]);
		if(crc1==crc2)
		{
			for( j=0;j<30;j++)
				if(RxData_modbus[j]!=0&&RxData_modbus[j+1]!=0&&RxData_modbus[j+1]!=0x83&&RxData_modbus[j+1]!=0x84&&RxData_modbus[j+1]<20&&RxData_modbus[j]==1)
				{
					for(int k=0;k<1*2;k+=2)
					{
						int res=(RxData_modbus[3+k]<<8)+RxData_modbus[4+k];
						if(register_high==0x01&&register_low==0x0D)
						{
							if(res>=32767)
							{
								res=res-65536;
							}
						}
						sprintf(RxData_ch,"%d",res);
						str2=RxData_ch;
						fl=true;
					}
					break;
				}
		}
		if(fl)
		{
			memset(RxData_modbus,0,300);
			fl=false;
			break;
		}
	}



	return str2;
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
	bool fl=false;
	for(int i=0;i<4;i+=1)
	{
		osDelay(70);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, RxData_modbus, 300);
		//		HAL_UART_Transmit(&huart3, (uint8_t*)"FROM_modbus: ", 13,50);
		//		for(int ii=0;ii<100;ii++)
		//		{
		//			sprintf( TxData_mb,"%02x ",RxData_modbus[ii]);
		//			str12+= TxData_mb;
		//		}
		//		HAL_UART_Transmit(&huart3,(uint8_t*)str12.c_str(), 40,100);
		//		HAL_UART_Transmit(&huart3, (uint8_t*)"\n\r", 2,50);
		for( j=0;j<30;j++)
			if(RxData_modbus[j]!=0&&RxData_modbus[j+1]!=0&&RxData_modbus[j+1]!=0x83&&RxData_modbus[j+1]!=0x84&&RxData_modbus[j+1]<20&&RxData_modbus[j]==1)
			{
				uint16_t crc1=crc16_modbus(RxData_modbus,3+num_reg*2);
				uint16_t crc2=(RxData_modbus[4+num_reg*2]<<8)+(RxData_modbus[3+num_reg*2]);

				//			HAL_UART_Transmit(&huart3, (uint8_t*)"MODBUS OK\n\r", 16,50);

				for(int k=0;k<num_reg*2;k+=2)
				{
					int res=(RxData_modbus[3+k]<<8)+RxData_modbus[4+k];
					if(device[read_modbus_count].type==2)
					{
						if(res>=32767)
						{
							res=res-65536;
						}
					}
					sprintf(RxData_ch,"%d",res);
					str2=RxData_ch;
					if(crc1==crc2)
					{

						device[read_modbus_count].res=str2;
					}
					fl=true;
					if(read_modbus_count==717)
					{
						string h1=read_single(0x01, 0x09, 0x0A, 4);
						string h2=read_single(0x01, 0x09, 0x0B, 4);
						string h3=read_single(0x01, 0x09, 0x0C, 4);
						if(h1!="err"&&h2!="err"&&h3!="err")
						{
							uint32_t new_unix1=atoi(modbus_to_unix(h1,h2 ,h3 ).c_str());

							if(new_unix1>1704056461)
							{
								device[717].res=to_string(new_unix1);
							}
							else
							{
								device[717].res=device[717].old_res;
							}
						}
						else
						{
							device[717].res=device[717].old_res;
						}
					}
					if(read_modbus_count==575)
					{
						string h4=read_single(0x01, 0x09, 0x1D, 4);
						string h5=read_single(0x01, 0x09, 0x1E, 4);
						string h6=read_single(0x01, 0x09, 0x1F, 4);
						if(h4!="err"&&h5!="err"&&h6!="err")
						{
							uint32_t new_unix2=atoi(modbus_to_unix(h4, h5, h6).c_str());

							if(new_unix2>1704056461)
							{
								device[575].res=to_string(new_unix2);
							}
							else
							{
								device[575].res=device[575].old_res;
							}
						}
						else
						{
							device[575].res=device[575].old_res;
						}

					}
					if(read_modbus_count==719)
					{
						uint32_t new_unix=atoi(modbus_to_unix(read_single(0x01, 0x00, 0xFA, 4), read_single(0x01, 0x00, 0xFB, 4), read_single(0x01, 0x00, 0xFC, 4)).c_str());
						if(new_unix>old_unix_su)
						{
							device[719].res=to_string(new_unix);
							old_unix_su=new_unix;
						}
						else
						{
							device[719].res=to_string(old_unix_su);
						}
					}
					if(read_modbus_count==3)
					{
						string high_data=read_single(0x01, 0x04, 0x30, 3);
						string low_data=read_single(0x01, 0x04, 0x31, 3);
						if(high_data!="err"&&low_data!="err")
						{
							uint16_t high=atoi(high_data.c_str());
							uint16_t low=atoi(low_data.c_str());
							device[3].res=to_string((high<<16)+low);
						}
						else {
							device[3].res=device[3].old_res;
						}
					}
					if(read_modbus_count==722||read_modbus_count==584)
					{
						string high_tim=read_single(0x01, 0x00, 0xFD, 4);
						string low_tim=read_single(0x01, 0x00, 0xFE, 4);
						if(high_tim!="err"&&low_tim!="err")
						{
							uint16_t h=atoi(high_tim.c_str());
							uint16_t ms=atoi(low_tim.c_str());
							std::stringstream ss4;
							uint16_t add4;
							ss4 << std::hex << ms;
							ss4 >> add4;
							uint8_t mi=add4&0xFF;
							uint8_t se=(add4>>8)&0xFF;
							string result=to_string(h)+":"+to_string(mi)+":"+to_string(se);
							device[584].res=result;
							device[722].res=result;
						}
						else
						{
							device[584].res=device[584].old_res;
							device[722].res=device[722].old_res;
						}


					}

					if(read_modbus_count<730)
					{
						read_modbus_count++;
					}
					else
					{
						last=0;
						read_modbus_count=0;
					}
				}

				break;

			}
		if(fl)
		{

			memset(RxData_modbus,0,300);

			break;
		}
	}

	string status=read_single(0x01, 0x00, 0xFF, 4);
	if(status!="err")
	{
		device[585].res=status;
	}
	if(last+100<read_modbus_count)
	{
		string frec=read_single(0x01, 0x01, 0x0D, 4);
		if(frec!="err")
		{
			device[598].res=frec;
		}
		string direct=read_single(0x01, 0x00, 0xF4, 4);
		if(direct!="err")
		{
			device[581].res=direct;
		}
		string work_mode=read_single(0x01, 0x00, 0xF9, 4);
		if(work_mode!="err")
		{
			device[582].res=work_mode;
		}
		string IA=read_single(0x01, 0x01, 0x05, 4);
		if(IA!="err")
		{
			device[590].res=IA;
		}
		string IB=read_single(0x01, 0x01, 0x06, 4);
		if(IB!="err")
		{
			device[591].res=IB;
		}
		string IC=read_single(0x01, 0x01, 0x07, 4);
		if(IC!="err")
		{
			device[592].res=IC;
		}
		string UA=read_single(0x01, 0x01, 0x02, 4);
		if(UA!="err")
		{
			device[587].res=UA;
		}
		string UB=read_single(0x01, 0x01, 0x03, 4);
		if(UB!="err")
		{
			device[588].res=UB;
		}
		string UC=read_single(0x01, 0x01, 0x04, 4);
		if(UC!="err")
		{
			device[589].res=UC;
		}
		uint32_t new_unix=atoi(modbus_to_unix(read_single(0x01, 0x00, 0xFA, 4), read_single(0x01, 0x00, 0xFB, 4), read_single(0x01, 0x00, 0xFC, 4)).c_str());
		if(new_unix>old_unix_su)
		{
			device[719].res=to_string(new_unix);
			old_unix_su=new_unix;
		}
		else
		{
			device[719].res=to_string(old_unix_su);
		}

		last=read_modbus_count;
	}
	if(!fl)
	{
		//HAL_UART_Transmit(&huart3, (uint8_t*)"MODBUS ERROR\n\r", 16,50);

		if(read_modbus_count<730)
		{
			read_modbus_count+=num_reg;
		}
		else
		{
			last=0;
			read_modbus_count=0;
		}
	}
	memset(TxData_modbus,0,50);
	return str2;
}

