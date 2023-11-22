/**
 * @file crc32.h
 * @version 1.0
 *
 * @brief   Hardware abstraction layer for the CRC32 unit on the STM32F446C and
 *          STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#ifndef CRC32_H_
#define CRC32_H_

#include <stdint.h>

/**
 * @brief   Computes the 4-byte CRC of the given 4-byte data word.
 *
 * @param[in] data   The data to perform the CRC calculation on
 *
 * @return   The calculated CRC value
 *
 * @note   This uses the CRC ethernet polynomial (0x4C11DB7).
 */
uint32_t crc32_compute(uint32_t data);

/**
 * @brief   Resets the CRC unit
 */
void crc32_reset(void);

/**
 * @brief   Temporarily store a byte of data in the CRC unit
 *
 * @param[in] data   The data to store
 */
void crc32_store_byte(uint8_t data);

/**
 * @brief   Read the stored data from the CRC unit
 *
 * @return   The stored byte
 */
uint8_t crc32_read_byte(void);

#endif /* CRC32_H_ */
