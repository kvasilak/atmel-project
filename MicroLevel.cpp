/*
 * Microlevel.cpp
 *
 * Created: 11/4/2014 9:24:15 PM
 *  Author: keith
 */ 

#include "common.h"
#include "CSerial.h"
#include "CController.h"

#include "CTimer.h"
#include "CBlink.h"
#include <avr/io.h>
#include "CLeds.h"
#include "CADC.h"

uint16_t a = 0;
uint8_t b = 0;
uint8_t txbuf[100];
uint8_t rxbuf[100];

//error handler that is invoked when a pure virtual function is called.
extern "C" void __cxa_pure_virtual()
{
	//reset!
	while (1);
}

int main(void)
{
	CController Controller;
	CSerial Serial = CSerial::is();
	Serial.Init();
	
	CTimer::Init();
	uint32_t start = CTimer::GetTick();
	CLeds LEDs = CLeds::is();
	uint8_t brightness = 66;
	bool dim = false;
	uint16_t adc0;
	uint16_t adc1;
	//CADC adc = CADC::is();

	//Turn on 5v, PA2
	DDRA |= _BV(DDA2);
	PORTA |= _BV(PA2);

	LEDs.Init();
	CADC::is().Init();
	
	LEDs.RightUpOn();
	LEDs.LeftUpOn();
	LEDs.RightDownOn();
	LEDs.LeftDownOn();
	LEDs.CampOn();
	LEDs.ActiveOn();
	LEDs.TravelOKOn();
	//LEDs.TravelWarnOn();
	
	LEDs.Dim(10);

    while(1)
    {
		if(CTimer::IsTimedOut(100, start))
		{
			
			Controller.Run();
			
			adc0 = 3;
			adc1 = 4;
			adc0 = CADC::is().Read(0);
			adc1 = CADC::is().Read(1);
			Serial <<  a++ << ", " << b-- << ", Hello World; " << adc0 << ", " << adc1 << "\r\n";
			
			start = CTimer::GetTick();

			LEDs.Dim(brightness);
			
			if(dim)
			{
				if(brightness-- <= 4)
				{
					 dim = false;
					 LEDs.LeftUpOn();
				}
			}
			else
			{
				if(brightness++ >50)
				{
					 dim = true;
					 LEDs.LeftUpOff();
				}
			}
		}
		
	
    }
}