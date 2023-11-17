#ifndef __DEBUG_MORT_H_
#define __DEBUG_MORT_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*Function definitions---------------------------------------------------------*/
void debugprint(uint16_t number);
void debugprint32(uint32_t number);
void debugprintRed( void );
void debugprintOrange( void );
void debugprintWhite( void );
void debugprintGreen( void );
void debugprintHelloWorld( void );

#ifdef __cplusplus
}
#endif

#endif /*__DEBUG_MORT_H */
