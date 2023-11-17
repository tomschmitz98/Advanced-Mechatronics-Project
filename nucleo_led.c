/**
  ******************************************************************************
  * @file    nucleo_led.c 
  * @author  mortamar@andrew.cmu.edu
  * @version 1.0
  * @date    Septembr-2021
  * @brief   Controls the LED's on the nucleo board
  ******************************************************************************
  */

#include "hardware_stm_gpio.h"
#include "hardware_stm_timer3.h"
#include "nucleo_led.h"

/************************************
* Initializes LED1 on the nucleo Board which is connected to Port B Pin 0
*************************************/
void init_LED1( void )
{
    // Call something from hardware_stm_gpio 
    initGpioB0AsOutput();
}
/************************************
* Toggles LED1 
*************************************/
void toggle_LED1( void )
{
    // Call something else from hardware_stm_gpio
    toggleGPIOB0();
}

/* Function that turns the LED on or off */
void LED_ONOROFF_FUNCTION( void )
{
    uint32_t valueC6 = 0;
    // check GPIOC PIN 6
    valueC6 = checkGPIOC6();
    // when pin 6 high
    if (valueC6 == 64)
    {
        // turn on LED
        setGPIOB0();
        
    }
    // when pin 6 low
    else
    {
        // turn off LED
        clearGPIOB0();
    }
}

//void LED_ONOROFF_TIMER3_FUNCTION( void )
//{
//    //initialize pin6 as input
//    initGpioC6AsInput();
//    //initialize timer3 channel3
//    initTimer3Channel3();
//    uint32_t valueT3C3;
//    // check GPIOC PIN 6
//    valueT3C3 = ValueTimer3Channel3();
//    // when pin 6 low
//    if (valueT3C3 == 0b0)
//    {
//        // turn off LED
//        clearGPIOB0();
//    }
//    // when pin 6 high
//    else if (valueT3C3 == 0b1)
//    {
//        // turn on LED
//        setGPIOB0();
//    }
//}