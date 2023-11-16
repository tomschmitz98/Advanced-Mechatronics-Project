/*
 * crc32.h
 *
 *  Created on: Jun 10, 2023
 *      Author: Tom
 */

#ifndef CRC32_H_
#define CRC32_H_

#include <stdint.h>

uint32_t crc32_compute(uint32_t data);
void crc32_reset(void);
void crc32_store_byte(uint8_t data);
uint8_t crc32_read_byte(void);

#endif /* CRC32_H_ */
