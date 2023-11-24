/**
 * @file sysconfig.h
 * @version 1.0
 * @brief   Hardware abstraction layer for configuring the system hardware.
 *          This file is is compatible with the STM32F446C and STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_

#include "stm_utils.h"
#include <stdint.h>

/**
 * @brief   The pin to connect the external interrupt line to
 */
typedef struct
{
    uint8_t pin;      ///< The pin number
    gpio_bank_t bank; ///< The pin group the pin belongs in
} exti_select_t;

// TODO: Add more sysconfig stuff

/**
 * @brief   Starts the sysconfig peripheral clock
 */
void start_sysconfig(void);

/**
 * @brief   Configures an external interrupt line
 *
 * @param[in] config   The pin that the external interrupt is getting
 *                     configured for
 */
void configure_exti_line(exti_select_t config);

/**
 * @brief   Retrieves the exti line configurations from the registers
 *
 * @param[in] channel        The channel to get the configurations for
 * @param[out] gpio_config   The exti line configurations
 */
void get_exti_line_config(uint8_t channel, exti_select_t *gpio_config);

#endif /* SYSCONFIG_H_ */
