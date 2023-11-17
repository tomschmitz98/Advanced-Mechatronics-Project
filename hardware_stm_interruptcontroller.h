/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_STM_INTERRUPTCONTROLLER_H_
#define __HARDWARE_STM_INTERRUPTCONTROLLER_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*Function definitions---------------------------------------------------------*/
void enableNVIC_Timer3(void);
void initTimer3ToPWMMode1( void );
void initTimer3ToInterruptNOutputCompCh3( void );
void TIM3_IRQHandler( void );
void enableEXTI6OnPortC(void);
void EXTI9_5_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /*__TIMER3_H_ */
