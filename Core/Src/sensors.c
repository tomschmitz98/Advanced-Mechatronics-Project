/*
 * ir_sensors.c
 *
 *  Created on: Dec 3, 2023
 *      Author: Tom
 */

#include "gpio.h"
#include "pinout.h"
#include "stm_utils.h"
#include "sensors.h"
#include "adc_bad.h"
#include <stdint.h>
#include <assert.h>
#include <sensors.h>

#define NUM_IR_SENSORS 5

#define IR_SENSOR_PIN0 PIN_B5
#define IR_SENSOR_PIN1 PIN_A1
#define IR_SENSOR_PIN2 PIN_A2
#define IR_SENSOR_PIN3 PIN_A3
#define IR_SENSOR_PIN4 PIN_A4

#define IR_SENSOR_0 0
#define IR_SENSOR_1 1
#define IR_SENSOR_2 2
#define IR_SENSOR_3 3
#define IR_SENSOR_4 4

#define IR_OPERATOR ==

const gpio_config_t ir0 = {
		.pin_number = 8,
		.gpio_bank = bank_c,
		.mode = input,
		.output_type = push_pull,
		.speed = high_speed,
		.resistor = no_pull
};

const gpio_config_t ir1 = {
		.pin_number = 9,
		.gpio_bank = bank_c,
		.mode = input,
		.output_type = push_pull,
		.speed = high_speed,
		.resistor = no_pull
};

const gpio_config_t ir2 = {
		.pin_number = 7,
		.gpio_bank = bank_d,
		.mode = input,
		.output_type = push_pull,
		.speed = high_speed,
		.resistor = no_pull
};

const gpio_config_t ir3 = {
		.pin_number = 6,
		.gpio_bank = bank_d,
		.mode = input,
		.output_type = push_pull,
		.speed = high_speed,
		.resistor = no_pull
};

const gpio_config_t ir4 = {
		.pin_number = 5,
		.gpio_bank = bank_d,
		.mode = input,
		.output_type = push_pull,
		.speed = high_speed,
		.resistor = no_pull
};

static bool ir_readings[NUM_IR_SENSORS] = { 0 };

void initialize_ir_sensors(void)
{
	init_gpio(ir0);
	init_gpio(ir1);
	init_gpio(ir2);
	init_gpio(ir3);
	init_gpio(ir4);
}

void refresh_ir_sensors(void)
{
	ir_readings[IR_SENSOR_0] = readPin(IR_SENSOR_PIN0) IR_OPERATOR 0;
	ir_readings[IR_SENSOR_1] = readPin(IR_SENSOR_PIN1) IR_OPERATOR 0;
	ir_readings[IR_SENSOR_2] = readPin(IR_SENSOR_PIN2) IR_OPERATOR 0;
	ir_readings[IR_SENSOR_3] = readPin(IR_SENSOR_PIN3) IR_OPERATOR 0;
	ir_readings[IR_SENSOR_4] = readPin(IR_SENSOR_PIN4) IR_OPERATOR 0;
}

bool all_ir_sensors_covered(void)
{
	uint8_t i = 0;

	for (; i < NUM_IR_SENSORS; i++)
	{
		if (!ir_readings[i])
		{
			return false;
		}
	}

	return true;
}

bool read_ir_sensor(ir_sensor_t sensor)
{
	bool retVal;

	switch(sensor)
	{
	case IR_0:
		retVal = readPin(IR_SENSOR_PIN0) IR_OPERATOR 0;
		break;
	case IR_1:
		retVal = readPin(IR_SENSOR_PIN1) IR_OPERATOR 0;
		break;
	case IR_2:
		retVal = readPin(IR_SENSOR_PIN2) IR_OPERATOR 0;
		break;
	case IR_3:
		retVal = readPin(IR_SENSOR_PIN3) IR_OPERATOR 0;
		break;
	case IR_4:
		retVal = readPin(IR_SENSOR_PIN4) IR_OPERATOR 0;
		break;
	default:
		assert(false);
	}

	return retVal;
}

void initialize_fsr(void)
{
	initADC3_5();
}
