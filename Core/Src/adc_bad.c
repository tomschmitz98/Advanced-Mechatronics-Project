/**
 ******************************************************************************
 * @file    hardware_stm_gpio.c
 * @author  mortamar@andrew.cmu.edu
 * @version 1.0
 * @date    Septembr-2021
 * @brief   Controls STM32F446ze GPIO
 ******************************************************************************
 */

#include "dma_bad.h"
#include "gpio.h"
#include "stm_rcc.h"
#include <stdint.h>

/* MACRO definitions----------------------------------------------------------*/

#define ADC123_BASE_ADDRESS     ((uint32_t)0x40012000)
#define ADC3_BASE_ADDRESS       (ADC123_BASE_ADDRESS + 0x200)

#define ADC3_SR_REGISTER        (ADC3_BASE_ADDRESS + 0x00)
#define ADC3_CR1_REGISTER       (ADC3_BASE_ADDRESS + 0x04)
#define ADC3_CR2_REGISTER       (ADC3_BASE_ADDRESS + 0x08)
#define ADC3_SMPR1_REGISTER     (ADC3_BASE_ADDRESS + 0x0C)
#define ADC3_SMPR2_REGISTER     (ADC3_BASE_ADDRESS + 0x10)
#define ADC3_HTR_REGISTER       (ADC3_BASE_ADDRESS + 0x24)
#define ADC3_LTR_REGISTER       (ADC3_BASE_ADDRESS + 0x28)
#define ADC3_SQR1_REGISTER      (ADC3_BASE_ADDRESS + 0x2C)
#define ADC3_SQR2_REGISTER      (ADC3_BASE_ADDRESS + 0x30)
#define ADC3_SQR3_REGISTER      (ADC3_BASE_ADDRESS + 0x34)
#define ADC3_JSQR_REGISTER      (ADC3_BASE_ADDRESS + 0x38)
#define ADC3_DR_REGISTER        (ADC3_BASE_ADDRESS + 0x4C)
#define ADC_CSR_REGISTER        (ADC123_BASE_ADDRESS + 0x300)
#define ADC_COMMON_CCR_REGISTER (ADC_CSR_REGISTER + 0x04)
#define ADC_CDR_REGISTER        (ADC_CSR_REGISTER + 0x08)

// ADC_CCR_PRESCALER
#define ADC_PRESCALER_4 ((uint32_t)0x10000)

// ADC_SR_EOC
#define ADC_EOC ((uint32_t)0b10)

// ADC_CR1
#define ADC_SCAN  ((uint32_t)0x100)
#define ADC_EOCIE ((uint32_t)0x20)

// ADC_CR2
#define ADC_EOCS    ((uint32_t)0x400)
#define ADC_CONT    ((uint32_t)0x2)
#define ADC_DDS     ((uint32_t)0x200)
#define ADC_DMA     ((uint32_t)0x100)
#define ADC_SWSTART ((uint32_t)0x40000000)

// conversions
#define ADC_1_CONVERSIONS ((uint32_t)0b0000)
#define ADC_3_CONVERSIONS (((uint32_t)0b0010) << 20)

// SMPR2 Cycle
#define ADC_SMP_5_MX ((uint32_t)0x38000)
#define ADC_SMP_6_MX (((uint32_t)0b111) << 18)
#define ADC_SMP_7_MX (((uint32_t)0b111) << 21)

// SQR3 conversion
#define ADC_CHANNEL_5_MORT2 ((uint32_t)0b101)
#define ADC_CHANNEL_6_MORT2 ((uint32_t)0b110)
#define ADC_CHANNEL_7_MORT2 ((uint32_t)0b111)

// CR2
#define ADC_ADON ((uint32_t)0b1)

uint16_t adcvalue[3];
uint8_t adcnewvalue;
uint8_t adcindex;

/* function
 * definitions----------------------------------------------------------*/

static void initGpioF7AsAnalog(void) {
    gpio_config_t gpio_pin = {.gpio_bank = bank_f,
                              .pin_number = 7,
                              .output_type = push_pull,
                              .resistor = no_pull,
                              .mode = analog};
    init_gpio(gpio_pin);
}

void initADC3_5(void) {
    uint32_t *reg_pointer;
    /* Initialize adcvalues */
    adcvalue[0] = 0;
    /* Turn on ADC3 bus clock */
    enable_peripheral_clock(ADC3_EN);
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    /* Initialize GPIO F7 as analog */
    initGpioF7AsAnalog();
    initDMAForADC3_1channel();
    enableDMAForAdc3_3channels();
    /*Setup the clock Prescalers*/
    reg_pointer = (uint32_t *)ADC_COMMON_CCR_REGISTER;
    *reg_pointer = ADC_PRESCALER_4;
    /* configure ADC 12bit resolution, End of conversion interrupt Enabled,
    SCAN Mode enable to be able to scan a group of channels */
    reg_pointer = (uint32_t *)ADC3_CR1_REGISTER;
    *reg_pointer = ADC_SCAN;
    /* configure ADC External trigger disabled, right data alignment, DMA,
    EOC is set at the end of each regular conversion, conitnuous conversion
    enabled */
    reg_pointer = (uint32_t *)ADC3_CR2_REGISTER;
    *reg_pointer = ADC_EOCS + ADC_CONT + ADC_DDS + ADC_DMA;
    /* There will be 1 channels in the sequence of conversions */
    reg_pointer = (uint32_t *)ADC3_SQR1_REGISTER;
    *reg_pointer = ADC_1_CONVERSIONS;
    /* There Channels 5 to max sampling times (480 cycles) */
    reg_pointer = (uint32_t *)ADC3_SMPR2_REGISTER;
    *reg_pointer = ADC_SMP_5_MX;
    /* Configure the sequence of conversion for the ADC 5,6,7 */
    reg_pointer = (uint32_t *)ADC3_SQR3_REGISTER;
    *reg_pointer = ADC_CHANNEL_5_MORT2;
    /* Enable the ADC3 */
    reg_pointer = (uint32_t *)ADC3_CR2_REGISTER;
    *reg_pointer = *reg_pointer | ADC_ADON;
}

void startADCConversion(void) {
    uint32_t *reg_pointer_32;
    /* Clear any pending flags in the status register */
    reg_pointer_32 = (uint32_t *)ADC3_SR_REGISTER;
    *reg_pointer_32 = 0;
    /* start conversion of regular channels */
    reg_pointer_32 = (uint32_t *)ADC3_CR2_REGISTER;
    *reg_pointer_32 = *reg_pointer_32 | ADC_SWSTART;
}
