
/*
 * mqtt.h
 *
 *  Created on: 16 нояб. 2022 г.
 *      Author: user
 */

#ifndef INC_MQTT_H_
#define INC_MQTT_H_
#include "main.h"
#include "stdio.h"
#include "string.h"

#include <string>
#include "vector"

using namespace std;


typedef struct
{
	char topic[100];
	char payload[256];
} message_mqtt_recive;

/*Инициализация модема и mqtt необходимо передать apn, адрес сервера и имя пользователя
 * если модем уже запущен, а контролер перезагрузился то это тоже работает*/
int Mqtt_Init();

/*Инициализация TLS. Будет дооступна в следующих обновлениях*/
void TLS_Init();


/*Публикация собщения по mqtt необходимо передать названия топика(не более 1024 символов) и сообщение(не более 1024 символов)*/
void Mqtt_Publish(char* topic,char* message);


/* Подписка на топик, если после функции идет ещё функция взаидоействующая с uart необходима задрежка 3000мс*/
void Mqtt_Subscribe();
int Initv2(string serv, string user, string pass);
extern bool sub;
extern bool pub;
extern char buffer3[1500];
/*Прием сообщения mqtt возвращает и имя топика и сообщение
 * Читаем входящий поток по uart ищем слово +CMQTTRXEND: 0 после
 * этого отбрасываем служебную информаци и записываем название топика и текст сообщения в структуру message_mqtt_recive */

void Mqtt_Recive();
int Mqtt_Init1(string serv, string user, string pass,string topic, string mess);




#endif /* INC_MQTT_H_ */
