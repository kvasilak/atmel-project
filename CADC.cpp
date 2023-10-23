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
#include "CSerial.h"

//led brightness table
//y (x^3)/65636
//To give better resolution at the dim end
static const uint8_t BrightnessLU[] =
{32,32,32,32,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,7,7,7,7,8,8,8,9,9,9,
9,10,10,10,11,11,11,12,12,12,13,13,14,14,15,15,15,16,16,17,17,18,18,19,19,20,20,21,
21,22,22,23,24,24,25,25,26,27,27,28,29,29,30,31,32,32,33,34,35,35,36,37,38,39,39,
40,41,42,43,44,45,46,47,48,48,49,50,51,52,54,55,56,57,58,59,60,61,62,64,65,66,67,
68,70,71,72,73,75,76,77,79,80,81,83,84,86,87,89,90,91,93,95,96,98,99,101,102,104,
106,107,109,111,112,114,116,118,119,121,123,125,127,129,131,132,134,136,138,140,
142,144,146,148,151,153,155,157,159,161,163,166,168,170,173,175,177,179,182,184,
187,189,192,194,197,199,202,204,207,209,212,215,217,220,223,225,228,231,234,237,
239,242,245,248,251,254,255};


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
    
    LeftHeightAverageCount = 0;
    for(int i=0;i<10;i++)
    {
        LeftHeightAverages[i] = GetLeftHeight();
    }
    
    RightHeightAverageCount = 0;
    for(int i=0;i<10;i++)
    {
        RightHeightAverages[i] = GetRightHeight();
    }
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

uint16_t CADC::GetLeftHeight(void)
{
	return Read(0);
}

int32_t CADC::GetRightAvgHeight(void)
{
    int32_t height=0;
    int32_t h = GetRightHeight();
    
    RightHeightAverages[RightHeightAverageCount++] = h;
    if(RightHeightAverageCount >= 10) RightHeightAverageCount = 0;
    
    for(int i=0;i<10;i++)
    {
        height += RightHeightAverages[i];
    }
    
    height /= 10;
    
    return height;
}
    
int32_t CADC::GetLeftAvgHeight(void)
{
    int32_t height=0;
    int32_t h = GetLeftHeight();
    
    LeftHeightAverages[LeftHeightAverageCount++] = h;
    if(LeftHeightAverageCount >= 10) LeftHeightAverageCount = 0;
    
    for(int i=0;i<10;i++)
    {
        height += LeftHeightAverages[i];
    }
    
    height /= 10;
    
    return height;
}

uint16_t CADC::GetRightHeight()
{
	return Read(1);
}

uint8_t CADC::GetDimmer()
{
	uint16_t d = Read(3)/4;
    uint8_t dim = BrightnessLU[d];
    
   // CSerial::is() << "Dim; " << d << ", " << dim << "\n";
	
	return (uint8_t)dim;
}
