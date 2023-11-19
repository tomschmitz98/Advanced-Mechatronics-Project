/*
 * spi_i2s.c
 *
 *  Created on: Nov 18, 2023
 *      Author: Tom
 */

#include "spi_i2s.h"
#include "stm_utils.h"
#include "stm_rcc.h"
#include <assert.h>
#include <stdbool.h>

#define SPI_I2S_BASE(n, reg) *(((uint32_t *)0x40003800) + n + reg)
#define SPI_CR1 0
#define SPI_CR2 1
#define SPI_SR 2
#define SPI_DR 3
#define SPI_CRCPR 4
#define SPI_RXCRCR 5
#define SPI_TXCRCR 6
#define SPI_I2SCFGR 7
#define SPI_I2SPR 8

#define _SPI_MODE false
#define _I2S_MODE true

typedef bool mode_t;

//#define TEST_BUILD

#if defined ( __GNUC__ ) && defined ( TEST_BUILD )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

static mode_t _check_mode(spi_i2s_channel_t channel)
{
	uint32_t mode;
	if (channel == SPI_1 || channel == SPI_4)
	{
		return false;
	}

	mode = SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) & BITB;

	return mode != 0;
}

static void _set_mode(spi_i2s_channel_t channel, mode_t mode)
{
	if (channel == SPI_1 || channel == SPI_4)
	{
		return;
	}

	if (mode == _I2S_MODE)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) |= BITB;
	}
	else
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) &= ~BITB;
	}
}

static bool _is_enabled(spi_i2s_channel_t channel)
{
	mode_t mode = _check_mode(channel);

	if (mode == SPI_MODE)
	{
		return (SPI_I2S_BASE((uint32_t)channel, SPI_CR1) & BIT6) != 0;
	}

	return (SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) & BITA) != 0;
}

static void _disable_spi_i2s_channel(spi_i2s_channel_t channel)
{
	disable_spi(channel);
	if (channel != SPI_1 && channel != SPI_4)
	{
		disable_i2s(channel);
	}
}

static void set_spi_bidirectional_mode(spi_i2s_channel_t channel, bool mode)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BITF;
	if (mode)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BITF;
	}
}

static void set_spi_bidirectional_direction(spi_i2s_channel_t channel, bool direction)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BITE;
	if (direction)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BITE;
	}
}

static void set_spi_crc_en(spi_i2s_channel_t channel, bool crc_en)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BITD;
	if (crc_en)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BITD;
	}
}

static void set_spi_crc_next_transfer(spi_i2s_channel_t channel, bool next_transfer)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BITC;
	if (next_transfer)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BITC;
	}
}

static void set_spi_dataframe_format(spi_i2s_channel_t channel, bool format)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BITB;
	if (format)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BITB;
	}
}

static void set_spi_rx_only(spi_i2s_channel_t channel, bool rx_only)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BITA;
	if (rx_only)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BITF;
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BITA;
	}
}

static void set_spi_software_slave_mngmt(spi_i2s_channel_t channel, bool software_control)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BIT9;
	if (software_control)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BIT9;
	}
}

static void set_spi_lsb_transmitted_first(spi_i2s_channel_t channel, bool lsb_transmitted)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BIT7;
	if (lsb_transmitted)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BIT7;
	}
}

static void set_spi_baud_div(spi_i2s_channel_t channel, spi_baud_t baud)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~(BIT5 | BIT4 | BIT3);
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= ((uint32_t)baud) << 3;
}

static void set_spi_master_mode(spi_i2s_channel_t channel, bool master)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BIT2;
	if (master)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BIT2;
	}
}

static void set_spi_idle_ck_polarity(spi_i2s_channel_t channel, bool polarity)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BIT1;
	if (polarity)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BIT1;
	}
}

static void set_spi_ck_phase(spi_i2s_channel_t channel, bool phase)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BIT0;
	if (phase)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BIT0;
	}
}

static void set_tx_int(spi_i2s_channel_t channel, bool tx_empty)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR2) &= ~BIT7;
	if (tx_empty)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR2) |= BIT7;
	}
}

static void set_rx_int(spi_i2s_channel_t channel, bool rx_not_empty)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR2) &= ~BIT6;
	if (rx_not_empty)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR2) |= BIT6;
	}
}

static void set_error_int(spi_i2s_channel_t channel, bool error_int)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR2) &= ~BIT5;
	if (error_int)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR2) |= BIT5;
	}
}

static void set_frame_format(spi_i2s_channel_t channel, bool format)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR2) &= ~BIT4;
	if (format)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR2) |= BIT4;
	}
}

static void set_ss_output_enable(spi_i2s_channel_t channel, bool ss_output_en)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR2) &= ~BIT2;
	if (ss_output_en)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR2) |= BIT2;
	}
}

static void set_tx_dma_enable(spi_i2s_channel_t channel, bool dma_enable)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR2) &= ~BIT1;
	if (dma_enable)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BITC;
		SPI_I2S_BASE((uint32_t)channel, SPI_CR2) |= BIT1;
	}
}

static void set_rx_dma_enable(spi_i2s_channel_t channel, bool dma_enable)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR2) &= ~BIT0;
	if (dma_enable)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BITC;
		SPI_I2S_BASE((uint32_t)channel, SPI_CR2) |= BIT0;
	}
}

static void configure_spi_control1(spi_i2s_channel_t channel, spi_config_t config)
{
	set_spi_bidirectional_mode(channel, config.bidirectional_mode);
	set_spi_bidirectional_direction(channel, config.bidirection_output_enable);
	set_spi_crc_en(channel, config.use_crc);
	set_spi_crc_next_transfer(channel, config.crc_in_next_transfer);
	set_spi_dataframe_format(channel, config.dataframe16bit);
	set_spi_rx_only(channel, config.rxonly);
	set_spi_software_slave_mngmt(channel, config.software_slave_mngmt);
	set_spi_lsb_transmitted_first(channel, config.lsb_first);
	set_spi_baud_div(channel, config.spi_baud);
	set_spi_master_mode(channel, config.master_mode);
	set_spi_idle_ck_polarity(channel, config.clock_polarity);
	set_spi_ck_phase(channel, config.clock_phase);
}

static void configure_spi_control2(spi_i2s_channel_t channel, spi_config_t config)
{
	set_tx_int(channel, config.TXE_int_en);
	set_rx_int(channel, config.RXNE_int_en);
	set_error_int(channel, config.err_int_en);
	set_frame_format(channel, config.frame_format);
	set_ss_output_enable(channel, config.SS_out_en);
	set_tx_dma_enable(channel, config.dma_tx);
	set_rx_dma_enable(channel, config.dma_rx);
}

void enable_spi(spi_i2s_channel_t channel)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) |= BIT6;
}

void disable_spi(spi_i2s_channel_t channel)
{
	// Wait until TX is empty
	while (!check_spi_i2s_status(channel, SPI_TXE));

	// Wait until bus is not busy
	while(check_spi_i2s_status(channel, SPI_BSY));

	// TODO: Read data

	SPI_I2S_BASE((uint32_t)channel, SPI_CR1) &= ~BIT6;
}

uint16_t check_spi_i2s_status(spi_i2s_channel_t channel, uint16_t mask)
{
	return (uint16_t)SPI_I2S_BASE((uint32_t)channel, SPI_SR) & mask;
}

void acknowledge_crc_error(spi_i2s_channel_t channel)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_SR) = ~BIT4;
}

static void configure_spi(spi_i2s_channel_t channel, spi_config_t config)
{
	assert(!_is_enabled(channel));

	configure_spi_control1(channel, config);
	configure_spi_control2(channel, config);
	SPI_I2S_BASE((uint32_t)channel, SPI_CRCPR) = (uint32_t)config.crc_polynomial;

	if (config.enableWhenDone)
	{
		enable_spi(channel);
	}
}

static void enable_spi_i2s_clock(spi_i2s_channel_t channel)
{
	switch(channel)
	{
	case SPI_1:
		enable_peripheral_clock(SPI1_EN);
		break;
	case SPI_2:
		enable_peripheral_clock(SPI2_EN);
		break;
	case SPI_3:
		enable_peripheral_clock(SPI3_EN);
		break;
	case SPI_4:
		enable_peripheral_clock(SPI4_EN);
		break;
	default:
		assert(0);
	}
}

static void set_async_start(spi_i2s_channel_t channel, bool async_en)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) &= ~BITC;
	if (async_en)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) |= BITC;
	}
}

static void set_i2s_config_mode(spi_i2s_channel_t channel, i2s_config_mode_t mode)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) &= ~(BIT9 | BIT8);
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) |= ((uint32_t)mode) << 8;
}

static void set_i2s_frame_sync(spi_i2s_channel_t channel, bool long_format)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) &= ~BIT7;
	if (long_format)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) |= BIT7;
	}
}

static void set_i2s_standard(spi_i2s_channel_t channel, i2s_standard_t standard)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) &= ~(BIT4 | BIT5);
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) |= ((uint32_t)standard) << 4;
}

static void set_i2s_clk_pol(spi_i2s_channel_t channel, bool steady_high)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) &= ~BIT3;
	if (steady_high)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) |= BIT3;
	}
}

static void set_i2s_data_len(spi_i2s_channel_t channel, i2s_data_length_t length)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) &= ~(BIT2 | BIT1);
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) |= ((uint32_t)length) << 1;
}

static void set_i2s_channel_length(spi_i2s_channel_t channel, bool width_32bits)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) &= ~BIT0;
	if (width_32bits)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) |= BIT0;
	}
}

static void set_i2s_master_clk_out(spi_i2s_channel_t channel, bool enable_out)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SPR) &= ~BIT9;
	if (enable_out)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_I2SPR) |= BIT9;
	}
}

static void set_i2s_odd_factor(spi_i2s_channel_t channel, bool odd_factor)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SPR) &= ~BIT8;
	if (odd_factor)
	{
		SPI_I2S_BASE((uint32_t)channel, SPI_I2SPR) |= BIT8;
	}
}

static void set_i2s_prescale(spi_i2s_channel_t channel, uint8_t prescale)
{
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SPR) &= ~0xFF;
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SPR) |= (uint32_t)prescale;
}

static void configure_i2s(spi_i2s_channel_t channel, i2s_config_t configs)
{
	assert(!_is_enabled(channel));
	set_tx_int(channel, configs.TXE_int_en);
	set_rx_int(channel, configs.RXNE_int_en);
	set_error_int(channel, configs.err_int_en);
	set_tx_dma_enable(channel, configs.dma_tx);
	set_rx_dma_enable(channel, configs.dma_rx);
	set_async_start(channel, configs.async_start);
	set_i2s_config_mode(channel, configs.config_mode);
	set_i2s_frame_sync(channel, configs.pcm_long_frame_sync);
	set_i2s_standard(channel, configs.standard);
	set_i2s_clk_pol(channel, configs.ckpol);
	set_i2s_data_len(channel, configs.data_length);
	set_i2s_channel_length(channel, configs.channel_length);
	set_i2s_master_clk_out(channel, configs.master_clk_en);
	set_i2s_odd_factor(channel, configs.odd_psc_factor);
	set_i2s_prescale(channel, configs.i2s_div);

	if (configs.enableWhenDone)
	{
		enable_i2s(channel);
	}
}

void enable_i2s(spi_i2s_channel_t channel)
{
	assert(_check_mode(channel) == _I2S_MODE);
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) |= BITA;
}

void disable_i2s(spi_i2s_channel_t channel)
{
	assert(_check_mode(channel) == _I2S_MODE);
	SPI_I2S_BASE((uint32_t)channel, SPI_I2SCFGR) &= ~BITA;
}

void configure_spi_i2s(spi_i2s_channel_t channel, spi_i2s_configs_t configs)
{
	enable_spi_i2s_clock(channel);

	_disable_spi_i2s_channel(channel);

	switch(configs.peripheral_mode)
	{
	case SPI_MODE:
		_set_mode(channel, _SPI_MODE);
		configure_spi(channel, configs.spi_config);
		break;
	case I2S_MODE:
		_set_mode(channel, _I2S_MODE);
		configure_i2s(channel, configs.i2s_config);
		break;
	default:
		assert(0);
	}
}

#if defined ( __GNUC__ ) && defined ( TEST_BUILD )
#pragma GCC diagnostic pop
#endif
