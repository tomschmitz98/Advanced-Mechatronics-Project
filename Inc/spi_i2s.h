/*
 * spi_i2s.h
 *
 *  Created on: Nov 16, 2023
 *      Author: Tom
 */

#ifndef SPI_I2S_H_
#define SPI_I2S_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm_utils.h"

// CR1
#define SPI_CK_POL_LOW false
#define SPI_CK_POL_HIGH true
#define SPI_FIRST_CK_DATA_CAPTURE false
#define SPI_SECOND_CK_DATA_CAPTURE true

// CR2
#define SPI_MOTOROLA_MODE false
#define SPI_TI_MODE true

// SR
#define FRE BIT8
#define BSY BIT7
#define OVR BIT6
#define MODF BIT5 // Not used in I2S mode
#define CRCERR BIT4 // Not used in I2S mode
#define UDR BIT3 // Not used in SPI mode
#define CHSIDE BIT2 // Not used in SPI mode
#define TXE BIT1
#define RXNE BIT0

// I2S config reg
#define I2S_CK_STEADY_STATE_LOW false
#define I2S_CK_STEADY_STATE_HIGH true
#define I2S_16BIT_CHANNEL false
#define I2S_32BIT_CHANNEL true

typedef enum {
	SPI_MODE,
	I2S_MODE
} spi_i2s_mode_t;

typedef enum {
	SPI_BAUD_DIV_2 = 0x0,
	SPI_BAUD_DIV_4 = 0x1,
	SPI_BAUD_DIV_8 = 0x2,
	SPI_BAUD_DIV_16 = 0x3,
	SPI_BAUD_DIV_32 = 0x4,
	SPI_BAUD_DIV_64 = 0x5,
	SPI_BAUD_DIV_128 = 0x6,
	SPI_BAUD_DIV_256 = 0x7
} spi_baud_t;

typedef enum {
	I2S_SLAVE_TRANSMIT,
	I2S_SLAVE_RECEIVE,
	I2S_MASTER_TRANSMIT,
	I2S_MASTER_RECEIVE
} i2s_config_mode_t;

typedef enum {
	I2S_PHILLIPS_STANDARD,
	I2S_MSB_JUSTIFIED,
	I2S_LSB_JUSTIFIED,
	I2S_PCM_STANDARD
} i2s_standard_t;

typedef enum {
	I2S_16_BIT_DATA_LEN,
	I2S_24_BIT_DATA_LEN,
	I2S_32_BIT_DATA_LEN
} i2s_data_length_t;

typedef enum {
	SPI_1 = 0x3E00,
	SPI_2 = 0x0000,
	SPI_3 = 0x0100,
	SPI_4 = 0x3F00,
	I2S_2 = 0x0000,
	I2S_3 = 0x0100
} spi_i2s_channel_t;

typedef struct {
	//spi_i2s_mode_t peripheral_mode;
	// CR1 - Not used for I2S
	bool bidirectional_mode;
	bool bidirection_output_enable;
	bool use_crc;
	bool crc_in_next_transfer;
	bool dataframe16bit;
	bool rxonly;
	bool software_slave_mngmt;
	bool lsb_first;
	spi_baud_t spi_baud;
	bool master_mode;
	bool clock_polarity;
	bool clock_phase;

	// CR2
	bool TXE_int_en;
	bool RXNE_int_en;
	bool err_int_en;
	bool frame_format;  // Not used for I2S
	bool SS_out_en;  // Not used for I2S or SPI TI mode
	bool dma_tx;
	bool dma_rx;

	// CRC
	uint16_t crc_polynomial;
} spi_config_t;

typedef struct {
	// CR2
	bool TXE_int_en;
	bool RXNE_int_en;
	bool err_int_en;
	bool dma_tx;
	bool dma_rx;

	//I2S configuration reg
	bool async_start;
	i2s_config_mode_t config_mode;
	bool pcm_long_frame_sync;
	i2s_standard_t standard;
	bool ckpol;
	i2s_data_length_t data_length;
	bool channel_length;

	//I2S clock
	bool master_clk_en;
	bool odd_psc_factor;
	uint8_t i2s_div;
} i2s_config_t;

typedef struct {
	spi_i2s_mode_t peripheral_mode;

	union {
		spi_config_t spi_config;
		i2s_config_t i2s_config;
	};
} spi_i2s_configs_t;

#endif /* SPI_I2S_H_ */
