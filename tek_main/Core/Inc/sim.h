/*
 * sim.h
 *
 *  Created on: Nov 28, 2023
 *      Author: 566kb523
 */

#ifndef INC_SIM_H_
#define INC_SIM_H_



void mqtt_init(string serv, string user);
void mqtt_send(string topic,string message);
string mqtt_recive();
void mqtt_sub(string topic1, int size);

#endif /* INC_SIM_H_ */
