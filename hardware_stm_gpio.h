/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_STM_GPIO_H_
#define __HARDWARE_STM_GPIO_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Macros for Everyone--------------------------------------------------------*/
#define PIN_0   0
#define PIN_1   1
#define PIN_2   2
#define PIN_3   3
#define PIN_4   4
#define PIN_5   5
#define PIN_6   6
#define PIN_7   7
#define PIN_8   8
#define PIN_9   9
#define PIN_10  10




/*Function definitions---------------------------------------------------------*/
void initGpioB0AsOutput( void );
void initGpioB5AsOutput( void );
void initGpioB12AsOutput( void );
void initGpioB13AsOutput( void );
void initGpioB12AsInput( void );
void initGpioB13AsInput( void );
void toggleGPIOB0( void );
void setGPIOB0( void );
void setGPIOB5( void );
void clearGPIOB0( void );
void clearGPIOB5( void );
void initGpioC6AsInput( void );
void initGpioC7AsInput( void );
void initGpioB0MappedToTim3Chn3AsOutput(void);
void initGpioB5MappedToTim3Chn3AsOutput( void );
void initGpioC6MappedToTim3Chn1AsInputCapture( void );
uint32_t checkGPIOC6(void);
uint32_t checkGPIOC7(void);
uint32_t checkGPIOB12(void);
uint32_t checkGPIOB13(void);
void initGpioF7AsAnalog( void );
void initGpioF789AsAnalog( void );

#ifdef __cplusplus
}
#endif

#endif /*__GPIO_H */
