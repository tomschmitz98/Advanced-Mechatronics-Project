/**
 * @file gpio.c
 * @version 1.0
 * @brief   Hardware abstraction layer for the gpio pins.
 *          This file is compatible with the STM32F446E and
 *          STM32F446C.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#include "gpio.h"
#include "stm_rcc.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

/* GPIO Base Address */
#define GPIO_BASE(bank, reg) *(((uint32_t *)0x40020000) + bank + reg)

/* GPIO Offsets */
#define GPIO_MODER    0x0
#define GPIO_OTYPER   0x1
#define GPIO_OSPEEDR  0x2
#define GPIO_PUPDR    0x3
#define GPIO_IDR      0x4
#define GPIO_ODR      0x5
#define GPIO_BSRR     0x6
#define GPIO_LCKR     0x7
#define GPIO_AFRL     0x8
#define GPIO_AFRH     0x9

#define GPIO_LOCK_KEY 0x1 << 16

/* function definitions */

/**
 * @brief set the mode for the given GPIO pin
 *
 * @param[in] bank   The GPIO bank, otherwise pin group, that the pin belongs
 *                   in
 * @param[in] pin    The GPIO pin number
 * @param[in] mode   The new mode of the GPIO pin
 */
static void set_mode(uint32_t bank, uint32_t pin, uint32_t mode)
{
    GPIO_BASE(bank, GPIO_MODER) &= ~(0x3UL << (2UL * pin));
    GPIO_BASE(bank, GPIO_MODER) |= (mode & 0x3) << (2 * pin);
}

/**
 * @brief   set the output type of the GPIO pin
 *
 * @param[in] bank   The GPIO bank, otherwise pin group, that the pin belongs
 *                   in
 * @param[in] pin    The GPIO pin number
 * @param[in] type   The output type of the GPIO pin
 */
static void set_output_type(uint32_t bank, uint32_t pin, uint32_t type)
{
    GPIO_BASE(bank, GPIO_OTYPER) &= ~(0x1UL << pin);
    GPIO_BASE(bank, GPIO_OTYPER) |= type << pin;
}

/**
 * @brief   Set the speed of the GPIO pin
 *
 * @param[in] bank    The GPIO bank, otherwise pin group, that the pin belongs
 *                    in
 * @param[in] pin     The GPIO pin number
 * @param[in] speed   The speed of the GPIO pin
 */
static void set_speed(uint32_t bank, uint32_t pin, uint32_t speed)
{
    GPIO_BASE(bank, GPIO_OSPEEDR) &= ~(0x3UL << (2UL * pin));
    GPIO_BASE(bank, GPIO_OSPEEDR) |= speed << (2UL * pin);
}

/**
 * @brief   Set the pull resistor of the GPIO pin
 *
 * @param[in] bank       The GPIO bank, otherwise pin group, that the pin
 *                       belongs in
 * @param[in] pin        The GPIO pin number
 * @param[in] resistor   The type of resistor pull
 */
static void set_resistor(uint32_t bank, uint32_t pin, uint32_t resistor)
{
    GPIO_BASE(bank, GPIO_PUPDR) &= ~(0x3UL << (2UL * pin));
    GPIO_BASE(bank, GPIO_PUPDR) |= resistor << (2UL * pin);
}

/**
 * @brief   Sets the alternate function of a GPIO pin
 *
 * @param[in] bank      The GPIO bank, otherwise pin group, that the pin
 *                      belongs in
 * @param[in] pin       The GPIO pin number
 * @param[in] altFunc   The alternate function of the pin
 */
static void set_alt_function(uint32_t bank, uint32_t pin, uint32_t altFunc)
{
    uint32_t Reg = (pin < 8) ? GPIO_AFRL : GPIO_AFRH;
    GPIO_BASE(bank, Reg) &= ~(0xFUL << (4UL * pin));
    GPIO_BASE(bank, Reg) |= altFunc << (4UL * pin);
}

/**
 * @brief   Configures a given GPIO pin
 *
 * @param[in] config   The GPIO pin configurations
 */
void init_gpio(gpio_config_t config)
{
    enable_gpio_clock(config.gpio_bank);

    set_mode((uint32_t)config.gpio_bank, (uint32_t)config.pin_number,
             (uint32_t)config.mode);

    set_output_type((uint32_t)config.gpio_bank, (uint32_t)config.pin_number,
                    (uint32_t)config.output_type);

    set_speed((uint32_t)config.gpio_bank, (uint32_t)config.pin_number,
              (uint32_t)config.speed);

    set_resistor((uint32_t)config.gpio_bank, (uint32_t)config.pin_number,
                 (uint32_t)config.resistor);

    set_alt_function((uint32_t)config.gpio_bank, (uint32_t)config.pin_number,
                     (uint32_t)config.alternate_function);

    if (config.mode == output)
    {
        clearPin(config.gpio_bank, config.pin_number);
    }

    if (config.intialOutValue)
    {
        setPin(config.gpio_bank, config.pin_number);
    }
}

/**
 * @brief   Initializes multiple GPIO pins
 *
 * @param[in] config   The GPIO pin configurations
 * @param[in] size     The number of GPIO pin configurations to be done
 */
void init_gpios(const gpio_config_t config[], uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        init_gpio(config[i]);
    }
}

/**
 * @brief   Reads a GPIO input pin
 *
 * @param[in] bank   The GPIO bank, otherwise pin group, that the pin belongs
 *                   in
 * @param[in] pin    The GPIO pin number
 *
 * @return   true if the pin has a logic high voltage
 * @return   false if the pin has a logic low voltage
 */
bool readPin(gpio_bank_t bank, uint8_t pin)
{
    uint16_t mask = 1UL << (pin & 0xF);
    return (GPIO_BASE((uint32_t)bank, GPIO_IDR) & mask) != 0;
}

/**
 * @brief   Reads a GPIO bank given a bitmask
 *
 * @param[in] bank      The GPIO bank, otherwise pin group, to read from
 * @param[in] bitmask   The value to mask the result by
 *
 * @return   The GPIO bank masked results
 */
uint32_t readGPIOInputs(gpio_bank_t bank, uint16_t bitmask)
{
    return GPIO_BASE((uint32_t)bank, GPIO_IDR) & (bitmask & 0xFFFF);
}

/**
 * @brief   Reads a GPIO bank without a mask
 *
 * @param[in] bank   The GPIO bank, otherwise pin group, to read from
 *
 * @return   The logic levels of the GPIO bank
 */
uint32_t readBank(gpio_bank_t bank)
{
    return GPIO_BASE((uint32_t)bank, GPIO_IDR) & 0xFFFF;
}

/**
 * @brief   Sets a GPIO output pin to be logic high
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void setPin(gpio_bank_t bank, uint8_t pin)
{
    GPIO_BASE((uint32_t)bank, GPIO_ODR) |= 1 << (pin & 0xF);
}

/**
 * @brief   Clears a GPIO output pin (sets it to be logic low)
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void clearPin(gpio_bank_t bank, uint8_t pin)
{
    GPIO_BASE((uint32_t)bank, GPIO_ODR) &= ~(1UL << (pin & 0xFUL));
}

/**
 * @brief   Toggles a GPIO output pin. If the pin is high, then using this will
 *          clear the pin, and vice versa
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void togglePin(gpio_bank_t bank, uint8_t pin)
{
    GPIO_BASE((uint32_t)bank, GPIO_ODR) ^= 1 << (pin & 0xF);
}

/**
 * @brief   Reads the output values of the GPIO bank
 *
 * @param[in] bank   The GPIO bank, otherwise pin group, to read from
 *
 * @return   The output data values
 */
uint32_t readBankOutput(gpio_bank_t bank)
{
    return GPIO_BASE((uint32_t)bank, GPIO_ODR) & 0xFFFF;
}

/**
 * @brief   Reads the output value of a pin
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 *
 * @return   true if the pin is outputting logic high
 * @return   false if the pin is outputting logic low
 */
bool readPinOutput(gpio_bank_t bank, uint8_t pin)
{
    return ((GPIO_BASE((uint32_t)bank, GPIO_ODR) & 0xF) >> (pin & 0xF)) != 0;
}

/**
 * @brief   Atomically sets the GPIO output pin high
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void atomicSetPin(gpio_bank_t bank, uint8_t pin)
{
    GPIO_BASE((uint32_t)bank, GPIO_BSRR) |= 1 << (pin & 0xF);
}

/**
 * @brief   Atomically clears the GPIO output pin (sets it low)
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void atomicClearPin(gpio_bank_t bank, uint8_t pin)
{
    GPIO_BASE((uint32_t)bank, GPIO_BSRR) |= UPPER16BITS(1 << (pin & 0xF));
}

/**
 * @brief   Locks the pin configurations in a given GPIO bank
 *
 * @param[in] bank   The GPIO bank, otherwise pin group, to lock
 * @param[in] mask   The mask of the GPIO pins to lock
 */
void lockGPIOConfigurations(gpio_bank_t bank, uint16_t mask)
{
    uint32_t lock = GPIO_LOCK_KEY | (uint32_t)mask;

    if (GPIO_BASE((uint32_t)bank, GPIO_LCKR) & GPIO_LOCK_KEY)
    {
        return;
    }

    GPIO_BASE((uint32_t)bank, GPIO_LCKR) = lock;
    GPIO_BASE((uint32_t)bank, GPIO_LCKR) = (uint32_t)mask;
    GPIO_BASE((uint32_t)bank, GPIO_LCKR) = lock;
    lock = GPIO_BASE((uint32_t)bank, GPIO_LCKR);
    lock = GPIO_BASE((uint32_t)bank, GPIO_LCKR);

    assert(lock & GPIO_LOCK_KEY);
}
