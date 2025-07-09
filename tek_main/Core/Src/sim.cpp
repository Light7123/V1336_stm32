/*
 * sim.cpp
 *
 *  Created on: Nov 28, 2023
 *      Author: 566kb523
 */


#include "main.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <strings.h>

using namespace std;
string imei="999999";
void mqtt_init(string serv, string user,string pass)
{
	string ch="+init,"+serv+","+user+","+pass+"\n\r";
	HAL_UART_Transmit(&huart1, (uint8_t*)ch.c_str(), ch.length(), ch.length()*2);

}

void mqtt_send(string topic,string message)
{
	string ch="+send,"+topic+","+message;
	HAL_UART_Transmit(&huart1, (uint8_t*)ch.c_str(), ch.length(), ch.length()*3);
}

string mqtt_recive()
{
	//HAL_UART_Transmit(&huart1, (uint8_t*)"res", 3, 100);
	string res;
	uint8_t data[150];
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, data, 150);
	return res;

}

void mqtt_sub(string topic1, int size)
{
	string ch="+subs,"+topic1+",";
	HAL_UART_Transmit(&huart1, (uint8_t*)ch.c_str(), ch.length(), ch.length()*3);
}


uint8_t buffer_pub[256];
uint8_t buffer_sub[256];
int buflen;
int sub_len;
uint8_t packet[256];
size_t packetSize;
void publishMessage(const std::string& topic, const std::string& message) {
	//    // Буфер для хранения пакета
	//     buflen = 0;
	//
	//    // Формирование пакета публикации
	//    buffer_pub[buflen++] = 0b00110001; // Фиксированный заголовок пакета публикации
	//    // Длина переменной части пакета
	//    buflen += snprintf(reinterpret_cast<char*>( buffer_pub + buflen), sizeof( buffer_pub) - buflen, "\x00\x04%s", topic.c_str());
	//    // Идентификатор сообщения
	//    buffer_pub[buflen++] = 0x00;
	//    buffer_pub[buflen++] = 0x01; // Номер сообщения
	//    // Добавление содержимого сообщения
	//    buflen += snprintf(reinterpret_cast<char*>( buffer_pub + buflen), sizeof( buffer_pub) - buflen, "%s", payload.c_str());
	//
	//    // Отправка пакета на сервер
	//  //  send(socket, buffer, buflen, 0);


	std::string clientID = imei;
	std::string topicFlag, messageFlag;

	if (!topic.empty()) {
		topicFlag = "\x00\x04" "user" + topic;
	}

	if (!message.empty()) {
		messageFlag = "\x00\x04" "pass" + message;
	}

	buflen = 6 +topicFlag.size() + messageFlag.size();


	size_t pos = 0;
//	buffer_pub[pos++]='p';
//	buffer_pub[pos++]='u';
//	buffer_pub[pos++]='b';
//	buffer_pub[pos++]=':';
//	buffer_pub[pos++]=' ';
	buffer_pub[pos++] = 0x30; // Заголовок pub
	buffer_pub[pos++] = buflen - 4; // Длина сообщения
	buffer_pub[pos++] = 0x00; buffer_pub[pos++] = topicFlag.size();
	memcpy(buffer_pub + pos, topicFlag.c_str(), topicFlag.size());
	pos += topicFlag.size();
	//	    buffer_pub[pos++] = 0x00; // Версия протокола 3.1.1
	//	    buffer_pub[pos++] = 0x01; // Флаги CONNECT (Clean Session = 1, уровень QoS = 0, оставить <Will> = 0, Retain = 0)
	memcpy(buffer_pub + pos, messageFlag.c_str(), messageFlag.size());
	pos += messageFlag.size();
//	buffer_pub[pos++]='\n';
//	buffer_pub[pos++]='\r';
}


void sendMQTTConnect(const std::string& username, const std::string& password) {
	// Формирование пакета CONNECT
	std::string clientID = imei;
	std::string usernameFlag, passwordFlag;

	if (!username.empty()) {
		usernameFlag = "\x00\x04" "user" + username;
	}

	if (!password.empty()) {
		passwordFlag = "\x00\x04" "pass" + password;
	}

	packetSize = 20 +clientID.size() + usernameFlag.size() + passwordFlag.size();


	size_t pos = 0;
	packet[pos++]='i';
	packet[pos++]='n';
	packet[pos++]='i';
	packet[pos++]=':';
	packet[pos++]=' ';
	packet[pos++] = 0x10; // Заголовок CONNECT
	packet[pos++] = packetSize - 4; // Длина сообщения
	packet[pos++] = 0x00; packet[pos++] = 0x04;
	packet[pos++] = 'M'; packet[pos++] = 'Q'; packet[pos++] = 'T'; packet[pos++] = 'T';
	packet[pos++] = 0x4; // Версия протокола 3.1.1
	packet[pos++] = 0xC0; // Флаги CONNECT (Clean Session = 1, уровень QoS = 0, оставить <Will> = 0, Retain = 0)
	packet[pos++] = 0x00; packet[pos++] = 0x3C; // Время жизни сессии (60 секунд)
	packet[pos++] = 0x00; packet[pos++]=clientID.size();
	memcpy(packet + pos, clientID.c_str(), clientID.size());
	pos += clientID.size();
	packet[pos++] = 0x00; packet[pos++] = usernameFlag.size();
	memcpy(packet + pos, usernameFlag.c_str(), usernameFlag.size());
	pos += usernameFlag.size();
	packet[pos++] = 0x00; packet[pos++] = passwordFlag.size();
	memcpy(packet + pos, passwordFlag.c_str(), passwordFlag.size());
	pos += passwordFlag.size();
	packet[pos++]='\n';
	packet[pos++]='\r';
	// Отправка пакета на сервер
	//send(socket, packet, packetSize, 0);
}


void MQTT_SUB(std::string topic)
{
	//std::string clientID = "1001999";
	std::string topicFlag, messageFlag;

	if (!topic.empty()) {
		topicFlag = "\x00\x04" "user" + topic;
	}


	sub_len = 9 +topicFlag.size();


	size_t pos = 0;
	buffer_sub[pos++]='s';
	buffer_sub[pos++]='u';
	buffer_sub[pos++]='b';
	buffer_sub[pos++]=':';
	buffer_sub[pos++]=' ';
	buffer_sub[pos++] = 0x82; // Заголовок pub
	buffer_sub[pos++] = sub_len - 4; // Длина сообщения
	buffer_sub[pos++] = 0x00;
	buffer_sub[pos++] = sub_len*rand();
	buffer_sub[pos++] = 0x00; buffer_sub[pos++] = topicFlag.size();
	memcpy(buffer_sub + pos, topicFlag.c_str(), topicFlag.size());
	pos += topicFlag.size();
	buffer_sub[pos++]=0x01;
	buffer_sub[pos++]='\n';
	buffer_sub[pos++]='\r';
	//	    buffer_pub[pos++] = 0x00; // Версия протокола 3.1.1
	//	    buffer_pub[pos++] = 0x01; // Флаги CONNECT (Clean Session = 1, уровень QoS = 0, оставить <Will> = 0, Retain = 0)
	//	memcpy(buffer_pub + pos, messageFlag.c_str(), messageFlag.size());
	//	pos += topicFlag.size();
}
