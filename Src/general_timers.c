/**
 * @file general_timers.c
 * @version 1.0
 *
 * @brief   Hardware abstraction layer for the general purpose timers,
 *          otherwise timers 2, 3, 4, and 5. This file is compatible with the
 *          STM32F446C and STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#include "general_timers.h"
#include "stm_rcc.h"
#include "stm_utils.h"
#include <assert.h>
#include <stdint.h>

/* General Timers Base Address and Register Offsets */
#define TIMER_BASE_32BIT(bank, reg) *(((uint32_t *)0x40000000) + bank + reg)
#define TIMER32BIT_CR1              0x00
#define TIMER32BIT_CR2              0x01
#define TIMER32BIT_SMCR             0x02
#define TIMER32BIT_DIER             0x03
#define TIMER32BIT_SR               0x04
#define TIMER32BIT_EGR              0x05
#define TIMER32BIT_CCMR1            0x06
#define TIMER32BIT_CCMR2            0x07
#define TIMER32BIT_CCER             0x08
#define TIMER32BIT_CNT              0x09
#define TIMER32BIT_PSC              0x0A
#define TIMER32BIT_ARR              0x0B
#define TIMER32BIT_CCR1             0x0D
#define TIMER32BIT_CCR2             0x0E
#define TIMER32BIT_CCR3             0x0F
#define TIMER32BIT_CCR4             0x10
#define TIMER32BIT_DCR              0x12
#define TIMER32BIT_DMAR             0x13
#define TIMER32BIT_OR               0x14

/* General Timer Bitmasks */
#define GEN_TIMER_INTE_RESERVED BITF | BITD | BIT7 | BIT5
#define GEN_TIMER_SR_RESERVED   BITF | BITE | BITD | BIT8 | BIT7 | BIT5
/* clang-format off */
#define GEN_TIMER_EGR_RESERVED  BITF | BITE | BITD | BITC | BITB | BITA | \
                                BIT9 | BIT8 | BIT7 | BIT5
/* clang-format on */

/**
 * @brief   Sets the clock divider of the timer
 *
 * @param[in] timer   The timer being configured
 * @param[in] div     The clock division ratio
 */
static void controlReg1_clkdiv(general_timers_32bit_t timer,
                               general_timer_clkdiv_t div)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~(BIT9 | BIT8);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)div) << 8;
}

/**
 * @brief   Sets the auto reload preload enable of the timer
 *
 * @param[in] timer    The timer being configured
 * @param[in] enable   Enable the auto reload preload ability of the timer
 */
static void controlReg1_autoreload(general_timers_32bit_t timer, bool enable)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT7;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)enable)
                                                         << 7;
}

/**
 * @brief   Configures the center alignment mode of the timer
 *
 * @param[in] timer    The timer being configured
 * @param[in] mode     The center alignment mode
 */
static void controlReg1_centerAligned(general_timers_32bit_t timer,
                                      uint8_t mode)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~(BIT6 | BIT5);
    mode &= (BIT1 | BIT0);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)mode) << 5;
}

/**
 * @brief   Configures the direction of the counter for the given timer
 *
 * @param[in] timer    The timer being configured
 * @param[in] dir      The direction of the counter
 */
static void controlReg1_direction(general_timers_32bit_t timer, bool dir)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT4;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)dir) << 4;
}

/**
 * @brief   Configures the one pulse ability of the given timer
 *
 * @param[in] timer       The timer being configured
 * @param[in] one_pulse   Enable the one pulse ability of the timer
 */
static void controlReg1_one_pulse(general_timers_32bit_t timer, bool one_pulse)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT3;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)one_pulse)
                                                         << 3;
}

/**
 * @brief   Configures the request source of the given timer
 *
 * @param[in] timer                   The timer being configured
 * @param[in] update_request_source   The update request source
 */
static void controlReg1_update_request_source(general_timers_32bit_t timer,
                                              bool update_request_source)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT2;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |=
        ((uint32_t)update_request_source) << 2;
}

/**
 * @brief   Configure the event generation of the timer
 *
 * @param[in] timer     The timer being configured
 * @param[in] disable   Disable the event generation
 */
static void controlReg1_update_disable(general_timers_32bit_t timer,
                                       bool disable)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT1;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)disable)
                                                         << 1;
}

/**
 * @brief   Start a given timer
 *
 * @param[in] timer   The timer to start
 */
void enableTimer(general_timers_32bit_t timer)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= BIT0;
}

/**
 * @brief   Stop a given timer
 *
 * @param[in] timer   The timer to stop
 */
void disableTimer(general_timers_32bit_t timer)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT0;
}

/**
 * @brief   Selects the TI1 input
 *
 * @param[in] timer   The timer being configured
 * @param[in] xor     XOR the channel 1, 2, and 3 pins
 */
static void controlReg2_selection(general_timers_32bit_t timer, bool xor)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) &= ~BIT7;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) |= ((uint32_t) xor) << 7;
}

/**
 * @brief   Selects the master mode
 * @param[in] timer   The timer being configured
 * @param[in] mode    The master mode selected
 */
static void controlReg2_master_mode_selection(general_timers_32bit_t timer,
                                              uint8_t mode)
{
    mode &= (BIT2 | BIT1 | BIT0);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) &= ~(BIT6 | BIT5 | BIT4);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) |= ((uint32_t)mode) << 4;
}

/**
 * @brief   Determines if DMA requests are sent when a CCx event occurs or
 *          when an update event occurs
 *
 * @param[in] timer    The timer being configured
 * @param[in] update   Update when an update event occurs
 */
static void controlReg2_capture_compare_dma_select(general_timers_32bit_t timer,
                                                   bool update)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) &= ~BIT3;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) |= ((uint32_t)update)
                                                         << 3;
}

/**
 * @brief   Configures the slave mode register for the given timer channel
 *
 * @param[in] timer       The timer being configured
 * @param[in] slaveAttr   The slave attributes for the timer
 */
static void configureSlave(general_timers_32bit_t timer,
                           timer_slave_attr_t slaveAttr)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SMCR) = 0;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SMCR) |=
        ((uint32_t)slaveAttr.ETP) << 15;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SMCR) |=
        ((uint32_t)slaveAttr.ECE) << 14;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SMCR) |=
        (((uint32_t)slaveAttr.ETPS) & (BIT1 | BIT0)) << 12;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SMCR) |=
        (((uint32_t)slaveAttr.ETF) & (BIT3 | BIT2 | BIT1 | BIT0)) << 8;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SMCR) |=
        ((uint32_t)slaveAttr.MSM) << 7;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SMCR) |=
        (((uint32_t)slaveAttr.TS) & (BIT2 | BIT1 | BIT0)) << 4;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SMCR) |=
        (((uint32_t)slaveAttr.SMS) & (BIT2 | BIT1 | BIT0));
}

/**
 * @brief   Enables interrupts based off of the bit field provided
 *
 * @param[in] timer   The timer being configured
 * @param[in] mask    The bitfield of interrupts to be enabled
 */
static void enableTimerInterrupts(general_timers_32bit_t timer, uint16_t mask)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_DIER) &=
        GEN_TIMER_INTE_RESERVED;
    mask &= (uint16_t)(~GEN_TIMER_INTE_RESERVED);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_DIER) |= (uint32_t)mask;
}

/**
 * @brief   Checks the timer status given the interrupt status mask.
 *          Additionally, this clears the appropriate pending interrupts.
 *
 * @param[in] timer   The timer being checked
 * @param[in] mask    The interrupt status mask to check and clear
 *
 * @return   true if the status was asserted and cleared
 * @return   false if the status was not asserted
 */
bool checkTimerStatus(general_timers_32bit_t timer, uint16_t mask)
{
    uint32_t status;
    mask &= (uint16_t)(~GEN_TIMER_SR_RESERVED);
    status = TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SR) & mask;
    mask = ~mask;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SR) = mask;
    return status != 0;
}

/**
 * @brief   Generates timer software events given the events mask
 *
 * @param[in] timer   The timer the events are being generated for
 * @param[in] mask    The event generation mask
 */
void updateEventGeneration(general_timers_32bit_t timer, uint16_t mask)
{
    mask &= (uint16_t)(~GEN_TIMER_EGR_RESERVED);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_EGR) |= mask;
}

/**
 * @brief   Configures the capture/compare register as a compare output for
 *          a given channel
 *
 * @param[in] timer         The timer being configured
 * @param[in] compareAttr   The output compare attributes
 * @param[in] channel       The capture/compare channel being configured
 */
static void configureCaptureCompareModeAsOutput(general_timers_32bit_t timer,
                                                outputCompareMode_t compareAttr,
                                                uint8_t channel)
{
    uint32_t reg = (channel < 3) ? TIMER32BIT_CCMR1 : TIMER32BIT_CCMR2;
    uint32_t shift = (channel & BIT0) ? 0 : 8;
    TIMER_BASE_32BIT((uint32_t)timer, reg) &= ~(0xFFUL << shift);
    TIMER_BASE_32BIT((uint32_t)timer, reg) |=
        ((uint32_t)compareAttr.outputCaptureClearEnable) << (7 + shift);
    TIMER_BASE_32BIT((uint32_t)timer, reg) |=
        (((uint32_t)compareAttr.outputCompareMode) & (BIT2 | BIT1 | BIT0))
        << (4 + shift);
    TIMER_BASE_32BIT((uint32_t)timer, reg) |=
        ((uint32_t)compareAttr.outputComparePreloadEnable) << (3 + shift);
    TIMER_BASE_32BIT((uint32_t)timer, reg) |=
        ((uint32_t)compareAttr.outputCompareFastEnable) << (2 + shift);
    TIMER_BASE_32BIT((uint32_t)timer, reg) |=
        (((uint32_t)compareAttr.captureCompareSelection) & (BIT1 | BIT0))
        << shift;
}

/**
 * @brief   Configures the capture/compare register as a capture input for
 *          a given channel
 *
 * @param[in] timer         The timer being configured
 * @param[in] captureAttr   The input capture attributes
 * @param[in] channel       The capture/compare channel being configured
 */
static void configureCaptureCompareModeAsInput(general_timers_32bit_t timer,
                                               inputCaptureMode_t captureAttr,
                                               uint8_t channel)
{
    uint32_t reg = (channel < 3) ? TIMER32BIT_CCMR1 : TIMER32BIT_CCMR2;
    uint32_t shift = (channel & BIT0) ? 0 : 8;
    TIMER_BASE_32BIT((uint32_t)timer, reg) &= ~(0xFFUL << shift);
    TIMER_BASE_32BIT((uint32_t)timer, reg) |=
        (((uint32_t)captureAttr.inputCaptureFilter) &
         (BIT3 | BIT2 | BIT1 | BIT0))
        << (4 + shift);
    TIMER_BASE_32BIT((uint32_t)timer, reg) |=
        (((uint32_t)captureAttr.inputCapturePrescaler) & (BIT1 | BIT0))
        << (2 + shift);
    TIMER_BASE_32BIT((uint32_t)timer, reg) |=
        (((uint32_t)captureAttr.captureCompareSelection) & (BIT1 | BIT0))
        << shift;
}

/**
 * @brief   Enables a given capture/compare channel
 *
 * @param[in] timer     The timer the channel belongs to
 * @param[in] channel   The channel being enabled
 */
static void enableCaptureCompare(general_timers_32bit_t timer, uint8_t channel)
{
    // Look at bits in CCMR
    uint32_t mask = BIT0 << ((channel - 1) << 2);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) |= mask;
}

/**
 * @brief   Disables a given capture/compare channel
 *
 * @param[in] timer     The timer the channel belongs to
 * @param[in] channel   The channel being enabled
 */
static void disableCaptureCompare(general_timers_32bit_t timer, uint8_t channel)
{
    // Look at bits in CCMR
    uint32_t mask = BIT0 << ((channel - 1) << 2);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) &= ~mask;
}

/**
 * @brief   Configures the capture polarity of a given capture/compare channel
 *
 * @param[in] timer      The timer being configured
 * @param[in] channel    The channel being configured
 * @param[in] polarity   The capture polarity
 */
static void configureCapturePolarity(general_timers_32bit_t timer,
                                     uint8_t channel, uint8_t polarity)
{
    // Look at bits in CCMR
    uint32_t mask = 0;

    switch (polarity)
    {
    case CAPTURE_NONINVERTING_RISING:
        break;
    case CAPTURE_INVERTING_FALLING:
        mask = BIT1;
        break;
    case CAPTURE_INVERTING_BOTH_EDGES:
        mask = BIT1 | BIT3;
        break;
    default:
        assert(0);
    }

    mask <<= (channel - 1) << 2;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) &=
        ~((BIT1 | BIT3) << (channel - 1) << 2);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) |= mask;
}

/**
 * @brief   Configures the compare polarity of a given capture/compare channel
 *
 * @param[in] timer      The timer being configured
 * @param[in] channel    The channel being configured
 * @param[in] polarity   The compare polarity
 */
static void configureComparePolarity(general_timers_32bit_t timer,
                                     uint8_t channel, uint8_t polarity)
{
    // Look at bits in CCMR
    uint32_t mask = (polarity & 0x1 << 1) << ((channel - 1) << 2);

    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) &=
        ~((BIT1 | BIT3) << (channel - 1) << 2);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) |= mask;
}

/**
 * @brief   Sets the counter value of the given timer
 *
 * @param[in] timer   The timer that controls the counter
 * @param[in] value   The new value of the count register
 */
void setCounterValue(general_timers_32bit_t timer, uint16_t value)
{
    disableTimer(timer);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CNT) = value;
    enableTimer(timer);
}

/**
 * @brief   Retrieves the current counter value of the timer
 *
 * @param[in] timer   The timer that controls the counter being read
 *
 * @return   The current counter value of the timer
 */
uint16_t getCounterValue(general_timers_32bit_t timer)
{
    return (uint16_t)TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CNT);
}

/**
 * @brief   A helper to set the timer prescalar value
 *
 * @param[in] timer   The timer being configured
 * @param[in] value   The prescale value of the timer
 */
static void setPrescalar(general_timers_32bit_t timer, uint16_t value)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_PSC) = (uint32_t)value;
}

/**
 * @brief   A helper to set the timer auto-reload value. In up counting mode,
 *          the timer is reset when it reaches the specified value. In down
 *          counting mode, the specified value is loaded into the counter after
 *          the counter reaches 0. If this is zero, then the timer is
 *          essentially blocked
 *
 * @param[in] timer   The timer being configured
 * @param[in] value   The auto-reload value
 */
static void setAutoReload(general_timers_32bit_t timer, uint16_t value)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_ARR) = (uint32_t)value;
}

/**
 * @brief   A helper to set the compare value for a given timer channel
 *
 * @param[in] timer     The timer being configured
 * @param[in] channel   The channel the compare value is being set for
 * @param[in] value     The value of the compare value
 */
static void setCompare(general_timers_32bit_t timer, uint8_t channel,
                       uint32_t value)
{
    uint32_t reg = TIMER32BIT_CCR1 + (uint32_t)channel - 1;

    if (!(timer == TIMER2 || timer == TIMER5))
    {
        value &= 0xFFFF;
    }

    TIMER_BASE_32BIT((uint32_t)timer, reg) = value;
}

/**
 * @brief   A helper for reading the capture value for the given timer channel
 *
 * @param[in] timer     The timer that runs the counter being captured
 * @param[in] channel   The capture channel being read from
 * @return
 */
static uint32_t readCapture(general_timers_32bit_t timer, uint8_t channel)
{
    uint32_t reg = TIMER32BIT_CCR1 + (uint32_t)channel - 1;
    return TIMER_BASE_32BIT((uint32_t)timer, reg);
}

/**
 * @brief   A helper function for configuring the DMA control register
 *
 * @param[in] timer   The timer being configured
 * @param[in] attr    The general timer configuration attributes
 */
static void configureDMAcontrol(general_timers_32bit_t timer,
                                general_timer_attr_t attr)
{
    uint32_t dbl, dba;
    dbl = (uint32_t)attr.dmaBurstLength;
    dbl &= 0x1F;
    assert(dbl < 18);

    dba = (uint32_t)attr.dmaBaseAddr;
    dba &= 0x1F;

    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_DCR) &=
        ~(BITC | BITB | BITA | BIT9 | BIT8 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_DCR) |= dba;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_DCR) |= dbl << 8;
}

/**
 * @brief   Helper for configuring the DMA address for full transfer for a
 *          given timer
 *
 * @param[in] timer   The timer being configured
 * @param[in] addr    The DMA register for burst access
 */
static void configureDMAfullTransferAddress(general_timers_32bit_t timer,
                                            uint16_t addr)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_DMAR) = (uint32_t)addr;
}

/**
 * @brief   Helper for configuring the timer option register. This function
 *          has no affect for timers that do not support this register
 *
 * @param[in] timer         The timer being configured
 * @param[in] timerOption   The timer option
 */
static void configureTimerOption(general_timers_32bit_t timer,
                                 uint8_t timerOption)
{
    uint32_t config = timerOption & (BIT1 | BIT0);

#if defined(__GNUC__)
#pragma GCC diagnostic push
// This error is ignored because not all the cases need to be handled here...
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif
    switch (timer)
    {
    case TIMER2:
        config <<= 10;
        break;
    case TIMER5:
        config <<= 6;
        break;
    default:
        return;
    }
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_OR) = config;
}

/**
 * @brief   Helper function for configuring the capture compare registers
 *
 * @param[in] timer   The timer being configured
 * @param[in] attr    The timer configuration attributes
 */
static void configureCaptureCompare(general_timers_32bit_t timer,
                                    general_timer_attr_t attr)
{
    // Configure channel 1
    if (attr.ccMode1 == CAPTURE_MODE)
    {
        configureCaptureCompareModeAsInput(timer, attr.capture1, 1);
        configureCapturePolarity(timer, 1, attr.captureCompareOutputPolarity1);
    }
    else
    {
        configureCaptureCompareModeAsOutput(timer, attr.compare1, 1);
        configureComparePolarity(timer, 1, attr.captureCompareOutputPolarity1);
        setCompare(timer, 1, attr.compare1.compareValue);
    }

    // Configure channel 2
    if (attr.ccMode2 == CAPTURE_MODE)
    {
        configureCaptureCompareModeAsInput(timer, attr.capture2, 2);
        configureCapturePolarity(timer, 2, attr.captureCompareOutputPolarity2);
    }
    else
    {
        configureCaptureCompareModeAsOutput(timer, attr.compare2, 2);
        configureComparePolarity(timer, 2, attr.captureCompareOutputPolarity2);
        setCompare(timer, 2, attr.compare2.compareValue);
    }

    // Configure channel 3
    if (attr.ccMode3 == CAPTURE_MODE)
    {
        configureCaptureCompareModeAsInput(timer, attr.capture3, 3);
        configureCapturePolarity(timer, 3, attr.captureCompareOutputPolarity3);
    }
    else
    {
        configureCaptureCompareModeAsOutput(timer, attr.compare3, 3);
        configureComparePolarity(timer, 3, attr.captureCompareOutputPolarity3);
        setCompare(timer, 3, attr.compare3.compareValue);
    }

    // Configure channel 4
    if (attr.ccMode4 == CAPTURE_MODE)
    {
        configureCaptureCompareModeAsInput(timer, attr.capture4, 4);
        configureCapturePolarity(timer, 4, attr.captureCompareOutputPolarity4);
    }
    else
    {
        configureCaptureCompareModeAsOutput(timer, attr.compare4, 4);
        configureComparePolarity(timer, 4, attr.captureCompareOutputPolarity4);
        setCompare(timer, 4, attr.compare4.compareValue);
    }
}

/**
 * @brief   Helper function for enabling the peripheral clock for the timer
 *
 * @param[in] timer   The timer to enable the peripheral clock for
 */
static void enableClock(general_timers_32bit_t timer)
{
    switch (timer)
    {
    case TIMER2:
        enable_peripheral_clock(TIM2_EN);
        break;
    case TIMER3:
        enable_peripheral_clock(TIM3_EN);
        break;
    case TIMER4:
        enable_peripheral_clock(TIM4_EN);
        break;
    case TIMER5:
        enable_peripheral_clock(TIM5_EN);
        break;
    default:
        assert(0);
    }
}

/**
 * @brief   Configures the given timer given the requested attributes of the
 *          timer
 *
 * @param[in] timer   The timer being configured
 * @param[in] attr    The configuration attributes of the timer
 */
void configureGeneralTimer(general_timers_32bit_t timer,
                           general_timer_attr_t attr)
{
    // Enable peripheral clock
    enableClock(timer);

    disableTimer(timer);

    // Clear the event flag
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SR) = 0;

    // Control reg 1
    controlReg1_clkdiv(timer, attr.clkDiv);
    controlReg1_autoreload(timer, attr.autoReload);
    controlReg1_centerAligned(timer, attr.centerAligned);
    controlReg1_direction(timer, attr.direction);
    controlReg1_one_pulse(timer, attr.onePulse);
    controlReg1_update_request_source(timer, attr.updateRequestSource);
    controlReg1_update_disable(timer, attr.updateDisable);

    // Control reg 2
    controlReg2_selection(timer, attr.timerInput1);
    controlReg2_master_mode_selection(timer, attr.masterMode);
    controlReg2_capture_compare_dma_select(timer, attr.captureDMA);

    // Slave/Master Reg
    configureSlave(timer, attr.slaveAttr);

    // IER
    enableTimerInterrupts(timer, attr.interruptEnableMask);

    // Configure DMA
    configureDMAcontrol(timer, attr);
    configureDMAfullTransferAddress(timer, attr.dmaBurstAddr);
    configureTimerOption(timer, attr.timerOption);

    // Configure Prescalar
    setPrescalar(timer, attr.prescaler);

    // Configure autoreload
    setAutoReload(timer, attr.auto_reload_value);

    // Capture/compare
    configureCaptureCompare(timer, attr);

    // Configure initial count
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CNT) =
        (uint32_t)attr.initialCount;

    if (attr.enableAfterConfig)
    {
        enableTimer(timer);
    }
}

/**
 * @brief   Enables a given capture/compare channel in the timer
 *
 * @param[in] timer     The timer that controls the capture/compare channel
 * @param[in] channel   The channel to enable
 */
void enableCaptureCompareChannel(general_timers_32bit_t timer, uint8_t channel)
{
    if (channel < 1 || channel > 4)
    {
        return;
    }
    enableCaptureCompare(timer, channel);
}

/**
 * @brief   Disables a given capture/compare channel in the timer
 *
 * @param[in] timer     The timer that controls the capture/compare channel
 * @param[in] channel   The channel to disable
 */
void disableCaptureCompareChannel(general_timers_32bit_t timer, uint8_t channel)
{
    if (channel < 1 || channel > 4)
    {
        return;
    }
    disableCaptureCompare(timer, channel);
}

/**
 * @brief   Reads the capture value from a given timer channel
 *
 * @param[in] timer     The timer that controls the capture/compare channel
 * @param[in] channel   The channel to read the capture value from
 *
 * @return   The capture value if the channel parameter is valid
 * @return   0 on an invalid value of the channel parameter
 */
uint32_t readCaptureValue(general_timers_32bit_t timer, uint8_t channel)
{
    if (channel < 1 || channel > 4)
    {
        return 0;
    }

    return readCapture(timer, channel);
}

/**
 * @brief   Clears all the status flags from the status register
 *
 * @param[in] timer   The timer to clear the status flags for
 */
void clearTimerStatusRegister(general_timers_32bit_t timer)
{
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SR) = 0;
}
