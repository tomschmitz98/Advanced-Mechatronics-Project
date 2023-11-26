/*
 * dma.c
 *
 *  Created on: Nov 20, 2023
 *      Author: Tom
 */

#include "dma.h"
#include "stm_utils.h"
#include "stm_rcc.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define DMA_BASE(n, x) *(((uint32_t *)0x40026000) + n + x)
#define DMA_LISR 0
#define DMA_HISR 1
#define DMA_LIFCR 2
#define DMA_HIFCR 3
#define DMA_SxCR 4
#define DMA_SxNDTR 5
#define DMA_SxPAR 6
#define DMA_SxM0AR 7
#define DMA_SxM1AR 8
#define DMA_SxFCR 9
#define DMA_STREAM_OFFSET(stream) ((uint32_t)(0x6 * stream))

#define DMA_VALID_MASK (BIT0 | BIT2 | BIT3 | BIT4 | BIT5)
#define DMA_INT_EN_VALID_MASK (BIT4 | BIT3 | BIT2 | BIT1)

#define NUM_DMA_STREAMS 8

static uint32_t generate_flag_mask(uint8_t flags, uint8_t stream)
{
	uint32_t retVal, shift;

	flags &= DMA_VALID_MASK;
	shift = (stream > 1) ? 16 : 0;
	shift += (stream & BIT0) ? 6 : 0;
	retVal = ((uint32_t)flags) << shift;
	return retVal;
}

static void set_dma_channel_selection(dma_channel_t channel, uint8_t stream, dma_channel_select_t select)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~UPPER16BITS(BITB | BITA | BIT9);
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= ((uint32_t)select) << 25;
}

static void set_dma_mem_burst(dma_channel_t channel, uint8_t stream, dma_burst_t mburst)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~UPPER16BITS(BIT8 | BIT7);
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= ((uint32_t)mburst) << 23;
}

static void set_dma_peripheral_burst(dma_channel_t channel, uint8_t stream, dma_burst_t pburst)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~UPPER16BITS(BIT6 | BIT5);
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= ((uint32_t)pburst) << 21;
}

static void set_dma_current_target(dma_channel_t channel, uint8_t stream, dma_target_t target)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~UPPER16BITS(BIT3);

	if (target == DMA_TARGET_1)
	{
		DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= UPPER16BITS(BIT3);
	}
}

static void set_dma_double_buffer_mode(dma_channel_t channel, uint8_t stream, bool double_buffer)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~UPPER16BITS(BIT2);

	if (double_buffer)
	{
		DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= UPPER16BITS(BIT2);
	}
}

static void set_dma_priority_level(dma_channel_t channel, uint8_t stream, dma_priority_t priority)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~UPPER16BITS(BIT1 | BIT0);
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= ((uint32_t)priority) << 16;
}

static void set_dma_pincos(dma_channel_t channel, uint8_t stream, dma_periph_incr_offset_size_t offset)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~BITF;

	if (offset == DMA_PINCOS_FIXED)
	{
		DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= BITF;
	}
}

static void set_dma_mem_size(dma_channel_t channel, uint8_t stream, dma_data_size_t mem_size)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~(BITE | BITD);
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= ((uint32_t)mem_size) << 13;
}

static void set_dma_peripheral_size(dma_channel_t channel, uint8_t stream, dma_data_size_t peripheral_size)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~(BITC | BITB);
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= ((uint32_t)peripheral_size) << 11;
}

static void set_dma_mem_incr_mode(dma_channel_t channel, uint8_t stream, bool incremented)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~BITA;

	if (incremented)
	{
		DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= BITA;
	}
}

static void set_dma_peripheral_incr_mode(dma_channel_t channel, uint8_t stream, bool incremented)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~BIT9;

	if (incremented)
	{
		DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= BIT9;
	}
}

static void set_dma_circular_mode(dma_channel_t channel, uint8_t stream, bool circular)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~BIT8;

	if (circular)
	{
		DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= BIT8;
	}
}

static void set_dma_xfer_direction(dma_channel_t channel, uint8_t stream, dma_direction_t direction)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~(BIT7 | BIT6);
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= ((uint32_t)direction) << 6;
}

static void set_dma_peripheral_flow_controller(dma_channel_t channel, uint8_t stream, dma_peripheral_flow_ctrl_t flow_control)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~BIT5;

	if (flow_control == DMA_FLOW_CONTROL_PERIPH)
	{
		DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= BIT5;
	}
}

static void set_dma_int_en(dma_channel_t channel, uint8_t stream, uint8_t mask)
{
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~DMA_INT_EN_VALID_MASK;
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= (mask & DMA_INT_EN_VALID_MASK);
}

bool dma_read_status(dma_channel_t channel, uint8_t flags, uint8_t stream)
{
	uint32_t reg, flags32bit;

	if (stream >= NUM_DMA_STREAMS)
	{
		return false;
	}

	if (stream > 3)
	{
		reg = DMA_HISR;
		stream -= 4;
	}
	else
	{
		reg = DMA_LISR;
	}

	flags32bit = generate_flag_mask(flags, stream);

	return (DMA_BASE((uint32_t)channel, reg) & flags32bit) != 0;
}

void dma_clear_status(dma_channel_t channel, uint8_t flags, uint8_t stream)
{
	uint32_t reg, flags32bit;

	if (stream >= NUM_DMA_STREAMS)
	{
		return;
	}

	if (stream > 3)
	{
		reg = DMA_HIFCR;
		stream -= 4;
	}
	else
	{
		reg = DMA_LIFCR;
	}

	flags32bit = generate_flag_mask(flags, stream);

	DMA_BASE((uint32_t)channel, reg) = flags32bit;
}

static void set_dma_peripheral_address(dma_channel_t channel, uint8_t stream, uint32_t address)
{
    assert(!check_dma_enabled(channel, stream));
    DMA_BASE((uint32_t)channel, DMA_SxPAR + DMA_STREAM_OFFSET(stream)) = address;
}

static inline void set_dma_memory_stream_address(dma_channel_t channel, uint8_t stream, uint32_t address, uint32_t offset)
{
    DMA_BASE((uint32_t)channel, offset + DMA_STREAM_OFFSET(stream)) = address;
}

static void set_dma_fifo_err_en(dma_channel_t channel, uint8_t stream, bool enable)
{
    DMA_BASE((uint32_t)channel, DMA_SxFCR + DMA_STREAM_OFFSET(stream)) &= ~BIT7;

    if (enable) {
        DMA_BASE((uint32_t) channel, DMA_SxFCR + DMA_STREAM_OFFSET(stream)) |= BIT7;
    }
}

static void set_dma_fifo_direct_disable(dma_channel_t channel, uint8_t stream, bool disable)
{
    DMA_BASE((uint32_t)channel, DMA_SxFCR + DMA_STREAM_OFFSET(stream)) &= ~BIT2;

    if (disable) {
        DMA_BASE((uint32_t)channel, DMA_SxFCR + DMA_STREAM_OFFSET(stream)) |= BIT2;
    }
}

static void set_dma_fifo_threshold(dma_channel_t channel, uint8_t stream, dma_fifo_threshold_t threshold)
{
    DMA_BASE((uint32_t)channel, DMA_SxFCR + DMA_STREAM_OFFSET(stream)) &= ~(BIT1 | BIT0);
    DMA_BASE((uint32_t)channel, DMA_SxFCR + DMA_STREAM_OFFSET(stream)) |= (uint32_t)threshold;
}

static void enable_dma_clock(dma_channel_t channel)
{
    switch(channel)
    {
        case DMA_1:
            enable_peripheral_clock(DMA1_EN);
            break;
        case DMA_2:
            enable_peripheral_clock(DMA2_EN);
            break;
        default:
            asser(false);
    }
}

void enable_dma(dma_channel_t channel, uint8_t stream)
{
    assert(stream < NUM_DMA_STREAMS);
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) |= BIT0;
}

void disable_dma(dma_channel_t channel, uint8_t stream)
{
    assert(stream < NUM_DMA_STREAMS);
	DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) &= ~BIT0;
}

bool check_dma_enabled(dma_channel_t channel, uint8_t stream)
{
    assert(stream < NUM_DMA_STREAMS);
	return (DMA_BASE((uint32_t)channel, DMA_SxCR + DMA_STREAM_OFFSET(stream)) & BIT0) != 0;
}

void configure_dma(dma_channel_t channel, uint8_t stream, dma_config_t config)
{
    assert(stream < NUM_DMA_STREAMS);

    enable_dma_clock(channel);

	disable_dma(channel, stream);
	set_dma_channel_selection(channel, stream, config.channel);
	set_dma_mem_burst(channel, stream, config.mem_burst);
	set_dma_peripheral_burst(channel, stream, config.peripheral_burst);
	set_dma_current_target(channel, stream, config.current_target);
	set_dma_double_buffer_mode(channel, stream, config.double_buffer_mode);
	set_dma_priority_level(channel, stream, config.priority);
	set_dma_pincos(channel, stream, config.pincos);
	set_dma_mem_size(channel, stream, config.mem_data_size);
	set_dma_pheripheral_size(channel, stream, config.periph_data_size);
	set_dma_mem_incr_mode(channel, stream, config.incr_mem_after_xfer);
	set_dma_peripheral_incr_mode(channel, stream, config.incr_periph_after_xfer);
	set_dma_circular_mode(channel, stream, config.circular_mode);
	set_dma_xfer_direction(channel, stream, config.xfer_direction);
	set_dma_peripheral_flow_controller(channel, stream, config.flow_control);
	set_dma_int_en(channel, stream, config.interrupt_enable);

    set_dma_peripheral_address(channel, stream, config.peripheral_address);

    set_dma_memory_stream_address(channel, stream, config.stream_address[0], DMA_SxM0AR);
    set_dma_memory_stream_address(channel, stream, config.stream_address[1], DMA_SxM1AR);

    set_dma_fifo_err_en(channel, stream, config.fifo_err_int_en);
    set_dma_fifo_direct_disable(channel, stream, config.disable_direct_mode);
    set_dma_fifo_threshold(channel, stream, config.threshold);

	if (config.enableWhenDone)
	{
		enable_dma(channel, stream);
	}
}

void dma_write_number_of_transfers(dma_channel_t channel, uint8_t stream, uint16_t transfers)
{
    assert(stream < NUM_DMA_STREAMS);
    disable_dma(channel, stream);
    DMA_BASE((uint32_t)channel, DMA_SxNDTR + DMA_STREAM_OFFSET(stream)) = (uint32_t)transfers;
    enable_dma(channel, stream);
}

uint16_t dma_read_number_of_transfers(dma_channel_t channel, uint8_t stream)
{
    assert(stream < NUM_DMA_STREAMS);
    return (uint16_t)DMA_BASE((uint32_t)channel, DMA_SxNDTR + DMA_STREAM_OFFSET(stream));
}

static bool check_dma_ct(dma_channel_t channel, uint8_t stream)
{
    return (DMA_BASE((uint32_t)channel, DMA_SxCR) & UPPER16BITS(BIT3)) != 0;
}

void dma_write_stream_address(dma_channel_t channel, uint8_t stream, uint32_t address, bool first_reg)
{
    assert(stream < NUM_DMA_STREAMS);
    if (first_reg)
    {
        if (check_dma_enabled(channel, stream) && check_dma_ct(channel, stream))
        {
            return;
        }

        set_dma_memory_stream_address(channel, stream, address, DMA_SxM0AR);
    }
    else
    {
        if (check_dma_enabled(channel, stream && !check_dma_ct(channel, stream)))
        {
            return;
        }

        set_dma_memory_stream_address(channel, stream, address, DMA_SxM1AR);
    }
}

uint32_t dma_read_stream_address(dma_channel_t channel, uint8_t stream, bool first_reg)
{
    uint32_t offset = (first_reg) ? DMA_SxM0AR : DMA_SxM1AR;
    assert(stream < NUM_DMA_STREAMS);
    return DMA_BASE((uint32_t)channel, offset + DMA_STREAM_OFFSET(stream));
}

dma_fifo_status_t dma_check_fifo(dma_channel_t channel, uint8_t stream)
{
    assert(stream < NUM_DMA_STREAMS);
    return (dma_fifo_status_t)((DMA_BASE((uint32_t)channel, DMA_SxFCR + DMA_STREAM_OFFSET(stream)) & (BIT5 | BIT4 | BIT3)) >> 3);
}
