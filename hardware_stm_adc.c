/**
  ******************************************************************************
  * @file    hardware_stm_gpio.c 
  * @author  mortamar@andrew.cmu.edu
  * @version 1.0
  * @date    Septembr-2021
  * @brief   Controls STM32F446ze GPIO
  ******************************************************************************
  */

#include "hardware_stm_gpio.h"
#include "hardware_stm_dma.h"
#include "stm32f4xx_rcc_mort.h"
#include <cstdint>



/* MACRO definitions----------------------------------------------------------*/
#define SYSTEM_CONTROL_BASE_ADDRESS (0xE000E000)
#define NVIC_BASE_ADDRESS (SYSTEM_CONTROL_BASE_ADDRESS + 0x100)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_0_31 (NVIC_BASE_ADDRESS)
#define ADC_INTERRUPT_BIT ((uint32_t)0x40000)

#define ADC123_BASE_ADDRESS ((uint32_t)0x40012000)
#define ADC1_BASE_ADDRESS (ADC123_BASE_ADDRESS + 0x000)
#define ADC2_BASE_ADDRESS (ADC123_BASE_ADDRESS + 0x100)
#define ADC3_BASE_ADDRESS (ADC123_BASE_ADDRESS + 0x200)

// status register
#define ADC1_SR_REGISTER (ADC1_BASE_ADDRESS + 0x00)
#define ADC2_SR_REGISTER (ADC2_BASE_ADDRESS + 0x00)
#define ADC3_SR_REGISTER (ADC3_BASE_ADDRESS + 0x00)

// control register 1
#define ADC1_CR1_REGISTER (ADC1_BASE_ADDRESS + 0x04)
#define ADC2_CR1_REGISTER (ADC3_BASE_ADDRESS + 0x04)
#define ADC3_CR1_REGISTER (ADC3_BASE_ADDRESS + 0x04)

// control register 2
#define ADC1_CR2_REGISTER (ADC1_BASE_ADDRESS + 0x08)
#define ADC2_CR2_REGISTER (ADC3_BASE_ADDRESS + 0x08)
#define ADC3_CR2_REGISTER (ADC3_BASE_ADDRESS + 0x08)

// sample time register 1
#define ADC1_SMPR1_REGISTER (ADC1_BASE_ADDRESS + 0x0C)
#define ADC2_SMPR1_REGISTER (ADC2_BASE_ADDRESS + 0x0C)
#define ADC3_SMPR1_REGISTER (ADC3_BASE_ADDRESS + 0x0C)

// sample time register 2
#define ADC1_SMPR2_REGISTER (ADC1_BASE_ADDRESS + 0x10)
#define ADC2_SMPR2_REGISTER (ADC2_BASE_ADDRESS + 0x10)
#define ADC3_SMPR2_REGISTER (ADC3_BASE_ADDRESS + 0x10)

// injected channel data offset register x

// watchdog higher threshold register
#define ADC1_HTR_REGISTER (ADC1_BASE_ADDRESS + 0x24)
#define ADC2_HTR_REGISTER (ADC2_BASE_ADDRESS + 0x24)
#define ADC3_HTR_REGISTER (ADC3_BASE_ADDRESS + 0x24)

// watchdog higher threshold register
#define ADC1_LTR_REGISTER (ADC1_BASE_ADDRESS + 0x28)
#define ADC2_LTR_REGISTER (ADC2_BASE_ADDRESS + 0x28)
#define ADC3_LTR_REGISTER (ADC3_BASE_ADDRESS + 0x28)

// regular sequence 1 register
#define ADC1_SQR1_REGISTER (ADC1_BASE_ADDRESS + 0x2C)
#define ADC2_SQR1_REGISTER (ADC2_BASE_ADDRESS + 0x2C)
#define ADC3_SQR1_REGISTER (ADC3_BASE_ADDRESS + 0x2C)

// regular sequence 2 register
#define ADC1_SQR2_REGISTER (ADC1_BASE_ADDRESS + 0x30)
#define ADC2_SQR2_REGISTER (ADC2_BASE_ADDRESS + 0x30)
#define ADC3_SQR2_REGISTER (ADC3_BASE_ADDRESS + 0x30)

// regular sequence 3 register
#define ADC1_SQR3_REGISTER (ADC1_BASE_ADDRESS + 0x34)
#define ADC2_SQR3_REGISTER (ADC2_BASE_ADDRESS + 0x34)
#define ADC3_SQR3_REGISTER (ADC3_BASE_ADDRESS + 0x34)

// injected sequence register
#define ADC1_JSQR_REGISTER (ADC1_BASE_ADDRESS + 0x38)
#define ADC2_JSQR_REGISTER (ADC2_BASE_ADDRESS + 0x38)
#define ADC3_JSQR_REGISTER (ADC3_BASE_ADDRESS + 0x38)

// injected data register x

// regular data register
#define ADC1_DR_REGISTER (ADC1_BASE_ADDRESS + 0x4C)
#define ADC2_DR_REGISTER (ADC2_BASE_ADDRESS + 0x4C)
#define ADC3_DR_REGISTER (ADC3_BASE_ADDRESS + 0x4C)

// common status register
#define ADC_CSR_REGISTER (ADC1_BASE_ADDRESS + 0x300)

// common control register
#define ADC_COMMON_CCR_REGISTER (ADC_CSR_REGISTER + 0x04)

// common regular data register for dual and triple modes
#define ADC_CDR_REGISTER (ADC_CSR_REGISTER + 0x08)

// ADC_CCR_PRESCALER
#define ADC_PRESCALER_4 ((uint32_t)0x10000)

// ADC_SR_EOC
#define ADC_EOC ((uint32_t)0b10)

// ADC_CR1
#define ADC_SCAN ((uint32_t)0x100)
#define ADC_EOCIE ((uint32_t)0x20)

// ADC_CR2
#define ADC_EOCS ((uint32_t)0x400)
#define ADC_CONT ((uint32_t)0x2)
#define ADC_DDS ((uint32_t)0x200)
#define ADC_DMA ((uint32_t)0x100)
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

/* function definitions----------------------------------------------------------*/
void enableNVIC_ADC( void )
{
    uint32_t * reg_pointer_32;
    reg_pointer_32 = (uint32_t *)NVIC_INTERRUPT_SET_ENABLE_REGISTER_0_31;
    *reg_pointer_32 = ADC_INTERRUPT_BIT;
}

void initADC3_567( void )
{
    uint32_t * reg_pointer;
    /* Initialize adcvalues */
    adcvalue[0] = 0;
    adcvalue[1] = 0;
    adcvalue[2] = 0;
    adcindex = 0;
    adcnewvalue = 0;
    /* Turn on ADC3 bus clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    /* Initialize GPIO F 7 8 9 as analog */
    initGpioF789AsAnalog();
    enableNVIC_ADC();
    /*Setup the clock Prescalers*/
    reg_pointer = (uint32_t *)ADC_COMMON_CCR_REGISTER;
    *reg_pointer = ADC_PRESCALER_4;
    /* configure ADC 12bit resolution, End of conversion interrupt Enabled,
    SCAN Mode enable to be able to scan a group of channels */
    reg_pointer = (uint32_t *)ADC3_CR1_REGISTER;
    *reg_pointer = ADC_EOCIE + ADC_SCAN;
    /* configure ADC External trigger disabled, right data alignment, no DMA,
    EOC is set at the end of each regular conversion, conitnuous conversion disabled */
    reg_pointer = (uint32_t *)ADC3_CR2_REGISTER;
    *reg_pointer = ADC_EOCS;
    /* There will be 3 channels in the sequence of conversions */
    reg_pointer = (uint32_t *)ADC3_SQR1_REGISTER;
    *reg_pointer = ADC_3_CONVERSIONS;
    /* There Channels 5,6,7 to max sampling times (480 cycles) */
    reg_pointer = (uint32_t *)ADC3_SMPR2_REGISTER;
    *reg_pointer = ADC_SMP_5_MX + ADC_SMP_6_MX + ADC_SMP_7_MX;
    /* Configure the sequence of conversion for the ADC 5,6,7 */
    reg_pointer = (uint32_t *)ADC3_SQR3_REGISTER;
    *reg_pointer = ADC_CHANNEL_5_MORT2 + (ADC_CHANNEL_6_MORT2 << 5) + (ADC_CHANNEL_7_MORT2 << 10);
    /* Enable the ADC3 */
    reg_pointer = (uint32_t *)ADC3_CR2_REGISTER;
    *reg_pointer = *reg_pointer | ADC_ADON;
}

void initADC3_5( void )
{
    uint32_t * reg_pointer;
    /* Initialize adcvalues */
    adcvalue[0] = 0;
    /* Turn on ADC3 bus clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    /* Initialize GPIO F 7 8 9 as analog */
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
    EOC is set at the end of each regular conversion, conitnuous conversion enabled */
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

void initADC3_567_withDMA( void )
{
    uint32_t * reg_pointer;
    /* Turn on ADC3 bus clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    /* Initialize GPIO F 7 8 9 as analog */
    initGpioF789AsAnalog();
    initDMAForADC3_3channels();
    enableDMAForAdc3_3channels();
    /*Setup the clock Prescalers*/
    reg_pointer = (uint32_t *)ADC_COMMON_CCR_REGISTER;
    *reg_pointer = ADC_PRESCALER_4;
    /* configure ADC 12bit resolution, End of conversion interrupt Enabled,
    SCAN Mode enable to be able to scan a group of channels */
    reg_pointer = (uint32_t *)ADC3_CR1_REGISTER;
    *reg_pointer = ADC_SCAN;
    /* configure ADC External trigger disabled, right data alignment, DMA,
    EOC is set at the end of each regular conversion, conitnuous conversion enabled */
    reg_pointer = (uint32_t *)ADC3_CR2_REGISTER;
    *reg_pointer = ADC_EOCS + ADC_CONT + ADC_DDS + ADC_DMA;
    /* There will be 3 channels in the sequence of conversions */
    reg_pointer = (uint32_t *)ADC3_SQR1_REGISTER;
    *reg_pointer = ADC_3_CONVERSIONS;
    /* There Channels 5,6,7 to max sampling times (480 cycles) */
    reg_pointer = (uint32_t *)ADC3_SMPR2_REGISTER;
    *reg_pointer = ADC_SMP_5_MX + ADC_SMP_6_MX + ADC_SMP_7_MX;
    /* Configure the sequence of conversion for the ADC 5,6,7 */
    reg_pointer = (uint32_t *)ADC3_SQR3_REGISTER;
    *reg_pointer = ADC_CHANNEL_5_MORT2 + (ADC_CHANNEL_6_MORT2 << 5) + (ADC_CHANNEL_7_MORT2 << 10);
    /* Enable the ADC3 */
    reg_pointer = (uint32_t *)ADC3_CR2_REGISTER;
    *reg_pointer = *reg_pointer | ADC_ADON;
}

void storeNewADCValueInSequence( void )
{
    uint32_t *reg_pointer_32;
    /*Get value stored in data register*/
    reg_pointer_32 = (uint32_t *)ADC3_DR_REGISTER;
    /*Store it in an array*/
    adcvalue[adcindex] = *reg_pointer_32 & 0xFFF;
    /*Increase the index of the array*/
    adcindex = adcindex + 1;
    if (adcindex > 2)
    {
        adcindex = 0; //if index is past the max value
    }
}

void startADCConversion ( void )
{
    uint32_t *reg_pointer_32;
    /* Clear any pending flags in the status register */
    reg_pointer_32 = (uint32_t *)ADC3_SR_REGISTER;
    *reg_pointer_32 = 0;
    /* start conversion of regular channels */
    reg_pointer_32 = (uint32_t *)ADC3_CR2_REGISTER;
    *reg_pointer_32 = *reg_pointer_32 | ADC_SWSTART;
}

uint16_t returnNewADCValue( void )
{
    return  adcvalue[adcindex];  
}

uint8_t returnIfADChasnewValue( void )
{
    return adcnewvalue;
}

void clearIfADChasnewValue( void )
{
    adcnewvalue = 0;
}

void ADC_IRQHanlder( void )
{
    uint32_t *reg_pointer_32_sr;
    uint32_t *reg_pointer32_cr1;
    uint32_t value;

    reg_pointer_32_sr = (uint32_t *) ADC3_SR_REGISTER;
    reg_pointer32_cr1 = (uint32_t *) ADC3_CR1_REGISTER;
    value = *reg_pointer_32_sr;

    if (( (value & ADC_EOC) > 0) && ((*reg_pointer32_cr1 & ADC_EOCIE)))
    { // if we wanted the end of conversion interrupt and that is the one that triggered
        /* store the Data value */
        storeNewADCValueInSequence(); // reading the value from the data register clears the interrupt
        adcnewvalue = adcnewvalue + 1; // we have one more new value
    }
}