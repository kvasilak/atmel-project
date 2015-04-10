/* 
* CADC.cpp
*
* Created: 1/23/2015 6:37:30 PM
* Author: keith
*/


#include "CADC.h"
#include <avr/io.h>

// default constructor
CADC::CADC()
{
} //CADC

void CADC::Init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);
	    
	// ADC Enable and prescaler of 128
	// 16000000/128 = 125000
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	DIDR0 = 0x03; //disable Digital on ADC 1 and 2
}

uint16_t CADC::Read(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with ’7? will always keep the value
	// of ‘ch’ between 0 and 7
	ch &= 0x7;  // AND operation with 7
	ADMUX = (ADMUX & 0xE0)|ch; // clears the bottom 3 bits before ORing
  
	// start single convertion
	// write ’1? to ADSC
	ADCSRA |= (1<<ADSC);
  
	// wait for conversion to complete, about 13 adc clocks or .0000108 seconds +-
	// ADSC becomes ’0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));

	return ADC;
}

uint16_t CADC::GetLeftHeight()
{
	return Read(0);
}

uint16_t CADC::GetRightHeight()
{
	return Read(1);
}
