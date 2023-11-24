/**
 * @file general_timers.h
 * @version 1.0
 *
 * @brief   Hardware abstraction layer for the general purpose timers,
 *          otherwise timers 2, 3, 4, and 5. This file is compatible with the
 *          STM32F446C and STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#ifndef GENERAL_TIMERS_H_
#define GENERAL_TIMERS_H_

#include "stm_utils.h"
#include <stdbool.h>
#include <stdint.h>

/*
 * ---------------------------------------------------------------------------
 *                               BEGIN MACROS
 * ---------------------------------------------------------------------------
 */

/**
 * Macros for the center aligned parameter in the general_timer_attr_t struct
 */
#define EDGE_ALIGNED          0x0
#define CENTER_ALIGNED_MODE_1 0x1
#define CENTER_ALIGNED_MODE_2 0x2
#define CENTER_ALIGNED_MODE_3 0x3

/**
 * Macros for determining the direction of the counter
 */
#define UP_COUNTER   false
#define DOWN_COUNTER true

/**
 * Macros for the updateRequestSource parameter in the general_timer_attr_t
 * struct
 */
#define ANY_UPDATE_SOURCE     false
#define OVERFLOW_OR_UNDERFLOW true

/**
 * Macros for the timerInput parameter in the general_timer_attr_t struct
 */
#define CH1_ONLY        false
#define XOR_CH1_CH2_CH3 true

/**
 * Macros for the masterMode parameter in the general_timer_attr_t struct
 */
#define TIM_MM_RESET          0x0
#define TIM_MM_ENABLE         0x1
#define TIM_MM_UPDATE         0x2
#define TIM_MM_COMPARE        0x3
#define TIM_MM_0C1REF_COMPARE 0x4
#define TIM_MM_0C2REF_COMPARE 0x5
#define TIM_MM_0C3REF_COMPARE 0x6
#define TIM_MM_0C4REF_COMPARE 0x7

/**
 * Macros for the captureDMA parameter in the general_timer_attr_t struct
 */
#define CCx_EVENT    false
#define UPDATE_EVENT true

/**
 * Macros for the timer interrupt enable register. These macros can be bitwise
 * ORed together to create a readable bit mask for the interruptEnableMask
 * in the general_timer_attr_t struct
 */
#define TDE   BITE ///< Trigger DMA request
#define CC4DE BITC ///< CC4 DMA request
#define CC3DE BITB ///< CC3 DMA request
#define CC2DE BITA ///< CC2 DMA request
#define CC1DE BIT9 ///< CC1 DMA request
#define UDE   BIT8 ///< Update DMA request
#define TIE   BIT6 ///< Trigger interrupt
#define CC4IE BIT4 ///< CC4 interrupt
#define CC3IE BIT3 ///< CC3 interrupt
#define CC2IE BIT2 ///< CC2 interrupt
#define CC1IE BIT1 ///< CC1 interrupt
#define UIE   BIT0 ///< Update interrupt

/**
 * Macros for checking the interrupt status of a timer. These macros can be
 * bitwise ORed together to create a readable bit mask so multiple timer
 * statuses can be read at the same time
 */
#define CC4OF BITC ///< Capture/Compare 4 overcapture flag
#define CC3OF BITB ///< Capture/Compare 3 overcapture flag
#define CC2OF BITA ///< Capture/Compare 2 overcapture flag
#define CC1OF BIT9 ///< Capture/Compare 1 overcapture flag
#define TIF   BIT6 ///< Trigger interrupt flag
#define CC4IF BIT4 ///< Capture/Compare 4 interrupt flag
#define CC3IF BIT3 ///< Capture/Compare 3 interrupt flag
#define CC2IF BIT2 ///< Capture/Compare 2 interrupt flag
#define CC1IF BIT1 ///< Capture/Compare 1 interrupt flag
#define UIF   BIT0 ///< Update interrupt flag

/**
 * Macros for generating software events from the timer. These macros can be
 * bitwise ORed together to create a readable bit mask
 */
#define TG   BIT6 ///< Trigger generation
#define CC4G BIT4 ///< Capture/compare 4 generation
#define CC3G BIT3 ///< Capture/compare 3 generation
#define CC2G BIT2 ///< Capture/compare 2 generation
#define CC1G BIT1 ///< Capture/compare 1 generation
#define UG   BIT0 ///< Update generation

/**
 * Macros for determining the polarity of the compare/capture register. These
 * macros should be used for the captureCompareOutputPolarityX parameters in
 * the general_timer_attr_t struct
 */
#define COMPARE_ACTIVE_HIGH          0
#define COMPARE_ACTIVE_LOW           1
#define CAPTURE_NONINVERTING_RISING  0
#define CAPTURE_INVERTING_FALLING    1
#define CAPTURE_INVERTING_BOTH_EDGES 2

/**
 * Macros for determining if a channel should be operating in capture or
 * compare mode
 */
#define CAPTURE_MODE true
#define COMPARE_MODE false

/**
 * Macros for the compare mode of a capture/compare channel. These macros
 * should be used for the outputCompareMode parameter in the
 * outputCompareMode_t struct
 */
/* clang-format off */
#define COMPARE_FROZEN          0 ///< The comparison between the output
                                  ///< compare register TIMx_CCR1 and the
                                  ///< counter TIMx_CNT has no effect on the
                                  ///< outputs. (this mode is used to
                                  ///< generate a timing base).
#define COMPARE_ACTIVE_MATCH    1 ///< Set channel 1 to active level on
                                  ///< match. OC1REF signal is forced high
                                  ///< when the counter TIMx_CNT matches the
                                  ///< capture/compare register 1
                                  ///< (TIMx_CCR1).
#define COMPARE_ACTIVE_MISMATCH 2 ///<  Set channel 1 to inactive level on
                                  ///< match. OC1REF signal is forced low
                                  ///< when the counter TIMx_CNT matches the
                                  ///< capture/compare register 1
                                  ///< (TIMx_CCR1).
#define COMPARE_TOGGLE          3 ///< OC1REF toggles when
                                  ///< TIMx_CNT=TIMx_CCR1.
#define COMPARE_FORCE_INACTIVE  4 ///< OC1REF is forced low
#define COMPARE_FORCE_ACTIVE    5 ///< OC1REF is forced high
#define COMPARE_PWM_MODE_1      6 ///< In upcounting, channel 1 is active as
                                  ///< long as TIMx_CNT<TIMx_CCR1 else
                                  ///< inactive. In downcounting, channel 1
                                  ///< is inactive (OC1REF=‘0) as long as
                                  ///< TIMx_CNT>TIMx_CCR1 else active
                                  ///< (OC1REF=1)
#define COMPARE_PWM_MODE_2      7 ///<  In upcounting, channel 1 is inactive
                                  ///< as long as TIMx_CNT<TIMx_CCR1 else
                                  ///< active. In downcounting, channel 1 is
                                  ///< active as long as TIMx_CNT>TIMx_CCR1
                                  ///< else inactive
/* clang-format on */

/*
 * ---------------------------------------------------------------------------
 *                                END MACROS
 * ---------------------------------------------------------------------------
 */

/**
 * @brief   Available general timers.
 */
typedef enum
{
    TIMER2 = 0x000,
    TIMER3 = 0x100,
    TIMER4 = 0x200,
    TIMER5 = 0x300,
} general_timers_32bit_t;

/**
 * @brief   The amount to divide the clock by. The base frequency of these
 *          clocks is 90 MHz
 */
typedef enum
{
    CLKDIV_1 = 0x0,
    CLKDIV_2 = 0x1,
    CLKDIV_4 = 0x2
} general_timer_clkdiv_t;

/**
 * @brief   Configurations for output compare mode
 */
typedef struct
{
    bool outputCaptureClearEnable;   ///< Clear OCrRef as soon as a High Level
                                     ///< is detected on ETRF input
    uint8_t outputCompareMode;       ///< Output compare mode. See macros
                                     ///< above
    bool outputComparePreloadEnable; ///< Read/Write operations access the
                                     ///< preload register. TIMx_CCR1 preload
                                     ///< value is loaded in the active
                                     ///< register at each update event.
    bool outputCompareFastEnable;    ///< An active edge on the trigger input
                                     ///< acts like a compare match on CC1
                                     ///< output. Then, OC is set to the
                                     ///< compare level independently from the
                                     ///< result of the comparison. Delay to
                                     ///< sample the trigger input and to
                                     ///< activate CC1 output is reduced to 3
                                     ///< clock cycles. OCFE acts only if the
                                     ///< channel is configured in PWM1 or
                                     ///< PWM2 mode.
    uint8_t captureCompareSelection; ///< TODO: Get rid of this
    uint32_t compareValue;           ///< The initial value being compared to
} outputCompareMode_t;

/**
 * @brief   Configurations for input capture mode
 */
typedef struct
{
    uint8_t inputCaptureFilter;      ///< This bit-field defines the frequency
                                     ///< used to sample TI1 input and the
                                     ///< length of the digital filter applied
                                     ///< to TI1. The digital filter is made
                                     ///< of an event counter in which N
                                     ///< consecutive events are needed to
                                     ///< validate a transition on the output
    uint8_t inputCapturePrescaler;   ///< This bit-field defines the ratio of
                                     ///< the prescaler acting on CC1 input
                                     ///< (IC1). The prescaler is reset as
                                     ///< soon as CC1E=0 (TIMx_CCER register).
    uint8_t captureCompareSelection; ///< TODO: Create enum or macros for this
} inputCaptureMode_t;

/**
 * @brief   Parameters for the slave mode control register
 */
typedef struct
{
    bool ETP;     ///< External trigger polarity
    bool ECE;     ///< External clock enable
    uint8_t ETPS; ///< External trigger prescaler
    uint8_t ETF;  ///<  External trigger filter
                  ///< (TODO: Create enum or macros for this)
    bool MSM;     ///< : Master/Slave mode
    uint8_t TS;   ///< Trigger selection TODO: See above
    uint8_t SMS;  ///< Slave mode selection TODO: See above
} timer_slave_attr_t;

/**
 * @brief   Configuration data for the general timers
 *
 * Parameter Register Mappings:
 * clkDiv - Control Register 1
 * timerInput1 - Control Register 2
 * slaveAttr - Slave Mode Control Register
 * interruptEnableMask - DMA/Interrupt enable register
 * captureCompareOutputPolarity4 - capture/compare enable register
 */
typedef struct
{
    general_timer_clkdiv_t clkDiv; ///< This bit-field indicates the division
                                   ///< ratio between the timer clock (CK_INT)
                                   ///< frequency and sampling clock used by
                                   ///< the digital filters (ETR, TIx),
    bool autoReload;               ///< Auto-reload preload enable
    uint8_t centerAligned;         ///< Center-aligned mode selection
    bool direction;                ///< Direction of the counter
    bool onePulse;                 ///< Counter stops counting at the next
                                   ///< update event (clearing the bit CEN)
    bool updateRequestSource;      ///< 0: Any of the following events
                                   ///< generate an update interrupt or DMA
                                   ///< request if enabled. These events can
                                   ///< be:
                                   ///<    – Counter overflow/underflow
                                   ///<    – Setting the UG bit
                                   ///<    – Update generation through the
                                   ///<      slave mode controller
                                   ///< 1: Only counter overflow/underflow
                                   ///< generates an update interrupt or DMA
                                   ///< request if enabled.
    bool updateDisable;            ///< 0: UEV enabled. The Update (UEV)
                                   ///< event is generated by one of the
                                   ///< following events:
                                   ///<    – Counter overflow/underflow
                                   ///<    – Setting the UG bit
                                   ///<    – Update generation through the
                                   ///<      slave mode controller
                                   ///< Buffered registers are then loaded with
                                   ///< their preload values.
                                   ///<  1: UEV disabled. The Update event is
                                   ///< not generated, shadow registers keep
                                   ///< their value (ARR, PSC, CCRx). However
                                   ///< the counter and the prescaler are
                                   ///< reinitialized if the UG bit is set or
                                   ///< if a hardware reset is received from
                                   ///< the slave mode controller
    bool timerInput1;              ///< TI1 selection
    uint8_t masterMode;            ///< Master mode selection
    bool captureDMA;               ///< Capture/compare DMA selection
    timer_slave_attr_t slaveAttr;  ///< Configurations for slave mode control
    uint16_t interruptEnableMask;  ///< Enables DMA/interrupts generation
    union
    {
        outputCompareMode_t compare4; ///< Output compare attributes of
                                      ///< channel 4
        inputCaptureMode_t capture4;  ///< Input capture attributes of
                                      ///< channel 4
    };

    union
    {
        outputCompareMode_t compare3; ///< Output compare attributes of
                                      ///< channel 3
        inputCaptureMode_t capture3;  ///< Input capture attributes of
                                      ///< channel 3
    };

    union
    {
        outputCompareMode_t compare2; ///< Output compare attributes of
                                      ///< channel 2
        inputCaptureMode_t capture2;  ///< Input capture attributes of
                                      ///< channel 2
    };

    union
    {
        outputCompareMode_t compare1; ///< Output compare attributes of
                                      ///< channel 1
        inputCaptureMode_t capture1;  ///< Input capture attributes of
                                      ///< channel 1
    };
    bool ccMode1;                          ///< The capture/compare mode
                                           ///< selection for channel 1
    bool ccMode2;                          ///< The capture/compare mode
                                           ///< selection for channel 2
    bool ccMode3;                          ///< The capture/compare mode
                                           ///< selection for channel 3
    bool ccMode4;                          ///< The capture/compare mode
                                           ///< selection for channel 4
    uint8_t captureCompareOutputPolarity4; ///< Capture/Compare 4 output
                                           ///< Polarity. Use the above macros
                                           ///< to determine the polarity
    uint8_t captureCompareOutputPolarity3; ///< Capture/Compare 3 output
                                           ///< Polarity. Use the above macros
                                           ///< to determine the polarity
    uint8_t captureCompareOutputPolarity2; ///< Capture/Compare 2 output
                                           ///< Polarity. Use the above macros
                                           ///< to determine the polarity
    uint8_t captureCompareOutputPolarity1; ///< Capture/Compare 1 output
                                           ///< Polarity. Use the above macros
                                           ///< to determine the polarity
    uint16_t prescaler;                    ///< The counter clock frequency
                                           ///< CK_CNT is equal to
                                           ///< fCK_PSC / (PSC[15:0] + 1).
                                           ///< PSC contains the value to be
                                           ///< loaded in the active prescaler
                                           ///< register at each update event
    uint16_t auto_reload_value;            ///< ARR is the value to be loaded
                                           ///< in the actual auto-reload
                                           ///< register. The counter is
                                           ///< blocked while the auto-reload
                                           ///< value is null.
    uint8_t dmaBurstLength;                ///< This 5-bit vector defines the
                                           ///< number of DMA transfers (the
                                           ///< timer recognizes a burst
                                           ///< transfer when a read or a
                                           ///< write access is done to the
                                           ///< TIMx_DMAR address).
    uint8_t dmaBaseAddr;                   ///< This 5-bit vector defines the
                                           ///< base-address for DMA transfers
                                           ///< (when read/write access are
                                           ///< done through the TIMx_DMAR
                                           ///< address). DBA is defined as an
                                           ///< offset starting from the
                                           ///< address of the TIMx_CR1
                                           ///< register.
    uint16_t dmaBurstAddr;                 ///< A read or write operation to
                                           ///< the DMAR register accesses the
                                           ///< register located at the
                                           ///< address (TIMx_CR1 address) +
                                           ///< (DBA + DMA index) x 4 where
                                           ///< TIMx_CR1 address is the
                                           ///< address of the control
                                           ///< register 1, DBA is the DMA
                                           ///< base address configured in
                                           ///< TIMx_DCR register, DMA index
                                           ///< is automatically controlled by
                                           ///< the DMA transfer, and ranges
                                           ///< from 0 to DBL (DBL configured
                                           ///< in TIMx_DCR).
    uint8_t timerOption;                   ///< Timer 2:
                                           ///< Internal trigger 1 remap
                                           ///< Set and cleared by software.
                                           ///<    00: TIM8_TRGOUT
                                           ///<    01: Reserved
                                           ///<    10: OTG FS SOF is connected
                                           ///<        to the TIM2_ITR1 input
                                           ///<    11: OTG HS SOF is connected
                                           ///<        to the TIM2_ITR1 input
                                           ///< Timer 5:
                                           ///< Timer Input 4 remap
                                           ///<    00: TIM5 Channel4 is
                                           ///<        connected to the GPIO:
                                           ///<        Refer to the Alternate
                                           ///<        function mapping table
                                           ///<        in the datasheets.
                                           ///<    01: the LSI internal clock
                                           ///<        is connected to the
                                           ///<        TIM5_CH4 input for
                                           ///<        calibration purposes
                                           ///<    10: the LSE internal clock
                                           ///<        is connected to the
                                           ///<        TIM5_CH4 input for
                                           ///<        calibration purposes
                                           ///<    11: the RTC wakeup
                                           ///<        interrupt is connected
                                           ///<        to TIM5_CH4 input for
                                           ///<        calibration purposes.
                                           ///<        Wakeup interrupt
                                           ///<        should be enabled.
    uint16_t initialCount;                 ///< Initial counter value
    bool enableAfterConfig;                ///< Enable the timer after done
                                           ///< configuring
} general_timer_attr_t;

/**
 * @brief   Configures the given timer given the requested attributes of the
 *          timer
 *
 * @param[in] timer   The timer being configured
 * @param[in] attr    The configuration attributes of the timer
 */
void configureGeneralTimer(general_timers_32bit_t timer,
                           general_timer_attr_t attr);

/**
 * @brief   Start a given timer
 *
 * @param[in] timer   The timer to start
 */
void enableTimer(general_timers_32bit_t timer);

/**
 * @brief   Stop a given timer
 *
 * @param[in] timer   The timer to stop
 */
void disableTimer(general_timers_32bit_t timer);

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
bool checkTimerStatus(general_timers_32bit_t timer, uint16_t mask);

/**
 * @brief   Generates timer software events given the events mask
 *
 * @param[in] timer   The timer the events are being generated for
 * @param[in] mask    The event generation mask
 */
void updateEventGeneration(general_timers_32bit_t timer, uint16_t mask);

/**
 * @brief   Enables a given capture/compare channel in the timer
 *
 * @param[in] timer     The timer that controls the capture/compare channel
 * @param[in] channel   The channel to enable
 */
void enableCaptureCompareChannel(general_timers_32bit_t timer, uint8_t channel);

/**
 * @brief   Disables a given capture/compare channel in the timer
 *
 * @param[in] timer     The timer that controls the capture/compare channel
 * @param[in] channel   The channel to disable
 */
void disableCaptureCompareChannel(general_timers_32bit_t timer,
                                  uint8_t channel);

/**
 * @brief   Sets the counter value of the given timer
 *
 * @param[in] timer   The timer that controls the counter
 * @param[in] value   The new value of the count register
 */
uint16_t getCounterValue(general_timers_32bit_t timer);

/**
 * @brief   Retrieves the current counter value of the timer
 *
 * @param[in] timer   The timer that controls the counter being read
 *
 * @return   The current counter value of the timer
 */
void setCounterValue(general_timers_32bit_t timer, uint16_t value);

/**
 * @brief   Reads the capture value from a given timer channel
 *
 * @param[in] timer     The timer that controls the capture/compare channel
 * @param[in] channel   The channel to read the capture value from
 *
 * @return   The capture value if the channel parameter is valid
 * @return   0 on an invalid value of the channel parameter
 */
uint32_t readCaptureValue(general_timers_32bit_t timer, uint8_t channel);

/**
 * @brief   Clears all the status flags from the status register
 *
 * @param[in] timer   The timer to clear the status flags for
 */
void clearTimerStatusRegister(general_timers_32bit_t timer);

/**
 * @brief   Sets the compare value of a capture/compare channel of the timer
 *
 * @param[in] timer     The timer that controls the capture/compare channel
 * @param[in] channel   The capture/compare channel to set the value for
 * @param[in] value     The new value of the compare register
 */
void setCompareValue(general_timers_32bit_t timer, uint8_t channel,
                     uint32_t value);

#endif /* GENERAL_TIMERS_H_ */
