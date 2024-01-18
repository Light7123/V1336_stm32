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


typedef struct
{
	char topic[100];
	char payload[256];
} message_mqtt_recive;

char buffer_mqtt[1024];


//Uart_sendstring("AT+HTTPACTION=1\r\n");
//HAL_Delay(1500);
//Uart_flush();
//Get_after("",1024, buffer);
////	HAL_Delay(2000);
//UARTPrint(buffer);
//HAL_Delay(300);



/*Инициализация модема и mqtt необходимо передать apn, адрес сервера и имя пользователя
 * если модем уже запущен, а контролер перезагрузился то это тоже работает*/
int Mqtt_Init(char* serv, char* user, char* pass)
{

	//char mqttInit[100];

	char mqttInit[100];
	int initFlag = 0;
	char data[100];
	char buffer[1024];
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPSHUT\r\n",14,500);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CMEE=1\r\n",14,500);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CGATT=1\r\n",14,500);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CGATT?\r\n",14,500);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CGDCONT=1,\"IP\",\"internet.ru\"\r\n",39,500);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}

	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPSTATUS\r\n",16,500);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}

	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CDNSCFG=\"4.4.4.4\",\"8.8.8.8\"\r\n",38,500);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CSTT=\"internet.ru\",\"gdata\",\"gdata\"\r\n",47,500);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CIICR\r\n" , 12,100);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CIFSR\r\n" , 12,100);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CIPSTART=\"TCP\",\"kz-kudu.irz.ru\",\"1883\"\r\n" , 61,500);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}
	for(int i=0;i<10;i++)
	{
		memset(buffer, 9, sizeof(buffer));
		HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CIPSTATUS\r\n" , 16,100);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 51);
		HAL_Delay(100);
	}

//	for(int i=0;i<10;i++)
//	{
//		memset(buffer, 9, sizeof(buffer));
//		HAL_UART_Transmit(&huart1,(uint8_t*)"AT+CHTTPACT=\"kz-kudu.irz.ru\",80\r\n" , 42,100);
//		HAL_Delay(100);
//		HAL_UART_Receive(&huart1, (uint8_t*)buffer, 512,1000);
//		HAL_Delay(100);
//	}

	memset(buffer, 9, sizeof(buffer));
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"AT+CMQTTREL=0\r\n",15);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 1024);

	//Uart_sendstring("AT+CMQTTREL=0\r\n");
	HAL_Delay(100);
	//Uart_sendstring("AT+CMQTTSTOP\r\n");
	memset(buffer, 9, sizeof(buffer));
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"AT+CMQTTSTOP\r\n",14);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 1024);
	HAL_Delay(100);
	//Uart_flush();
	memset(buffer, 9, sizeof(buffer));
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"AT+CMQTTSTART\r\n",15);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 1024);

	//Uart_sendstring("AT+CMQTTSTART\r\n");
	HAL_Delay(1000);



	while(strstr(buffer,"23") == NULL && strstr(buffer,"0") == NULL)
	{

		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 1024);
		//Get_after("",1024, buffer);
		UARTPrint(buffer);
		HAL_Delay(300);
		initFlag = 1;
	}
	sprintf(data,"AT+CMQTTACCQ=1,\"%s\"\r\n","client");
	//Uart_sendstring(data);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)data, strlen(data));
	HAL_Delay(1000);
	//Uart_flush();
	memset(buffer, 9, sizeof(buffer));
	sprintf(data,"AT+CMQTTCONNECT=1,\"%s\",200,1,\"%s\",\"%s\"\r\n","tcp://kz-kudu.irz.ru:1883", "bob","GG168jcbD");
	//Uart_sendstring(data);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)data, strlen(data));
	HAL_Delay(1000);

	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 1024);
	while(strstr(buffer,"CONNECT:") == NULL)
	{
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)buffer, 1024);
		//Get_after("",1024, buffer);
		UARTPrint(buffer);
		HAL_Delay(300);
		HAL_UART_Transmit_DMA(&huart2, (uint8_t*)"OK\n", 3);
		initFlag = 1;
	}
	//Uart_flush();

	return initFlag;

	//	int initFlag = 0;
	//	int mqtt_err=0;
	//	char data[100];
	//	int error_count=0;
	//	while(1)
	//	{
	//		HAL_UART_Transmit(&huart1, (uint8_t*)"AT\r\n", 4,100);
	//		HAL_Delay(500);
	//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);
	//		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
	//		HAL_Delay(500);
	//		if(strstr(buffer_mqtt,"SIM not inserted"))
	//		{
	//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	//			HAL_Delay(500);
	//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	//		}
	//		else if(strstr(buffer_mqtt,"OK"))
	//		{
	//			//UARTPrint("MODEM READY\r\n");
	//			//	glcd_int_puts(modem_ok, 0, 0);
	//			error_count=0;
	//			break;
	//		}
	//		else
	//		{
	//			//UARTPrint("MODEM ERROR\r\n");
	//			//glcd_int_puts(modem_error, 0, 0);
	//			//auth_flag=0;
	//			error_count++;
	//			if(error_count>15)
	//			{
	//				error_count=0;
	//				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
	//				HAL_Delay(500);
	//				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	//			}
	//		}
	//	}
	//	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CGDCONT=1,\"IP\",\"internet\"\r\n",34,500);
	//	for(int i=0;i<10;i++)
	//	{
	//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);
	//		//HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer, 100);
	//		//Uart_sendstring("AT+CMQTTREL=0\r\n");
	//		HAL_Delay(100);
	//	}
	//	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CMQTTREL=0\r\n",15,100);
	//	for(int i=0;i<10;i++)
	//	{
	//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);
	//		//HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buffer, 100);
	//		//Uart_sendstring("AT+CMQTTREL=0\r\n");
	//		HAL_Delay(100);
	//	}
	//	memset(buffer_mqtt, 9, sizeof(buffer_mqtt));
	//	//Uart_sendstring("AT+CMQTTSTOP\r\n");
	//	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CMQTTSTOP\r\n",14,100);
	//	for(int i=0;i<10;i++)
	//	{
	//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);
	//
	//		//Uart_sendstring("AT+CMQTTREL=0\r\n");
	//		HAL_Delay(100);
	//	}
	//	//Uart_flush();
	//	memset(buffer_mqtt, 9, sizeof(buffer_mqtt));
	//	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CMQTTSTART\r\n",15,100);
	//	for(int i=0;i<10;i++)
	//	{
	//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);
	//
	//		//Uart_sendstring("AT+CMQTTREL=0\r\n");
	//		HAL_Delay(100);
	//	}
	//
	//	//Uart_sendstring("AT+CMQTTSTART\r\n");
	//	HAL_Delay(1000);
	//
	//
	//
	//	while(strstr( buffer_mqtt,"23") == NULL && strstr( buffer_mqtt,"0") == NULL)
	//	{
	//		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CMQTTSTART\r\n",15,100);
	//		HAL_Delay(300);
	//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);
	//
	//		if(mqtt_err>10)
	//		{
	//			return 0;
	//		}
	//		else
	//		{
	//			mqtt_err++;
	//		}
	//		//Get_after("",1024, buffer);
	//		//UARTPrint( buffer_mqtt);
	//		HAL_Delay(300);
	//	}
	//	mqtt_err=0;
	//	sprintf(data,"AT+CMQTTACCQ=0,\"%s\"\r\n","bob");
	//	//Uart_sendstring(data);
	//	HAL_UART_Transmit(&huart1, (uint8_t*)data, strlen(data),strlen(data)*4);
	//	HAL_Delay(1000);
	//	//Uart_flush();
	//	memset(buffer_mqtt, 9, sizeof(buffer_mqtt));
	//	sprintf(data,"AT+CMQTTCONNECT=0,\"%s\",200,1,\"%s\",\"%s\"\r\n","tcp://unitek.irz.ru:1883", "bob","GG168jcbD");
	//	//Uart_sendstring(data);
	//	HAL_UART_Transmit(&huart1, (uint8_t*)data, strlen(data),strlen(data)*4);
	//	HAL_Delay(1000);
	//
	//	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);
	//	for(int i=0;i<10;i++)
	//	{
	//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);
	//
	//		//Uart_sendstring("AT+CMQTTREL=0\r\n");
	//		HAL_Delay(100);
	//	}
	//	while(strstr( buffer_mqtt,"CONNECT:") == NULL)
	//	{
	//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);
	//		if(mqtt_err>10)
	//		{
	//			return 0;
	//		}
	//		else
	//		{
	//			mqtt_err++;
	//		}
	//		//Get_after("",1024, buffer);
	//		//UARTPrint( buffer_mqtt);
	//		HAL_Delay(300);
	//
	//	}
	//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)"INIT\n", 5);
	//	initFlag = 1;
	//	init_ready=false;
	//	//Uart_flush();
	//
	//	return initFlag;
}
/*Инициализация TLS. Будет дооступна в следующих обновлениях*/
void TLS_Init()
{
	HAL_UART_Transmit(&huart2,  (uint8_t*)"AT+CSSLCFG=\"sslversion\",0,4\n", 32, 2000);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,  (uint8_t*)"AT+CSSLCFG=\"authmode\",0,0\n", 30, 2000);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart2, (uint8_t*) "AT-CSSLCFG=\"ignorelocaltime\",0,4", 20, 2000);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,  (uint8_t*)"AT+CCHSET=1\n", 14, 2000);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,  (uint8_t*)"AT+CCHSTART\n", 14, 2000);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,  (uint8_t*)"AT+CCHSSLCFG=0,0\n", 18, 2000);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart2,  (uint8_t*)"AT+CCHOPEN=0,\"TLSserver\",443,2\n", 35, 2000);
	HAL_Delay(1000);

}

/*Публикация собщения по mqtt необходимо передать названия топика(не более 1024 символов) и сообщение(не более 1024 символов)*/
void Mqtt_Publish(char* topic,char* message)
{
	char data[256];
	topic=(char*)"1001999";
	message=(char*)"test";
	memset(buffer_mqtt, 9, sizeof(buffer_mqtt));
	int error_count=0;
	while(1)
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT\r\n", 4,100);
		HAL_Delay(500);
		HAL_UART_Receive_DMA(&huart1,  (uint8_t*)buffer_mqtt, 500);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
		HAL_Delay(500);
		if(strstr(buffer_mqtt,"SIM not inserted"))
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
		}
		else if(strstr(buffer_mqtt,"OK"))
		{
			//UARTPrint("MODEM READY\r\n");
			//	glcd_int_puts(modem_ok, 0, 0);
			error_count=0;
			break;
		}
		else
		{
			//UARTPrint("MODEM ERROR\r\n");
			//glcd_int_puts(modem_error, 0, 0);
			//auth_flag=0;
			error_count++;
			if(error_count>15)
			{
				error_count=0;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
				HAL_Delay(500);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
			}
		}
	}
	sprintf(data, "AT+CMQTTTOPIC=0,%d\r\n", strlen(topic));
	//Uart_sendstring(datamqtt);
	HAL_UART_Transmit(&huart1, (uint8_t*)data, strlen(data), strlen(data)*4);
	HAL_Delay(100);
	HAL_UART_Transmit(&huart1, (uint8_t*)topic, strlen(topic), strlen(topic)*4);
	//Uart_sendstring(topic);
	HAL_Delay(100);

	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2,50);
	//Uart_sendstring("\r\n");
	HAL_Delay(100);
	for(int i=0;i<10;i++)
	{
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);

		//Uart_sendstring("AT+CMQTTREL=0\r\n");
		HAL_Delay(100);
	}
	memset(buffer_mqtt, 9, sizeof(buffer_mqtt));
	sprintf(data, "AT+CMQTTPAYLOAD=0,%d\r\n", strlen(message));
	//Uart_sendstring(datamqtt);
	HAL_UART_Transmit(&huart1, (uint8_t*)data, strlen(data), strlen(data)*4);
	HAL_Delay(100);

	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), strlen(message)*4);
	//Uart_sendstring(message);
	HAL_Delay(100 + strlen(message));
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2,50);
	HAL_Delay(100);
	for(int i=0;i<10;i++)
	{
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);

		//Uart_sendstring("AT+CMQTTREL=0\r\n");
		HAL_Delay(100);
	}
	memset(buffer_mqtt, 9, sizeof(buffer_mqtt));
	//Uart_sendstring("AT+CMQTTPUB=0,1,60\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CMQTTPUB=0,1,60\r\n",20, strlen(buffer_mqtt)*4);
	HAL_Delay(100);
	for(int i=0;i<10;i++)
	{
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*) buffer_mqtt, 100);

		//Uart_sendstring("AT+CMQTTREL=0\r\n");
		HAL_Delay(100);
	}
	memset(buffer_mqtt, 9, sizeof(buffer_mqtt));
	//Uart_flush();
	pub_ready=false;
}

/* Подписка на топик, если после функции идет ещё функция взаидоействующая с uart необходима задрежка 3000мс*/
char* Mqtt_Subscribe(const char* topic1, int size)
{
	UARTPrint("MQTT TOPIC: ");
	UARTPrint(topic1);
	UARTPrint("\n");

	char datamqtt1[256];


	sprintf(datamqtt1, "AT+CMQTTSUBTOPIC=0,%03d,1\r\n", size);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)datamqtt1, strlen(datamqtt1));
	//	Uart_sendstring(datamqtt1);

	HAL_Delay(100);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)topic1, strlen(topic1));
	//Uart_sendstring(topic1);
	HAL_Delay(100);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"\r\n", 2);
	//	Uart_sendstring("\r\n");
	HAL_Delay(100);

	//Uart_sendstring("AT+CMQTTSUB=0,4,1\r\n");
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"AT+CMQTTSUB=0,4,1\r\n", 19);
	HAL_Delay(100);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"HAII\r\n", 6);
	//Uart_sendstring("HAII\r\n");//не понятный костыль из документации по sim7600, но без него не работает
	HAL_Delay(100);
	//	Uart_flush();
}

/*Прием сообщения mqtt возвращает и имя топика и сообщение
 * Читаем входящий поток по uart ищем слово +CMQTTRXEND: 0 после
 * этого отбрасываем служебную информаци и записываем название топика и текст сообщения в структуру message_mqtt_recive */

message_mqtt_recive Mqtt_Recive()
{
	message_mqtt_recive message;
	char rxmessage[1024];
	char filtrmessage[1000];
	char *rxtopic;
	int flagmqtt=1;

	int count=80;
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t*)rxmessage, 1024);
	//Get_after("",1024, rxmessage);

	UARTPrint(rxmessage);
	if(strstr(rxmessage,"CMQTTRXSTART:") == NULL) //ждем пока что-то придет на вход по подписке
	{
		memcpy(message.topic,0,100);
		return message;
	}


	char* ist = strstr(rxmessage,"CMQTTRXTOPIC:");
	char* ist2 = strstr(ist,"\r\n");


	for(int i=2;i<strlen(ist2);i++)//пропускаем перевый перенос строки и копируем имя топика до следующего
	{
		if(ist2[i] == '\r')
		{
			message.topic[i-2] = '\0';
			break;
		}
		message.topic[i-2] = ist2[i];
	}

	ist = strstr(rxmessage,"MQTTRXPAYLOAD:");
	ist2 = strstr(ist,"\r\n");

	for(int i=2;i<strlen(ist2);i++)//пропускаем перевый перенос строки и копируем имя топика до следующего
	{
		if(ist2[i] == '\r')
		{
			message.payload[i-2] = '\0';
			break;
		}
		message.payload[i-2] = ist2[i];
	}

	return message;
}




