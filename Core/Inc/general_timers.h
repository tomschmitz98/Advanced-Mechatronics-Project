/*
 * general_timers.h
 *
 *  Created on: Oct 5, 2023
 *      Author: Tom
 */

#ifndef GENERAL_TIMERS_H_
#define GENERAL_TIMERS_H_

#include "stm_utils.h"
#include <stdbool.h>
#include <stdint.h>

// Center aligned
#define EDGE_ALIGNED          0x0
#define CENTER_ALIGNED_MODE_1 0x1
#define CENTER_ALIGNED_MODE_2 0x2
#define CENTER_ALIGNED_MODE_3 0x3

// direction
#define UP_COUNTER   false
#define DOWN_COUNTER true

// updateRequestSource
#define ANY_UPDATE_SOURCE     false
#define OVERFLOW_OR_UNDERFLOW true

// TI1S
#define CH1_ONLY        false
#define XOR_CH1_CH2_CH3 true

// Master Mode Selection
#define TIM_MM_RESET          0x0
#define TIM_MM_ENABLE         0x1
#define TIM_MM_UPDATE         0x2
#define TIM_MM_COMPARE        0x3
#define TIM_MM_0C1REF_COMPARE 0x4
#define TIM_MM_0C2REF_COMPARE 0x5
#define TIM_MM_0C3REF_COMPARE 0x6
#define TIM_MM_0C4REF_COMPARE 0x7

// captureDMA
#define CCx_EVENT    false
#define UPDATE_EVENT true

// Interrupt enable
#define TDE   BITE
#define CC4DE BITC
#define CC3DE BITB
#define CC2DE BITA
#define CC1DE BIT9
#define UDE   BIT8
#define TIE   BIT6
#define CC4IE BIT4
#define CC3IE BIT3
#define CC2IE BIT2
#define CC1IE BIT1
#define UIE   BIT0

// Interrupt status
#define CC4OF BITC
#define CC3OF BITB
#define CC2OF BITA
#define CC1OF BIT9
#define TIF   BIT6
#define CC4IF BIT4
#define CC3IF BIT3
#define CC2IF BIT2
#define CC1IF BIT1
#define UIF   BIT0

// Event generation
#define TG   BIT6
#define CC4G BIT4
#define CC3G BIT3
#define CC2G BIT2
#define CC1G BIT1
#define UG   BIT0

// Capture/Compare Enable
#define COMPARE_ACTIVE_HIGH          0
#define COMPARE_ACTIVE_LOW           1
#define CAPTURE_NONINVERTING_RISING  0
#define CAPTURE_INVERTING_FALLING    1
#define CAPTURE_INVERTING_BOTH_EDGES 2

#define CAPTURE_MODE                 true
#define COMPARE_MODE                 false

// Compare Modes
#define COMPARE_FROZEN          0
#define COMPARE_ACTIVE_MATCH    1
#define COMPARE_ACTIVE_MISMATCH 2
#define COMPARE_TOGGLE          3
#define COMPARE_FORCE_INACTIVE  4
#define COMPARE_FORCE_ACTIVE    5
#define COMPARE_PWM_MODE_1      6
#define COMPARE_PWM_MODE_2      7

typedef enum {
    TIMER2 = 0x000,
    TIMER3 = 0x100,
    TIMER4 = 0x200,
    TIMER5 = 0x300,
} general_timers_32bit_t;

typedef enum {
    CLKDIV_1 = 0x0,
    CLKDIV_2 = 0x1,
    CLKDIV_4 = 0x2
} general_timer_clkdiv_t;

typedef struct {
    bool outputCaptureClearEnable;
    uint8_t outputCompareMode;
    bool outputComparePreloadEnable;
    bool outputCompareFastEnable;
    uint8_t captureCompareSelection;
    uint32_t compareValue;
} outputCompareMode_t;

typedef struct {
    uint8_t inputCaptureFilter;
    uint8_t inputCapturePrescaler;
    uint8_t captureCompareSelection;
} inputCaptureMode_t;

typedef struct {
    bool ETP;
    bool ECE;
    uint8_t ETPS;
    uint8_t ETF;
    bool MSM;
    uint8_t TS;
    uint8_t SMS;
} timer_slave_attr_t;

typedef struct {
    // CR1
    general_timer_clkdiv_t clkDiv;
    bool autoReload;
    uint8_t centerAligned;
    bool direction;
    bool onePulse;
    bool updateRequestSource;
    bool updateDisable;
    // CR2
    bool timerInput1;
    uint8_t masterMode;
    bool captureDMA;
    // SMCR
    timer_slave_attr_t slaveAttr;
    // DIER
    uint16_t interruptEnableMask;
    // CCMR1/2
    union {
        outputCompareMode_t compare4;
        inputCaptureMode_t capture4;
    };

    union {
        outputCompareMode_t compare3;
        inputCaptureMode_t capture3;
    };

    union {
        outputCompareMode_t compare2;
        inputCaptureMode_t capture2;
    };

    union {
        outputCompareMode_t compare1;
        inputCaptureMode_t capture1;
    };
    bool ccMode1;
    bool ccMode2;
    bool ccMode3;
    bool ccMode4;

    // CCER
    uint8_t captureCompareOutputPolarity4;
    uint8_t captureCompareOutputPolarity3;
    uint8_t captureCompareOutputPolarity2;
    uint8_t captureCompareOutputPolarity1;

    uint16_t prescaler;
    uint16_t auto_reload_value;

    uint8_t dmaBurstLength;
    uint8_t dmaBaseAddr;
    uint16_t dmaBurstAddr;
    uint8_t timerOption;

    uint16_t initialCount;
    bool enableAfterConfig;
} general_timer_attr_t;

void configureGeneralTimer(general_timers_32bit_t timer,
                           general_timer_attr_t attr);
void enableTimer(general_timers_32bit_t timer);
void disableTimer(general_timers_32bit_t timer);
bool checkTimerStatus(general_timers_32bit_t timer, uint16_t mask);
void updateEventGeneration(general_timers_32bit_t timer, uint16_t mask);
void enableCaptureCompareChannel(general_timers_32bit_t timer, uint8_t channel);
void disableCaptureCompareChannel(general_timers_32bit_t timer,
                                  uint8_t channel);

uint16_t getCounterValue(general_timers_32bit_t timer);
void setCounterValue(general_timers_32bit_t timer, uint16_t value);

uint32_t readCaptureValue(general_timers_32bit_t timer, uint8_t channel);

void clearTimerStatusRegister(general_timers_32bit_t timer);

#endif /* GENERAL_TIMERS_H_ */
