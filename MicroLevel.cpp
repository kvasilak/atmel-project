/*
 * Microlevel.cpp
 *
 * Created: 11/4/2014 9:24:15 PM
 *  Author: keith
 */ 

#include "common.h"
//#include <avr/io.h>
#include "CSerial.h"
#include "CTimer.h"
#include "CBlink.h"

uint16_t a = 0;
uint8_t b = 0;
uint8_t txbuf[100];
uint8_t rxbuf[100];

int main(void)
{
	CSerial Serial = CSerial::Instance();
	Serial.Init(txbuf, 100, rxbuf, 100);
	
	CTimer::Init();
	uint16_t start = CTimer::GetTick();

    while(1)
    {
		if(CTimer::IsTimedOut(250, start))
		{
			Serial <<  a++ << ", " << b-- << ", Hello World 3!\r\n";
			
			start = CTimer::GetTick();
		}
    }
}