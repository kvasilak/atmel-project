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
#include "i2cmaster.h"
#include <avr/io.h>
#include "CLed.h"


uint16_t a = 0;
uint8_t b = 0;
uint8_t txbuf[100];
uint8_t rxbuf[100];


int main(void)
{
	CSerial Serial = CSerial::Instance();
	Serial.Init();
	
	CTimer::Init();
	uint32_t start = CTimer::GetTick();
	CLed LEDs;
	uint8_t brightness = 0;
	bool dim = false;
	
	//Turn on 5v, PA2
	DDRA |= _BV(DDA2);
	PORTA |= _BV(PA2);
	
	i2c_init();                             // initialize I2C library

    while(1)
    {
		if(CTimer::IsTimedOut(100, start))
		{
			Serial <<  a++ << ", " << b-- << ", Hello World y!\r\n";
			
			start = CTimer::GetTick();
			
			LEDs.Set(1, 1, 1, 1, 1, 0, 0, 0, brightness);
			
			if(dim)
			{
				if(brightness-- <= 2)
				{
					 dim = false;
				}
			}
			else
			{
				if(brightness++ >30)
				{
					 dim = true;
				}
			}
		}
		
	
    }
}