/**
 * @file stm_utils.h
 * @version 1.0
 * @brief   Defines bitfields, useful macros, and any useful, general purpose
 *          types. This file is compatible with the STM32F446C and STM32F446E
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#ifndef STM_UTILS_H_
#define STM_UTILS_H_

#include <stdint.h>

/* Useful 16-bit Bitfield Macros */
#define BIT0 ((uint32_t)0x0001)
#define BIT1 ((uint32_t)0x0002)
#define BIT2 ((uint32_t)0x0004)
#define BIT3 ((uint32_t)0x0008)
#define BIT4 ((uint32_t)0x0010)
#define BIT5 ((uint32_t)0x0020)
#define BIT6 ((uint32_t)0x0040)
#define BIT7 ((uint32_t)0x0080)
#define BIT8 ((uint32_t)0x0100)
#define BIT9 ((uint32_t)0x0200)
#define BITA ((uint32_t)0x0400)
#define BITB ((uint32_t)0x0800)
#define BITC ((uint32_t)0x1000)
#define BITD ((uint32_t)0x2000)
#define BITE ((uint32_t)0x4000)
#define BITF ((uint32_t)0x8000)

/* Useful macro for using 16-bit macros in the upper 16-bits */
#define UPPER16BITS(BITFIELD) (((uint32_t)BITFIELD) << 16)

/* Useful macros for setting, clearing, and checking a certain bit */
#define SET_BIT(BITFIELD, N)   (BITFIELD |= ((uint32_t)0x1 << N))
#define CLEAR_BIT(BITFIELD, N) (BITFIELD &= ~((uint32_t)0x1 << N))
#define CHECK_BIT(BITFIELD, N) ((BITFIELD >> N) & 0x1)

/**
 * @brief   Defines the different GPIO banks
 */
typedef enum
{
    bank_a = 0x000,
    bank_b = 0x100,
    bank_c = 0x200,
    bank_d = 0x300,
    bank_e = 0x400,
    bank_f = 0x500,
    bank_g = 0x600,
    bank_h = 0x700
} gpio_bank_t;

#endif /* STM_UTILS_H_ */
