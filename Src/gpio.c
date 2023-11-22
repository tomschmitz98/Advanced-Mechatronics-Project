/*
 * gpio.c
 *
 *  Created on: Jun 8, 2023
 *      Author: Tom
 */

#include <stdint.h>
#include "gpio.h"
#include "stm_rcc.h"


/* GPIO Base Addresses */
#define GPIO_BASE(bank, reg)  *(((uint32_t *)0x40020000) + bank + reg)

/* GPIO Offsets */
#define GPIO_MODER 0x0
#define GPIO_OTYPER 0x1
#define GPIO_OSPEEDR 0x2
#define GPIO_PUPDR 0x3
#define GPIO_IDR 0x4
#define GPIO_ODR 0x5
#define GPIO_BSRR 0x6
#define GPIO_LCKR 0x7
#define GPIO_AFRL 0x8
#define GPIO_AFRH 0x9

#define GPIO_LOCK_KEY 0x1 << 16


/* function definitions */

static void set_mode(uint32_t bank, uint32_t pin, uint32_t mode)
{
	GPIO_BASE(bank, GPIO_MODER) &= ~(0x3UL << (2UL * pin));
	GPIO_BASE(bank, GPIO_MODER) |= (mode & 0x3) << (2 * pin);
}

static void set_output_type(uint32_t bank, uint32_t pin, uint32_t type)
{
	GPIO_BASE(bank, GPIO_OTYPER) &= ~(0x1UL << pin);
	GPIO_BASE(bank, GPIO_OTYPER) |= type << pin;
}

static void set_speed(uint32_t bank, uint32_t pin, uint32_t speed)
{
	GPIO_BASE(bank, GPIO_OSPEEDR) &= ~(0x3UL << (2UL * pin));
	GPIO_BASE(bank, GPIO_OSPEEDR) |= speed << (2UL * pin);
}

static void set_resistor(uint32_t bank, uint32_t pin, uint32_t resistor)
{
	GPIO_BASE(bank, GPIO_PUPDR) &= ~(0x3UL << (2UL * pin));
	GPIO_BASE(bank, GPIO_PUPDR) |= resistor << (2UL * pin);
}

static void set_alt_function(uint32_t bank, uint32_t pin, uint32_t altFunc)
{
	uint32_t Reg = (pin < 8) ? GPIO_AFRL : GPIO_AFRH;
	GPIO_BASE(bank, Reg) &= ~(0xFUL << (4UL * pin));
	GPIO_BASE(bank, Reg) |= altFunc << (4UL * pin);
}

void init_gpio(gpio_config_t config)
{
	enable_gpio_clock(config.gpio_bank);
	set_mode((uint32_t)config.gpio_bank, (uint32_t)config.pin_number, (uint32_t)config.mode);
	set_output_type((uint32_t)config.gpio_bank, (uint32_t)config.pin_number, (uint32_t)config.output_type);
	set_speed((uint32_t)config.gpio_bank, (uint32_t)config.pin_number, (uint32_t)config.speed);
	set_resistor((uint32_t)config.gpio_bank, (uint32_t)config.pin_number, (uint32_t)config.resistor);
	set_alt_function((uint32_t)config.gpio_bank, (uint32_t)config.pin_number, (uint32_t)config.alternate_function);

	if (config.mode == output)
	{
		clearPin(config.gpio_bank, config.pin_number);
	}

	if (config.intialOutValue)
	{
		setPin(config.gpio_bank, config.pin_number);
	}
}

void init_gpios(const gpio_config_t config[], uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		init_gpio(config[i]);
	}
}

uint32_t readPin(gpio_bank_t bank, uint8_t bitmask)
{
	return GPIO_BASE((uint32_t)bank, GPIO_IDR) & (bitmask & 0xFFFF);
}

uint32_t readBank(gpio_bank_t bank)
{
	return GPIO_BASE((uint32_t)bank, GPIO_IDR) & 0xFFFF;
}

void setPin(gpio_bank_t bank, uint8_t pin)
{
	GPIO_BASE((uint32_t)bank, GPIO_ODR) |= 1 << (pin & 0xF);
}

void clearPin(gpio_bank_t bank, uint8_t pin)
{
	GPIO_BASE((uint32_t)bank, GPIO_ODR) &= ~(1UL << (pin & 0xFUL));
}

void togglePin(gpio_bank_t bank, uint8_t pin)
{
	GPIO_BASE((uint32_t)bank, GPIO_ODR) ^= 1 << (pin & 0xF);
}

uint32_t readBankOutput(gpio_bank_t bank)
{
	return GPIO_BASE((uint32_t)bank, GPIO_ODR) & 0xFFFF;
}

uint32_t readPinOutput(gpio_bank_t bank, uint8_t pin)
{
	return ((GPIO_BASE((uint32_t)bank, GPIO_ODR) & 0xF) >> (pin & 0xF)) & 0x1;
}

/* Atomic Functions */
void atomicSetPin(gpio_bank_t bank, uint8_t pin)
{
	GPIO_BASE((uint32_t)bank, GPIO_BSRR) |= 1 << (pin & 0xF);
}

void atomicClearPin(gpio_bank_t bank, uint8_t pin)
{
	GPIO_BASE((uint32_t)bank, GPIO_BSRR) |= UPPER16BITS(1 << (pin & 0xF));
}
