/*
 * modbus.h
 *
 *  Created on: Nov 28, 2023
 *      Author: 566kb523
 */

#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_

#include "main.h"
#include "modbus_crc.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <strings.h>

using namespace std;


void sendData_rs485(uint8_t *data, int size);
void write_modbus(uint8_t slave_address,uint8_t register_high,uint8_t register_low, int num_reg, uint8_t* data );
string read_modbus(uint8_t slave_address,uint8_t register_high,uint8_t register_low, int num_reg);


#endif /* INC_MODBUS_H_ */
