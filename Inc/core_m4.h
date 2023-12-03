/**
 * @file core_m4.h
 * @version 1.0
 *
 * @brief   Hardware abstraction layer for implementing core functionalities
 *          of the ARM Cortex M4 processor. This file is compatible for the
 *          STM32F446C and STM32F446E.
 *
 * @author Tom Schmitz <tschmitz@andrew.cmu.edu>
 */

#ifndef CORE_M4_H_
#define CORE_M4_H_

#include <stdint.h>
#include <stdbool.h>

/* Available Interrupt Numbers */
#define INT_NUM_WWDG               0
#define INT_NUM_PVD                1
#define INT_NUM_TAMP_STAMP         2
#define INT_NUM_RTC_WKUP           3
#define INT_NUM_FLASH              4
#define INT_NUM_RCC                5
#define INT_NUM_EXTI0              6
#define INT_NUM_EXTI1              7
#define INT_NUM_EXTI2              8
#define INT_NUM_EXTI3              9
#define INT_NUM_EXTI4              10
#define INT_NUM_DMA1_STREAM0       11
#define INT_NUM_DMA1_STREAM1       12
#define INT_NUM_DMA1_STREAM2       13
#define INT_NUM_DMA1_STREAM3       14
#define INT_NUM_DMA1_STREAM4       15
#define INT_NUM_DMA1_STREAM5       16
#define INT_NUM_DMA1_STREAM6       17
#define INT_NUM_ADC                18
#define INT_NUM_CAN1_TX            19
#define INT_NUM_CAN1_RX0           20
#define INT_NUM_CAN1_RX1           21
#define INT_NUM_CAN1_SCE           22
#define INT_NUM_EXTI9_5            23
#define INT_NUM_TIM1_BRK_TIM9      24
#define INT_NUM_TIM1_UP_TIM10      25
#define INT_NUM_TIM1_TRG_COM_TIM11 26
#define INT_NUM_TIM1_CC            27
#define INT_NUM_TIM2               28
#define INT_NUM_TIM3               29
#define INT_NUM_TIM4               30
#define INT_NUM_I2C1_EV            31
#define INT_NUM_I2C1_ER            32
#define INT_NUM_I2C2_EV            33
#define INT_NUM_I2C2_ER            34
#define INT_NUM_SPI1               35
#define INT_NUM_SPI2               36
#define INT_NUM_USART1             37
#define INT_NUM_USART2             38
#define INT_NUM_USART3             39
#define INT_NUM_EXTI15_10          40
#define INT_NUM_RTC_ALARM          41
#define INT_NUM_OTG_FS_WKUP        42
#define INT_NUM_TIM8_BRK_TIM12     43
#define INT_NUM_TIM8_UP_TIM13      44
#define INT_NUM_TIM8_TRG_COM_TIM14 45
#define INT_NUM_TIM8_CC            46
#define INT_NUM_DMA1_STREAM7       47
#define INT_NUM_FMC                48
#define INT_NUM_SDIO               49
#define INT_NUM_TIM5               50
#define INT_NUM_SPI3               51
#define INT_NUM_UART4              52
#define INT_NUM_UART5              53
#define INT_NUM_TIM6_DAC           54
#define INT_NUM_TIM7               55
#define INT_NUM_DMA2_STREAM0       56
#define INT_NUM_DMA2_STREAM1       57
#define INT_NUM_DMA2_STREAM2       58
#define INT_NUM_DMA2_STREAM3       59
#define INT_NUM_DMA2_STREAM4       60
#define INT_NUM_CAN2_TX            63
#define INT_NUM_CAN2_RX0           64
#define INT_NUM_CAN2_RX1           65
#define INT_NUM_CAN2_SCE           66
#define INT_NUM_OTG_FS             67
#define INT_NUM_DMA2_STREAM5       68
#define INT_NUM_DMA2_STREAM6       69
#define INT_NUM_DMA2_STREAM7       70
#define INT_NUM_USART6             71
#define INT_NUM_I2C3_EV            72
#define INT_NUM_I2C3_ER            73
#define INT_NUM_OTG_HS_EP1_OUT     74
#define INT_NUM_OTG_HS_EP1_IN      75
#define INT_NUM_OTG_HS_WKUP        76
#define INT_NUM_OTG_HS             77
#define INT_NUM_DCMI               78
#define INT_NUM_FPU                81
#define INT_NUM_SPI4               84
#define INT_NUM_SAI1               87
#define INT_NUM_SAI2               91
#define INT_NUM_QUAD_SPI           92
#define INT_NUM_HDMI_CEC           93
#define INT_NUM_SPDIF_RX           94
#define INT_NUM_FMPI2C1            95
#define INT_NUM_FMPI2C1_ERR        96

/**
 * @brief   Configuration information for a given interrupt
 */
typedef struct
{
    uint8_t interrupt_id; ///< ID of the interrupt being configured
    uint8_t priority;     ///< Priority of the interrupt; A larger number
                          ///< correlates to a lower priority
} irq_info_t;

/**
 * @brief   Blocks all interrupts on the system
 */
void disable_global_irq(void);

/**
 * @brief   Unblocks all interrupts on the system
 */
void enable_global_irq(void);

/**
 * @brief   Configures an interrupt given the appropriate information.
 *
 * @param[in] config   The interrupt configuration info
 */
void configure_interrupt(irq_info_t config);

/**
 * @brief   Configures multiple interrupts given the appropriate information.
 *
 * @param[in] config   The interrupts to configure
 * @param n            The number of interrupts to configure
 */
void configure_interrupts(irq_info_t config[], uint8_t n);

/**
 * @brief   Enables a certain interrupt
 *
 * @param[in] irq   The interrupt to enable
 */
void enable_irq(irq_info_t irq);

/**
 * @brief   Disables a certain interrupt
 *
 * @param[in] irq   The interrupt to disable
 */
void disable_irq(irq_info_t irq);

/**
 * @brief   Sets a given interrupt as pending
 *
 * @param[in] irq   The interrupt to as pending
 */
void set_pending_irq(irq_info_t irq);

/**
 * @brief   Clears a given pending interrupt
 *
 * @param[in] irq   The pending interrupt is cleared
 */
void clear_pending_irq(irq_info_t irq);

/**
 * @brief   Checks if an interrupt is active
 *
 * @param[in] irq   The interrupt to see if active
 *
 * @return   true if interrupt is active
 * @return   false if interrupt is not active
 */
bool check_irq_active(irq_info_t irq);

/**
 * @brief   Generates a software interrupt for a given interrupt
 *
 * @param[in] irq   The interrupt to generate
 */
void send_software_irq(irq_info_t irq);

#endif /* CORE_M4_H_ */
