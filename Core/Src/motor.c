/*
 * motor.c
 *
 *  Created on: Dec 6, 2023
 *      Author: Tom
 */

#include "motor.h"
#include "core_m4.h"
#include "exti.h"
#include "general_timers.h"
#include "gpio.h"
#include "pinout.h"
#include "productDef.h"
#include "timers.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define MOTOR_TX PIN_B0
#define MOTOR_RX PIN_B12

const gpio_config_t motorTx = {.gpio_bank = bank_b,
                               .pin_number = 0,
                               .intialOutValue = LOW,
                               .mode = output,
                               .resistor = no_pull,
                               .output_type = push_pull,
                               .speed = high_speed};

const gpio_config_t motorRx = {.gpio_bank = bank_b,
                               .pin_number = 12,
                               .mode = input,
                               .resistor = no_pull,
                               .output_type = push_pull,
                               .speed = high_speed};

const exti_config_t exti15_10 = {.exti_gpio = {12, bank_b},
                                 .falling_edge = true,
                                 .rising_edge = true,
                                 .unmask_int = true};

const irq_info_t exti15_10_info = {.interrupt_id = INT_NUM_EXTI9_5,
                                   .priority = MOTOR_PRIORITY};

static bool actuation_done = false;

void EXTI15_10_IRQHandler(void) {
    if (check_exti_channel_pending(12)) {
        gEvents |= E_ACTUATION_DONE;
        actuation_done = true;
    }
}

void init_motor_pins(void) {
    init_gpio(motorTx);
    init_gpio(motorRx);
    configure_interrupt(exti15_10_info);
    disable_irq(exti15_10_info);
    config_exti(exti15_10);
}

void start_slap(void) {
    acknowledge_multiple_exti_events(10, 11, 12, 13, 14, 15);
    enable_irq(exti15_10_info);
    setPin(MOTOR_TX);
}

void reset_slap(void) {
    acknowledge_multiple_exti_events(10, 11, 12, 13, 14, 15);
    enable_irq(exti15_10_info);
    clearPin(MOTOR_TX);
}

bool done_with_actuation(void) {
    return actuation_done;
}

void block_actuation_events(void) {
    disable_irq(exti15_10_info);
}
