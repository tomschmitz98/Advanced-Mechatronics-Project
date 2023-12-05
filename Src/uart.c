/*
 * uart.c
 *
 *  Created on: Dec 5, 2023
 *      Author: Tom
 */

#include "uart.h"
#include "stm_rcc.h"
#include <stdint.h>
#include <assert.h>

#define UART_BASE(n, x) *(((uint32_t *)0x40004400) + n + x)
#define UART_SR 0
#define UART_DR 1
#define USART_BRR 2
#define USART_CR1 3
#define USART_CR2 4
#define USART_CR3 5
#define USART_GTPR 6

uint32_t read_uart_status(uart_t channel, uint32_t flags)
{
	return UART_BASE((uint32_t)channel, UART_SR) & flags;
}

void acknowledge_uart_status(uart_t channel, uint32_t flags)
{
	UART_BASE((uint32_t)channel, UART_SR) = ~flags;
}

static bool check_oversample8(uart_t channel)
{
	return (UART_BASE((uint32_t)channel, USART_CR1) & BITF) != 0;
}

static void set_brr(uart_t channel, uint16_t mantissa, uint8_t fraction)
{
	UART_BASE((uint32_t)channel, USART_BRR) = ((uint32_t)(mantissa & 0xFFF)) << 4;
	if (check_oversample8(channel))
	{
		UART_BASE((uint32_t)channel, USART_BRR) |= (uint32_t)(fraction & 0xF);
	}
}

static void set_oversample8(uart_t channel, bool oversample8)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BITF;
	if (oversample8)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BITF;
	}
}

void uart_disable(uart_t channel)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BITD;
}

void uart_enable(uart_t channel)
{
	UART_BASE((uint32_t)channel, USART_CR1) |= BITD;
}

static void set_word_length(uart_t channel, bool length)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BITC;
	if (length)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BITC;
	}
}

static void set_wake(uart_t channel, bool wake)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BITB;
	if (wake)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BITB;
	}
}

static void set_parity_control(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BITA;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BITA;
	}
}

static void select_parity(uart_t channel, bool selection)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BIT9;
	if (selection)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BIT9;
	}
}

static void set_pe_int(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BIT8;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BIT8;
	}
}

static void set_txe_int(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BIT7;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BIT7;
	}
}

static void set_tc_int(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BIT6;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BIT6;
	}
}

static void set_rxne_int(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BIT5;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BIT5;
	}
}

static void set_idle_int(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BIT4;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BIT4;
	}
}

void enable_uart_tx(uart_t channel)
{
	UART_BASE((uint32_t)channel, USART_CR1) |= BIT3;
}

void enable_uart_rx(uart_t channel)
{
	UART_BASE((uint32_t)channel, USART_CR1) |= BIT2;
}

void disable_uart_tx(uart_t channel)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BIT3;
}

void disable_uart_rx(uart_t channel)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BIT2;
}

static void set_rx_wake(uart_t channel, bool mode)
{
	UART_BASE((uint32_t)channel, USART_CR1) &= ~BIT1;
	if (mode)
	{
		UART_BASE((uint32_t)channel, USART_CR1) |= BIT1;
	}
}

void send_uart_break(uart_t channel)
{
	UART_BASE((uint32_t)channel, USART_CR1) |= BIT0;
}

static void set_lin_mode(uart_t channel, bool lin)
{
	UART_BASE((uint32_t)channel, USART_CR2) &= ~BITE;
	if (lin)
	{
		UART_BASE((uint32_t)channel, USART_CR2) |= BITE;
	}
}

static void set_stop_bits(uart_t channel, uart_stop_bits_t stop_bits)
{
	UART_BASE((uint32_t)channel, USART_CR2) &= ~(BITD | BITC);
	UART_BASE((uint32_t)channel, USART_CR2) |= ((uint32_t)stop_bits) << 12;
}

static void set_sclk(uart_t channel, bool sclk)
{
	UART_BASE((uint32_t)channel, USART_CR2) &= ~BITB;
	if (sclk)
	{
		UART_BASE((uint32_t)channel, USART_CR2) |= BITB;
	}
}

static void set_clkpol(uart_t channel, bool pol)
{
	UART_BASE((uint32_t)channel, USART_CR2) &= ~BITA;
	if (pol)
	{
		UART_BASE((uint32_t)channel, USART_CR2) |= BITA;
	}
}

static void set_clkpha(uart_t channel, bool pha)
{
	UART_BASE((uint32_t)channel, USART_CR2) &= ~BIT9;
	if (pha)
	{
		UART_BASE((uint32_t)channel, USART_CR2) |= BIT9;
	}
}

static void set_last_clk_pulse(uart_t channel, bool last_pulse)
{
	UART_BASE((uint32_t)channel, USART_CR2) &= ~BIT8;
	if (last_pulse)
	{
		UART_BASE((uint32_t)channel, USART_CR2) |= BIT8;
	}
}

static void set_lbd_int(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR2) &= ~BIT6;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR2) |= BIT6;
	}
}

static void set_lbd_length(uart_t channel, bool length)
{
	UART_BASE((uint32_t)channel, USART_CR2) &= ~BIT5;
	if (length)
	{
		UART_BASE((uint32_t)channel, USART_CR2) |= BIT5;
	}
}

static void set_addr(uart_t channel, uint8_t addr)
{
	const uint32_t mask = (BIT3 | BIT2 | BIT1 | BIT0);
	UART_BASE((uint32_t)channel, USART_CR2) &= ~mask;
	UART_BASE((uint32_t)channel, USART_CR2) |= ((uint32_t)addr & mask);
}

static void set_onebit(uart_t channel, bool onebit)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BITB;
	if (onebit)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BITB;
	}
}

static void set_cts_int(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BITA;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BITA;
	}
}

static void set_cts(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT9;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT9;
	}
}

static void set_rts(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT8;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT8;
	}
}

static void set_dma_tx(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT7;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT7;
	}
}

static void set_dma_rx(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT6;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT6;
	}
}

static void set_smartcard(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT5;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT5;
	}
}

static void set_nack(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT4;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT4;
	}
}

static void set_half_duplex(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT3;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT3;
	}
}

static void set_irda_low_power(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT2;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT2;
	}
}

static void set_irda(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT1;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT1;
	}
}

static void set_err_int(uart_t channel, bool enable)
{
	UART_BASE((uint32_t)channel, USART_CR3) &= ~BIT0;
	if (enable)
	{
		UART_BASE((uint32_t)channel, USART_CR3) |= BIT0;
	}
}

static void set_guard_time(uart_t channel, uint8_t guard_time, uint8_t prescale)
{
	if (!prescale)
	{
		assert(false);
	}

	UART_BASE((uint32_t)channel, USART_GTPR) = (uint32_t)prescale;
	UART_BASE((uint32_t)channel, USART_GTPR) |= ((uint32_t)guard_time) << 8;
}

uint8_t uart_read_data(uart_t channel)
{
	return (uint8_t)UART_BASE((uint32_t)channel, UART_DR);
}

void uart_write_data(uart_t channel, uint8_t data)
{
	while (read_uart_status(channel, UART_RXNE))
		;
	UART_BASE((uint32_t)channel, UART_DR) = (uint32_t)data;
}

static void enable_uart_clock(uart_t channel)
{
	switch(channel)
	{
	case UART_1: enable_peripheral_clock(UART1_EN);
		break;
	case UART_2: enable_peripheral_clock(UART2_EN);
		break;
	case UART_3: enable_peripheral_clock(UART3_EN);
		break;
	case UART_4: enable_peripheral_clock(UART4_EN);
		break;
	case UART_5: enable_peripheral_clock(UART5_EN);
		break;
	case UART_6: enable_peripheral_clock(UART6_EN);
		break;
	default:
		assert(false);
	}
}

void config_uart(uart_t channel, uart_config_t config)
{
	enable_uart_clock(channel);

	uart_disable(channel);
	disable_uart_tx(channel);
	disable_uart_rx(channel);

	set_oversample8(channel, config.oversample8);
	set_brr(channel, config.baud_mantissa, config.baud_fraction);
	set_word_length(channel, config.wordlength);
	set_wake(channel, config.wake);
	set_parity_control(channel, config.parity_enable);
	select_parity(channel, config.parity_type);
	set_pe_int(channel, config.pe_int);
	set_txe_int(channel, config.txe_int);
	set_tc_int(channel, config.tc_int);
	set_rxne_int(channel, config.rxne_int);
	set_idle_int(channel, config.idle_int);
	set_rx_wake(channel, config.rx_wake);
	set_lin_mode(channel, config.lin_mode);
	set_stop_bits(channel, config.stop_bits);
	set_sclk(channel, config.sclk);
	set_clkpol(channel, config.clkpol);
	set_clkpha(channel, config.clkpha);
	set_last_clk_pulse(channel, config.last_pulse);
	set_lbd_int(channel, config.lin_int);
	set_lbd_length(channel, config.break_len);
	set_addr(channel, config.addr);
	set_onebit(channel, config.onebit_sample);
	set_cts_int(channel, config.cts_int);
	set_cts(channel, config.cts_en);
	set_rts(channel, config.rts_en);
	set_dma_tx(channel, config.tx_dma_en);
	set_dma_rx(channel, config.rx_dma_en);
	set_smartcard(channel, config.smartcard_en);
	set_nack(channel, config.smartcard_nack_en);
	set_half_duplex(channel, config.half_duplex);
	set_irda_low_power(channel, config.low_power_mode);
	set_irda(channel, config.irda_en);
	set_err_int(channel, config.error_int);
	set_guard_time(channel, config.guard_time, config.lpm_prescaler);

	uart_enable(channel);
	enable_uart_tx(channel);
	enable_uart_rx(channel);
}
