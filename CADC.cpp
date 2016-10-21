/* 
* CADC.cpp
*
* Created: 1/23/2015 6:37:30 PM
* Author: keith
*/


#include "CADC.h"
#include <avr/io.h>
#include <avr/eeprom.h>

#include "nvm.h"

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
	
	DIDR0 = 0x0B; //disable Digital on ADC 1 and 2 and 4
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

uint8_t CADC::GetDimmer()
{
	uint16_t dim = Read(3);
	
	dim /= 4;
	
	return (uint8_t)dim;
}

//current height is greater than calibrated minimum height
bool CADC::LeftHeightOK(void)
{
	bool isok = true;
	uint16_t low = nvm::is().GetLeftLowest();
	
	// ignore check if not calibrated
	if(low >0)
	{
		isok = false;
		
		if(GetLeftHeight() > low)
		{
			isok = true;
		}
	}
	
	return isok;
}

bool CADC::RightHeightOK(void)
{
	#ifdef LOW_LIMIT
	bool isok = true;
	uint16_t low = nvm::is().GetRightLowest();
	
	// ignore check if not calibrated
	if(low >0)
	{
		isok = false;
		
		if(GetRightHeight() > low)
		{
			isok = true;
		}
	}
	
	return isok;
	#else
	return true;// always OK for now...
	#endif
}