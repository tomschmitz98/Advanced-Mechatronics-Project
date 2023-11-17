#include "mbed.h"
#include "debug_mort.h"

void debugprint(uint16_t number)
{
    printf("ADC Value: %u\n",number);    
}

void debugprint32(uint32_t number)
{
    printf("Got to %u\n",number);    
}

void debugprintRed( void )
{
    printf("Red Bear!!!\n");
} 

void debugprintOrange( void )
{
    printf("Orange Bear!!!\n");
} 

void debugprintWhite( void )
{
    printf("White Bear!!!\n");
} 

void debugprintGreen( void )
{
    printf("Green Bear!!!\n");
} 

void debugprintHelloWorld( void )
{
    printf("I'm aliiiiiivveeee!!!\n");
} 