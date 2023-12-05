/*
 * crappy_uart_code.h
 *
 *  Created on: Dec 5, 2023
 *      Author: Tom
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm_utils.h"

// SR
#define UART_CTS  BIT9
#define UART_LBD  BIT8
#define UART_TXE  BIT7
#define UART_TC   BIT6
#define UART_RXNE BIT5
#define UART_IDLE BIT4
#define UART_ORE  BIT3
#define UART_NF   BIT2
#define UART_FE   BIT1
#define UART_PE   BIT0

// CR1
#define WORD_LENGTH_8 false
#define WORD_LENGTH_9 true

#define WAKE_IDLE false
#define WAKE_ADDRESS_MARK true

#define EVEN_PARITY false
#define ODD_PARITY true

// CR2
#define UART_CLK_POL_LOW false
#define UART_CLK_POL_HIGH true

#define UART_CLK_PHA_FIRST_TR false
#define UART_CLK_PHA_SECOND_TR true

#define UART_LIN_BR_DETECT_10 false
#define UART_LIN_BR_DETECT_11 true

typedef enum {
	UART_1 = 0x3300,
	UART_2 = 0x0000,
	UART_3 = 0x0100,
	UART_4 = 0x0200,
	UART_5 = 0x0300,
	UART_6 = 0x3400
} uart_t;

typedef enum {
	UART_STOP_BITS_1 = 0x0,
	UART_STOP_BITS_05 = 0x1,
	UART_STOP_BITS_2 = 0x2,
	UART_STOP_BITS_15 = 0x3
} uart_stop_bits_t;

typedef struct
{
	// BRR
	uint16_t baud_mantissa;
	uint8_t baud_fraction;

	// CR1
	bool oversample8;
	bool wordlength;
	bool wake;
	bool parity_enable;
	bool parity_type;
	bool pe_int;
	bool txe_int;
	bool tc_int;
	bool rxne_int;
	bool idle_int;
	bool rx_wake;

	// CR2
	bool lin_mode;
	uart_stop_bits_t stop_bits;
	bool sclk;
	bool clkpol;
	bool clkpha;
	bool last_pulse;
	bool lin_int;
	bool break_len;
	uint8_t addr;

	// CR3
	bool onebit_sample;
	bool cts_int;
	bool cts_en;
	bool rts_en;
	bool tx_dma_en;
	bool rx_dma_en;
	bool smartcard_en;
	bool smartcard_nack_en;
	bool half_duplex;
	bool low_power_mode;
	bool irda_en;
	bool error_int;

	// GTPR
	uint8_t guard_time;
	uint8_t lpm_prescaler;
} uart_config_t;

uint32_t read_uart_status(uart_t channel, uint32_t flags);
void acknowledge_uart_status(uart_t channel, uint32_t flags);

// CR1
void uart_disable(uart_t channel);
void uart_enable(uart_t channel);
void enable_uart_tx(uart_t channel);
void enable_uart_rx(uart_t channel);
void disable_uart_tx(uart_t channel);
void disable_uart_rx(uart_t channel);

void send_uart_break(uart_t channel);

uint8_t uart_read_data(uart_t channel);
void uart_write_data(uart_t channel, uint8_t data);

void config_uart(uart_t channel, uart_config_t config);

#endif /* UART_H_ */
