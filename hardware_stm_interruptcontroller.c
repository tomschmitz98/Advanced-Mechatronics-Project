#include "debug_mort.h"
#include "hardware_stm_gpio.h"
#include "stm32f4xx_rcc_mort.h"
#include "debug_mort.h"
#include "hardware_stm_adc.h"
#include "hardware_stm_dma.h"

/* MACRO definitions----------------------------------------------------------*/
#define TIM3_BASE_ADDRESS ((uint32_t)0x40000400)
// Timer 3 control register 1
#define TIM3_CR1_REGISTER_1 (TIM3_BASE_ADDRESS + 0x00)
#define TIM3_CR2_REGISTER_1 (TIM3_BASE_ADDRESS + 0x04)
//flags for CR1 register:
#define COUNTER_ENABLE_BIT (uint16_t)0x01
// Timer 3 status register
#define TIM3_STATUS_REGISTER (TIM3_BASE_ADDRESS + 0x10)
//flags for Status register:
#define TIM_UIF 0x01 //timer 3 overflow flag
#define TIM_CH1_CC1IF 0x02 //timer channel 1 capture/compare event
#define TIM_CH3_CC3IF 0x8 //timer channel 3 capture/compare event
//timer 3 interrupt enable register
# define TIM3_INTERRUPT_ENABLE_REGISTER (TIM3_BASE_ADDRESS + 0x0C)
//flags for interrupt enable register:
#define TIM_CH3_CC_INTERRUPT_ENABLE 0x8 //timer channel 3 capture/compare interrupt
#define TIM_UPDATE_INTERRUPT_ENABLE 0x1 //timer overflow or event interrupt
//Capture compare enable register
#define TIM3_CAPTURE_COMPARE_ENABLE_REGISTER (TIM3_BASE_ADDRESS + 0x20)
//flags for TIM3_CCER registers for output:
#define TIM3_CCER_CC3E (0x0100)
#define TIM3_CCER_CC2E (0x10)
//Capture compare mode registers
#define TIM3_CAPTURE_COMPARE_MODE_1_REGISTER (TIM3_BASE_ADDRESS + 0x18)
#define TIM3_CAPTURE_COMPARE_MODE_2_REGISTER (TIM3_BASE_ADDRESS + 0x1C)
//flags for Capture compare mode register
#define TIM_CCMR13_OCPE (0b00001000) // enable preload register channels 1 and 3
#define TIM_CCMR1_OC2PE (0x800)
// Compare, autoreload and Prescaler registers
#define TIM3_COMPARE_1_REGISTER (TIM3_BASE_ADDRESS + 0x34)
#define TIM3_COMPARE_2_REGISTER (TIM3_BASE_ADDRESS + 0x38)
#define TIM3_COMPARE_3_REGISTER (TIM3_BASE_ADDRESS + 0x3C)
#define TIM3_COMPARE_4_REGISTER (TIM3_BASE_ADDRESS + 0x40)
#define TIM3_PRESCALER_REGISTER (TIM3_BASE_ADDRESS + 0x28)
#define TIM3_AUTORELOAD_REGISTER (TIM3_BASE_ADDRESS + 0X2C)

/* MACRO definitions----------------------------------------------------------*/
#define SYSTEM_CONTROL_BASE_ADDRESS (0xE000E000)
#define NVIC_BASE_ADDRESS (SYSTEM_CONTROL_BASE_ADDRESS + 0x100)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_0_31 (NVIC_BASE_ADDRESS)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_32_63 (NVIC_BASE_ADDRESS+0x4)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_64_95 (NVIC_BASE_ADDRESS+0x8)
#define TIM3_INTERRUPT_BIT (0x20000000)

/* definitions */
#define CCMR_OC3M_PWM_MODE1 (0b1100000)
#define CCMR_OC2M_PWM_MODE1 (0x6000)
#define TIM_CCMR13_OC3M_0 (0b00010000)
#define TIM_CCMR13_OC3M_1 (0b00100000)
#define TIM_CCMR13_OC3M_2 (0b01000000)
#define TIM_CCMR13_OUTPUT (0x00)
#define CCMR_CC3s_OUTPUT (0b00000000)
#define CCMR_CC2s_OUTPUT (0b0000000000)

/* MACRO definitions----------------------------------------------------------*/
#define SYSTEM_CONTROL_BASE_ADDRESS (0xE000E000)
#define NVIC_BASE_ADDRESS (SYSTEM_CONTROL_BASE_ADDRESS + 0x100)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_0_31 (NVIC_BASE_ADDRESS)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_32_63 (NVIC_BASE_ADDRESS+0x4)
#define NVIC_INTERRUPT_SET_ENABLE_REGISTER_64_95 (NVIC_BASE_ADDRESS+0x8)
#define NVIC_INTERRUPT_CLEAR_ENABLE_REGISTER_0_31 (NVIC_BASE_ADDRESS + 0x80)
#define NVIC_INTERRUPT_CLEAR_ENABLE_REGISTER_32_63 (NVIC_INTERRUPT_CLEAR_ENABLE_REGISTER_0_31 + 0x4)
#define NVIC_INTERRUPT_CLEAR_ENABLE_REGISTER_64_95 (NVIC_INTERRUPT_CLEAR_ENABLE_REGISTER_0_31 + 0x8)
#define NVIC_INTERRUPT_SET_PENDING_REGISTER_0_31 (NVIC_BASE_ADDRESS + 0x100)
#define NVIC_INTERRUPT_SET_PENDING_REGISTER_32_63 (NVIC_INTERRUPT_SET_PENDING_REGISTER_0_31 + 0x4)
#define NVIC_INTERRUPT_SET_PENDING_REGISTER_64_95 (NVIC_INTERRUPT_SET_PENDING_REGISTER_0_31 + 0x8)
#define NVIC_INTERRUPT_CLEAR_PENDING_REGISTER_0_31 (NVIC_BASE_ADDRESS + 0x180)
#define NVIC_INTERRUPT_CLEAR_PENDING_REGISTER_32_63 (NVIC_INTERRUPT_CLEAR_PENDING_REGISTER_0_31 + 0x4)
#define NVIC_INTERRUPT_CLEAR_PENDING_REGISTER_64_95 (NVIC_INTERRUPT_CLEAR_PENDING_REGISTER_0_31 + 0x8)
#define TIM3_INTERRUPT_BIT (0x20000000)
#define EXTI9_5_INTERRUPT_BIT (0x800000)
//For external interrupts:
#define SYSCFG_BASE_ADDRESS ((uint32_t)(0x40013800))
#define SYSCFG_EXTERNAL_INTERRUPT_REGISTER_2 (SYSCFG_BASE_ADDRESS + 0x0C)
#define SYSCFG_EXTERNAL_INTERRUPT_6_BITS ((uint32_t)0xF00) //flags for External interrupt register 2
#define SYSCFG_EXTERNAL_INTERRUPT_6_PORTC ((uint32_t)0x200)
//External interrupt controller :
#define EXTERNAL_INTERRUPT_CONTROLLER_BASE_ADDRESS ((uint32_t)(0x40013C00))
#define EXTERNAL_INTERRUPT_CONTROLLER_MASK_REGISTER (EXTERNAL_INTERRUPT_CONTROLLER_BASE_ADDRESS)
#define EXTERNAL_INTERRUPT_CONTROLLER_MASK_REGISTER_EXTI6 ((uint32_t)0x40) //flags for external interrupt controller mask register
#define EXTERNAL_INTERRUPT_CONTROLLER_RTSR (EXTERNAL_INTERRUPT_CONTROLLER_BASE_ADDRESS+0x08)
#define EXTERNAL_INTERRUPT_CONTROLLER_RTSR_EXTI6 ((uint32_t)0x40)
#define EXTERNAL_INTERRUPT_CONTROLLER_FTSR (EXTERNAL_INTERRUPT_CONTROLLER_BASE_ADDRESS+0x0C)
#define EXTERNAL_INTERRUPT_CONTROLLER_FTSR_EXTI6 ((uint32_t)0x40)
#define EXTERNAL_INTERRUPT_CONTROLLER_PENDING_REGISTER (EXTERNAL_INTERRUPT_CONTROLLER_BASE_ADDRESS+0x14)
#define EXTERNAL_INTERRUPT_CONTROLLER_PENDING_EXTI6 ((uint32_t)0x40)

// global variable
uint16_t ButtonClick = 0;
uint16_t maxDMA = 4100;

void enableNVIC_Timer3(void)
{
    uint32_t * reg_pointer_32;
    reg_pointer_32 = (uint32_t *)NVIC_INTERRUPT_SET_ENABLE_REGISTER_0_31;
    *reg_pointer_32 = TIM3_INTERRUPT_BIT;
}

void initTimer3ToPWMMode1( void )
{
    uint16_t * reg_pointer_16;
    uint16_t prescalervalue2, autoreloadvalue;
    /* Timer 3 APB clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    /*enable the interrupt that would go to timer 3*/
    enableNVIC_Timer3();
    /* Compute Prescale and Autorreload */
    // 30kHz = 90MHz / ((prescalervalue+1)*(autoreloadvalue))
    prescalervalue2 = 99; //Frequency of clock is 90 MHz
    autoreloadvalue = 30;
    /* Clear any pending flags in the status register */
    reg_pointer_16 = (uint16_t *)TIM3_STATUS_REGISTER;
    *reg_pointer_16 = 0;
    /* Set Prescale and Autorreload */
    reg_pointer_16 = (uint16_t *)TIM3_PRESCALER_REGISTER;
    *reg_pointer_16 = prescalervalue2;
    reg_pointer_16 = (uint16_t *)TIM3_AUTORELOAD_REGISTER;
    *reg_pointer_16 = autoreloadvalue;
    /* Capture Compare Register 2 OC3M to PWM Mode 1 */
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_MODE_1_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | CCMR_OC2M_PWM_MODE1;
    /* Capture Compare Register 2 CC3S to OUTPUT */
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_MODE_2_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | CCMR_CC2s_OUTPUT;
    /* Set Compare Value input duty cycle*/
    reg_pointer_16 = (uint16_t *)TIM3_COMPARE_2_REGISTER;
    *reg_pointer_16 = autoreloadvalue/(maxDMA / (returnADC3StoredValueforone()+1));
    /* Enable Preload Register (Don’t HAVE to, but good practice) */
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_MODE_1_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM_CCMR1_OC1PE;
    /* enable the TIM3 channel 3 counter and keep the default configuration for channel polarity */
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_ENABLE_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM3_CCER_CC2E;
    /* enable interrupt on capture compare channel 3 */
    //reg_pointer_16 = (uint16_t *)TIM3_INTERRUPT_ENABLE_REGISTER;
    //*reg_pointer_16 = (TIM_CH3_CC_INTERRUPT_ENABLE | TIM_UPDATE_INTERRUPT_ENABLE);
    /* enable timer 3 */
    reg_pointer_16 = (uint16_t *)TIM3_CR1_REGISTER_1;
    *reg_pointer_16 = *reg_pointer_16 | COUNTER_ENABLE_BIT;
}


void initTimer3ToInterruptNOutputCompCh3( void )
{
    uint16_t * reg_pointer_16;
    uint16_t prescalervalue2, autoreloadvalue;
    /* Timer 3 APB clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    /*enable the interrupt that would go to timer 3*/
    enableNVIC_Timer3();
    /* Compute Prescale and Autorreload */
    // LED to 0.25Hz
    // 1Hz = 90MHz / ((prescalervalue+1)*(autoreloadvalue))
    prescalervalue2 = 8999; //Frequency of clock is 90 MHz
    autoreloadvalue = 40000;
    /* Clear any pending flags in the status register */
    reg_pointer_16 = (uint16_t *)TIM3_STATUS_REGISTER;
    *reg_pointer_16 = 0;
    /* Set Prescale and Autorreload */
    reg_pointer_16 = (uint16_t *)TIM3_PRESCALER_REGISTER;
    *reg_pointer_16 = prescalervalue2;
    reg_pointer_16 = (uint16_t *)TIM3_AUTORELOAD_REGISTER;
    *reg_pointer_16 = autoreloadvalue;
    /* Set Compare Value */
    reg_pointer_16 = (uint16_t *)TIM3_COMPARE_3_REGISTER;
    *reg_pointer_16 = autoreloadvalue/4;
    /* Enable Preload Register (Don’t HAVE to, but good practice) */
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_MODE_2_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM_CCMR13_OCPE;
    /*enable the TIM3 channel 3 OC3M to active level on match mode for output compare mode*/
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_MODE_2_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM_CCMR13_OUTPUT;
    /*enable the TIM3 channel 3 counter and keep the default configuration for channel polarity*/
    reg_pointer_16 = (uint16_t *)TIM3_CAPTURE_COMPARE_ENABLE_REGISTER;
    *reg_pointer_16 = *reg_pointer_16 | TIM3_CCER_CC3E;
    /*enable interrupt on capture compare channel 3*/
    reg_pointer_16 = (uint16_t *)TIM3_INTERRUPT_ENABLE_REGISTER;
    *reg_pointer_16 = (TIM_CH3_CC_INTERRUPT_ENABLE | TIM_UPDATE_INTERRUPT_ENABLE);
    /*enable timer 3*/
    reg_pointer_16 = (uint16_t *)TIM3_CR1_REGISTER_1;
    *reg_pointer_16 = *reg_pointer_16 | COUNTER_ENABLE_BIT;
}

void TIM3_IRQHandler( void )
{
    uint16_t *reg_pointer_16_sr;
    uint16_t *reg_pointer_16_dier;
    reg_pointer_16_sr = (uint16_t*)TIM3_STATUS_REGISTER;
    reg_pointer_16_dier = (uint16_t*)TIM3_INTERRUPT_ENABLE_REGISTER;
    // check which interrupts fired and if they were supposed to fire, then clear the flags
    // sp they don't keep firing, then perform actions according to these interrupts

    //chcek if Output Compare 3 triggered the interrupt:
    if (((*reg_pointer_16_sr & TIM_CH3_CC3IF) == TIM_CH3_CC3IF) && ((*reg_pointer_16_dier & TIM_CH3_CC_INTERRUPT_ENABLE) == TIM_CH3_CC_INTERRUPT_ENABLE))
    {
        //clear interrupt
        *reg_pointer_16_sr = ~((uint16_t)TIM_CH3_CC3IF);
        //perform action
        clearGPIOB0();
    }
    // check if Overflow triggered the interrupt: i.e. Timer Counter 3 >= Autoreload value
    if (((*reg_pointer_16_sr & TIM_UIF) == TIM_UIF) && ((*reg_pointer_16_dier & TIM_UPDATE_INTERRUPT_ENABLE) == TIM_UPDATE_INTERRUPT_ENABLE))
    {
        //clear interrupt
        *reg_pointer_16_sr = ~((uint16_t)TIM_UIF);
        //perform action
        setGPIOB0();
    }
}

void enableEXTI6OnPortC(void)
{
    uint32_t * reg_pointer_32;
    /*Init GPIO 6 C as input*/
    initGpioC6AsInput();
    /*As a test, Init GPIO B0 as output for debugging*/
    initGpioB0AsOutput();
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /*map EXTI6 to port C bit 6*/
    reg_pointer_32 = (uint32_t *)SYSCFG_EXTERNAL_INTERRUPT_REGISTER_2;
    //clear EXTI6
    *reg_pointer_32 = *reg_pointer_32 & ~SYSCFG_EXTERNAL_INTERRUPT_6_BITS;
    //set EXTI6 to Port C
    *reg_pointer_32 = *reg_pointer_32 | SYSCFG_EXTERNAL_INTERRUPT_6_PORTC;
    /*un-mask EXTI6*/
    reg_pointer_32 = (uint32_t *)EXTERNAL_INTERRUPT_CONTROLLER_MASK_REGISTER;
    *reg_pointer_32 = *reg_pointer_32 | EXTERNAL_INTERRUPT_CONTROLLER_MASK_REGISTER_EXTI6;
    /*trigger on rising edge*/
    reg_pointer_32 = (uint32_t *)EXTERNAL_INTERRUPT_CONTROLLER_RTSR;
    *reg_pointer_32 = *reg_pointer_32 | EXTERNAL_INTERRUPT_CONTROLLER_RTSR_EXTI6;
    /* set the NVIC to respond to EXTI9_5*/
    reg_pointer_32 = (uint32_t *)NVIC_INTERRUPT_SET_ENABLE_REGISTER_0_31;
    *reg_pointer_32 = EXTI9_5_INTERRUPT_BIT;
}

void EXTI9_5_IRQHandler(void)
{
    uint32_t *reg_pointer_32;
    reg_pointer_32 = (uint32_t *)EXTERNAL_INTERRUPT_CONTROLLER_PENDING_REGISTER;
    uint32_t value = 0;
    uint16_t increment = 1;
    uint16_t ReadValQa, ReadValQb, ReadValQd = 0;
    value = *reg_pointer_32;
    //check which interrupt fired:
    if ((*reg_pointer_32 & EXTERNAL_INTERRUPT_CONTROLLER_PENDING_EXTI6) > 0)
    {
        //clear the interrupt:
        *reg_pointer_32 = EXTERNAL_INTERRUPT_CONTROLLER_PENDING_EXTI6;
        // increment button click
        ButtonClick++;
        debugprint(ButtonClick);
        // Output high if Button is clicked 
        if ((ButtonClick % 5 == 0) && (ButtonClick > 1))
        {
            // Set GPIOB5 High
            setGPIOB5();
            // Read the shift register values
            ReadValQa = checkGPIOB13();
            ReadValQb = checkGPIOB12();
            ReadValQd = checkGPIOC7();
            // Decide shich colored bear it is
            if ((ReadValQa == 8192) && (ReadValQb == 0) && (ReadValQd == 0))
            {
                debugprintRed();
            }
            else if ((ReadValQa == 8192) && (ReadValQb == 0) && (ReadValQd == 128))
            {
                debugprintOrange();
            }
            else if ((ReadValQa == 8192) && (ReadValQb == 4096) && (ReadValQd == 128))
            {
                debugprintWhite();
            }
            else if ((ReadValQa == 0) && (ReadValQb == 0) && (ReadValQd == 128))
            {
                debugprintGreen();
            }
            else
            {
                debugprintHelloWorld();
            }
        }
        else // set GPIOB5 low
        {
            clearGPIOB5();
        }
    }
}