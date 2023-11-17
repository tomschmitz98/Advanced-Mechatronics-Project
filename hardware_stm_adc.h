/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_STM_ADC_H_
#define __HARDWARE_STM_ADC_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/*Function definitions---------------------------------------------------------*/
void enableNVIC_ADC( void );
void initADC3_567( void );
void initADC3_5( void );
void initADC3_567_withDMA( void );
void storeNewADCValueInSequence( void );
void startADCConversion ( void );
uint16_t returnNewADCValue( void );
uint8_t returnIfADChasnewValue( void );
void clearIfADChasnewValue( void );
void ADC_IRQHanlder( void );


#ifdef __cplusplus
}
#endif

#endif /*__GPIO_H */
