/*
 * mqtt.cpp
 *
 *  Created on: 19 июн. 2023 г.
 *      Author: user
 */



#include "main.h"
#include "stdio.h"
#include "string.h"

#include <string>
#include "vector"
//#include "UartRingbuffer.h"


using namespace std;

bool sub=false;
uint8_t buffer_pub[256];
uint8_t buffer_sub[256];
int buflen;
//int sub_len;
int err_flag=1;
int pub_err_count=0;
bool pub=false;
/*Инициализация модема и mqtt необходимо передать apn, адрес сервера и имя пользователя
 * если модем уже запущен, а контролер перезагрузился то это тоже работает*/
char buffer3[1500];
int Mqtt_Init()
{

	//char mqttInit[100];

	//	char mqttInit[100];
	//int initFlag = 0;
	//char data[100];


	//uint32_t timer;
	//	sendMQTTConnect("bob","GG168jcbD");
	//	 publishMessage("1001999", "test");
	//	if(topic.empty())
	//	{
	//		return 0;
	//	}
	//	if(mess.empty())
	//	{
	//		return 0;
	//	}
	//	sendMQTTConnect(user,pass);
	//	publishMessage(topic,mess);

	if(err_flag==1)
	{
		bool net_flag=false;

		for(int i=0;i<5;i++)
		{
			memset(buffer3, 9, sizeof(buffer3));
			//HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CGDCONT=1,\"IP\",\"internet.tele2.ru\"\r\n",41,100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CGDCONT=1,\"IP\",\"internet\"\r\n",41,100);
			//log_uart("AT+CGDCONT=1,\"IP\",\"internet.tele2.ru\"");
			//timer=HAL_GetTick();
			//log_buffer.append("LOG: STM32F0: ").append("AT+CGDCONT=1,\"IP\",\"internet.tele2.ru\"\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			HAL_Delay(1500);
			//			log_buffer.clear();
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3,150);
			//HAL_Delay(300);
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: MODEM: ").append(buffer).append("\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			//HAL_Delay(1000);
			//			log_buffer.clear();
			log_uart(buffer3);
			if(strstr(buffer3,"OK"))
			{
				net_flag=true;
				break;
			}
			else
			{
				net_flag=false;
				HAL_Delay(2000);
			}
		}
		if(!net_flag)
		{
			modem_flag=false;
			return 0;
		}
		for(int i=0;i<5;i++)
		{
			memset(buffer3, 10, sizeof(buffer3));
			//HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CGSOCKCONT=1,\"IP\",\"internet.tele2.ru\"\r\n",44,100);
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CGSOCKCONT=1,\"IP\",\"internet\"\r\n",44,100);
			//log_uart("AT+CGSOCKCONT=1,\"IP\",\"internet.tele2.ru\"");
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: STM32F0: ").append("AT+CGSOCKCONT=1,\"IP\",\"internet.tele2.ru\"\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			HAL_Delay(1500);
			//	log_buffer.clear();
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
			log_uart(buffer3);
			//HAL_Delay(300);
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: MODEM: ").append(buffer).append("\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			//HAL_Delay(1000);

			if(strstr(buffer3,"OK"))
			{
				net_flag=true;
				break;
			}
			else
			{
				net_flag=false;
				HAL_Delay(2000);
			}
		}
		if(!net_flag)
		{
			modem_flag=false;
			return 0;
		}
		for(int i=0;i<5;i++)
		{
			memset(buffer3, 10, sizeof(buffer3));
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CSOCKSETPN=1\r\n",19,100);
			log_uart("AT+CSOCKSETPN=1");
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: STM32F0: ").append("AT+CSOCKSETPN=1\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			HAL_Delay(1500);
			//			log_buffer.clear();
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
			log_uart(buffer3);
			//HAL_Delay(1000);
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: MODEM: ").append(buffer).append("\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);

			if(strstr(buffer3,"OK"))
			{
				net_flag=true;
				break;
			}
			else
			{
				net_flag=false;
				HAL_Delay(2000);
			}
		}
		if(!net_flag)
		{
			modem_flag=false;
			return 0;
		}

		for(int i=0;i<5;i++)
		{
			memset(buffer3, 10, sizeof(buffer3));
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPMODE=0\r\n",16,100);
			log_uart("AT+CIPMODE=0");
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: STM32F0: ").append("AT+CIPMODE=0\r\n");
			HAL_Delay(1500);
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			//			HAL_Delay(500);
			//			log_buffer.clear();
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
			log_uart(buffer3);
			//HAL_Delay(1500);
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: MODEM: ").append(buffer).append("\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			//			HAL_Delay(1000);
			//			log_buffer.clear();
			if(strstr(buffer3,"OK"))
			{
				net_flag=true;
				break;
			}
			else
			{
				//net_flag=false;
				HAL_Delay(1000);
			}
		}
		if(!net_flag)
		{
			modem_flag=false;
			return 0;
		}
		for(int i=0;i<5;i++)
		{
			memset(buffer3, 10, sizeof(buffer3));
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPRXGET=1\r\n",17,100);
			log_uart("AT+CIPRXGET=1");
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: STM32F0: ").append("AT+CIPRXGET=1\r\n");
			HAL_Delay(1500);
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			//			HAL_Delay(500);
			//			log_buffer.clear();
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
			log_uart(buffer3);
			//HAL_Delay(1500);
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: MODEM: ").append(buffer).append("\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			//		HAL_Delay(1000);
			//log_buffer.clear();
			if(strstr(buffer3,"OK"))
			{
				net_flag=true;
				break;
			}
			else
			{
				HAL_Delay(2000);
			}
		}
		if(!net_flag)
		{
			return 0;
		}
		for(int i=0;i<5;i++)
		{
			memset(buffer3, 10, sizeof(buffer3));
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+NETOPEN\r\n",14,100);
			log_uart("AT+NETOPEN");
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: STM32F0: ").append("AT+NETOPEN\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			HAL_Delay(1500);
			//			log_buffer.clear();
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
			log_uart(buffer3);
			//HAL_Delay(1000);
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: MODEM: ").append(buffer).append("\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			//		HAL_Delay(10000);
			//			log_buffer.clear();
			if(strstr(buffer3,"OK"))
			{
				net_flag=true;
				break;
			}
			else if(strstr(buffer3,"opened"))
			{
				net_flag=true;
				break;
			}
			else
			{
				net_flag=false;
				HAL_Delay(2000);
			}
		}
		if(!net_flag)
		{
			modem_flag=false;
			return 0;
		}

		for(int i=0;i<5;i++)
		{
			memset(buffer3, 10, sizeof(buffer3));
			HAL_UART_Transmit(&huart1, (uint8_t*)"AT+IPADDR\r\n",13,100);
			log_uart("AT+IPADDR");
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: STM32F0: ").append("AT+IPADDR\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			HAL_Delay(1500);
			//			log_buffer.clear();
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
			log_uart(buffer3);
			//	HAL_Delay(1000);
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: MODEM: ").append(buffer).append("\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			//		HAL_Delay(10000);
			//log_buffer.clear();
			if(strstr(buffer3,"OK"))
			{
				net_flag=true;
				break;
			}
			else
			{
				net_flag=false;
				HAL_Delay(2000);
			}
		}
		if(!net_flag)
		{
			modem_flag=false;
			return 0;
		}
		for(int i=0;i<5;i++)
		{
			memset(buffer3, 10, sizeof(buffer3));
			HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CIPOPEN=0,\"TCP\",\"\",1883\r\n" , 46,100);

			HAL_Delay(3500);
			//			log_buffer.clear();
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
			log_uart(buffer3);
			//HAL_Delay(1000);
			//			timer=HAL_GetTick();
			//			log_buffer.append("LOG: MODEM: ").append(buffer).append("\r\n");
			//			HAL_UART_Transmit(&huart2, (uint8_t*)log_buffer.c_str(), log_buffer.size(), log_buffer.size()*3);
			//		HAL_Delay(10000);
			//log_buffer.clear();
			if(strstr(buffer3,"OK"))
			{
				net_flag=true;
				//while(!strstr(buffer,"CIPOPEN:"))
				break;
			}
			else if(strstr(buffer3,"CIPOPEN: 0,4"))
			{
				net_flag=true;
				break;
			}
			else
			{
				net_flag=false;
				HAL_Delay(2000);
			}
		}
		if(!net_flag)
		{
			modem_flag=false;
			return 0;
		}

		memset(buffer3, 10, sizeof(buffer3));
		sprintf(buffer3,"AT+CIPSEND=0,%d\r\n",con_len);
		HAL_UART_Transmit(&huart1,(uint8_t*)buffer3 , 30,100);
		log_uart(buffer3);
		HAL_Delay(100);
		HAL_UART_Transmit(&huart1,(uint8_t*)recive_packet_init ,con_len,100);
		log_uart(recive_packet_init);
		HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n" , 2,50);
		HAL_Delay(1500);
		HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 400);
		HAL_Delay(1000);
		log_uart(buffer3);

		memset(buffer3, 9, 400);
		sprintf(buffer3,"AT+CIPSEND=0,%d\r\n",sub_len);
		HAL_UART_Transmit(&huart1,(uint8_t*)buffer3 , 22,100);
		log_uart(buffer3);
		HAL_Delay(200);
		HAL_UART_Transmit(&huart1,(uint8_t*)recive_packet_sub , sub_len,sub_len*3);
		log_uart(recive_packet_sub);
		HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n" , 2,50);
		HAL_Delay(500);
		HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 512);
		log_uart(buffer3);
		HAL_Delay(100);

		err_flag=0;
	}



	memset(buffer3, 9, sizeof(buffer3));
	sprintf(buffer3,"AT+CIPSEND=0,%d\r\n",pub_len);
	HAL_UART_Transmit(&huart1,(uint8_t*)buffer3 , 22,100);
	log_uart(buffer3);
	HAL_Delay(200);
	HAL_UART_Transmit(&huart1,(uint8_t*)recive_packet_send , pub_len,pub_len*2);
	log_uart(recive_packet_send);
	HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n" , 2,50);
	HAL_Delay(500);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 1500);
	log_uart(buffer3);
	//	for(int i=0;i<1;i++)
	//	{
	memset(recive_packet_send, 0,1010);
	//		HAL_UART_Transmit_DMA(&huart2, (uint8_t*)"*****\r\n", 7);
	//		HAL_Delay(30);
	//	}

	for(int i=0;i<1450;i++)
	{
		if(buffer3[i]=='I'&&buffer3[i+1]=='P'&&buffer3[i+2]=='C'&&buffer3[i+3]=='L'&&buffer3[i+4]=='O'&&buffer3[i+5]=='S'&&buffer3[i+6]=='E')

		{
			//			for(int i=0;i<5;i++)
			//			{
			//			//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)"*****\r\n", 7);
			//				HAL_Delay(30);
			//			}
			sub=false;
			pub_len=0;
			memset(buffer3, 10, sizeof(buffer3));\
			HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CIPOPEN=0,\"TCP\",\"\",1883\r\n" , 46,100);

			HAL_Delay(3500);
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
			HAL_Delay(1000);
			memset(buffer3, 10, sizeof(buffer3));
			sprintf(buffer3,"AT+CIPSEND=0,%d\r\n",con_len);
			HAL_UART_Transmit(&huart1,(uint8_t*)buffer3 , 30,100);
			HAL_Delay(100);
			HAL_UART_Transmit(&huart1,(uint8_t*)recive_packet_init ,con_len+3,150);
			HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n" , 2,50);
			//			HAL_Delay(1500);
			//			HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer3, 400);
			HAL_Delay(300);
			memset(buffer3, 9, 400);
			sprintf(buffer3,"AT+CIPSEND=0,%d\r\n",sub_len);
			HAL_UART_Transmit(&huart1,(uint8_t*)buffer3 , 22,100);
			HAL_Delay(200);
			HAL_UART_Transmit(&huart1,(uint8_t*)recive_packet_sub , sub_len,sub_len*3);
			HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n" , 2,50);

			pub=false;
		}

		else if(buffer3[i]=='E'&&buffer3[i+1]=='R'&&buffer3[i+2]=='R'&&buffer3[i+3]=='O'&&buffer3[i+4]=='R')

		{
			pub_len=0;
			pub=false;
			memset(buffer3, 10, sizeof(buffer3));
			HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CIPOPEN=0,\"TCP\",\"\",1883\r\n" , 46,100);

			HAL_Delay(3500);
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
			HAL_Delay(1000);
			memset(buffer3, 10, sizeof(buffer3));
			sprintf(buffer3,"AT+CIPSEND=0,%d\r\n",con_len);
			HAL_UART_Transmit(&huart1,(uint8_t*)buffer3 , 30,100);
			HAL_Delay(100);
			HAL_UART_Transmit(&huart1,(uint8_t*)recive_packet_init , con_len,100);
			HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n" , 2,50);
			//			HAL_Delay(1500);
			//			HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer3, 400);
			HAL_Delay(300);
			memset(buffer3, 9, 400);
			sprintf(buffer3,"AT+CIPSEND=0,%d\r\n",sub_len);
			HAL_UART_Transmit(&huart1,(uint8_t*)buffer3 , 22,100);
			HAL_Delay(200);
			HAL_UART_Transmit(&huart1,(uint8_t*)recive_packet_sub , sub_len,sub_len*3);
			HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n" , 2,50);
			sub=false;
		}

		else if(buffer3[i]=='O'&&buffer3[i+1]=='K')
		{
			pub_len=0;
			memset(buffer3, 9, sizeof(buffer3));
			//			for(int i=0;i<5;i++)
			//			{
			//			//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)"*****\r\n", 7);
			//				HAL_Delay(30);
			//			}
			pub=true;
			pub_err_count=0;
			//message_count++;
			break;
		}
		else
		{
			sub=false;
			pub_len=0;
			pub=false;
		}
	}

	if(!pub)
	{
		pub_err_count+=1;
		if(pub_err_count>7)
		{
			for(int i=0;i<5;i++)
			{

				sub=false;
				modem_flag=false;
				memset(buffer3, 9, sizeof(buffer3));
				HAL_UART_Transmit(&huart1, (uint8_t*)"AT+NETCLOSE\r\n",15,100);
				HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 151);
				HAL_Delay(100);
				init_flag=0;
			}
			err_flag=1;
		}

	}


	return 0;


}




/*Публикация собщения по mqtt необходимо передать названия топика(не более 1024 символов) и сообщение(не более 1024 символов)*/

char buffer4[400];
/* Подписка на топик, если после функции идет ещё функция взаидоействующая с uart необходима задрежка 3000мс*/
void Mqtt_Subscribe()
{
	if(err_flag!=1)
	{

		//	UARTPrint("MQTT TOPIC: ");
		//	UARTPrint(topic1);
		//	UARTPrint("\n");
		//		MQTT_SUB(topic1);
		//sendMQTTConnect("bob","GG168jcbD");
		memset(buffer4, 9, 400);
		sprintf(buffer4,"AT+CIPSEND=0,%d\r\n",sub_len);
		HAL_UART_Transmit(&huart1,(uint8_t*)buffer4 , 22,100);
		HAL_Delay(200);
		HAL_UART_Transmit(&huart1,(uint8_t*)recive_packet_sub , sub_len,sub_len*3);
		HAL_UART_Transmit(&huart1,(uint8_t*)"\r\n" , 2,50);
		HAL_Delay(1000);
		HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer4, 400);
		HAL_Delay(100);

		for(int i=0;i<350;i++)
		{

			if((buffer4[i]=='O'&&buffer4[i+1]=='K')||(buffer4[i]=='R'&&buffer4[i+1]=='E'&&buffer4[i+2]=='C'&&buffer4[i+3]=='V'))
			{
				memset(buffer3, 9, 400);
				pub_err_count=0;
				sub=true;
				//message_count++;
				break;
			}
		}
	}
	//return "";

}

/*Прием сообщения mqtt возвращает и имя топика и сообщение
 * Читаем входящий поток по uart ищем слово +CMQTTRXEND: 0 после
 * этого отбрасываем служебную информаци и записываем название топика и текст сообщения в структуру message_mqtt_recive */
uint8_t rxmessage1[500];

//uint8_t test[100];
uint32_t all_len=0;
void Mqtt_Recive()
{

	int last_len=0;
	if(err_flag!=1)
	{

		//message_mqtt_recive message;
		//		uint8_t topic[100];
		//		uint8_t message[100];

		//string res;
		int len1=0;
		char ui[50];
		//		int len2;
		//		bool flag=false;
		//		int message_len;
		memset(buffer3, 9, 1500);
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPRXGET=4,0\r\n", 24, 50);
		//log_uart("AT+CIPRXGET=4,0");
		HAL_Delay(200);
		HAL_UART_Receive_DMA(&huart1, (uint8_t*)buffer3, 1500);
		//log_uart((char*)rxmessage1);
		//memcpy(rxmessage1,test,100);
		for(int i=0;i<1450;i++)
		{
			if(buffer3[i]=='+'&&buffer3[i+1]=='C'&&buffer3[i+2]=='I'&&buffer3[i+3]=='P'&&buffer3[i+4]=='R'&&buffer3[i+5]=='X'&&buffer3[i+6]=='G'&&buffer3[i+7]=='E'&&buffer3[i+8]=='T'&&buffer3[i+9]==':')
			{
				memcpy(rxmessage1,strstr((char*)buffer3,"+CIPRXGET:"),500);
				len1=rxmessage1[15]-'0';
				if(rxmessage1[16]!='\n'&&rxmessage1[16]!='\r')
				{
					len1=len1*10+rxmessage1[16]-'0';
					if(rxmessage1[17]!='\n'&&rxmessage1[17]!='\r')
					{
						len1=len1*10+rxmessage1[17]-'0';
						if(rxmessage1[18]!='\n'&&rxmessage1[18]!='\r')
						{
							len1=len1*10+rxmessage1[18]-'0';
						}
					}
				}
				if(len1>=500)
				{
					last_len=len1-400;
					len1=400;
				}
				else
				{
					last_len=len1;
				}
				break;
			}
		}
		while(last_len>10)
		{
			//			if(len1>10)
			//			{ee
			int len2;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
			memset(buffer3, 10, sizeof(buffer3));
			sprintf(ui,"AT+CIPRXGET=2,0,%d\r\n",len1);

			HAL_UART_Transmit(&huart1, (uint8_t*)ui, 22,50);
			log_uart(ui);
			HAL_Delay(150);
			HAL_UART_Receive_DMA(&huart1,  (uint8_t*)buffer3, 1500);
			log_uart((char*)rxmessage1);
			for(int i=0;i<1450;i++)
			{
				if(buffer3[i]=='+'&&buffer3[i+1]=='C'&&buffer3[i+2]=='I'&&buffer3[i+3]=='P'&&buffer3[i+4]=='R'&&buffer3[i+5]=='X'&&buffer3[i+6]=='G'&&buffer3[i+7]=='E'&&buffer3[i+8]=='T'&&buffer3[i+9]==':')
				{
					memcpy(rxmessage1,strstr((char*)buffer3,"+CIPRXGET:"),500);
					len2=rxmessage1[19]-'0';
					if(rxmessage1[20]!='\n'&&rxmessage1[20]!='\r')
					{
						len2=len2*10+rxmessage1[20]-'0';
						if(rxmessage1[21]!='\n'&&rxmessage1[21]!='\r')
						{
							len2=len2*10+rxmessage1[21]-'0';
							if(rxmessage1[22]!='\n'&&rxmessage1[22]!='\r')
							{
								len2=len2*10+rxmessage1[22]-'0';
							}
						}
					}
					last_len=len2;
					if(len2>=500)
					{
						len2=400;
					}
					if(len2>10)
					{
						all_len+=len2;
						pub=false;
						len2=0;
						last_len=last_len-len1;
						memcpy(rxmessage1,strstr((char*)buffer3,"+CIPRXGET:"),500);
						for(int z=0;z<1;z++)
						{
							HAL_Delay(30);
							HAL_UART_Transmit(&huart2, (uint8_t*)"+CIPRXGET:", 12,30);
							HAL_UART_Transmit(&huart2, rxmessage1, 440,250);
							HAL_Delay(30);
							memset( rxmessage1, 9, 500);
						}

					}
				}
				//}
			}

		}
		len1=0;
		if(all_len>200000)
		{

			HAL_Delay(1);
			all_len++;
		}
	}

}




