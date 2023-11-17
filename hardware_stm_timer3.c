#include "hardware_stm_gpio.h"
#include "stm32f4xx_rcc_mort.h"

/* MACRO definitions----------------------------------------------------------*/
#define TIM3_BASE_ADDRESS ((uint32_t)0x40000400)
// Timer 3 control register 1
#define TIM3_CR1_REGISTER_1 (TIM3_BASE_ADDRESS + 0x00)
//flags for CR1 register:
#define COUNTER_ENABLE_BIT (uint16_t)0x01
// Timer 3 status register
#define TIM3_STATUS_REGISTER (TIM3_BASE_ADDRESS + 0x10)
//flags for Status register:
#define TIM_UIF 0x01 //timer 3 overflow flag
#define TIM_CH1_CC1IF 0x2 //timer channel 1 capture/compare event
#define TIM_CH3_CC3IF 0x8 //timer channel 3 capture/compare event
//timer 3 interrupt enable register
#define TIM3_INTERRUPT_ENABLE_REGISTER (TIM3_BASE_ADDRESS + 0x0C)
//flags for interrupt enable register:
#define TIM_CH3_CC_INTERRUPT_ENABLE 0x8 //timer channel 3 capture/compare interrupt
#define TIM_CH1_CC_INTERRUPT_ENABLE 0x2 //timer channel 1 capture/compare interrupt
#define TIM_UPDATE_INTERRUPT_ENABLE 0x1 //timer overflow or event interrupt
//Capture compare enable register
#define TIM3_CAPTURE_COMPARE_ENABLE_REGISTER (TIM3_BASE_ADDRESS + 0x20)
//flags for TIM3_CCER registers for output:
#define TIM3_CCER_CC3E (0x0100)
#define TIM3_CCER_CC1E (0b1)
//Capture compare mode registers
#define TIM3_CAPTURE_COMPARE_MODE_1_REGISTER (TIM3_BASE_ADDRESS + 0x18)
#define TIM3_CAPTURE_COMPARE_MODE_2_REGISTER (TIM3_BASE_ADDRESS + 0x1C)
//flags for Capture compare mode register
#define TIM_CCMR13_OC3M_0 (0b00010000)
#define TIM_CCMR13_OC3M_1 (0b00100000)
#define TIM_CCMR13_OC3M_2 (0b01000000)
#define TIM_CCMR13_OCPE (0b00001000) // enable preload register channels 1 and 3
#define TIM_CCMR13_OUTPUT (0x00)
#define TIM_CCMR1_CC1S_INPUT_TI1 (0b01)
// Compare, autoreload and Prescaler registers
#define TIM3_COMPARE_1_REGISTER (TIM3_BASE_ADDRESS + 0x34)
#define TIM3_COMPARE_2_REGISTER (TIM3_BASE_ADDRESS + 0x38)
#define TIM3_COMPARE_3_REGISTER (TIM3_BASE_ADDRESS + 0x3C)
#define TIM3_COMPARE_4_REGISTER (TIM3_BASE_ADDRESS + 0x40)
#define TIM3_PRESCALER_REGISTER (TIM3_BASE_ADDRESS + 0x28)
#define TIM3_AUTORELOAD_REGISTER (TIM3_BASE_ADDRESS + 0X2C)
// Timer counter value
#define TIM3_CNT (TIM3_BASE_ADDRESS + 0x24)

/* MACRO definitions----------------------------------------------------------*/
#define SYSTEM_CONTROL_BASE_ADDRESS (0xE000E000)
#define NVIC_BASE_ADDRESS (SYSTEM_CONTROL_BASE_ADDRESS + 0x100)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_0_31 (NVIC_BASE_ADDRESS)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_32_63 (NVIC_BASE_ADDRESS+0x4)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_64_95 (NVIC_BASE_ADDRESS+0x8)
#define TIM3_INTERRUPT_BIT (0x20000000)

/* definitions */
#define CCMR_OC3M_PWM_MODE1 (0b01100000)
#define CCMR_CC3s_OUTPUT (0b00000000)


void initTimer3Channel3( void )
{
    uint16_t * reg_pointer_16;
    uint16_t prescalervalue2, autoreloadvalue;
    /* Timer 3 APB clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    /* Compute Prescale and Autorreload */
    // if we want LED freq to be 1Hz
    // 1Hz = 90MHz / ((prescalervalue+1)*(autoreloadvalue))
    // Frequency of system clock is 90 MHz
    prescalervalue2 = 8999;
    autoreloadvalue = 10000;
    /* Clear any pending flags in the status register */
    reg_pointer_16 = (uint16_t *)TIM3_STATUS_REGISTER;
    *reg_pointer_16 = 0;
    /* Set Prescale and Autorreload */
    reg_pointer_16 = (uint16_t *)TIM3_PRESCALER_REGISTER;
    *reg_pointer_16 = prescalervalue2;
    reg_pointer_16 = (uint16_t *)TIM3_AUTORELOAD_REGISTER;
    *reg_pointer_16 = autoreloadvalue;
    /* Capture Compare Register 2 OC3M to toggle mode */
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_MODE_2_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM_CCMR13_OC3M_1 | TIM_CCMR13_OC3M_0 | TIM_CCMR13_OUTPUT;
    /* Set Compare Value */
    reg_pointer_16 = (uint16_t *)TIM3_COMPARE_3_REGISTER;
    *reg_pointer_16 = autoreloadvalue/2; // duty cycle
    /* Enable Preload Register (Don’t HAVE to, but good practice) */
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_MODE_2_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM_CCMR13_OCPE;
    /*enable the TIM3 channel 3 counter and keep the default configuration for channel polarity*/
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_ENABLE_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM3_CCER_CC3E;
    /*enable timer 3*/
    reg_pointer_16 = (uint16_t *)TIM3_CR1_REGISTER_1;
    *reg_pointer_16 = *reg_pointer_16 | COUNTER_ENABLE_BIT;
}

//uint32_t ValueTimer3Channel3 ( void )
//{
//    uint32_t valueT3C3;
//    uint32_t * reg_pointer;
//    reg_pointer = (uint32_t*)TIM3_CAPTURE_COMPARE_MODE_2_REGISTER;
//    valueT3C3 = *reg_pointer;
//    return valueT3C3;
//}

void initTimer3Channel1ToInputCapture( void )
{
    uint16_t * reg_pointer_16;
    uint16_t prescalervalue2, autoreloadvalue;
    /* Timer 3 APB clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    /* Compute Prescale and Autorreload */
    // if we want LED freq to be 1Hz
    // 1Hz = 90MHz / ((prescalervalue+1)*(autoreloadvalue))
    // Frequency of system clock is 90 MHz
    prescalervalue2 = 8999;
    autoreloadvalue = 40000;
    /* Clear any pending flags in the status register */
    reg_pointer_16 = (uint16_t *)TIM3_STATUS_REGISTER;
    *reg_pointer_16 = 0;
    /* Set Prescale and Autorreload */
    reg_pointer_16 = (uint16_t *)TIM3_PRESCALER_REGISTER;
    *reg_pointer_16 = prescalervalue2;
    reg_pointer_16 = (uint16_t *)TIM3_AUTORELOAD_REGISTER;
    *reg_pointer_16 = autoreloadvalue;
    /* Capture Compare Register 1 CC1S to input mode, IC1 mapped to TI1 */
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_MODE_1_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM_CCMR1_CC1S_INPUT_TI1;
    /* Enable Preload Register (Don’t HAVE to, but good practice) */
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_MODE_2_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM_CCMR13_OCPE;
    /*enable the TIM3 channel 1 counter and keep the default configuration for channel polarity*/
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_ENABLE_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM3_CCER_CC1E;
    /*enable timer 3*/
    reg_pointer_16 = (uint16_t *)TIM3_CR1_REGISTER_1;
    *reg_pointer_16 = *reg_pointer_16 | COUNTER_ENABLE_BIT;
}

uint16_t ReadInputCaputreFlagNTakeAction( void )
{
    uint16_t TIM3_Value = 0;
    uint16_t *reg_pointer_16_sr;
    uint16_t *reg_pointer_32;
    // read TIM3_SR value & clear by reading
    reg_pointer_16_sr = (uint16_t *)TIM3_STATUS_REGISTER;
    //TIM3SR_Value = *reg_pointer_16;
    // check if the flag is triggered
    if ((*reg_pointer_16_sr & TIM_CH1_CC1IF) == TIM_CH1_CC1IF)
    {
        // get timer value form cap/com register ch1
        reg_pointer_32 = (uint16_t *)TIM3_COMPARE_1_REGISTER;
        TIM3_Value = *reg_pointer_32;
    }
    return TIM3_Value;
}

void ReadInputCaputreFlagNReadInput( void )
{
    uint16_t TIM3_Value = 0;
    uint16_t *reg_pointer_16_sr;
    uint16_t *reg_pointer_32;
    // read TIM3_SR value & clear by reading
    reg_pointer_16_sr = (uint16_t *)TIM3_STATUS_REGISTER;
    //TIM3SR_Value = *reg_pointer_16;
    // check if the flag is triggered
    if ((*reg_pointer_16_sr & TIM_CH1_CC1IF) == TIM_CH1_CC1IF)
    {
        // checkGPIOB13
        // checkGPIOB12
        // checkGPIOC7
    }
}