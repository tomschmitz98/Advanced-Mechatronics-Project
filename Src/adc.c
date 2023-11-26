//
// Created by Tom on 11/26/2023.
//

#include <stdint.h>
#include <stdbool.h>
#include "adc.h"

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
