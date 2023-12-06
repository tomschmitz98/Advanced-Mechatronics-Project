/*
 * button_io.c
 *
 *  Created on: Dec 6, 2023
 *      Author: Tom
 */

#include <stdbool.h>
#include "button_io.h"
#include "productDef.h"
#include "gpio.h"
#include "pinout.h"
#include <assert.h>
#include "stm_utils.h"

#define BUTTON_0 PIN_B3
#define BUTTON_1 PIN_B4

const gpio_config_t btn0 = {
		.gpio_bank = bank_b,
		.pin_number = 3,
		.mode = input,
		.output_type = push_pull,
		.resistor = pull_up,
		.speed = high_speed
};

const gpio_config_t btn1 = {
		.gpio_bank = bank_b,
		.pin_number = 4,
		.mode = input,
		.output_type = push_pull,
		.resistor = pull_up,
		.speed = high_speed
};

void init_buttons(void)
{
	init_gpio(btn0);
	init_gpio(btn1);
}

bool read_button(button_t btn)
{
	uint32_t ret;

	switch(btn)
	{
	case PAUSE_BUTTON: ret = readPin(BUTTON_0);
		break;
	case START_BUTTON: ret = readPin(BUTTON_1);
		break;
	default:
		assert(false);
	}

	return ret != 0;
}

bool button_changed_state(button_t btn)
{
	static bool btn_state_0 = false;
	static bool btn_state_1 = false;
	bool result, retVal;

	result = read_button(btn);

	switch(btn)
	{
	case PAUSE_BUTTON:
		retVal = result != btn_state_0;
		btn_state_0 = result;
		break;
	case START_BUTTON:
		retVal = result != btn_state_0;
		btn_state_0 = result;
		break;
	default:
		assert(false);
	}

	return retVal;
}
