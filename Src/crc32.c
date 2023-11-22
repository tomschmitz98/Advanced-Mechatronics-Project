/**
 * @file crc32.c
 * @version 1.0
 *
 * @brief   Hardware abstraction layer for the CRC32 unit on the STM32F446C and
 *          STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#include "crc32.h"
#include <stdint.h>

/* Base address of the CRC32 Calculation Unit */
#define CRC_BASE(n)    *(((uint32_t *)0x40023000) + n)

#define CRC_DR         0x0
#define CRC_IDR        0x1
#define CRC_CR         0x2

#define MAX_ITERATIONS 10

/**
 * @brief   Computes the 4-byte CRC of the given 4-byte data word.
 *
 * @param[in] data   The data to perform the CRC calculation on
 *
 * @return   The calculated CRC value
 *
 * @note   This uses the CRC ethernet polynomial (0x4C11DB7).
 */
uint32_t crc32_compute(uint32_t data)
{
    CRC_BASE(CRC_DR) = data;

    for (uint8_t i = 0; i < MAX_ITERATIONS; i++)
        ;

    return CRC_BASE(CRC_DR);
}

/**
 * @brief   Resets the CRC unit
 */
void crc32_reset(void)
{
    CRC_BASE(CRC_CR) |= 1;
}

/**
 * @brief   Temporarily store a byte of data in the CRC unit
 *
 * @param[in] data   The data to store
 */
void crc32_store_byte(uint8_t data)
{
    CRC_BASE(CRC_IDR) = data;
}

/**
 * @brief   Read the stored data from the CRC unit
 *
 * @return   The stored byte
 */
uint8_t crc32_read_byte(void)
{
    return (uint8_t)CRC_BASE(CRC_IDR);
}
