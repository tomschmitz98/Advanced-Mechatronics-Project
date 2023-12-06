/*
 * sysconfig.c
 *
 *  Created on: Nov 15, 2023
 *      Author: Tom
 */

#include "sysconfig.h"
#include "stm_rcc.h"
#include "stm_utils.h"
#include <assert.h>
#include <stdint.h>

#define SYSCONFIG_BASE(n) *(((uint32_t *)0x40013800) + n)
#define SYSCFG_MEMRMP     0 // TODO: Write stuff
#define SYSCFG_PMC        1 // TODO: Write stuff
#define SYSCFG_EXTICR1    2
#define SYSCFG_EXTICR2    3
#define SYSCFG_EXTICR3    4
#define SYSCFG_EXTICR4    5
#define SYSCFG_CMPCR      6 // TODO: Write stuff
#define SYSCFG_CFGR       7 // TODO: Write stuff

#define EXTI_MASK         0xF

static uint32_t select_exti_value(gpio_bank_t bank) {
    return (uint32_t)bank >> 8;
}

static uint32_t select_exti_reg(uint8_t pin, uint32_t *shift) {
    assert(pin < 16);
    *shift = ((uint32_t)pin & 0x3) << 2;
    return (uint32_t)(pin >> 2) + SYSCFG_EXTICR1;
}

static void configure_exti_source(gpio_bank_t bank, uint8_t pin) {
    uint32_t value, reg, shift;
    reg = select_exti_reg(pin, &shift);
    value = select_exti_value(bank);
    SYSCONFIG_BASE(reg) &= ~(EXTI_MASK << shift);
    SYSCONFIG_BASE(reg) |= (value << shift);
}

void start_sysconfig(void) { enable_peripheral_clock(SYSCONFIG_EN); }

void configure_exti_line(exti_select_t config) {
    configure_exti_source(config.bank, config.pin);
}

void get_exti_line_config(uint8_t channel, exti_select_t *gpio_config) {
    uint32_t shift, reg, value;
    gpio_config->pin = channel;
    reg = select_exti_reg(channel, &shift);
    value = SYSCONFIG_BASE(reg);
    value >>= shift;
    value &= EXTI_MASK;
    gpio_config->bank = (gpio_bank_t)(value << 8);
}
