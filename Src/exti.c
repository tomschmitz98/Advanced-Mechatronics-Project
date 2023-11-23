/**
 * @file exti.h
 * @version 1.0
 *
 * @brief   Hardware abstraction layer for the external interrupts. This file
 *          is compatible with the STM32F446C and STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#include "exti.h"
#include "sysconfig.h"
#include <assert.h>
#include <stdint.h>

/* Valid masks for external interrupts */
#define EXTI_VALID_MASK    0x7FFFFF
#define EXTI_EDGE_REG_MASK 0x77FFFF

/* General Macros */
#define EXTI_CHANNELS 23
#define PINS_PER_BANK 16

/* EXTI Base Address and Offsets */
#define EXTI_BASE(n) *(((uint32_t *)0x40013C00) + n)
#define EXTI_IMR     0
#define EXTI_EMR     1
#define EXTI_RTSR    2
#define EXTI_FTSR    3
#define EXTI_SWIER   4
#define EXTI_PR      5

/**
 * @brief   Unmask external interrupts
 *
 * @param[in] mask   Mask of the external interrupts to enable
 */
static void set_exti_int_mask(uint32_t mask)
{
    mask &= EXTI_VALID_MASK;
    EXTI_BASE(EXTI_IMR) |= mask;
}

/**
 * @brief   Mask external interrupts
 *
 * @param[in] mask   Mask of the external interrupts to disable
 */
static void clear_exti_int_mask(uint32_t mask)
{
    mask &= EXTI_VALID_MASK;
    EXTI_BASE(EXTI_IMR) &= ~mask;
}

/**
 * @brief   Reads the external interrupt mask register
 *
 * @return   The external interrupt mask
 */
static uint32_t read_exti_int_mask(void)
{
    return EXTI_BASE(EXTI_IMR);
}

/**
 * @brief   Unmask external events
 *
 * @param[in] mask   Mask of the external events to enable
 */
static void set_exti_event_mask(uint32_t mask)
{
    mask &= EXTI_VALID_MASK;
    EXTI_BASE(EXTI_EMR) |= mask;
}

/**
 * @brief   Mask external events
 *
 * @param[in] mask   Mask of the external events to disable
 */
static void clear_exti_event_mask(uint32_t mask)
{
    mask &= EXTI_VALID_MASK;
    EXTI_BASE(EXTI_EMR) &= ~mask;
}

/**
 * @brief   Reads the external event mask register
 *
 * @return   The external event mask
 */
static uint32_t read_exti_event_mask(void)
{
    return EXTI_BASE(EXTI_EMR);
}

/**
 * @brief   Sets an event or interrupt to be detected on the rising edge
 *
 * @param[in] mask   The external interrupts to detect on rising edge
 */
static void set_exti_rising_edge(uint32_t mask)
{
    mask &= EXTI_EDGE_REG_MASK;
    EXTI_BASE(EXTI_RTSR) |= mask;
}

/**
 * @brief   Sets an event or interrupt to not be detected on the rising edge
 *
 * @param[in] mask   The external interrupts to not detect on rising edge
 */
static void clear_exti_rising_edge(uint32_t mask)
{
    mask &= EXTI_EDGE_REG_MASK;
    EXTI_BASE(EXTI_RTSR) &= ~mask;
}

/**
 * @brief   Reads the rising edge configurations of the external interrupts
 *
 * @return   Unmasked external interrupts
 */
static uint32_t read_exti_rising_edge(void)
{
    return EXTI_BASE(EXTI_RTSR);
}

/**
 * @brief   Sets an event or interrupt to be detected on the falling edge
 *
 * @param[in] mask   The external interrupts to detect on falling edge
 */
static void set_exti_falling_edge(uint32_t mask)
{
    mask &= EXTI_EDGE_REG_MASK;
    EXTI_BASE(EXTI_FTSR) |= mask;
}

/**
 * @brief   Sets an event or interrupt to not be detected on the falling edge
 *
 * @param[in] mask   The external interrupts to not detect on falling edge
 */
static void clear_exti_falling_edge(uint32_t mask)
{
    mask &= EXTI_EDGE_REG_MASK;
    EXTI_BASE(EXTI_FTSR) &= ~mask;
}

/**
 * @brief   Reads the falling edge configurations of the external interrupts
 *
 * @return   Unmasked external interrupts
 */
static uint32_t read_exti_falling_edge(void)
{
    return EXTI_BASE(EXTI_FTSR);
}

/**
 * @brief   Generates events or interrupts from software
 *
 * @param[in] mask   The interrupts to generate events for
 */
static void set_exti_software_int_event(uint32_t mask)
{
    mask &= EXTI_VALID_MASK;
    EXTI_BASE(EXTI_SWIER) |= mask;
}

/**
 * @brief   Reads the status of the software event generation register
 *
 * @return   The active software events
 */
static uint32_t read_exti_software_int_event(void)
{
    return EXTI_BASE(EXTI_SWIER);
}

/**
 * @brief   Reads the pending external interrupts
 *
 * @return   Mask of the pending external interrupts
 */
static uint32_t read_exti_pending(void)
{
    return EXTI_BASE(EXTI_PR);
}

/**
 * @brief   Clears or acknowledges the pending external interrupts. This also
 *          clears the appropriate bits in the software event generation
 *          register
 *
 * @param[in] mask   The mask of interrupts to clear
 */
static void clear_exti_pending(uint32_t mask)
{
    mask &= EXTI_VALID_MASK;
    EXTI_BASE(EXTI_PR) = mask;
}

/**
 * @brief   Helper function to either mask or unmask the appropriate bits for
 *          external interrupts
 *
 * @param[in] unmask            true if the bits are getting unmasked, false if
 *                              the bits are getting masked
 * @param[in] callback_unmask   The helper for unmasking the bits
 * @param[in] callback_mask     The helper for masking the bits
 * @param[in] mask              The mask of the external interrupt(s)
 */
static void configure_exti_channel(bool unmask,
                                   void (*callback_unmask)(uint32_t),
                                   void (*callback_mask)(uint32_t),
                                   uint32_t mask)
{
    if (unmask)
    {
        callback_unmask(mask);
    }
    else
    {
        callback_mask(mask);
    }
}

/**
 * @brief   Helper function for configuring the external interrupts for GPIO
 *          pins
 *
 * @param[in] config   The external interrupt configurations
 */
static void config_gpio_exti(exti_config_t config)
{
    uint32_t mask = 0;
    start_sysconfig();
    configure_exti_line(config.exti_gpio);
    SET_BIT(mask, config.exti_gpio.pin);

    configure_exti_channel(config.unmask_int, set_exti_int_mask,
                           clear_exti_int_mask, mask);
    configure_exti_channel(config.unmask_event, set_exti_event_mask,
                           clear_exti_event_mask, mask);
    configure_exti_channel(config.rising_edge, set_exti_rising_edge,
                           clear_exti_rising_edge, mask);
    configure_exti_channel(config.falling_edge, set_exti_falling_edge,
                           clear_exti_falling_edge, mask);
}

/**
 * @brief   Helper function for configuring the special external interrupts
 *
 * @param[in] config   The external interrupt configurations
 */
static void config_special_exti(exti_config_t config)
{
    uint32_t mask = 0;

    SET_BIT(mask, config.exti_line);

    configure_exti_channel(config.unmask_int, set_exti_int_mask,
                           clear_exti_int_mask, mask);
    configure_exti_channel(config.unmask_event, set_exti_event_mask,
                           clear_exti_event_mask, mask);
    configure_exti_channel(config.rising_edge, set_exti_rising_edge,
                           clear_exti_rising_edge, mask);
    configure_exti_channel(config.falling_edge, set_exti_falling_edge,
                           clear_exti_falling_edge, mask);
}

/**
 * @brief   Configures the given external interrupt
 *
 * @param[in] config   The external interrupt configurations
 */
void config_exti(exti_config_t config)
{
    if (config.exti_line >= PINS_PER_BANK)
    {
        config_special_exti(config);
    }
    else
    {
        config_gpio_exti(config);
    }
}

/**
 * @brief   Retrieves the external configurations of a given external interrupt
 *          channel.
 *
 * @param[in] channel   The channel to retrieve the configurations for
 * @param[out] config   The configurations of the channel
 */
void get_exti_configs(uint8_t channel, exti_config_t *config)
{
    uint32_t mask = 0;

    assert(channel < EXTI_CHANNELS);

    if (channel < PINS_PER_BANK)
    {
        get_exti_line_config(channel, &(config->exti_gpio));
    }
    else
    {
        config->exti_line = channel;
    }

    SET_BIT(mask, channel);

    config->unmask_int = (read_exti_int_mask() & mask) != 0;
    config->unmask_event = (read_exti_event_mask() & mask) != 0;
    config->rising_edge = (read_exti_rising_edge() & mask) != 0;
    config->falling_edge = (read_exti_falling_edge() & mask) != 0;
}

/**
 * @brief   Generates a software event for a given external interrupt line
 *
 * @param[in] channel   The channel to generate the event for
 */
void generate_exti_software_event(uint8_t channel)
{
    uint32_t mask = 0;
    assert(channel < EXTI_CHANNELS);
    SET_BIT(mask, channel);
    set_exti_software_int_event(channel);
}

/**
 * @brief   Checks if a software event was generated
 *
 * @param[in] channel   The channel to check for the event on
 *
 * @return   true if an event was generated
 *           false if an event was not generated
 */
bool exti_software_event_generated(uint8_t channel)
{
    uint32_t mask = 0;
    assert(channel < EXTI_CHANNELS);
    SET_BIT(mask, channel);
    return (read_exti_software_int_event() & mask) != 0;
}

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
bool check_exti_channel_pending(uint8_t channel)
{
    uint32_t mask = 0;
    assert(channel < EXTI_CHANNELS);
    SET_BIT(mask, channel);
    return (read_exti_pending() & mask) != 0;
}

/**
 * @brief   Acknowledges or clears the external event or interrupt. Failure
 *          to acknowledge an external event or interrupt will result in a
 *          deadlock.
 *
 * @param[in] channel   The channel to acknowledge the interrupt for
 */
void acknowledge_exti_event(uint8_t channel)
{
    uint32_t mask = 0;
    assert(channel < EXTI_CHANNELS);
    SET_BIT(mask, channel);
    clear_exti_pending(mask);
}

/**
 * @brief   Helper function for generating the appropriate mask for EXTI
 *
 * @param[in] channel   The channel to generate the mask for
 *
 * @return   The appropriate mask for the channel. If the channel is out of
 *           range, then 0 is returned
 */
static uint32_t generate_channel_mask(uint8_t channel)
{
    uint32_t mask = 0;
    if (channel < EXTI_CHANNELS)
    {
        SET_BIT(mask, channel);
    }
    return mask;
}

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
                                      uint8_t channel4, uint8_t channel5)
{
    uint32_t mask = 0;

    mask |= generate_channel_mask(channel0);
    mask |= generate_channel_mask(channel1);
    mask |= generate_channel_mask(channel2);
    mask |= generate_channel_mask(channel3);
    mask |= generate_channel_mask(channel4);
    mask |= generate_channel_mask(channel5);

    clear_exti_pending(mask);
}

/**
 * @brief   Clears all pending external interrupts
 */
void clear_pending_exti_events(void)
{
    clear_exti_pending(EXTI_VALID_MASK);
}
