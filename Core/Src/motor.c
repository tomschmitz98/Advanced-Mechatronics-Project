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
#include "timers.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "stdio.h"
#include "core_m4.h"
#include "exti.h"

#define DIRECTION_0     PIN_B6
#define DIRECTION_1     PIN_B15

#define ENCODER_0 PIN_B12
#define ENCODER_1 PIN_B13

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

const gpio_config_t encoder0 = {
		.gpio_bank = bank_b,
		.pin_number = 12,
		.mode = input,
		.output_type = push_pull,
		.speed = high_speed,
		.resistor = pull_down
};

const gpio_config_t encoder1 = {
		.gpio_bank = bank_b,
		.pin_number = 13,
		.mode = input,
		.output_type = push_pull,
		.speed = high_speed,
		.resistor = pull_down
};

const exti_config_t exti12 = {
		.exti_gpio = {12, bank_b},
		.rising_edge = true,
		.falling_edge = true,
		.unmask_int = true
};

const irq_info_t exti15_10_info = {
		.interrupt_id = INT_NUM_EXTI15_10,
		.priority = 8
};

typedef enum { CW, CCW } motor_direction_t;

static bool run_slap = false;
static uint32_t delT = 0;
static int16_t angle = 0;
static int16_t old_angle = 0;
static int16_t ierr = 0;

void init_motor(void) {
    init_gpio(dirPin0);
    init_gpio(dirPin1);
    init_gpio(pwmPin);

    init_gpio(encoder0);
    init_gpio(encoder1);

    configure_interrupt(exti15_10_info);
    config_exti(exti12);
}

static void updateT(void)
{
	delT = getCounterValue(TIMER4);
}

void EXTI15_10_IRQHandle(void)
{
	static bool a_curr = false;
	static bool b_curr = false;

    bool valueB13 = 0;
    bool valueB12 = 0;

    if (check_exti_channel_pending(12))
    {
    	valueB12 = readPin(ENCODER_0);
    	valueB13 = readPin(ENCODER_1);

    	if (valueB13 && !valueB12)
    	{
    		a_curr = true;
    	}
    	else if (!valueB13 && valueB12)
    	{
    		b_curr = true;
    	}
    	else if (valueB13 && valueB12)
    	{
    		a_curr = false;
    	}
    	else
    	{
    		b_curr = false;
    	}

    	if (a_curr && b_curr)
    	{
    		updateT();
    		angle += 1;
    	}
    	else if (!a_curr && !b_curr)
    	{
    		updateT();
    		angle -= 1;
    	}
    }
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

static void pwm(int16_t val) {
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

static int16_t pid_control(int16_t kp, int16_t kd, int16_t ki, int16_t desired_angle)
{
	int16_t err;
	int16_t derr;
	err = desired_angle - angle;

	if (!err)
	{
		derr = 0;
	}
	else
	{
		derr = (angle - old_angle) / delT;
	}
	ierr = ierr + delT * err;
	return kp * err + kd * derr;
}

static void pid_loop(int16_t desired_angle)
{
	const uint32_t timediff = 100;
	int16_t result = 1;

	uint32_t startTime = current_ts();
	uint32_t currentTime;

	old_angle = -20;

	while (1)
	{
		result = pid_control(28, 0.3, 0.4, desired_angle);

		pwm(result);

		if (desired_angle == old_angle && desired_angle == angle)
		{
			break;
		}

		currentTime = current_ts();

		if (currentTime >= (startTime + timediff))
		{
			startTime = currentTime;
			old_angle = angle;
		}
	}

	setPin(DIRECTION_0);
	setPin(DIRECTION_1);
	pwm(0);
}

void slap(void)
{
	pid_loop(30);
}

void reset_motor(void)
{
	pid_loop(0);
}

void start_slap(void) { run_slap = true; }
