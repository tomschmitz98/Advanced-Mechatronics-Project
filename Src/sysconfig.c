/**
 * @file sysconfig.h
 * @version 1.0
 * @brief   Hardware abstraction layer for configuring the system hardware.
 *          This file is is compatible with the STM32F446C and STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#include "sysconfig.h"
#include "stm_rcc.h"
#include "stm_utils.h"
#include <assert.h>
#include <stdint.h>

/* System Config Base Address and Offsets */
#define SYSCONFIG_BASE(n) *(((uint32_t *)0x40013800) + n)
#define SYSCFG_MEMRMP     0 // TODO: Write stuff
#define SYSCFG_PMC        1 // TODO: Write stuff
#define SYSCFG_EXTICR1    2
#define SYSCFG_EXTICR2    3
#define SYSCFG_EXTICR3    4
#define SYSCFG_EXTICR4    5
#define SYSCFG_CMPCR      6 // TODO: Write stuff
#define SYSCFG_CFGR       7 // TODO: Write stuff

/* Mask for the pins */
#define EXTI_MASK (uint32_t)0xF

/**
 * @brief   Generates the appropriate bitfield to select the correct pin for
 *          external interrupts
 *
 * @param[in] bank   The pin group the pin belongs in
 *
 * @return   The select value
 */
static uint32_t select_exti_value(gpio_bank_t bank)
{
    return (uint32_t)bank >> 8;
}

/**
 * @brief   Generates the appropriate register offset for external interrupt
 *          configuration
 *
 * @param[in] pin      The pin number the external interrupt is for
 * @param[out] shift   The shift amount to get to the appropriate position in
 *                     the bitfield
 *
 * @return   The exti system configuration register offset
 */
static uint32_t select_exti_reg(uint8_t pin, uint32_t *shift)
{
    assert(pin < 16);
    *shift = ((uint32_t)pin & 0x3) << 2;
    return (uint32_t)(pin >> 2) + SYSCFG_EXTICR1;
}

/**
 * @brief   Helper function to configure the source of external interrupts
 *
 * @param[in] bank   The pin group that the pin belongs to
 * @param[in] pin    The pin the external interrupt is getting configured for
 */
static void configure_exti_source(gpio_bank_t bank, uint8_t pin)
{
    uint32_t value, reg, shift;
    reg = select_exti_reg(pin, &shift);
    value = select_exti_value(bank);
    SYSCONFIG_BASE(reg) &= ~(EXTI_MASK << shift);
    SYSCONFIG_BASE(reg) |= (value << shift);
}

/**
 * @brief   Starts the sysconfig peripheral clock
 */
void start_sysconfig(void)
{
    enable_peripheral_clock(SYSCONFIG_EN);
}

/**
 * @brief   Configures an external interrupt line
 *
 * @param[in] config   The pin that the external interrupt is getting
 *                     configured for
 */
void configure_exti_line(exti_select_t config)
{
    configure_exti_source(config.bank, config.pin);
}

/**
 * @brief   Retrieves the exti line configurations from the registers
 *
 * @param[in] channel        The channel to get the configurations for
 * @param[out] gpio_config   The exti line configurations
 */
void get_exti_line_config(uint8_t channel, exti_select_t *gpio_config)
{
    uint32_t shift, reg, value;
    gpio_config->pin = channel;
    reg = select_exti_reg(channel, &shift);
    value = SYSCONFIG_BASE(reg);
    value >>= shift;
    value &= EXTI_MASK;
    gpio_config->bank = (gpio_bank_t)(value << 8);
}
