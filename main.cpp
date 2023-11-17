#include "main.h"
#include "mbed.h"
#include "debug_mort.h"
#include "nucleo_led.h"
#include "hardware_stm_gpio.h"
#include "hardware_stm_timer3.h"
#include "hardware_stm_adc.h"
#include "hardware_stm_dma.h"
#include "hardware_stm_interruptcontroller.h"

int main (void)
{
    uint32_t i, j, k;
    uint16_t previousADCValue = 0;
    uint16_t currentADCValue = 0;

    initADC3_5();
    initGpioB0AsOutput();
    startADCConversion();
    
    // initialize GPIOB12 as Output high
    initGpioB12AsOutput();
    // initialize GPIOB13 as Output low
    initGpioB13AsOutput();
    // initialize GPIOB5 as AF2 Output
    initGpioB5MappedToTim3Chn3AsOutput();

    while(1)
    {
        currentADCValue = returnADC3StoredValueforone();
        for (i = 0; i < 10000000; i++)
        {
            for (k = 0; k < 10000000; k++)
                j = j + 1; // tiny delay
        }
        if (abs(currentADCValue - previousADCValue) > 100)
        {
            // initialize PWM mode 1 to 30kHz input duty cycle
            initTimer3ToPWMMode1();
        }
        // code for DMA:
        debugprint(currentADCValue);
        toggleGPIOB0();
        previousADCValue = currentADCValue;
    }
}