/**
 * @file pinout.h
 * @version 1.0
 * @brief   Defines easy to access macros for the gpio library. This file is
 *          compatible with the STM32F446C and the STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#ifndef PINOUT_H_
#define PINOUT_H_

#include "stm_utils.h"

#define PIN_A0  bank_a, BIT0
#define PIN_A1  bank_a, BIT1
#define PIN_A2  bank_a, BIT2
#define PIN_A3  bank_a, BIT3
#define PIN_A4  bank_a, BIT4
#define PIN_A5  bank_a, BIT5
#define PIN_A6  bank_a, BIT6
#define PIN_A7  bank_a, BIT7
#define PIN_A8  bank_a, BIT8
#define PIN_A9  bank_a, BIT9
#define PIN_A10 bank_a, BITA
#define PIN_A11 bank_a, BITB
#define PIN_A12 bank_a, BITC
#define PIN_A13 bank_a, BITD
#define PIN_A14 bank_a, BITE
#define PIN_A15 bank_a, BITF

#define PIN_B0  bank_b, BIT0
#define PIN_B1  bank_b, BIT1
#define PIN_B2  bank_b, BIT2
#define PIN_B3  bank_b, BIT3
#define PIN_B4  bank_b, BIT4
#define PIN_B5  bank_b, BIT5
#define PIN_B6  bank_b, BIT6
#define PIN_B7  bank_b, BIT7
#define PIN_B8  bank_b, BIT8
#define PIN_B9  bank_b, BIT9
#define PIN_B10 bank_b, BITA
#define PIN_B11 bank_b, BITB
#define PIN_B12 bank_b, BITC
#define PIN_B13 bank_b, BITD
#define PIN_B14 bank_b, BITE
#define PIN_B15 bank_b, BITF

#define PIN_C0  bank_c, BIT0
#define PIN_C1  bank_c, BIT1
#define PIN_C2  bank_c, BIT2
#define PIN_C3  bank_c, BIT3
#define PIN_C4  bank_c, BIT4
#define PIN_C5  bank_c, BIT5
#define PIN_C6  bank_c, BIT6
#define PIN_C7  bank_c, BIT7
#define PIN_C8  bank_c, BIT8
#define PIN_C9  bank_c, BIT9
#define PIN_C10 bank_c, BITA
#define PIN_C11 bank_c, BITB
#define PIN_C12 bank_c, BITC
#define PIN_C13 bank_c, BITD
#define PIN_C14 bank_c, BITE
#define PIN_C15 bank_c, BITF

#define PIN_D0  bank_d, BIT0
#define PIN_D1  bank_d, BIT1
#define PIN_D2  bank_d, BIT2
#define PIN_D3  bank_d, BIT3
#define PIN_D4  bank_d, BIT4
#define PIN_D5  bank_d, BIT5
#define PIN_D6  bank_d, BIT6
#define PIN_D7  bank_d, BIT7
#define PIN_D8  bank_d, BIT8
#define PIN_D9  bank_d, BIT9
#define PIN_D10 bank_d, BITA
#define PIN_D11 bank_d, BITB
#define PIN_D12 bank_d, BITC
#define PIN_D13 bank_d, BITD
#define PIN_D14 bank_d, BITE
#define PIN_D15 bank_d, BITF

#define PIN_E0  bank_e, BIT0
#define PIN_E1  bank_e, BIT1
#define PIN_E2  bank_e, BIT2
#define PIN_E3  bank_e, BIT3
#define PIN_E4  bank_e, BIT4
#define PIN_E5  bank_e, BIT5
#define PIN_E6  bank_e, BIT6
#define PIN_E7  bank_e, BIT7
#define PIN_E8  bank_e, BIT8
#define PIN_E9  bank_e, BIT9
#define PIN_E10 bank_e, BITA
#define PIN_E11 bank_e, BITB
#define PIN_E12 bank_e, BITC
#define PIN_E13 bank_e, BITD
#define PIN_E14 bank_e, BITE
#define PIN_E15 bank_e, BITF

#define PIN_F0  bank_f, BIT0
#define PIN_F1  bank_f, BIT1
#define PIN_F2  bank_f, BIT2
#define PIN_F3  bank_f, BIT3
#define PIN_F4  bank_f, BIT4
#define PIN_F5  bank_f, BIT5
#define PIN_F6  bank_f, BIT6
#define PIN_F7  bank_f, BIT7
#define PIN_F8  bank_f, BIT8
#define PIN_F9  bank_f, BIT9
#define PIN_F10 bank_f, BITA
#define PIN_F11 bank_f, BITB
#define PIN_F12 bank_f, BITC
#define PIN_F13 bank_f, BITD
#define PIN_F14 bank_f, BITE
#define PIN_F15 bank_f, BITF

#define PIN_G0  bank_g, BIT0
#define PIN_G1  bank_g, BIT1
#define PIN_G2  bank_g, BIT2
#define PIN_G3  bank_g, BIT3
#define PIN_G4  bank_g, BIT4
#define PIN_G5  bank_g, BIT5
#define PIN_G6  bank_g, BIT6
#define PIN_G7  bank_g, BIT7
#define PIN_G8  bank_g, BIT8
#define PIN_G9  bank_g, BIT9
#define PIN_G10 bank_g, BITA
#define PIN_G11 bank_g, BITB
#define PIN_G12 bank_g, BITC
#define PIN_G13 bank_g, BITD
#define PIN_G14 bank_g, BITE
#define PIN_G15 bank_g, BITF

#define PIN_H0  bank_h, BIT0
#define PIN_H1  bank_h, BIT1
#define PIN_H2  bank_h, BIT2
#define PIN_H3  bank_h, BIT3
#define PIN_H4  bank_h, BIT4
#define PIN_H5  bank_h, BIT5
#define PIN_H6  bank_h, BIT6
#define PIN_H7  bank_h, BIT7
#define PIN_H8  bank_h, BIT8
#define PIN_H9  bank_h, BIT9
#define PIN_H10 bank_h, BITA
#define PIN_H11 bank_h, BITB
#define PIN_H12 bank_h, BITC
#define PIN_H13 bank_h, BITD
#define PIN_H14 bank_h, BITE
#define PIN_H15 bank_h, BITF

#endif /* PINOUT_H_ */
