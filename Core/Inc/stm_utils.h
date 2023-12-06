/*
 * stm_utils.h
 *
 *  Created on: Jun 8, 2023
 *      Author: Tom
 */

#ifndef STM_UTILS_H_
#define STM_UTILS_H_

#include <stdint.h>

#define BIT0                  0x0001
#define BIT1                  0x0002
#define BIT2                  0x0004
#define BIT3                  0x0008
#define BIT4                  0x0010
#define BIT5                  0x0020
#define BIT6                  0x0040
#define BIT7                  0x0080
#define BIT8                  0x0100
#define BIT9                  0x0200
#define BITA                  0x0400
#define BITB                  0x0800
#define BITC                  0x1000
#define BITD                  0x2000
#define BITE                  0x4000
#define BITF                  0x8000

#define UPPER16BITS(BITFIELD) ((uint32_t)BITFIELD << 16)

#ifndef SET_BIT
#define SET_BIT(BITFIELD, N) (BITFIELD |= ((uint32_t)0x1 << N))
#endif

#ifndef CLEAR_BIT
#define CLEAR_BIT(BITFIELD, N) (BITFIELD &= ~((uint32_t)0x1 << N))
#endif

#define CHECK_BIT(BITFIELD, N) (((uint32_t)BITFIELD >> N) & 0x1)

typedef enum {
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
