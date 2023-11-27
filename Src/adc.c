//
// Created by Tom on 11/26/2023.
//

#include <stdint.h>
#include <stdbool.h>
#include "adc.h"
#include "stm_utils.h"

#define COMMON_ADC 0xC0

#define ADC_BASE(adc, reg) *(((uint32_t *)0x40012000) + adc + reg)
// ADC channel registers
#define ADC_SR 0
#define ADC_CR1 1
#define ADC_CR2 2
#define ADC_SMPR1 3
#define ADC_SMPR2 4
#define ADC_JOFR1 5
#define ADC_JOFR2 6
#define ADC_JOFR3 7
#define ADC_JOFR4 8
#define ADC_HTR 9
#define ADC_LTR 10
#define ADC_SQR1 11
#define ADC_SQR2 12
#define ADC_SQR3 13
#define ADC_JSQR 14
#define ADC_JDR1 15
#define ADC_JDR2 16
#define ADC_JDR3 17
#define ADC_JDR4 18
#define ADC_DR 19
//ADC common registers
#define ADC_CSR 0
#define ADC_CCR 1
#define ADC_CDR 2

#define ADC_VALID_FLAGS (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define ADC_VALID_INJECTED_DATA 0xFFF
#define ADC_WATCHDOG_VALID 0xFFF

bool check_adc_status(adc_channel_t channel, uint8_t flags)
{
    flags &= ADC_VALID_FLAGS;
    return (ADC_BASE((uint32_t)channel, ADC_SR) & (uint32_t)flags) != 0;
}

void clear_adc_status(adc_channel_t channel, uint8_t flags)
{
    flags &= ADC_VALID_FLAGS;
    ADC_BASE((uint32_t)channel, ADC_SR) = (uint32_t)flags;
}

static void set_adc_overrun_int_enable(adc_channel_t channel, bool enable)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~UPPER16BITS(BITA);
    if (enable)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= UPPER16BITS(BITA);
    }
}

static void set_adc_resolution(adc_channel_t channel, adc_resolution_t resolution)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~UPPER16BITS(BIT9 | BIT8);
    ADC_BASE((uint32_t)channel, ADC_CR1) |= ((uint32_t)resolution) << 24;
}

static void set_adc_reg_watchdog(adc_channel_t channel, bool enable)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~UPPER16BITS(BIT7);
    if (enable)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= UPPER16BITS(BIT7);
    }
}

static void set_adc_inj_watchdog(adc_channel_t channel, bool enable)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~UPPER16BITS(BIT6);
    if (enable)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= UPPER16BITS(BIT6);
    }
}

static void set_discontinuous_mode_channel_count(adc_channel_t channel, adc_discontinuous_channels_t count)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~(BITF | BITE | BITD);
    ADC_BASE((uint32_t)channel, ADC_CR1) |= ((uint32_t)count) << 13;
}

static void set_discontinuous_mode_inj(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~BITC;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= BITC;
    }
}

static void set_discontinuous_mode_reg(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~BITB;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= BITB;
    }
}

static void set_auto_group_conversion(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~BITA;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= BITA;
    }
}

static void set_watchdog_single_scan_mode(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~BIT9;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= BIT9;
    }
}

static void set_scan_mode(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~BIT8;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= BIT8;
    }
}

static void set_inj_channel_int_en(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~BIT7;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= BIT7;
    }
}

static void set_watchdog_int_en(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~BIT6;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= BIT6;
    }
}

static void set_eoc_int_en(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~BIT5;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR1) |= BIT5;
    }
}

static void set_watchdog_channel(adc_channel_t channel, adc_watchdog_channel_select_t channelSelect)
{
    ADC_BASE((uint32_t)channel, ADC_CR1) &= ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
    ADC_BASE((uint32_t)channel, ADC_CR1) |= (uint32_t)channelSelect;
}

static void set_reg_external_trigger_en(adc_channel_t channel, adc_trigger_detection_t triggerDetection)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) &= ~UPPER16BITS(BITD | BITC);
    ADC_BASE((uint32_t)channel, ADC_CR2) |= ((uint32_t)triggerDetection) << 28;
}

static void select_reg_trigger_event(adc_channel_t channel, adc_regular_external_trigger_event_t event)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) &= ~UPPER16BITS(BITB | BITA | BIT9 | BIT8);
    ADC_BASE((uint32_t)channel, ADC_CR2) |= ((uint32_t)event) << 24;
}

static void set_inj_external_trigger_en(adc_channel_t channel, adc_trigger_detection_t triggerDetection)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) &= ~UPPER16BITS(BIT5 | BIT4);
    ADC_BASE((uint32_t)channel, ADC_CR2) |= ((uint32_t)triggerDetection) << 20;
}

static void select_inj_trigger_event(adc_channel_t channel, adc_injected_external_trigger_event_t event)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) &= ~UPPER16BITS(BIT3 | BIT2 | BIT1 | BIT0);
    ADC_BASE((uint32_t)channel, ADC_CR2) |= ((uint32_t)event) << 16;
}

static void set_adc_alignment(adc_channel_t channel, bool alignment)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) &= ~BITB;
    if (alignment)
    {
        ADC_BASE((uint32_t)channel, ADC_CR2) |= BITB;
    }
}

static void set_eocs(adc_channel_t channel, bool eocs)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) &= ~BITA;
    if (eocs)
    {
        ADC_BASE((uint32_t)channel, ADC_CR2) |= BITA;
    }
}

static void set_dma_selection(adc_channel_t channel, bool dma_selection)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) |= ~BIT9;
    if (dma_selection)
    {
        ADC_BASE((uint32_t)channel, ADC_CR2) |= BIT9;
    }
}

static void set_dma_mode(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) |= ~BIT8;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR2) |= BIT8;
    }
}

static void set_continuous_mode(adc_channel_t channel, bool enabled)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) |= ~BIT1;
    if (enabled)
    {
        ADC_BASE((uint32_t)channel, ADC_CR2) |= BIT1;
    }
}

static void set_sample_times(adc_channel_t channel, adc_sample_time_t *sample_time)
{
    uint32_t offset, i = 0, shift;
    ADC_BASE((uint32_t)channel, ADC_SMPR1) = 0;
    ADC_BASE((uint32_t)channel, ADC_SMPR2) = 0;

    for (; i < 19; i++)
    {
        offset = (i < 10) ? ADC_SMPR2 : ADC_SMPR1;
        shift = (i << 1) + i;
        shift -= (offset == ADC_SMPR1) ? 30 : 0;
        ADC_BASE((uint32_t)channel, offset) |= ((uint32_t)sample_time[i]) << shift;
    }
}

static void set_injected_data_offset(adc_channel_t channel, uint16_t *injected_data)
{
    uint32_t i = 0;

    for(; i < 4; i++)
    {
        ADC_BASE((uint32_t)channel, ADC_JOFR1 + i) = ((uint32_t)injected_data[i]) & ADC_VALID_INJECTED_DATA;
    }
}

static void set_watchdog_threshold(adc_channel_t channel, uint16_t high, uint16_t low)
{
    ADC_BASE((uint32_t)channel, ADC_HTR) = ((uint32_t)high) & ADC_WATCHDOG_VALID;
    ADC_BASE((uint32_t)channel, ADC_LTR) = ((uint32_t)low) & ADC_WATCHDOG_VALID;
}

static void set_regular_sequence_length(adc_channel_t channel, adc_sequence_length_t length)
{
    ADC_BASE((uint32_t)channel, ADC_SQR1) &= ~UPPER16BITS(BIT7 | BIT6 | BIT5 | BIT4);
    ADC_BASE((uint32_t)channel, ADC_SQR1) |= ((uint32_t)length) << 20;
}

static uint32_t lookup_sequence_reg(uint32_t conversion, uint32_t *shift)
{
    uint32_t ret;
    switch(conversion)
    {
        case 15:
        case 14:
        case 13:
        case 12:
            ret = ADC_SQR1;
            *shift = (conversion - 12) * 5;
            break;
        case 11:
        case 10:
        case 9:
        case 8:
        case 7:
        case 6:
            ret = ADC_SQR2;
            *shift = (conversion - 6) * 5;
            break;
        case 5:
        case 4:
        case 3:
        case 2:
        case 1:
        case 0:
            ret = ADC_SQR3;
            *shift = conversion * 5;
            break;
        default:
            assert(false);
    }

    return ret;
}

static void set_sequence(adc_channel_t channel, uint8_t *sequence)
{
    uint32_t i = 0, offset, shift, sequence_val, mask;

    for (; i < 16; i++)
    {
        sequence_val = (uint32_t)sequence[i];
        assert(sequence_val < 19);
        offset = lookup_sequence_reg(i, &shift);
        sequence_val <<= shift;
        mask = 0x1F << shift;
        ADC_BASE((uint32_t)channel, offset) &= ~mask;
        ADC_BASE((uint32_t)channel, offset) |= sequence_val;
    }
}

static void set_injected_sequence_length(adc_channel_t channel, adc_inj_sequence_length_t length)
{
    ADC_BASE((uint32_t)channel, ADC_JSQR) &= ~UPPER16BITS(BIT5 | BIT4);
    ADC_BASE((uint32_t)channel, ADC_SQR1) |= ((uint32_t)length) << 20;
}

static void set_injected_sequence(adc_channel_t channel, uint8_t *injected_sequence)
{
    uint32_t i = 0, shift = 0, sequence_val, mask;

    for (; i < 4; i++, shift += 5)
    {
        sequence_val = (uint32_t)injected_sequence[i];
        sequence_val <<= shift;
        mask = 0x1F << shift;
        ADC_BASE((uint32_t)channel, ADC_JSQR) &= ~mask;
        ADC_BASE((uint32_t)channel, ADC_JSQR) |= sequence_val;
    }
}

void configure_adc(adc_channel_t channel, adc_config_t config)
{
    disable_adc(channel);

    set_adc_overrun_int_enable(channel, config.overrun_int_en);
    set_adc_resolution(channel, config.resolution);
    set_adc_reg_watchdog(channel, config.watchdog_en_regular);
    set_adc_inj_watchdog(channel, config.watchdog_en_injected);
    set_discontinuous_mode_channel_count(channel, config.channels);
    set_discontinuous_mode_inj(channel, config.enable_discontinuous_mode_on_injected_channels);
    set_discontinuous_mode_reg(channel, config.enable_discontinuous_mode_on_normal_channels);
    set_auto_group_conversion(channel, config.watchdog_en_single_channel_scan_mode);
    set_scan_mode(channel, config.scan_mode);
    set_inj_channel_int_en(channel, config.injected_int_en);
    set_watchdog_int_en(channel, config.watchdog_int_en);
    set_eoc_int_en(channel, config.eoc_int_en);
    set_watchdog_channel(channel, config.watchdogChannel);
    set_reg_external_trigger_en(channel, config.regular_detection);
    select_reg_trigger_event(channel, config.external_trigger_event);
    set_inj_external_trigger_en(channel, config.injected_detection);
    select_inj_trigger_event(channel, config.injected_trigger_event);
    set_adc_alignment(channel, config.alignment);
    set_eocs(channel, config.set_end_of_conversion);
    set_dma_selection(channel, config.dma_disable_selection);
    set_dma_mode(channel, config.dma_mode);
    set_continuous_mode(channel, config.continuous_mode);
    set_sample_times(channel, config.sample_times);
    set_injected_data_offset(channel, config.injected_data_offset);
    set_watchdog_threshold(channel, config.watchdog_high_threshold, config.watchdog_low_threshold);
    set_regular_sequence_length(channel, config.regular_length);
    set_sequence(channel, config.regular_sequence);
    set_injected_sequence_length(channel, config.injected_length);
    set_injected_sequence(channel, config.injected_sequence);
}

void start_regular_adc_conversion(adc_channel_t channel)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) |= UPPER16BITS(BITE);
}

void start_injected_adc_conversion(adc_channel_t channel)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) |= UPPER16BITS(BIT6);
}

void enable_adc(adc_channel_t channel)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) |= BIT0;
}

void disable_adc(adc_channel_t channel)
{
    ADC_BASE((uint32_t)channel, ADC_CR2) &= ~BIT0;
}

static inline uint16_t adc_read_injected_channel(adc_channel_t channel, uint8_t offset)
{
    return (uint16_t)ADC_BASE((uint32_t)channel, offset);
}

uint16_t adc_injected_read1(adc_channel_t channel)
{
    return adc_read_injected_channel(channel, ADC_JDR1);
}

uint16_t adc_injected_read2(adc_channel_t channel)
{
    return adc_read_injected_channel(channel, ADC_JDR2);
}

uint16_t adc_injected_read3(adc_channel_t channel)
{
    return adc_read_injected_channel(channel, ADC_JDR3);
}

uint16_t adc_injected_read4(adc_channel_t channel)
{
    return adc_read_injected_channel(channel, ADC_JDR4);
}

uint16_t adc_regular_read(adc_channel_t channel)
{
    return ADC_BASE((uint32_t)channel, ADC_DR);
}
