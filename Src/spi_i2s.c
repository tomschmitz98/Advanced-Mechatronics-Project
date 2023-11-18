/*
 * spi_i2s.c
 *
 *  Created on: Nov 18, 2023
 *      Author: Tom
 */

#include "spi_i2s.h"
#include "stm_utils.h"
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

#define SPI_MODE false
#define I2S_MODE true

typedef bool mode_t;

#if defined ( __GNUC__ )
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

	if (mode == I2S_MODE)
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

static void configure_spi_control1(spi_i2s_channel_t channel, spi_config_t config)
{
	config = config;
	channel = channel;
}

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
