/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_STM_DMA_H_
#define __HARDWARE_STM_DMA_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/*Function definitions---------------------------------------------------------*/
void initDMAForADC3_1channel( void );
void initDMAForADC3_3channels( void );
void enableDMAForAdc3_3channels( void );
uint16_t returnADC3StoredValue(uint8_t index);
uint16_t returnADC3StoredValueforone(void);


#ifdef __cplusplus
}
#endif

#endif /*__GPIO_H */
