/**
 * @file gpio.h
 * @version 1.0
 * @brief   Hardware abstraction layer for the gpio pins.
 *          This file is compatible with the STM32F446E and
 *          STM32F446C.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "stm_utils.h"
#include <stdbool.h>
#include <stdint.h>

/* Common macros for indicating high and low */
#define HIGH 1
#define LOW  0

/**
 * @brief   The mode to set the GPIO pin in
 */
typedef enum
{
    input = 0x0,              ///< Place into input mode
    output = 0x1,             ///< Place into output mode
    alternate_function = 0x2, ///< Use pin for an alternate function; Must
                              ///< specify an alternate function number
    analog = 0x3              ///< Configure pin to be an analog pin
} gpio_mode_t;

/**
 * @brief   Determines if the output will be treated as a push pull pin or
 *          an open drain
 */
typedef enum
{
    push_pull = 0x0,
    open_drain = 0x1
} gpio_output_type_t;

/**
 * @brief   Speed of the GPIO pin
 */
typedef enum
{
    low_speed = 0x0,
    medium_speed = 0x1,
    fast_speed = 0x2,
    high_speed = 0x3
} gpio_speed_t;

/**
 * @brief   The type of pull resistor used on the pin
 */
typedef enum
{
    no_pull = 0x0,
    pull_up = 0x1,
    pull_down = 0x2
} gpio_resistor_t;

/**
 * @brief   Configuration data of the GPIO pin
 */
typedef struct
{
    uint8_t pin_number;             ///< Pin number that is being configured
    uint8_t alternate_function;     ///< Alternate function of the pin; This
                                    ///< parameter has no affect when GPIO does not
                                    ///< get configured for alternate mode
    uint8_t intialOutValue;         ///< Initial output value of the pin; This
                                    ///< parameter has no affect when not operating
                                    ///< in output mode
    gpio_bank_t gpio_bank;          ///< The pin group that the pin belongs in
    gpio_mode_t mode;               ///< The mode of the GPIO pin. This can be
                                    ///< input, output, analog, or
                                    ///< alternate_function
    gpio_output_type_t output_type; ///< Configure the output type of the pin;
                                    ///< This has no affect in input mode
    gpio_speed_t speed;             ///< Configure the speed of the pin (How
                                    ///< quickly it reads)
    gpio_resistor_t resistor;       ///< Configures a pull up or pull down
                                    ///< resistor; If no resistor is needed,
                                    ///< no_pull should be used
} gpio_config_t;

/**
 * @brief   Configures a given GPIO pin
 *
 * @param[in] config   The GPIO pin configurations
 */
void init_gpio(gpio_config_t config);

/**
 * @brief   Initializes multiple GPIO pins
 *
 * @param[in] config   The GPIO pin configurations
 * @param[in] size     The number of GPIO pin configurations to be done
 */
void init_gpios(const gpio_config_t config[], uint8_t size);

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
bool readPin(gpio_bank_t bank, uint8_t pin);

/**
 * @brief   Reads a GPIO bank given a bitmask
 *
 * @param[in] bank      The GPIO bank, otherwise pin group, to read from
 * @param[in] bitmask   The value to mask the result by
 *
 * @return   The GPIO bank masked results
 */
uint32_t readGPIOInputs(gpio_bank_t bank, uint8_t bitmask);

/**
 * @brief   Reads a GPIO bank without a mask
 *
 * @param[in] bank   The GPIO bank, otherwise pin group, to read from
 *
 * @return   The logic levels of the GPIO bank
 */
uint32_t readBank(gpio_bank_t bank);

/**
 * @brief   Sets a GPIO output pin to be logic high
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void setPin(gpio_bank_t bank, uint8_t pin);

/**
 * @brief   Clears a GPIO output pin (sets it to be logic low)
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void clearPin(gpio_bank_t bank, uint8_t pin);

/**
 * @brief   Toggles a GPIO output pin. If the pin is high, then using this will
 *          clear the pin, and vice versa
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void togglePin(gpio_bank_t bank, uint8_t pin);

/**
 * @brief   Reads the output values of the GPIO bank
 *
 * @param[in] bank   The GPIO bank, otherwise pin group, to read from
 *
 * @return   The output data values
 */
uint32_t readBankOutput(gpio_bank_t bank);

/**
 * @brief   Reads the output value of a pin
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 *
 * @return   true if the pin is outputting logic high
 * @return   false if the pin is outputting logic low
 */
bool readPinOutput(gpio_bank_t bank, uint8_t pin);

/**
 * @brief   Atomically sets the GPIO output pin high
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void atomicSetPin(gpio_bank_t bank, uint8_t pin);

/**
 * @brief   Atomically clears the GPIO output pin (sets it low)
 *
 * @param[in] bank   The bank, otherwise pin group, the pin belongs in
 * @param[in] pin    The pin number of the GPIO pin
 */
void atomicClearPin(gpio_bank_t bank, uint8_t pin);

/**
 * @brief   Locks the pin configurations in a given GPIO bank
 *
 * @param[in] bank   The GPIO bank, otherwise pin group, to lock
 * @param[in] mask   The mask of the GPIO pins to lock
 */
void lockGPIOConfigurations(gpio_bank_t bank, uint16_t mask)

#endif /* GPIO_H_ */
