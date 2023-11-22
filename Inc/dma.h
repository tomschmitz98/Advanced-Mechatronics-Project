/*
 * dma.h
 *
 *  Created on: Nov 20, 2023
 *      Author: Tom
 */

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm_utils.h"

#define DMA_STREAM_XFER_COMPLETE BIT5
#define DMA_STREAM_HALF_XFER BIT4
#define DMA_STREAM_XFER_ERROR BIT3
#define DMA_STREAM_DIRECT_MODE_ERR BIT2
#define DMA_STREAM_FIFO_ERR BIT0

#define DMA_TARGET_0 false
#define DMA_TARGET_1 true

#define DMA_PINCOS_PSIZE false
#define DMA_PINCOS_FIXED true

#define DMA_FLOW_CONTROL_DMA false
#define DMA_FLOW_CONTROL_PERIPH true

#define TCIE BIT4
#define HTIE BIT3
#define TEIE BIT2
#define DMEIE BIT1

typedef enum {
	DMA_1 = 0x0,
	DMA_2 = 0x100
} dma_channel_t;

typedef enum {
	DMA_CHANNEL_0 = 0x0,
	DMA_CHANNEL_1 = 0x1,
	DMA_CHANNEL_2 = 0x2,
	DMA_CHANNEL_3 = 0x3,
	DMA_CHANNEL_4 = 0x4,
	DMA_CHANNEL_5 = 0x5,
	DMA_CHANNEL_6 = 0x6,
	DMA_CHANNEL_7 = 0x7,
} dma_channel_select_t;

typedef enum {
	DMA_SINGLE_XFER = 0x0,
	DMA_INCR4 = 0x1,
	DMA_INCR8 = 0x2,
	DMA_INCR16 = 0x3
} dma_burst_t;

typedef enum {
	DMA_LOW_PR = 0x0,
	DMA_MED_PR = 0x1,
	DMA_HIGH_PR = 0x2,
	DMA_VERY_HIGH_PR = 0x3
} dma_priority_t;

typedef enum {
	BYTE = 0x0,
	HALF_WORD = 0x1,
	WORD = 0x2
} dma_data_size_t;

typedef enum {
	PERIPH_TO_MEM = 0x0,
	MEM_TO_PERIPH = 0x1,
	MEM_TO_MEM = 0x2
} dma_direction_t;

typedef enum {
	DMA_FIFO_NEARLY_EMPTY = 0x0,
	DMA_FIFO_QUARTER_FULL = 0x1,
	DMA_FIFO_HALF_FULL = 0x2,
	DMA_FIFO_NEARLY_FULL = 0x3,
	DMA_FIFO_EMPTY = 0x4,
	DMA_FIFO_FULL = 0x5
} dma_fifo_status_t;

typedef enum {
	DMA_FIFO_THRH_QUARTER_FULL = 0x0,
	DMA_FIFO_THRH_HALF_FULL = 0x1,
	DMA_FIFO_THRH_3QUARTERS_FULL = 0x2,
	DMA_FIFO_THRH_FULL = 0x3
} dma_fifo_threshold_t;

typedef bool dma_target_t;
typedef bool dma_periph_incr_offset_size_t;
typedef bool dma_peripheral_flow_ctrl_t;

typedef struct {
	dma_channel_select_t channel;
	dma_burst_t mem_burst;
	dma_burst_t peripheral_burst;
	dma_target_t current_target;
	bool double_buffer_mode;
	dma_priority_t priority;
	dma_periph_incr_offset_size_t pincos;
	dma_data_size_t mem_data_size;
	dma_data_size_t periph_data_size;
	bool incr_mem_after_xfer;
	bool incr_periph_after_xfer;
	bool circular_mode;
	dma_direction_t xfer_direction;
	dma_peripheral_flow_ctrl_t flow_control;
	uint8_t interrupt_enable;
	uint32_t peripheral_address;
	uint32_t stream_address[2];
	bool fifo_err_int_en;
	bool disable_direct_mode;
	dma_fifo_threshold_t threshold;
	bool enableWhenDone;
} dma_config_t;

bool dma_read_status(dma_channel_t channel, uint8_t flags, uint8_t stream);
void dma_clear_status(dma_channel_t channel, uint8_t flags, uint8_t stream);

void enable_dma(dma_channel_t channel, uint8_t stream);
void disable_dma(dma_channel_t channel, uint8_t stream);
bool check_enabled(dma_channel_t channel, uint8_t stream);

void configure_dma(dma_channel_t channel, uint8_t stream, dma_config_t config);

void dma_write_number_of_transfers(dma_channel_t channel, uint8_t stream, uint16_t transfers);
uint16_t dma_read_number_of_transfers(dma_channel_t channel, uint8_t stream);

void dma_write_stream_address(dma_channel_t channel, uint8_t stream, uint32_t address, bool first_reg);
uint32_t dma_read_stream_address(dma_channel_t channel, uint8_t stream, bool first_reg);

dma_fifo_status_t dma_check_fifo(dma_channel_t channel, uint8_t stream);

#endif /* DMA_H_ */
