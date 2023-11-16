/*
 * gpio.h
 *
 *  Created on: Jun 8, 2023
 *      Author: Tom
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
#include "stm_utils.h"

#define HIGH 1
#define LOW 0

typedef enum {
	input = 0x0,
	output = 0x1,
	alternate_function = 0x2,
	analog = 0x3
} gpio_mode_t;

typedef enum {
	push_pull = 0x0,
	open_drain = 0x1
} gpio_output_type_t;

typedef enum {
	low_speed = 0x0,
	medium_speed = 0x1,
	fast_speed = 0x2,
	high_speed = 0x3
} gpio_speed_t;

typedef enum {
	no_pull = 0x0,
	pull_up = 0x1,
	pull_down = 0x2
} gpio_resistor_t;

typedef struct {
	uint8_t pin_number;
	uint8_t alternate_function;
	uint8_t intialOutValue;

	gpio_bank_t gpio_bank;
	gpio_mode_t mode;
	gpio_output_type_t output_type;
	gpio_speed_t speed;
	gpio_resistor_t resistor;
} gpio_config_t;

/* Initialization functions */
void init_gpio(gpio_config_t config);
void init_gpios(const gpio_config_t config[], uint8_t size);

/* Input Functions */
uint32_t readPin(gpio_bank_t bank, uint8_t bitmask);
uint32_t readBank(gpio_bank_t bank);

/* Output functions */
void setPin(gpio_bank_t bank, uint8_t pin);
void clearPin(gpio_bank_t bank, uint8_t pin);
void togglePin(gpio_bank_t bank, uint8_t pin);
uint32_t readBankOutput(gpio_bank_t bank);
uint32_t readPinOutput(gpio_bank_t bank, uint8_t pin);

/* Atomic Functions */
void atomicSetPin(gpio_bank_t bank, uint8_t pin);
void atomicClearPin(gpio_bank_t bank, uint8_t pin);

#endif /* GPIO_H_ */
