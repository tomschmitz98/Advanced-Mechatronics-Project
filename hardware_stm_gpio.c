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
#include "stm32f4xx_rcc_mort.h"


//led 1 is connected to PB0. 
// GPIO B addresses: 0x4002 0400 - 0x4002 07FF
// GPIO C addresses: 0x4002 0800 - 0x4002 0BFF


/* MACRO definitions----------------------------------------------------------*/
//Port B addresses:
#define PORTB_BASE_ADDRESS ((uint32_t)0x40020400)        //The first address in memory corresponding to Port B (this is in the user manual!)
// I gave you the first one, now you fill in the rest, check in the user manual what is the offset from the base address for each register!
#define PORTB_MODER_REGISTER (PORTB_BASE_ADDRESS + 0x00) //replace the question mark with the correct offset!
#define PORTB_OTYPER_REGISTER (PORTB_BASE_ADDRESS + 0x04)
#define PORTB_OSPEEDR_REGISTER (PORTB_BASE_ADDRESS + 0x08)
#define PORTB_PUPDR_REGISTER (PORTB_BASE_ADDRESS + 0x0C)
#define PORTB_IDR_REGISTER (PORTB_BASE_ADDRESS + 0x10)
#define PORTB_ODR_REGISTER (PORTB_BASE_ADDRESS + 0x14)
#define PORTB_BSRRL_REGISTER (PORTB_BASE_ADDRESS + 0x18)
#define PORTB_BSRR_REGISTER (PORTB_BASE_ADDRESS + 0x18)
#define PORTB_BSRRH_REGISTER (PORTB_BASE_ADDRESS + 0x1A)
#define PORTB_LCKR_REGISTER (PORTB_BASE_ADDRESS + 0x1C)
#define PORTB_AFR1_REGISTER (PORTB_BASE_ADDRESS + 0x20)
#define PORTB_AFR2_REGISTER (PORTB_BASE_ADDRESS + 0x24)
//#define PORTB_OSPEEDR_REGISTER (PORTB_BASE_ADDRESS + 0x08)

//Port C addresses:
#define PORTC_BASE_ADDRESS ((uint32_t)0x40020800)
#define PORTC_MODER_REGISTER (PORTC_BASE_ADDRESS + 0x00)
#define PORTC_OTYPER_REGISTER (PORTC_BASE_ADDRESS + 0x04)
#define PORTC_OSPEEDR_REGISTER (PORTC_BASE_ADDRESS + 0x08)
#define PORTC_PUPDR_REGISTER (PORTC_BASE_ADDRESS + 0x0C)
#define PORTC_IDR_REGISTER (PORTC_BASE_ADDRESS + 0x10)
#define PORTC_ODR_REGISTER (PORTC_BASE_ADDRESS + 0x14)
//#define PORTC_BSRRL_REGISTER (PORTB_BASE_ADDRESS + 0x18)
#define PORTC_BSRR_REGISTER (PORTC_BASE_ADDRESS + 0x18)
//#define PORTC_BSRRH_REGISTER (PORTB_BASE_ADDRESS + 0x1A)
#define PORTC_LCKR_REGISTER (PORTC_BASE_ADDRESS + 0x1C)
#define PORTC_AFRL_REGISTER (PORTC_BASE_ADDRESS + 0x20)
#define PORTC_AFRH_REGISTER (PORTC_BASE_ADDRESS + 0x24)
//#define PORTC_OSPEEDR_REGISTER (PORTB_BASE_ADDRESS + 0x08)

//Port F addresses:
#define PORTF_BASE_ADDRESS ((uint32_t)0x40021400)
#define PORTF_MODER_REGISTER (PORTF_BASE_ADDRESS + 0x00)
#define PORTF_OTYPER_REGISTER (PORTF_BASE_ADDRESS + 0x04)
#define PORTF_OSPEEDR_REGISTER (PORTF_BASE_ADDRESS + 0x08)
#define PORTF_PUPDR_REGISTER (PORTF_BASE_ADDRESS + 0x0C)
#define PORTF_IDR_REGISTER (PORTF_BASE_ADDRESS + 0x10)
#define PORTF_ODR_REGISTER (PORTF_BASE_ADDRESS + 0x14)
#define PORTF_BSRR_REGISTER (PORTF_BASE_ADDRESS + 0x18)
#define PORTF_LCKR_REGISTER (PORTF_BASE_ADDRESS + 0x1C)
#define PORTF_AFRL_REGISTER (PORTF_BASE_ADDRESS + 0x20)
#define PORTF_AFRH_REGISTER (PORTF_BASE_ADDRESS + 0x24)

//flags MODER Register:
#define PIN_0_INP 0b00
#define PIN_0_OUTP 0b01
#define PIN_0_ALTFUN 0b10
#define PIN_0_ANALOG 0b11

#define PIN_1_INP (uint32_t)(PIN_0_INP<<2)
#define PIN_1_OUTP (uint32_t)(PIN_0_OUTP<<2)
#define PIN_1_ALTFUN (uint32_t)(PIN_0_ALTFUN<<2)
#define PIN_1_ANALOG (uint32_t)(PIN_0_ANALOG<<2)

#define PIN_5_INP (uint32_t)(PIN_0_INP<<10)
#define PIN_5_OUTP (uint32_t)(PIN_0_OUTP<<10)
#define PIN_5_ALTFUN (uint32_t)(PIN_0_ALTFUN<<10)
#define PIN_5_ANALOG (uint32_t)(PIN_0_ANALOG<<10)

#define PIN_6_INP 0x0000
#define PIN_6_OUTP 0x1000
#define PIN_6_ALTFUN 0x2000
#define PIN_6_ANALOG 0x3000

#define PIN_7_INP (uint32_t)(PIN_0_INP<<14)
#define PIN_7_OUTP (uint32_t)(PIN_0_OUTP<<14)
#define PIN_7_ALTFUN (uint32_t)(PIN_0_ALTFUN<<14)
#define PIN_7_ANALOG (uint32_t)(PIN_0_ANALOG<<14)

#define PIN_8_INP (uint32_t)(PIN_0_INP<<16)
#define PIN_8_OUTP (uint32_t)(PIN_0_OUTP<<16)
#define PIN_8_ALTFUN (uint32_t)(PIN_0_ALTFUN<<16)
#define PIN_8_ANALOG (uint32_t)(PIN_0_ANALOG<<16)

#define PIN_9_INP (uint32_t)(PIN_0_INP<<18)
#define PIN_9_OUTP (uint32_t)(PIN_0_OUTP<<18)
#define PIN_9_ALTFUN (uint32_t)(PIN_0_ALTFUN<<18)
#define PIN_9_ANALOG (uint32_t)(PIN_0_ANALOG<<18)

#define PIN_12_INP (uint32_t)(PIN_0_INP<<24)
#define PIN_12_OUTP (uint32_t)(PIN_0_OUTP<<24)
#define PIN_12_ALTFUN (uint32_t)(PIN_0_ALTFUN<<24)
#define PIN_12_ANALOG (uint32_t)(PIN_0_ANALOG<<24)

#define PIN_13_INP (uint32_t)(PIN_0_INP<<26)
#define PIN_13_OUTP (uint32_t)(PIN_0_OUTP<<26)
#define PIN_13_ALTFUN (uint32_t)(PIN_0_ALTFUN<<26)
#define PIN_13_ANALOG (uint32_t)(PIN_0_ANALOG<<26)

//flags OTYPER Register:
#define PIN_0_OUTP_PUSHPULL 0b0
#define PIN_0_OUTP_OPENDRAIN 0b1

#define PIN_5_OUTP_PUSHPULL (uint32_t)(0b0<<5)
#define PIN_5_OUTP_OPENDRAIN (uint32_t)(0b1<<5)

#define PIN_6_OUTP_PUSHPULL 0x00
#define PIN_6_OUTP_OPENDRAIN (uint32_t)(0b1<<6)

#define PIN_7_OUTP_PUSHPULL (uint32_t)(0b0<<7)
#define PIN_7_OUTP_OPENDRAIN (uint32_t)(0b1<<7)

#define PIN_8_OUTP_PUSHPULL (uint32_t)(0b0<<8)
#define PIN_8_OUTP_OPENDRAIN (uint32_t)(0b1<<8)

#define PIN_9_OUTP_PUSHPULL (uint32_t)(0b0<<9)
#define PIN_9_OUTP_OPENDRAIN (uint32_t)(0b1<<9)

#define PIN_12_OUTP_PUSHPULL (uint32_t)(0b0<<12)
#define PIN_12_OUTP_OPENDRAIN (uint32_t)(0b1<<12)

#define PIN_13_OUTP_PUSHPULL (uint32_t)(0b0<<13)
#define PIN_13_OUTP_OPENDRAIN (uint32_t)(0b1<<13)

//flags OSPEEDR Register:
#define PIN_0_HIGHSPEED 0b11

#define PIN_5_HIGHSPEED (uint32_t)(PIN_0_HIGHSPEED << 10)

#define PIN_6_HIGHSPEED 0x3000

#define PIN_7_HIGHSPEED (uint32_t)(PIN_0_HIGHSPEED << 14)

#define PIN_12_HIGHSPEED (uint32_t)(PIN_0_HIGHSPEED << 24)

#define PIN_13_HIGHSPEED (uint32_t)(PIN_0_HIGHSPEED << 26)

//flags PUPDR Register:
#define PIN_0_NO 0b00
#define PIN_0_PULLUP 0b01
#define PIN_0_PULLDOWN 0b10

#define PIN_5_NO (uint32_t)(0b00<<10)
#define PIN_5_PULLUP (uint32_t)(0b01 << 10)
#define PIN_5_PULLDOWN (uint32_t)(0b10 << 10)

#define PIN_6_NO (uint32_t)(0b00<<12)
#define PIN_6_PULLUP (uint32_t)(0b01 << 12)
#define PIN_6_PULLDOWN 0x2000

#define PIN_7_NO (uint32_t)(0b00<<14)
#define PIN_7_PULLUP (uint32_t)(0b01 << 14)
#define PIN_7_PULLDOWN (uint32_t)(0b10 << 14)

#define PIN_8_NO (uint32_t)(0b00<<16)
#define PIN_8_PULLUP (uint32_t)(0b01 << 16)
#define PIN_8_PULLDOWN (uint32_t)(0b10 << 16)

#define PIN_9_NO (uint32_t)(0b00<<18)
#define PIN_9_PULLUP (uint32_t)(0b01 << 18)
#define PIN_9_PULLDOWN (uint32_t)(0b10 << 18)

#define PIN_12_NO (uint32_t)(0b00<<24)
#define PIN_12_PULLUP (uint32_t)(0b01 << 24)
#define PIN_12_PULLDOWN (uint32_t)(0b10 << 24)

#define PIN_13_NO (uint32_t)(0b00<<26)
#define PIN_13_PULLUP (uint32_t)(0b01 << 26)
#define PIN_13_PULLDOWN (uint32_t)(0b10 << 26)

//input data register:
#define PIN_0_IDR_HIGH 0b1
#define PIN_0_IDR_LOW 0b0

#define PIN_6_IDR_HIGH 0x40
#define PIN_6_IDR_LOW (uint32_t)(0b0 << 6)

#define PIN_7_IDR_HIGH (uint32_t)(0b1 << 7)
#define PIN_7_IDR_LOW (uint32_t)(0b0 << 7)

#define PIN_12_IDR_HIGH (uint32_t)(0b1 << 12)
#define PIN_12_IDR_LOW (uint32_t)(0b0 << 12)

#define PIN_13_IDR_HIGH (uint32_t)(0b1 << 13)
#define PIN_13_IDR_LOW (uint32_t)(0b0 << 13)

//flags AFR1 Register:
//#define AFRL1_AFX = 0b(AFX) << 4
#define AFRL0_CLEAR 0b1111
#define AFRL0_AF2 0b0010

#define AFRL5_CLEAR (uint32_t)(AFRL0_CLEAR << 20)
#define AFRL5_AF2 (uint32_t)(AFRL0_AF2 << 20)

#define AFRL6_CLEAR (uint32_t)(0b1111<< 24)
//#define AFRL6_AF2 AFRL0_AF2 << 24
#define AFRL6_AF2 0x2000000

//flags ODR Register:
#define PIN_0_ODR_HIGH 0b1
#define PIN_0_ODR_LOW 0b0

#define PIN_5_ODR_HIGH (uint32_t)(0b1 << 5)
#define PIN_5_ODR_LOW (uint32_t)(0b0 << 5)

#define PIN_6_ODR_HIGH (uint32_t)(0b1 << 6)
#define PIN_6_ODR_LOW (uint32_t)(0b0 << 6)

#define PIN_12_ODR_HIGH (uint32_t)(0b1 << 12)
#define PIN_12_ODR_LOW (uint32_t)(0b0 << 12)

#define PIN_13_ODR_HIGH (uint32_t)(0b1 << 13)
#define PIN_13_ODR_LOW (uint32_t)(0b0 << 13)

//pin clear
#define PIN_0_2B_CLEAR 0b11
#define PIN_0_1B_CLEAR 0b1

#define PIN_5_2B_CLEAR (uint32_t)(PIN_0_2B_CLEAR << 10)
#define PIN_5_1B_CLEAR (uint32_t)(PIN_0_1B_CLEAR << 5)

#define PIN_6_2B_CLEAR (uint32_t)(PIN_0_2B_CLEAR << 12)
#define PIN_6_1B_CLEAR (uint32_t)(PIN_0_1B_CLEAR << 6)

#define PIN_7_2B_CLEAR (uint32_t)(PIN_0_2B_CLEAR << 14)
#define PIN_7_1B_CLEAR (uint32_t)(PIN_0_1B_CLEAR << 7)

#define PIN_12_2B_CLEAR (uint32_t)(PIN_0_2B_CLEAR << 24)
#define PIN_12_1B_CLEAR (uint32_t)(PIN_0_1B_CLEAR << 12)

#define PIN_13_2B_CLEAR (uint32_t)(PIN_0_2B_CLEAR << 26)
#define PIN_13_1B_CLEAR (uint32_t)(PIN_0_1B_CLEAR << 13)

/* function definitions----------------------------------------------------------*/

void initGpioB12AsInput( void )
{
      uint32_t  * reg_pointer; 
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* GPIOB Pin 12 as input*/
    reg_pointer = (uint32_t *)PORTB_MODER_REGISTER;
    // clear pin 12
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_12_2B_CLEAR));
    // Pin 12 as input
    *reg_pointer = *reg_pointer | PIN_12_INP;

    /*PUSH-PULL Pin*/
    reg_pointer = (uint32_t*)PORTB_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_12_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_12_OUTP_PUSHPULL;
    
    /*GPIOB pin 12 high speed */
    reg_pointer = (uint32_t*)PORTB_OSPEEDR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_12_HIGHSPEED;
    
    /*Configure pulled-down*/
    reg_pointer = (uint32_t*)PORTB_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_12_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_12_PULLDOWN;
}

void initGpioB13AsInput( void )
{
      uint32_t  * reg_pointer; 
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* GPIOB Pin 13 as input*/
    reg_pointer = (uint32_t *)PORTB_MODER_REGISTER;
    // clear pin 13
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_13_2B_CLEAR));
    // Pin 13 as input
    *reg_pointer = *reg_pointer | PIN_13_INP;

    /*PUSH-PULL Pin*/
    reg_pointer = (uint32_t*)PORTB_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_13_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_13_OUTP_PUSHPULL;
    
    /*GPIOB pin 13 high speed */
    reg_pointer = (uint32_t*)PORTB_OSPEEDR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_13_HIGHSPEED;
    
    /*Configure pulled-down*/
    reg_pointer = (uint32_t*)PORTB_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_13_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_13_PULLDOWN;
}

void initGpioC6AsInput( void )
{
      uint32_t  * reg_pointer; 
      /* GPIOC Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* GPIOC Pin 6 as input*/
    reg_pointer = (uint32_t *)PORTC_MODER_REGISTER;
    //clear pin 6
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_6_2B_CLEAR));
    // Pin 6 as input
    *reg_pointer = *reg_pointer | PIN_6_INP;

    /*PUSH-PULL Pin*/
    reg_pointer = (uint32_t*)PORTC_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_6_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_6_OUTP_PUSHPULL;
    
    /*GPIOC pin 6 high speed */
    reg_pointer = (uint32_t*)PORTC_OSPEEDR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_6_HIGHSPEED;
    
    /*Configure pulled-down*/
    reg_pointer = (uint32_t*)PORTC_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_6_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_6_PULLDOWN;
}

void initGpioC7AsInput( void )
{
      uint32_t  * reg_pointer; 
      /* GPIOC Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* GPIOC Pin 7 as input*/
    reg_pointer = (uint32_t *)PORTC_MODER_REGISTER;
    //clear pin 7
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_7_2B_CLEAR));
    // Pin 7 as input
    *reg_pointer = *reg_pointer | PIN_7_INP;

    /*PUSH-PULL Pin*/
    reg_pointer = (uint32_t*)PORTC_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_7_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_7_OUTP_PUSHPULL;
    
    /*GPIOC pin 7 high speed */
    reg_pointer = (uint32_t*)PORTC_OSPEEDR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_7_HIGHSPEED;
    
    /*Configure pulled-down*/
    reg_pointer = (uint32_t*)PORTC_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_7_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_7_PULLDOWN;
}

void initGpioB0AsOutput( void )
{
      uint32_t  * reg_pointer;
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* GPIOB0 configured as output */
    reg_pointer = (uint32_t*)PORTB_MODER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_0_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_0_OUTP;
    
    /*GPIOB0 configured as push-pull */
    reg_pointer = (uint32_t*)PORTB_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_0_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_0_OUTP_PUSHPULL;
    
    /*GPIOB0 configured floating */
    reg_pointer = (uint32_t*)PORTB_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_0_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_0_NO;
    
    /* GPIOB0 driven high to start out with */
    reg_pointer = (uint32_t*)PORTB_ODR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_0_ODR_HIGH;  
}

void initGpioB5AsOutput( void )
{
      uint32_t  * reg_pointer;
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* GPIOB5 configured as output */
    reg_pointer = (uint32_t*)PORTB_MODER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_5_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_5_OUTP;
    
    /*GPIOB5 configured as push-pull */
    reg_pointer = (uint32_t*)PORTB_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_5_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_5_OUTP_PUSHPULL;
    
    /*GPIOB5 configured floating */
    reg_pointer = (uint32_t*)PORTB_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_5_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_5_NO;
    
    /* GPIOB5 driven High to start out with */
    reg_pointer = (uint32_t*)PORTB_ODR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_5_ODR_HIGH;  
}

void initGpioB12AsOutput( void )
{
      uint32_t  * reg_pointer;
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* GPIOB12 configured as output */
    reg_pointer = (uint32_t*)PORTB_MODER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_12_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_12_OUTP;
    
    /*GPIOB12 configured as push-pull */
    reg_pointer = (uint32_t*)PORTB_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_12_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_12_OUTP_PUSHPULL;
    
    /*GPIOB12 configured floating */
    reg_pointer = (uint32_t*)PORTB_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_12_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_12_NO;
    
    /* GPIOB12 driven High to start out with */
    reg_pointer = (uint32_t*)PORTB_ODR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_12_ODR_HIGH;  
}

void initGpioB13AsOutput( void )
{
      uint32_t  * reg_pointer;
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* GPIOB13 configured as output */
    reg_pointer = (uint32_t*)PORTB_MODER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_13_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_13_OUTP;
    
    /*GPIOB13 configured as push-pull */
    reg_pointer = (uint32_t*)PORTB_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_13_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_13_OUTP_PUSHPULL;
    
    /*GPIOB13 configured floating */
    reg_pointer = (uint32_t*)PORTB_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_13_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_13_NO;
    
    /* GPIOB13 driven High to start out with */
    reg_pointer = (uint32_t*)PORTB_ODR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_13_ODR_LOW;  
}

void initGpioF7AsAnalog( void )
{
      uint32_t  * reg_pointer;
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    
    /* GPIOB0 configured as Alternative Func */
    reg_pointer = (uint32_t*)PORTF_MODER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_7_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_7_ANALOG;
    
    /*GPIOB0 configured as push-pull */
    reg_pointer = (uint32_t*)PORTF_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_7_1B_CLEAR));
    *reg_pointer = *reg_pointer & ((PIN_7_OUTP_PUSHPULL));
    
    /*GPIOB0 configured floating */
    reg_pointer = (uint32_t*)PORTF_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_7_2B_CLEAR));
    *reg_pointer = *reg_pointer & ((PIN_7_NO));
}

void initGpioF789AsAnalog( void )
{
      uint32_t  * reg_pointer;
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    
    /* GPIOB0 configured as Alternative Func */
    reg_pointer = (uint32_t*)PORTF_MODER_REGISTER;
    *reg_pointer = *reg_pointer | (PIN_7_ANALOG + PIN_8_ANALOG + PIN_9_ANALOG);
    
    /*GPIOB0 configured as push-pull */
    reg_pointer = (uint32_t*)PORTF_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & ((PIN_7_OUTP_PUSHPULL + PIN_8_OUTP_PUSHPULL + PIN_9_OUTP_PUSHPULL));
    
    /*GPIOB0 configured floating */
    reg_pointer = (uint32_t*)PORTF_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & ((PIN_7_NO + PIN_8_NO + PIN_9_NO));
}

void toggleGPIOB0( void )
{
    uint32_t value;
    uint32_t  * reg_pointer;
    //get the current value of the pin 
    reg_pointer = (uint32_t*)PORTB_ODR_REGISTER;
    value = *reg_pointer & PIN_0_ODR_HIGH;
   
    if (value > 0)
    {
        //if high, clear the bit
        *reg_pointer = *reg_pointer & (~((uint32_t)PIN_0_ODR_HIGH));
    }
    else
    {
        //if low, set the bit
       *reg_pointer = *reg_pointer | PIN_0_ODR_HIGH;
    } 
}

void setGPIOB0( void )
{
    uint32_t  * reg_pointer;
    /* GPIOB Pin 0 high */
    reg_pointer = (uint32_t*)PORTB_ODR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_0_ODR_HIGH;
}

void clearGPIOB0( void )
{
    uint32_t  * reg_pointer;
    /* GPIOB Pin 0 low */
    reg_pointer = (uint32_t*)PORTB_ODR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t) 0b01));
}

void setGPIOB5( void )
{
    uint32_t  * reg_pointer;
    /* GPIOB Pin 0 high */
    reg_pointer = (uint32_t*)PORTB_ODR_REGISTER;
    *reg_pointer = *reg_pointer | PIN_5_ODR_HIGH;
}

void clearGPIOB5( void )
{
    uint32_t  * reg_pointer;
    /* GPIOB Pin 0 low */
    reg_pointer = (uint32_t*)PORTB_ODR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t) PIN_5_ODR_HIGH));
}

uint32_t checkGPIOC6(void)
{
 uint32_t valueC6;
 uint32_t * reg_pointer;
 reg_pointer = (uint32_t*)PORTC_IDR_REGISTER;
 //*reg_pointer = *reg_pointer & 0b1000000;
 valueC6 = *reg_pointer & PIN_6_IDR_HIGH;
 //valueC6 = *reg_pointer >> 6;
 return valueC6;   
}

uint32_t checkGPIOC7(void)
{
 uint32_t valueC7;
 uint32_t * reg_pointer;
 reg_pointer = (uint32_t*)PORTC_IDR_REGISTER;
 //*reg_pointer = *reg_pointer & 0b1000000;
 valueC7 = *reg_pointer & PIN_7_IDR_HIGH;
 //valueC6 = *reg_pointer >> 6;
 return valueC7;   
}

uint32_t checkGPIOB13(void)
{
 uint32_t valueB13;
 uint32_t * reg_pointer;
 reg_pointer = (uint32_t*)PORTB_IDR_REGISTER;
 //*reg_pointer = *reg_pointer & 0b1000000;
 valueB13 = *reg_pointer & PIN_13_IDR_HIGH;
 //valueC6 = *reg_pointer >> 6;
 return valueB13;   
}

uint32_t checkGPIOB12(void)
{
 uint32_t valueB12;
 uint32_t * reg_pointer;
 reg_pointer = (uint32_t*)PORTB_IDR_REGISTER;
 //*reg_pointer = *reg_pointer & 0b1000000;
 valueB12 = *reg_pointer & PIN_12_IDR_HIGH;
 //valueC6 = *reg_pointer >> 6;
 return valueB12;   
}

void initGpioB0MappedToTim3Chn3AsOutput( void )
{
      uint32_t  * reg_pointer;
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* GPIOB0 configured as Alternative Func */
    reg_pointer = (uint32_t*)PORTB_MODER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_0_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_0_ALTFUN;
    
    /*GPIOB0 configured as push-pull */
    reg_pointer = (uint32_t*)PORTB_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_0_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_0_OUTP_PUSHPULL;

    /*GPIOB0 configured as High speed port */
    reg_pointer = (uint32_t*)PORTB_OSPEEDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_0_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_0_HIGHSPEED;
    
    /*GPIOB0 configured floating */
    reg_pointer = (uint32_t*)PORTB_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_0_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_0_NO;

    /*GPIOB0 configured AF2 for AFRL register */
    reg_pointer = (uint32_t*)PORTB_AFR1_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)AFRL0_CLEAR));
    *reg_pointer = *reg_pointer | AFRL0_AF2;
}

void initGpioB5MappedToTim3Chn3AsOutput( void )
{
      uint32_t  * reg_pointer;
      /* GPIOB Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* GPIOB5 configured as Alternative Func */
    reg_pointer = (uint32_t*)PORTB_MODER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_5_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_5_ALTFUN;
    
    /*GPIOB5 configured as push-pull */
    reg_pointer = (uint32_t*)PORTB_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_5_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_5_OUTP_PUSHPULL;

    /*GPIOB5 configured as High speed port */
    reg_pointer = (uint32_t*)PORTB_OSPEEDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_5_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_5_HIGHSPEED;
    
    /*GPIOB5 configured floating */
    reg_pointer = (uint32_t*)PORTB_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_5_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_5_NO;

    /*GPIOB5 configured AF2 for AFRL register */
    reg_pointer = (uint32_t*)PORTB_AFR1_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)AFRL5_CLEAR));
    *reg_pointer = *reg_pointer | AFRL5_AF2;
}

void initGpioC6MappedToTim3Chn1AsInputCapture( void )
{
      uint32_t  * reg_pointer;
      /* GPIOC Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    
    /* GPIOC6 configured as Alternative Func */
    reg_pointer = (uint32_t*)PORTC_MODER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_6_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_6_ALTFUN;
    
    /*GPIOC6 configured as push-pull */
    reg_pointer = (uint32_t*)PORTC_OTYPER_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_6_1B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_6_OUTP_PUSHPULL;

    /*GPIOC6 configured as High speed port */
    reg_pointer = (uint32_t*)PORTC_OSPEEDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_6_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_6_HIGHSPEED;
    
    /*GPIOC6 configured pulldown so when C6 low in general */
    reg_pointer = (uint32_t*)PORTC_PUPDR_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)PIN_6_2B_CLEAR));
    *reg_pointer = *reg_pointer | PIN_6_PULLDOWN;

    /*GPIOC6 configured AF2 for AFRL register */
    reg_pointer = (uint32_t*)PORTC_AFRL_REGISTER;
    *reg_pointer = *reg_pointer & (~((uint32_t)AFRL6_CLEAR));
    *reg_pointer = *reg_pointer | AFRL6_AF2;
}
