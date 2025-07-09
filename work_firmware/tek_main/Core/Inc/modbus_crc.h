/*
 * modbus_crc.h
 *
 *  Created on: Sep 18, 2023
 *      Author: user
 */

#ifndef INC_MODBUS_CRC_H_
#define INC_MODBUS_CRC_H_

#ifdef __cplusplus
extern "C" {
#endif
uint16_t crc16_modbus(uint8_t *buffer, uint16_t buffer_length);

#ifdef __cplusplus
 }
#endif
#endif /* INC_MODBUS_CRC_H_ */
