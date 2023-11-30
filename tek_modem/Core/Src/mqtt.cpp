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
#include "UartRingbuffer.h"


using namespace std;


typedef struct
{
	char topic[100];
	char payload[256];
} message_mqtt_recive;




//Uart_sendstring("AT+HTTPACTION=1\r\n");
//HAL_Delay(1500);
//Uart_flush();
//Get_after("",1024, buffer);
////	HAL_Delay(2000);
//UARTPrint(buffer);
//HAL_Delay(300);



/*Инициализация модема и mqtt необходимо передать apn, адрес сервера и имя пользователя
 * если модем уже запущен, а контролер перезагрузился то это тоже работает*/
int Mqtt_Init(char* serv, char* user)
{
	char mqttInit[100];
	int initFlag = 0;
	char data[100];
	char buffer[1024];

	Uart_sendstring("AT+CMQTTREL=0\r\n");
	HAL_Delay(100);
	Uart_sendstring("AT+CMQTTSTOP\r\n");
	HAL_Delay(100);
	Uart_flush();

	Uart_sendstring("AT+CMQTTSTART\r\n");
	HAL_Delay(1000);



	while(strstr(buffer,"23") == NULL && strstr(buffer,"0") == NULL)
	{

		Get_after("",1024, buffer);
		UARTPrint(buffer);
		HAL_Delay(300);
		initFlag = 1;
	}
	sprintf(data,"AT+CMQTTACCQ=0,\"%s\"\r\n",user);
	Uart_sendstring(data);
	HAL_Delay(1000);
	Uart_flush();

	sprintf(data,"AT+CMQTTCONNECT=0,\"%s\",90,1\r\n",serv);
	Uart_sendstring(data);
	HAL_Delay(1000);


	while(strstr(buffer,"CONNECT:") == NULL)
	{

		Get_after("",1024, buffer);
		UARTPrint(buffer);
		HAL_Delay(300);
		initFlag = 1;
	}
	Uart_flush();

	return initFlag;
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
	char datamqtt[256];
	sprintf(datamqtt, "AT+CMQTTTOPIC=0,%03d\r\n", strlen(topic));
	Uart_sendstring(datamqtt);
	HAL_Delay(100);
	Uart_sendstring(topic);
	HAL_Delay(100);
	Uart_sendstring("\r\n");
	HAL_Delay(100);
	sprintf(datamqtt, "AT+CMQTTPAYLOAD=0,%03d\r\n", strlen(message));
	Uart_sendstring(datamqtt);
	HAL_Delay(100);
	Uart_sendstring(message);
	HAL_Delay(100 + strlen(message));
	Uart_sendstring("\r\n");
	HAL_Delay(100);
	Uart_sendstring("AT+CMQTTPUB=0,1,60\r\n");
	HAL_Delay(100);
	Uart_flush();

}

/* Подписка на топик, если после функции идет ещё функция взаидоействующая с uart необходима задрежка 3000мс*/
char* Mqtt_Subscribe(const char* topic1, int size)
{
	UARTPrint("MQTT TOPIC: ");
	UARTPrint(topic1);
	UARTPrint("\n");

	char datamqtt1[256];


	sprintf(datamqtt1, "AT+CMQTTSUBTOPIC=0,%03d,1\r\n", size);
	Uart_sendstring(datamqtt1);
	HAL_Delay(100);

	Uart_sendstring(topic1);
	HAL_Delay(100);
	Uart_sendstring("\r\n");
	HAL_Delay(100);

	Uart_sendstring("AT+CMQTTSUB=0,4,1\r\n");
	HAL_Delay(100);

	Uart_sendstring("HAII\r\n");//не понятный костыль из документации по sim7600, но без него не работает
	HAL_Delay(100);
	Uart_flush();
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

	Get_after("",1024, rxmessage);

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




