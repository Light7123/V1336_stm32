/*
 * sim.h
 *
 *  Created on: Nov 28, 2023
 *      Author: 566kb523
 */

#ifndef INC_SIM_H_
#define INC_SIM_H_



void mqtt_init(string serv, string user,string pass);
void mqtt_send(string topic,string message);
string mqtt_recive();
extern string imei;
void mqtt_sub(string topic1, int size);
void publishMessage(const std::string& topic, const std::string& message);
void sendMQTTConnect(const std::string& username, const std::string& password);
void MQTT_SUB(std::string topic);
extern uint8_t buffer_pub[256];
extern uint8_t buffer_sub[256];
extern int buflen;
extern int sub_len;
extern uint8_t packet[256];
extern size_t packetSize;
#endif /* INC_SIM_H_ */
