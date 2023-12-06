/*
 * motor.c
 *
 *  Created on: Dec 6, 2023
 *      Author: Tom
 */

#include "motor.h"
#include "general_timers.h"
#include "gpio.h"
#include "pinout.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define DIRECTION_0     PIN_B6
#define DIRECTION_1     PIN_B15

#define TIM3_AUTORELOAD 30
#define MAX_PWM_VAL     80

const gpio_config_t dirPin0 = {.gpio_bank = bank_b,
                               .pin_number = 6,
                               .mode = output,
                               .output_type = push_pull,
                               .resistor = no_pull,
                               .speed = high_speed,
                               .intialOutValue = LOW};

const gpio_config_t dirPin1 = {.gpio_bank = bank_b,
                               .pin_number = 15,
                               .mode = output,
                               .output_type = push_pull,
                               .resistor = no_pull,
                               .speed = high_speed,
                               .intialOutValue = HIGH};

const gpio_config_t pwmPin = {.gpio_bank = bank_b,
                              .pin_number = 5,
                              .mode = alternate_function,
                              .output_type = push_pull,
                              .speed = high_speed,
                              .resistor = no_pull,
                              .alternate_function = 2};

typedef enum { CW, CCW } motor_direction_t;

static bool run_slap = false;
static uint32_t delT = 0;

void init_motor(void) {
    init_gpio(dirPin0);
    init_gpio(dirPin1);
    init_gpio(pwmPin);
}

static void setDirection(motor_direction_t dir) {
    switch (dir) {
    case CW: {
        setPin(DIRECTION_0);
        clearPin(DIRECTION_1);
    } break;
    case CCW: {
        setPin(DIRECTION_1);
        clearPin(DIRECTION_0);
    } break;
    default: {
        assert(false);
    }
    }
}

void pwm(int16_t val) {
    motor_direction_t dir = CW;
    if (val < 0) {
        val = -val;
        dir = CCW;
    }
    setDirection(dir);

    val = (val > MAX_PWM_VAL) ? MAX_PWM_VAL : val;
    delT = (val * TIM3_AUTORELOAD) / 100;
    val = 100 / val;
    val = TIM3_AUTORELOAD / val;

    reconfigureCompareChannel(TIMER3, 3, val);
}

void start_slap(void) { run_slap = true; }
