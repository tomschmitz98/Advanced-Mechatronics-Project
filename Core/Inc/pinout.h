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

#define PIN_A0  bank_a, 0x0
#define PIN_A1  bank_a, 0x1
#define PIN_A2  bank_a, 0x2
#define PIN_A3  bank_a, 0x3
#define PIN_A4  bank_a, 0x4
#define PIN_A5  bank_a, 0x5
#define PIN_A6  bank_a, 0x6
#define PIN_A7  bank_a, 0x7
#define PIN_A8  bank_a, 0x8
#define PIN_A9  bank_a, 0x9
#define PIN_A10 bank_a, 0xA
#define PIN_A11 bank_a, 0xB
#define PIN_A12 bank_a, 0xC
#define PIN_A13 bank_a, 0xD
#define PIN_A14 bank_a, 0xE
#define PIN_A15 bank_a, 0xF

#define PIN_B0  bank_b, 0x0
#define PIN_B1  bank_b, 0x1
#define PIN_B2  bank_b, 0x2
#define PIN_B3  bank_b, 0x3
#define PIN_B4  bank_b, 0x4
#define PIN_B5  bank_b, 0x5
#define PIN_B6  bank_b, 0x6
#define PIN_B7  bank_b, 0x7
#define PIN_B8  bank_b, 0x8
#define PIN_B9  bank_b, 0x9
#define PIN_B10 bank_b, 0xA
#define PIN_B11 bank_b, 0xB
#define PIN_B12 bank_b, 0xC
#define PIN_B13 bank_b, 0xD
#define PIN_B14 bank_b, 0xE
#define PIN_B15 bank_b, 0xF

#define PIN_C0  bank_c, 0x0
#define PIN_C1  bank_c, 0x1
#define PIN_C2  bank_c, 0x2
#define PIN_C3  bank_c, 0x3
#define PIN_C4  bank_c, 0x4
#define PIN_C5  bank_c, 0x5
#define PIN_C6  bank_c, 0x6
#define PIN_C7  bank_c, 0x7
#define PIN_C8  bank_c, 0x8
#define PIN_C9  bank_c, 0x9
#define PIN_C10 bank_c, 0xA
#define PIN_C11 bank_c, 0xB
#define PIN_C12 bank_c, 0xC
#define PIN_C13 bank_c, 0xD
#define PIN_C14 bank_c, 0xE
#define PIN_C15 bank_c, 0xF

#define PIN_D0  bank_d, 0x0
#define PIN_D1  bank_d, 0x1
#define PIN_D2  bank_d, 0x2
#define PIN_D3  bank_d, 0x3
#define PIN_D4  bank_d, 0x4
#define PIN_D5  bank_d, 0x5
#define PIN_D6  bank_d, 0x6
#define PIN_D7  bank_d, 0x7
#define PIN_D8  bank_d, 0x8
#define PIN_D9  bank_d, 0x9
#define PIN_D10 bank_d, 0xA
#define PIN_D11 bank_d, 0xB
#define PIN_D12 bank_d, 0xC
#define PIN_D13 bank_d, 0xD
#define PIN_D14 bank_d, 0xE
#define PIN_D15 bank_d, 0xF

#define PIN_E0  bank_e, 0x0
#define PIN_E1  bank_e, 0x1
#define PIN_E2  bank_e, 0x2
#define PIN_E3  bank_e, 0x3
#define PIN_E4  bank_e, 0x4
#define PIN_E5  bank_e, 0x5
#define PIN_E6  bank_e, 0x6
#define PIN_E7  bank_e, 0x7
#define PIN_E8  bank_e, 0x8
#define PIN_E9  bank_e, 0x9
#define PIN_E10 bank_e, 0xA
#define PIN_E11 bank_e, 0xB
#define PIN_E12 bank_e, 0xC
#define PIN_E13 bank_e, 0xD
#define PIN_E14 bank_e, 0xE
#define PIN_E15 bank_e, 0xF

#define PIN_F0  bank_f, 0x0
#define PIN_F1  bank_f, 0x1
#define PIN_F2  bank_f, 0x2
#define PIN_F3  bank_f, 0x3
#define PIN_F4  bank_f, 0x4
#define PIN_F5  bank_f, 0x5
#define PIN_F6  bank_f, 0x6
#define PIN_F7  bank_f, 0x7
#define PIN_F8  bank_f, 0x8
#define PIN_F9  bank_f, 0x9
#define PIN_F10 bank_f, 0xA
#define PIN_F11 bank_f, 0xB
#define PIN_F12 bank_f, 0xC
#define PIN_F13 bank_f, 0xD
#define PIN_F14 bank_f, 0xE
#define PIN_F15 bank_f, 0xF

#define PIN_G0  bank_g, 0x0
#define PIN_G1  bank_g, 0x1
#define PIN_G2  bank_g, 0x2
#define PIN_G3  bank_g, 0x3
#define PIN_G4  bank_g, 0x4
#define PIN_G5  bank_g, 0x5
#define PIN_G6  bank_g, 0x6
#define PIN_G7  bank_g, 0x7
#define PIN_G8  bank_g, 0x8
#define PIN_G9  bank_g, 0x9
#define PIN_G10 bank_g, 0xA
#define PIN_G11 bank_g, 0xB
#define PIN_G12 bank_g, 0xC
#define PIN_G13 bank_g, 0xD
#define PIN_G14 bank_g, 0xE
#define PIN_G15 bank_g, 0xF

#define PIN_H0  bank_h, 0x0
#define PIN_H1  bank_h, 0x1
#define PIN_H2  bank_h, 0x2
#define PIN_H3  bank_h, 0x3
#define PIN_H4  bank_h, 0x4
#define PIN_H5  bank_h, 0x5
#define PIN_H6  bank_h, 0x6
#define PIN_H7  bank_h, 0x7
#define PIN_H8  bank_h, 0x8
#define PIN_H9  bank_h, 0x9
#define PIN_H10 bank_h, 0xA
#define PIN_H11 bank_h, 0xB
#define PIN_H12 bank_h, 0xC
#define PIN_H13 bank_h, 0xD
#define PIN_H14 bank_h, 0xE
#define PIN_H15 bank_h, 0xF

#endif /* PINOUT_H_ */
