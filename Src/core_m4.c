/**
 * @file core_m4.c
 * @version 1.0
 *
 * @brief   Hardware abstraction layer for implementing core functionalities
 *          of the ARM Cortex M4 processor. This file is compatible for the
 *          STM32F446C and STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#include "core_m4.h"
#include "stm_utils.h"
#include <stdbool.h>
#include <stdint.h>

#define __STATIC_INLINE static __inline

/* Interrupt ID Macros */
#define MAX_INT_ID        239
#define NUM_NORM_INT_REGS 8
#define NUM_PRIORITY_REGS 60

/* Core Base Addresses */
#define CORE_BASE   (uint32_t)0xE0000000
#define SCS_OFFSET  (uint32_t)0xE000
#define NVIC_OFFSET (uint32_t)(SCS_OFFSET + 0x100)

/* Nested Vector Interrupt Base Address and Offsets */
#define NVIC_BASE(n, x)                                                        \
    *(((uint32_t *)(uint32_t)(CORE_BASE + NVIC_OFFSET)) + n + x)
#define NVIC_ISER 0
#define NVIC_ICER 32
#define NVIC_ISPR 64
#define NVIC_ICPR 96
#define NVIC_IABR 128
#define NVIC_IPR  192
#define NVIC_STIR 832

/**
 * @brief   Generates the appropriate bit mask for the given interrupt ID.
 *          Additionally, the appropriate register offset is determined from
 *          the ID.
 *
 * @param[in] interrupt   The interrupt ID
 * @param[out] reg        The register offset
 *
 * @return   The bitmask for the interrupt ID
 */
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

/**
 * @brief   Enables a given interrupt.
 *
 * @param[in] interrupt   The interrupt ID
 */
static void nvic_set_enable(uint32_t interrupt)
{
    uint32_t mask, x;
    mask = generate_mask(interrupt, &x);
    NVIC_BASE(NVIC_ISER, x) = mask;
    mask = NVIC_BASE(NVIC_ISER, x);
}

/**
 * @brief   Disables a given interrupt.
 *
 * @param[in] interrupt   The interrupt ID
 */
static void nvic_clear_enable(uint32_t interrupt)
{
    uint32_t mask, x;
    mask = generate_mask(interrupt, &x);
    NVIC_BASE(NVIC_ICER, x) = mask;
}

/**
 * @brief   Sets the pending bit of a given interrupt.
 *
 * @param[in] interrupt   The interrupt ID
 */
static void nvic_set_pending(uint32_t interrupt)
{
    uint32_t mask, x;
    mask = generate_mask(interrupt, &x);
    NVIC_BASE(NVIC_ISPR, x) = mask;
}

/**
 * @brief   Clears the pending bit of a given interrupt.
 *
 * @param[in] interrupt   The interrupt ID
 */
static void nvic_clear_pending(uint32_t interrupt)
{
    uint32_t mask, x;
    mask = generate_mask(interrupt, &x);
    NVIC_BASE(NVIC_ICPR, x) = mask;
}

/**
 * @brief   Checks if an interrupt is active.
 *
 * @param[in] interrupt   The interrupt ID
 *
 * @return   true if the interrupt is active
 *           false if the interrupt is not active
 */
static bool nvic_interrupt_active(uint32_t interrupt)
{
    uint32_t mask, x, status;
    mask = generate_mask(interrupt, &x);
    status = NVIC_BASE(NVIC_IABR, x);
    return (status & mask) != 0;
}

/**
 * @brief   Sets the priority of a given interrupt.
 *
 * @param[in] interrupt   The interrupt ID
 * @param[in] priority    The priority level
 */
static void nvic_set_priority(uint32_t interrupt, uint8_t priority)
{
    uint32_t x = interrupt >> 2, priority_u32 = (uint32_t)priority, mask = 0xFF,
             shift;

    if (x >= NUM_PRIORITY_REGS || interrupt > MAX_INT_ID)
    {
        return;
    }

    shift = (interrupt - (x << 2)) << 3;
    NVIC_BASE(NVIC_IPR, x) &= ~(mask << shift);
    NVIC_BASE(NVIC_IPR, x) |= (priority_u32 << shift);
}

/**
 * @brief   Generates a given interrupt from software
 *
 * @param[in] interrupt   The interrupt ID
 */
static void nvic_generate_software_interrupt(uint8_t interrupt)
{
    if (interrupt > MAX_INT_ID)
    {
        return;
    }
    NVIC_BASE(NVIC_STIR, 0) = (uint32_t)interrupt;
}

/**
 * @brief   Enables global IRQs.
 */
__attribute__((always_inline)) __STATIC_INLINE void __enable_irq(void)
{
    __asm volatile("cpsie i" : : : "memory");
}

/**
 * @brief   Disables global IRQs.
 */
__attribute__((always_inline)) __STATIC_INLINE void __disable_irq(void)
{
    __asm volatile("cpsid i" : : : "memory");
}

/**
 * @brief   Blocks all interrupts on the system
 */
void disable_global_irq(void)
{
    __disable_irq();
}

/**
 * @brief   Unblocks all interrupts on the system
 */
void enable_global_irq(void)
{
    __enable_irq();
}

/**
 * @brief   Configures an interrupt given the appropriate information.
 *
 * @param[in] config   The interrupt configuration info
 */
void configure_interrupt(irq_info_t config)
{
    nvic_clear_enable((uint32_t)config.interrupt_id);
    nvic_clear_pending((uint32_t)config.interrupt_id);
    nvic_set_priority((uint32_t)config.interrupt_id, config.priority);
    nvic_set_enable((uint32_t)config.interrupt_id);
}

/**
 * @brief   Configures multiple interrupts given the appropriate information.
 *
 * @param[in] config   The interrupts to configure
 * @param n            The number of interrupts to configure
 */
void configure_interrupts(irq_info_t config[], uint8_t n)
{
    for (uint8_t i = 0; i < n; i++)
    {
        configure_interrupt(config[i]);
    }
}

/**
 * @brief   Enables a certain interrupt
 *
 * @param[in] irq   The interrupt to enable
 */
void enable_irq(irq_info_t irq)
{
    nvic_set_enable((uint32_t)irq.interrupt_id);
}

/**
 * @brief   Disables a certain interrupt
 *
 * @param[in] irq   The interrupt to disable
 */
void disable_irq(irq_info_t irq)
{
    nvic_clear_enable((uint32_t)irq.interrupt_id);
}

/**
 * @brief   Sets a given interrupt as pending
 *
 * @param[in] irq   The interrupt to as pending
 */
void set_pending_irq(irq_info_t irq)
{
    nvic_set_pending((uint32_t)irq.interrupt_id);
}

/**
 * @brief   Clears a given pending interrupt
 *
 * @param[in] irq   The pending interrupt is cleared
 */
void clear_pending_irq(irq_info_t irq)
{
    nvic_clear_pending((uint32_t)irq.interrupt_id);
}

/**
 * @brief   Checks if an interrupt is active
 *
 * @param[in] irq   The interrupt to see if active
 *
 * @return   true if interrupt is active
 * @return   false if interrupt is not active
 */
bool check_irq_active(irq_info_t irq)
{
    return nvic_interrupt_active((uint32_t)irq.interrupt_id);
}

/**
 * @brief   Generates a software interrupt for a given interrupt
 *
 * @param[in] irq   The interrupt to generate
 */
void send_software_irq(irq_info_t irq)
{
    nvic_generate_software_interrupt(irq.interrupt_id);
}
