/**
 ******************************************************************************
 * @file    hardware_stm_gpio.c
 * @author  mortamar@andrew.cmu.edu
 * @version 1.0
 * @date    Septembr-2021
 * @brief   Controls STM32F446ze GPIO
 ******************************************************************************
 */

#include "gpio.h"
#include "stm_rcc.h"
#include <stdint.h>

// led 1 is connected to PB0.
// GPIO B addresses: 0x4002 0400 - 0x4002 07FF
// GPIO C addresses: 0x4002 0800 - 0x4002 0BFF

/* MACRO definitions----------------------------------------------------------*/
#define DMA2_BASE_ADDRESS    ((uint32_t)0x40026400)
#define DMA2_LISR_REGISTER   (DMA2_BASE_ADDRESS + 0x00)
#define DMA2_HISR_REGISTER   (DMA2_BASE_ADDRESS + 0x04)
#define DMA2_LIFCR_REGISTER  (DMA2_BASE_ADDRESS + 0x08)
#define DMA2_HIFSR_REGISTER  (DMA2_BASE_ADDRESS + 0x0C)
#define DMA2_S0CR_REGISTER   (DMA2_BASE_ADDRESS + 0x10)
#define DMA2_S0NDTR_REGISTER (DMA2_BASE_ADDRESS + 0x14)
#define DMA2_S0PAR_REGISTER  (DMA2_BASE_ADDRESS + 0x18)
#define DMA2_S0M0AR_REGISTER (DMA2_BASE_ADDRESS + 0x1C)
#define DMA2_S0M1AR_REGISTER (DMA2_BASE_ADDRESS + 0x20)
#define DMA2_S0FCR_REGISTER  (DMA2_BASE_ADDRESS + 0x24)

// bits and flags
// DMA_SxCR: Dma Stream x configuration register
#define DMA_SxCR_CHANNEL_2_SELECT (((uint32_t)2) << 25)
#define DMA_SxCR_MSIZE_HALF_WORD  (((uint32_t)1) << 13)
#define DMA_SxCR_PSIZE_HALF_WORD  (((uint32_t)1) << 11)
#define DMA_SxCR_MINC_INCREMENT   (((uint32_t)1) << 10)
#define DMA_SxCR_CIRC_ENABLE      (((uint32_t)1) << 8)
#define DMA_SxCR_DIR_PERTOMEM     0
#define DMA_SxCR_STREAM_ENABLE    1

#define ADC123_BASE_ADDRESS       ((uint32_t)0x40012000)
#define ADC1_BASE_ADDRESS         (ADC123_BASE_ADDRESS + 0x000)
#define ADC2_BASE_ADDRESS         (ADC123_BASE_ADDRESS + 0x100)
#define ADC3_BASE_ADDRESS         (ADC123_BASE_ADDRESS + 0x200)

// regular data register
#define ADC1_DR_REGISTER (ADC1_BASE_ADDRESS + 0x4C)
#define ADC2_DR_REGISTER (ADC2_BASE_ADDRESS + 0x4C)
#define ADC3_DR_REGISTER (ADC3_BASE_ADDRESS + 0x4C)

/* function
 * definitions----------------------------------------------------------*/
uint16_t adcDmaDataStorageBuffer[3];
uint16_t adcDmaDataStorageBufferforone;

void initDMAForADC3_1channel(void) {
    uint32_t *reg_pointer;

    enable_peripheral_clock(DMA2_EN);

    /* Configure Stream 0 to use channel 2 (ADC3) */
    reg_pointer = (uint32_t *)DMA2_S0CR_REGISTER;
    *reg_pointer = DMA_SxCR_CHANNEL_2_SELECT + DMA_SxCR_MSIZE_HALF_WORD +
                   DMA_SxCR_PSIZE_HALF_WORD + DMA_SxCR_DIR_PERTOMEM +
                   DMA_SxCR_CIRC_ENABLE;

    /* we will transfer 1 data registers for 1 channel of ADC */
    reg_pointer = (uint32_t *)DMA2_S0NDTR_REGISTER;
    *reg_pointer = 1;

    /* we will transfer the ADC3 data register */
    reg_pointer = (uint32_t *)DMA2_S0PAR_REGISTER;
    *reg_pointer = ADC3_DR_REGISTER;

    /* we will transfer to the adcDmaDataStroageBuffer we just created */
    reg_pointer = (uint32_t *)DMA2_S0M0AR_REGISTER;
    *reg_pointer = (uint32_t)&adcDmaDataStorageBufferforone;
}

void enableDMAForAdc3_3channels(void) {
    uint32_t *reg_pointer;
    reg_pointer = (uint32_t *)DMA2_S0CR_REGISTER;
    *reg_pointer = *reg_pointer | DMA_SxCR_STREAM_ENABLE;
}

uint16_t returnADC3StoredValueforone(void) {
    return adcDmaDataStorageBufferforone;
}
