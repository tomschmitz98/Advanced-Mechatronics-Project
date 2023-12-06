/*
 * reaction.c
 *
 *  Created on: Dec 4, 2023
 *      Author: Tom
 */

#include "reaction.h"
#include "core_m4.h"
#include "exti.h"
#include "productDef.h"
#include "stm_utils.h"
#include "timers.h"
#include <stdint.h>

static const exti_config_t ir_exti_5 = {
    .exti_gpio = {5, bank_d}, .rising_edge = true, .unmask_int = true};

static const exti_config_t ir_exti_6 = {
    .exti_gpio = {6, bank_d}, .rising_edge = true, .unmask_int = true};

static const exti_config_t ir_exti_7 = {
    .exti_gpio = {7, bank_d}, .rising_edge = true, .unmask_int = true};

static const exti_config_t ir_exti_8 = {
    .exti_gpio = {8, bank_c}, .rising_edge = true, .unmask_int = true};

static const exti_config_t ir_exti_9 = {
    .exti_gpio = {9, bank_c}, .rising_edge = true, .unmask_int = true};

static const irq_info_t exti5_9_irq = {INT_NUM_EXTI9_5, 9};

void config_reaction(void) {
    disable_global_irq();
    configure_interrupt(exti5_9_irq);
    config_exti(ir_exti_5);
    config_exti(ir_exti_6);
    config_exti(ir_exti_7);
    config_exti(ir_exti_8);
    config_exti(ir_exti_9);
    disable_irq(exti5_9_irq);
    enable_global_irq();
}

void EXTI9_5_IRQHandler(void) {
    stop_measurement();
    gEvents |= E_REACTION;
    acknowledge_multiple_exti_events(5, 6, 7, 8, 9, UNUSED_CHANNEL);
}

void start_reaction(void) {
    acknowledge_multiple_exti_events(5, 6, 7, 8, 9, UNUSED_CHANNEL);
    enable_irq(exti5_9_irq);
    start_measurement();
}

void stop_reaction(void) { disable_irq(exti5_9_irq); }

uint32_t read_reaction(void) { return read_measurement(); }
