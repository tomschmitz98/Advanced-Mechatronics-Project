/*
 * core_m4.c
 *
 *  Created on: Nov 14, 2023
 *      Author: Tom
 */

#include <stdint.h>
#include <stdbool.h>
#include "core_m4.h"
#include "stm_utils.h"

#define __STATIC_INLINE static __inline

#define MAX_INT_ID 239
#define NUM_NORM_INT_REGS 8
#define NUM_PRIORITY_REGS 60

#define CORE_BASE 0xE0000000
#define SCS_OFFSET 0xE000
#define NVIC_OFFSET (SCS_OFFSET + 0x100)

#define NVIC_BASE(n, x) *(((uint32_t *)(CORE_BASE + NVIC_OFFSET)) + n + x)
#define NVIC_ISER 0
#define NVIC_ICER 32
#define NVIC_ISPR 64
#define NVIC_ICPR 96
#define NVIC_IABR 128
#define NVIC_IPR 192
#define NVIC_STIR 832

static uint32_t generate_mask(uint32_t interrupt, uint32_t *reg)
{
	uint32_t x = interrupt >> 5;
	if (x >= NUM_NORM_INT_REGS || interrupt > MAX_INT_ID)
	{
		*reg = 0;
		return 0;
	}
	*reg = x;

	return (1UL << (interrupt & 0x1F));
}

static void nvic_set_enable(uint32_t interrupt)
{
	uint32_t mask, x;
	mask = generate_mask(interrupt, &x);
	NVIC_BASE(NVIC_ISER, x) = mask;
	mask = NVIC_BASE(NVIC_ISER, x);
}

static void nvic_clear_enable(uint32_t interrupt)
{
	uint32_t mask, x;
	mask = generate_mask(interrupt, &x);
	NVIC_BASE(NVIC_ICER, x) = mask;
}

static void nvic_set_pending(uint32_t interrupt)
{
	uint32_t mask, x;
	mask = generate_mask(interrupt, &x);
	NVIC_BASE(NVIC_ISPR, x) = mask;
}

static void nvic_clear_pending(uint32_t interrupt)
{
	uint32_t mask, x;
	mask = generate_mask(interrupt, &x);
	NVIC_BASE(NVIC_ICPR, x) = mask;
}

static bool nvic_interrupt_active(uint32_t interrupt)
{
	uint32_t mask, x, status;
	mask = generate_mask(interrupt, &x);
	status = NVIC_BASE(NVIC_IABR, x);
	return (status & mask) != 0;
}

static void nvic_set_priority(uint32_t interrupt, uint8_t priority)
{
	uint32_t x = interrupt >> 2, priority_u32 = (uint32_t)priority, mask = 0xFF, shift;

	if (x >= NUM_PRIORITY_REGS || interrupt > MAX_INT_ID)
	{
		return;
	}

	shift = (interrupt - (x << 2)) << 3;
	NVIC_BASE(NVIC_IPR, x) &= ~(mask << shift);
	NVIC_BASE(NVIC_IPR, x) |= (priority_u32 << shift);
	priority = NVIC_BASE(NVIC_IPR, x);
}

static void nvic_generate_software_interrupt(uint8_t interrupt)
{
	if (interrupt > MAX_INT_ID)
	{
		return;
	}
	NVIC_BASE(NVIC_STIR, 0) = (uint32_t)interrupt;
}

__attribute__((always_inline)) __STATIC_INLINE void __enable_irq(void)
{
	__asm volatile ("cpsie i" : : : "memory");
}

__attribute__((always_inline)) __STATIC_INLINE void __disable_irq(void)
{
	__asm volatile ("cpsid i" : : : "memory");
}

void disable_global_irq(void)
{
	__disable_irq();
}

void enable_global_irq(void)
{
	__enable_irq();
}

void configure_interrupt(irq_info_t config)
{
	nvic_clear_enable((uint32_t)config.interrupt_id);
	nvic_clear_pending((uint32_t)config.interrupt_id);
	nvic_set_priority((uint32_t)config.interrupt_id, config.priority);
	nvic_set_enable((uint32_t)config.interrupt_id);
}

void configure_interrupts(irq_info_t config[], uint8_t n)
{
	for (uint8_t i = 0; i < n; i++)
	{
		configure_interrupt(config[i]);
	}
}

void enable_irq(irq_info_t irq)
{
	nvic_set_enable((uint32_t)irq.interrupt_id);
}

void disable_irq(irq_info_t irq)
{
	nvic_clear_enable((uint32_t)irq.interrupt_id);
}

void set_pending_irq(irq_info_t irq)
{
	nvic_set_pending((uint32_t)irq.interrupt_id);
}

void clear_pending_irq(irq_info_t irq)
{
	nvic_clear_pending((uint32_t)irq.interrupt_id);
}

bool check_irq_active(irq_info_t irq)
{
	return nvic_interrupt_active((uint32_t)irq.interrupt_id);
}

void send_software_irq(irq_info_t irq)
{
	nvic_generate_software_interrupt(irq.interrupt_id);
}
