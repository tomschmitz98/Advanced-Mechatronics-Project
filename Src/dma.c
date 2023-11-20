/*
 * dma.c
 *
 *  Created on: Nov 20, 2023
 *      Author: Tom
 */

#include "dma.h"
#include "stm_utils.h"
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
#define DMA_STREAM_OFFSET(stream) (0x6 * stream)

#define DMA_VALID_MASK (BIT0 | BIT2 | BIT3 | BIT4 | BIT5)

static uint32_t generate_flag_mask(uint8_t flags, uint8_t stream)
{
	uint32_t retVal, shift;

	flags &= DMA_VALID_MASK;
	shift = (stream > 1) ? 16 : 0;
	shift += (stream & BIT0) ? 6 : 0;
	retVal = ((uint32_t)flags) << shift;
	return retVal;
}

bool dma_read_status(dma_channel_t channel, uint8_t flags, uint8_t stream)
{
	uint32_t reg, flags32bit;

	if (stream > 7)
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

	return (DMA_BASE((uint32_t)channel, reg) & new_flags) != 0;
}
