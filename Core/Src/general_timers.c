/*
 * general_timers.c
 *
 *  Created on: Oct 5, 2023
 *      Author: Tom
 */

#include "general_timers.h"
#include "stm_rcc.h"
#include "stm_utils.h"
#include <assert.h>
#include <stdint.h>

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

#define GEN_TIMER_INTE_RESERVED     BITF | BITD | BIT7 | BIT5
#define GEN_TIMER_SR_RESERVED       BITF | BITE | BITD | BIT8 | BIT7 | BIT5
#define GEN_TIMER_EGR_RESERVED                                                 \
    BITF | BITE | BITD | BITC | BITB | BITA | BIT9 | BIT8 | BIT7 | BIT5

// Control reg 1
static void controlReg1_clkdiv(general_timers_32bit_t timer,
                               general_timer_clkdiv_t div) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~(BIT9 | BIT8);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)div) << 8;
}

static void controlReg1_autoreload(general_timers_32bit_t timer, bool enable) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT7;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)enable)
                                                         << 7;
}

static void controlReg1_centerAligned(general_timers_32bit_t timer,
                                      uint8_t mode) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~(BIT6 | BIT5);
    mode &= (BIT1 | BIT0);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)mode) << 5;
}

static void controlReg1_direction(general_timers_32bit_t timer, bool dir) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT4;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)dir) << 4;
}

static void controlReg1_one_pulse(general_timers_32bit_t timer,
                                  bool one_pulse) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT3;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)one_pulse)
                                                         << 3;
}

static void controlReg1_update_request_source(general_timers_32bit_t timer,
                                              bool update_request_source) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT2;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |=
        ((uint32_t)update_request_source) << 2;
}

static void controlReg1_update_disable(general_timers_32bit_t timer,
                                       bool disable) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT1;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= ((uint32_t)disable)
                                                         << 1;
}

void enableTimer(general_timers_32bit_t timer) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) |= BIT0;
}

void disableTimer(general_timers_32bit_t timer) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR1) &= ~BIT0;
}

// Control reg 2
static void controlReg2_selection(general_timers_32bit_t timer, bool xor) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) &= ~BIT7;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) |= ((uint32_t) xor) << 7;
}

static void controlReg2_master_mode_selection(general_timers_32bit_t timer,
                                              uint8_t mode) {
    mode &= (BIT2 | BIT1 | BIT0);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) &= ~(BIT6 | BIT5 | BIT4);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) |= ((uint32_t)mode) << 4;
}

static void controlReg2_capture_compare_dma_select(general_timers_32bit_t timer,
                                                   bool update) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) &= ~BIT3;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CR2) |= ((uint32_t)update)
                                                         << 3;
}

// slave mode control
static void configureSlave(general_timers_32bit_t timer,
                           timer_slave_attr_t slaveAttr) {
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

// Interrupt enable register
static void enableTimerInterrupts(general_timers_32bit_t timer, uint16_t mask) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_DIER) &=
        GEN_TIMER_INTE_RESERVED;
    mask &= ~GEN_TIMER_INTE_RESERVED;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_DIER) |= (uint32_t)mask;
}

// Interrupt status register
bool checkTimerStatus(general_timers_32bit_t timer, uint16_t mask) {
    uint16_t status;
    mask &= ~GEN_TIMER_SR_RESERVED;
    status = TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SR) & mask;
    mask = ~mask;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SR) = mask;
    return status != 0;
}

// Event generation
void updateEventGeneration(general_timers_32bit_t timer, uint16_t mask) {
    mask &= ~GEN_TIMER_EGR_RESERVED;
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_EGR) |= mask;
}

// Capture/compare mode registers
static void configureCaptureCompareModeAsOutput(general_timers_32bit_t timer,
                                                outputCompareMode_t compareAttr,
                                                uint8_t channel) {
    uint32_t reg = (channel < 3) ? TIMER32BIT_CCMR1 : TIMER32BIT_CCMR2;
    uint32_t shift = (channel & BIT0) ? 0 : 8;
    TIMER_BASE_32BIT((uint32_t)timer, reg) &= ~(0xFF << shift);
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

static void configureCaptureCompareModeAsInput(general_timers_32bit_t timer,
                                               inputCaptureMode_t captureAttr,
                                               uint8_t channel) {
    uint32_t reg = (channel < 3) ? TIMER32BIT_CCMR1 : TIMER32BIT_CCMR2;
    uint32_t shift = (channel & BIT0) ? 0 : 8;
    TIMER_BASE_32BIT((uint32_t)timer, reg) &= ~(0xFF << shift);
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

static void enableCaptureCompare(general_timers_32bit_t timer,
                                 uint8_t channel) {
    // Look at bits in CCMR
    uint32_t mask = BIT0 << ((channel - 1) << 2);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) |= mask;
}

static void disableCaptureCompare(general_timers_32bit_t timer,
                                  uint8_t channel) {
    // Look at bits in CCMR
    uint32_t mask = BIT0 << ((channel - 1) << 2);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) &= ~mask;
}

static void configureCapturePolarity(general_timers_32bit_t timer,
                                     uint8_t channel, uint8_t polarity) {
    // Look at bits in CCMR
    uint32_t mask = 0;

    switch (polarity) {
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

static void configureComparePolarity(general_timers_32bit_t timer,
                                     uint8_t channel, uint8_t polarity) {
    // Look at bits in CCMR
    uint32_t mask = (polarity & 0x1 << 1) << ((channel - 1) << 2);

    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) &=
        ~((BIT1 | BIT3) << (channel - 1) << 2);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CCER) |= mask;
}

void setCounterValue(general_timers_32bit_t timer, uint16_t value) {
    disableTimer(timer);
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CNT) = value;
    enableTimer(timer);
}

uint16_t getCounterValue(general_timers_32bit_t timer) {
    return (uint16_t)TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_CNT);
}

static void setPrescalar(general_timers_32bit_t timer, uint16_t value) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_PSC) = (uint32_t)value;
}

static void setAutoReload(general_timers_32bit_t timer, uint16_t value) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_ARR) = (uint32_t)value;
}

static void setCompare(general_timers_32bit_t timer, uint8_t channel,
                       uint32_t value) {
    uint32_t reg = TIMER32BIT_CCR1 + (uint32_t)channel - 1;

    if (!(timer == TIMER2 || timer == TIMER5)) {
        value &= 0xFFFF;
    }

    TIMER_BASE_32BIT((uint32_t)timer, reg) = value;
}

static uint32_t readCapture(general_timers_32bit_t timer, uint8_t channel) {
    uint32_t reg = TIMER32BIT_CCR1 + (uint32_t)channel - 1;
    return TIMER_BASE_32BIT((uint32_t)timer, reg);
}

static void configureDMAcontrol(general_timers_32bit_t timer,
                                general_timer_attr_t attr) {
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

static void configureDMAfullTransferAddress(general_timers_32bit_t timer,
                                            uint16_t addr) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_DMAR) = (uint32_t)addr;
}

static void configureTimerOption(general_timers_32bit_t timer,
                                 uint8_t timerOption) {
    uint32_t config = timerOption & (BIT1 | BIT0);

#if defined(__GNUC__)
#pragma GCC diagnostic push
// This error is ignored because not all the cases need to be handled here...
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif
    switch (timer) {
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

static void configureCaptureCompare(general_timers_32bit_t timer,
                                    general_timer_attr_t attr) {
    // Configure channel 1
    if (attr.ccMode1 == CAPTURE_MODE) {
        configureCaptureCompareModeAsInput(timer, attr.capture1, 1);
        configureCapturePolarity(timer, 1, attr.captureCompareOutputPolarity1);
    } else {
        configureCaptureCompareModeAsOutput(timer, attr.compare1, 1);
        configureComparePolarity(timer, 1, attr.captureCompareOutputPolarity1);
        setCompare(timer, 1, attr.compare1.compareValue);
    }

    // Configure channel 2
    if (attr.ccMode2 == CAPTURE_MODE) {
        configureCaptureCompareModeAsInput(timer, attr.capture2, 2);
        configureCapturePolarity(timer, 2, attr.captureCompareOutputPolarity2);
    } else {
        configureCaptureCompareModeAsOutput(timer, attr.compare2, 2);
        configureComparePolarity(timer, 2, attr.captureCompareOutputPolarity2);
        setCompare(timer, 2, attr.compare2.compareValue);
    }

    // Configure channel 3
    if (attr.ccMode3 == CAPTURE_MODE) {
        configureCaptureCompareModeAsInput(timer, attr.capture3, 3);
        configureCapturePolarity(timer, 3, attr.captureCompareOutputPolarity3);
    } else {
        configureCaptureCompareModeAsOutput(timer, attr.compare3, 3);
        configureComparePolarity(timer, 3, attr.captureCompareOutputPolarity3);
        setCompare(timer, 3, attr.compare3.compareValue);
    }

    // Configure channel 4
    if (attr.ccMode4 == CAPTURE_MODE) {
        configureCaptureCompareModeAsInput(timer, attr.capture4, 4);
        configureCapturePolarity(timer, 4, attr.captureCompareOutputPolarity4);
    } else {
        configureCaptureCompareModeAsOutput(timer, attr.compare4, 4);
        configureComparePolarity(timer, 4, attr.captureCompareOutputPolarity4);
        setCompare(timer, 4, attr.compare4.compareValue);
    }
}

static void enableClock(general_timers_32bit_t timer) {
    switch (timer) {
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

void configureGeneralTimer(general_timers_32bit_t timer,
                           general_timer_attr_t attr) {
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

    if (attr.enableAfterConfig) {
        enableTimer(timer);
    }
}

void enableCaptureCompareChannel(general_timers_32bit_t timer,
                                 uint8_t channel) {
    if (channel < 1 || channel > 4) {
        return;
    }
    enableCaptureCompare(timer, channel);
}

void disableCaptureCompareChannel(general_timers_32bit_t timer,
                                  uint8_t channel) {
    if (channel < 1 || channel > 4) {
        return;
    }
    disableCaptureCompare(timer, channel);
}

uint32_t readCaptureValue(general_timers_32bit_t timer, uint8_t channel) {
    if (channel < 1 || channel > 4) {
        return 0;
    }

    return readCapture(timer, channel);
}

void clearTimerStatusRegister(general_timers_32bit_t timer) {
    TIMER_BASE_32BIT((uint32_t)timer, TIMER32BIT_SR) = 0;
}

void reconfigureCompareChannel(general_timers_32bit_t timer, uint8_t channel,
                               uint32_t compare) {
    assert(timer == TIMER2 || timer == TIMER3 || timer == TIMER4 ||
           timer == TIMER5);

    if (channel < 1 || channel > 3) {
        return;
    }
    setCompare(timer, channel, compare);
}
