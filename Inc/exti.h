/**
 * @file exti.h
 * @version 1.0
 *
 * @brief   Hardware abstraction layer for the external interrupts. This file
 *          is compatible with the STM32F446C and STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#ifndef EXTI_H_
#define EXTI_H_

#include "stm_utils.h"
#include "sysconfig.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * Macro used to indicate an unused channel. This should only be used to
 * acknowledge multiple exti events.
 */
#define UNUSED_CHANNEL 32

/**
 * @brief   Configuration data for external interrupts
 */
typedef struct
{
    union
    {
        exti_select_t exti_gpio; ///< GPIO pin to configure for external
                                 ///< interrupts
        uint8_t exti_line;       ///< External interrupt line to configure for.
                                 ///< This should be used for the special
                                 ///< channels only
    };
    bool unmask_int;   ///< Unmask interrupts, thus allowing the IRQ to be
                       ///< delivered
    bool unmask_event; ///< Unmask events, thus allowing events to occur
    bool rising_edge;  ///< Interrupt or generate events on rising edges
    bool falling_edge; ///< Interrupt ot generate events on falling edges
} exti_config_t;

/**
 * @brief   Configures the given external interrupt
 *
 * @param[in] config   The external interrupt configurations
 */
void config_exti(exti_config_t config);

/**
 * @brief   Retrieves the external configurations of a given external interrupt
 *          channel.
 *
 * @param[in] channel   The channel to retrieve the configurations for
 * @param[out] config   The configurations of the channel
 */
void get_exti_configs(uint8_t channel, exti_config_t *config);

/**
 * @brief   Generates a software event for a given external interrupt line
 *
 * @param[in] channel   The channel to generate the event for
 */
void generate_exti_software_event(uint8_t channel);

/**
 * @brief   Checks if a software event was generated
 *
 * @param[in] channel   The channel to check for the event on
 *
 * @return   true if an event was generated
 *           false if an event was not generated
 */
bool exti_software_event_generated(uint8_t channel);

/**
 * @brief   Checks if an exti channel has a pending interrupt or event
 *
 * @param[in] channel   The exti channel to check for a pending interrupt or
 *                      event on
 *
 * @return   true if there is a pending event or interrupt on the given channel
 * @return   false if there is not a pending event or interrupt on the given
 *           channel
 */
bool check_exti_channel_pending(uint8_t channel);

/**
 * @brief   Acknowledges or clears the external event or interrupt. Failure
 *          to acknowledge an external event or interrupt will result in a
 *          deadlock.
 *
 * @param[in] channel   The channel to acknowledge the interrupt for
 */
void acknowledge_exti_event(uint8_t channel);

/**
 * @brief   Acknowledges or clears multiple external events or interrupts.
 *          Failure to clear external interrupts will result in a deadlock.
 *
 * @param[in] channel0   First channel to acknowledge
 * @param[in] channel1   Second channel to acknowledge
 * @param[in] channel2   Third channel to acknowledge
 * @param[in] channel3   Fourth channel to acknowledge
 * @param[in] channel4   Fifth channel to acknowledge
 * @param[in] channel5   Sixth channel to acknowledge
 */
void acknowledge_multiple_exti_events(uint8_t channel0, uint8_t channel1,
                                      uint8_t channel2, uint8_t channel3,
                                      uint8_t channel4, uint8_t channel5);

/**
 * @brief   Clears all pending external interrupts
 */
void clear_pending_exti_events(void);

#endif /* EXTI_H_ */
