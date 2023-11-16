/*
 * crc32.c
 *
 *  Created on: Jun 10, 2023
 *      Author: Tom
 */

#include <stdint.h>
#include "crc32.h"

#define CRC_BASE(n) *(((uint32_t *) 0x40023000) + n)

#define CRC_DR 0x0
#define CRC_IDR 0x1
#define CRC_CR 0x2

#define MAX_ITERATIONS 10

uint32_t crc32_compute(uint32_t data)
{
	CRC_BASE(CRC_DR) = data;

	for (uint8_t i = 0; i < MAX_ITERATIONS; i++);

	return CRC_BASE(CRC_DR);
}

void crc32_reset(void)
{
	CRC_BASE(CRC_CR) |= 1;
}

void crc32_store_byte(uint8_t data)
{
	CRC_BASE(CRC_IDR) = data;
}

uint8_t crc32_read_byte(void)
{
	return (uint8_t)CRC_BASE(CRC_IDR);
}
