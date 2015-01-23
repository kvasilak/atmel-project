/*
 * Microlevel.cpp
 *
 * Created: 11/4/2014 9:24:15 PM
 *  Author: keith
 */ 

#include "common.h"
#include "CSerial.h"
#include "CTimer.h"
#include "CBlink.h"
//#include "i2cmaster.h"
#include <avr/io.h>
#include "CLeds.h"


uint16_t a = 0;
uint8_t b = 0;
uint8_t txbuf[100];
uint8_t rxbuf[100];


int main(void)
{
	CSerial Serial = CSerial::is();
	Serial.Init();
	
	CTimer::Init();
	uint32_t start = CTimer::GetTick();
	CLeds LEDs = CLeds::is();
	uint8_t brightness = 66;
	bool dim = false;
	
	//Turn on 5v, PA2
	DDRA |= _BV(DDA2);
	PORTA |= _BV(PA2);

	LEDs.Init();
	
	LEDs.RightUpOn();
	LEDs.LeftUpOn();
	LEDs.RightDownOn();
	LEDs.LeftDownOn();
	
	LEDs.ActiveOn();
	
	LEDs.Dim(10);

    while(1)
    {
		if(CTimer::IsTimedOut(100, start))
		{
			Serial <<  a++ << ", " << b-- << ", Hello World z!\r\n";
			
			start = CTimer::GetTick();

			LEDs.Dim(brightness);
			
			if(dim)
			{
				if(brightness-- <= 2)
				{
					 dim = false;
					 LEDs.LeftUpOn();
				}
			}
			else
			{
				if(brightness++ >30)
				{
					 dim = true;
					 LEDs.LeftUpOff();
				}
			}
		}
		
	
    }
}