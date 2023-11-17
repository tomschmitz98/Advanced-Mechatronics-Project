/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_STM_TIMER3_H_
#define __HARDWARE_STM_TIMER3_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*Function definitions---------------------------------------------------------*/
void initTimer3Channel3( void );
//uint32_t ValueTimer3Channel3( void );
void initTimer3Channel1ToInputCapture( void );
uint16_t ReadInputCaputreFlagNTakeAction( void );
void ReadInputCaputreFlagNReadInput( void );

#ifdef __cplusplus
}
#endif

#endif /*__TIMER3_H_ */
