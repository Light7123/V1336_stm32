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

void mqtt_init(string serv, string user)
{
	string ch="+init,"+serv+","+user;
	HAL_UART_Transmit(&huart1, (uint8_t*)ch.c_str(), ch.length(), ch.length()*3);

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
