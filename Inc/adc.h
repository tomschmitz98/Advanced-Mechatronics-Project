//
// Created by Tom on 11/26/2023.
//

#ifndef ADC_H_
#define ADC_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm_utils.h"

//SR Flags
#define ADC_OVR BIT5
#define ADC_STRT BIT4
#define ADC_JSTRT BIT3
#define ADC_JEOC BIT2
#define ADC_EOC BIT1
#define ADC_AWD BIT0

// data alignment
#define RIGHT_ALIGNED false
#define LEFT_ALIGNED true

typedef enum {
    ADC_1 = 0x00,
    ADC_2 = 0x40,
    ADC_3 = 0x80
} adc_channel_t;

typedef enum {
    ADC_12BIT = 0x0,
    ADC_10BIT = 0x1,
    ADC_8BIT = 0x2,
    ADC_6BIT = 0x3
} adc_resolution_t;

typedef enum {
    DISCONTINUOUS_1_CHANNEL = 0x0,
    DISCONTINUOUS_2_CHANNEL = 0x1,
    DISCONTINUOUS_3_CHANNEL = 0x2,
    DISCONTINUOUS_4_CHANNEL = 0x3,
    DISCONTINUOUS_5_CHANNEL = 0x4,
    DISCONTINUOUS_6_CHANNEL = 0x5,
    DISCONTINUOUS_7_CHANNEL = 0x6,
    DISCONTINUOUS_8_CHANNEL = 0x7
} adc_discontinuous_channels_t;

typedef enum {
    ADC_INPUT_CHANNEL_0 = 0x00,
    ADC_INPUT_CHANNEL_1 = 0x01,
    ADC_INPUT_CHANNEL_2 = 0x02,
    ADC_INPUT_CHANNEL_3 = 0x03,
    ADC_INPUT_CHANNEL_4 = 0x04,
    ADC_INPUT_CHANNEL_5 = 0x05,
    ADC_INPUT_CHANNEL_6 = 0x06,
    ADC_INPUT_CHANNEL_7 = 0x07,
    ADC_INPUT_CHANNEL_8 = 0x08,
    ADC_INPUT_CHANNEL_9 = 0x09,
    ADC_INPUT_CHANNEL_10 = 0x0A,
    ADC_INPUT_CHANNEL_11 = 0x0B,
    ADC_INPUT_CHANNEL_12 = 0x0C,
    ADC_INPUT_CHANNEL_13 = 0x0D,
    ADC_INPUT_CHANNEL_14 = 0x0E,
    ADC_INPUT_CHANNEL_15 = 0x0F,
    ADC_INPUT_CHANNEL_16 = 0x10,
    ADC_INPUT_CHANNEL_17 = 0x11,
    ADC_INPUT_CHANNEL_18 = 0x12,
} adc_watchdog_channel_select_t;

typedef enum {
    ADC_TRIGGER_DISABLED = 0x0,
    ADC_TRIGGER_RISING = 0x1,
    ADC_TRIGGER_FALLING = 0x2,
    ADC_TRIGGER_BOTH = 0x3
} adc_trigger_detection_t;

typedef enum {
    ADC_TIMER1_CC1_EVENT = 0x0,
    ADC_TIMER1_CC2_EVENT = 0x1,
    ADC_TIMER1_CC3_EVENT = 0x2,
    ADC_TIMER2_CC2_EVENT = 0x3,
    ADC_TIMER2_CC3_EVENT = 0x4,
    ADC_TIMER2_CC4_EVENT = 0x5,
    ADC_TIMER2_TRGO_EVENT = 0x6,
    ADC_TIMER3_CC1_EVENT = 0x7,
    ADC_TIMER3_TRGO_EVENT = 0x8,
    ADC_TIMER4_CC4_EVENT = 0x9,
    ADC_TIMER5_CC1_EVENT = 0xA,
    ADC_TIMER5_CC2_EVENT = 0xB,
    ADC_TIMER5_CC3_EVENT = 0xC,
    ADC_TIMER8_CC1_EVENT = 0xD,
    ADC_TIMER8_TRGO_EVENT = 0xE,
    ADC_EXTI_LINE_11_EVENT = 0xF
} adc_regular_external_trigger_event_t;

typedef enum {
    ADC_INJ_TIMER1_CC4_EVENT = 0x0,
    ADC_INJ_TIMER1_TRGO_EVENT = 0x1,
    ADC_INJ_TIMER2_CC1_EVENT = 0x2,
    ADC_INJ_TIMER2_TRGO_EVENT = 0x3,
    ADC_INJ_TIMER3_CC2_EVENT = 0x4,
    ADC_INJ_TIMER3_CC4_EVENT = 0x5,
    ADC_INJ_TIMER4_CC1_EVENT = 0x6,
    ADC_INJ_TIMER4_CC2_EVENT = 0x7,
    ADC_INJ_TIMER4_CC3_EVENT = 0x8,
    ADC_INJ_TIMER4_TRGO_EVENT = 0x9,
    ADC_INJ_TIMER5_CC4_EVENT = 0xA,
    ADC_INJ_TIMER5_TRGO_EVENT = 0xB,
    ADC_INJ_TIMER8_CC2_EVENT = 0xC,
    ADC_INJ_TIMER8_CC3_EVENT = 0xD,
    ADC_INJ_TIMER8_CC4_EVENT = 0xE,
    ADC_INJ_EXTI_LINE_15_EVENT = 0xF,
} adc_injected_external_trigger_event_t;

typedef enum {
    ADC_SAMPLE_TIME_3_CYCLES = 0x0,
    ADC_SAMPLE_TIME_15_CYCLES = 0x1,
    ADC_SAMPLE_TIME_28_CYCLES = 0x2,
    ADC_SAMPLE_TIME_56_CYCLES = 0x3,
    ADC_SAMPLE_TIME_84_CYCLES = 0x4,
    ADC_SAMPLE_TIME_112_CYCLES = 0x5,
    ADC_SAMPLE_TIME_144_CYCLES = 0x6,
    ADC_SAMPLE_TIME_480_CYCLES = 0x7,
} adc_sample_time_t;

typedef enum {
    ADC_SEQUENCE_LENGTH_1 = 0x0,
    ADC_SEQUENCE_LENGTH_2 = 0x1,
    ADC_SEQUENCE_LENGTH_3 = 0x2,
    ADC_SEQUENCE_LENGTH_4 = 0x3,
    ADC_SEQUENCE_LENGTH_5 = 0x4,
    ADC_SEQUENCE_LENGTH_6 = 0x5,
    ADC_SEQUENCE_LENGTH_7 = 0x6,
    ADC_SEQUENCE_LENGTH_8 = 0x7,
    ADC_SEQUENCE_LENGTH_9 = 0x8,
    ADC_SEQUENCE_LENGTH_10 = 0x9,
    ADC_SEQUENCE_LENGTH_11 = 0xA,
    ADC_SEQUENCE_LENGTH_12 = 0xB,
    ADC_SEQUENCE_LENGTH_13 = 0xC,
    ADC_SEQUENCE_LENGTH_14 = 0xD,
    ADC_SEQUENCE_LENGTH_15 = 0xE,
    ADC_SEQUENCE_LENGTH_16 = 0xF,
} adc_sequence_length_t;

typedef enum {
    ADC_INJ_SEQUENCE_LENGTH_1 = 0x0,
    ADC_INJ_SEQUENCE_LENGTH_2 = 0x1,
    ADC_INJ_SEQUENCE_LENGTH_3 = 0x2,
    ADC_INJ_SEQUENCE_LENGTH_4 = 0x3,
} adc_inj_sequence_length_t;

typedef enum {
    ADC_PRESCALER_DIV_2 = 0x0,
    ADC_PRESCALER_DIV_4 = 0x1,
    ADC_PRESCALER_DIV_6 = 0x2,
    ADC_PRESCALER_DIV_8 = 0x3,
} adc_prescaler_t;

typedef enum {
    ADC_MULTI_DMA_DISABLED = 0x0,
    ADC_MULTI_DMA_MODE_1 = 0x1,
    ADC_MULTI_DMA_MODE_2 = 0x2,
    ADC_MULTI_DMA_MODE_3 = 0x3,
} adc_multi_dma_mode_t;

typedef enum {
    ADC_MULTI_MODE_INDEPENDENT = 0x00,
    ADC_MULTI_MODE_DUAL_REG_AND_INJ = 0x01,
    ADC_MULTI_MODE_DUAL_REG_AND_ALT_TRIGGER = 0x02,
    ADC_MULTI_MODE_DUAL_INJ = 0x05,
    ADC_MULTI_MODE_DUAL_REG = 0x06,
    ADC_MULTI_MODE_DUAL_INTERLEAVED = 0x07,
    ADC_MULTI_MODE_DUAL_ALT_TRIGGER = 0x09,
    ADC_MULTI_MODE_TRIPLE_REG_AND_INJ = 0x11,
    ADC_MULTI_MODE_TRIPLE_REG_AND_ALT_TRIGGER = 0x12,
    ADC_MULTI_MODE_TRIPLE_INJ = 0x15,
    ADC_MULTI_MODE_TRIPLE_REG = 0x16,
    ADC_MULTI_MODE_TRIPLE_INTERLEAVED = 0x17,
    ADC_MULTI_MODE_TRIPLE_ALT_TRIGGER = 0x19

} adc_multi_mode_t;

typedef struct {
    //CR1
    bool overrun_int_en;
    adc_resolution_t resolution;
    bool watchdog_en_regular;
    bool watchdog_en_injected;
    adc_discontinuous_channels_t channels;
    bool enable_discontinuous_mode_on_injected_channels;
    bool enable_discontinuous_mode_on_normal_channels;
    bool auto_inj_group_conv;
    bool watchdog_en_single_channel_scan_mode;
    bool scan_mode;
    bool injected_int_en;
    bool watchdog_int_en;
    bool eoc_int_en;
    adc_watchdog_channel_select_t watchdogChannel;
    //CR2
    adc_trigger_detection_t regular_detection;
    adc_regular_external_trigger_event_t external_trigger_event;
    adc_trigger_detection_t injected_detection;
    adc_injected_external_trigger_event_t injected_trigger_event;
    bool alignment;
    bool set_end_of_conversion;
    bool dma_disable_selection;
    bool dma_mode;
    bool continuous_mode;
    // sample times
    adc_sample_time_t sample_times[19];
    // Injected data offset
    uint16_t injected_data_offset[4];
    // Watchdog
    uint16_t watchdog_high_threshold;
    uint16_t watchdog_low_threshold;
    // ADC Sequence
    adc_sequence_length_t regular_length;
    uint8_t regular_sequence[16];
    adc_inj_sequence_length_t  injected_length;
    uint8_t injected_sequence[4];
} adc_config_t;

typedef struct {
    uint8_t adc_channel_1_sr;
    uint8_t adc_channel_2_sr;
    uint8_t adc_channel_3_sr;
} adc_common_status_t;

typedef struct {
    bool temp_sensor_enable;
    bool vbat_enable;
    adc_prescaler_t prescaler;
    adc_multi_dma_mode_t dma_mode;
    bool dma_disable_selection;
    uint8_t delay;
    adc_multi_mode_t multi_mode;
} adc_common_configs_t;

bool check_adc_status(adc_channel_t channel, uint8_t flags);
void clear_adc_status(adc_channel_t channel, uint8_t flags);

void configure_adc(adc_channel_t channel, adc_config_t config);

void start_regular_adc_conversion(adc_channel_t channel); //CR2 BIT 30
void start_injected_adc_conversion(adc_channel_t channel); //CR2 BIT 22
void enable_adc(adc_channel_t channel);
void disable_adc(adc_channel_t channel);

uint16_t adc_injected_read1(adc_channel_t channel);
uint16_t adc_injected_read2(adc_channel_t channel);
uint16_t adc_injected_read3(adc_channel_t channel);
uint16_t adc_injected_read4(adc_channel_t channel);
uint16_t adc_regular_read(adc_channel_t channel);

void read_adc_common_status(adc_common_status_t *status);
void read_adc_common_data(uint16_t *data1, uint16_t *data2);
void configure_common_adc(adc_common_configs_t configs);

#endif //ADC_H_
