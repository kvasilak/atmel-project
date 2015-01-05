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
	uint32_t LEDS = CTimer::GetTick();
	bool tock = false;
	
	//Turn on 5v, PA2
	DDRA |= _BV(DDA2);
	PORTA |= _BV(PA2);
	
	i2c_init();                             // initialize I2C library

	// LED controller
	//if(i2c_start_wait(0x40))     // set device address and write mode
	//{
		//Serial << "i2c start\r\n"
		//i2c_write(0x0c);                        // write address = 0x0c
		//i2c_write(0x55);                        // write value 0b01010101
		//i2c_stop();                             // set stop conditon = release bus
	//}
	//
	//i2c_start_wait(0x40);     // set device address and write mode
	//i2c_write(0x0D);                        // write address = 0x0c
	//i2c_write(0x55);                        // write value 0b01010101
	//i2c_stop();                             // set stop conditon = release bus

    while(1)
    {
		if(CTimer::IsTimedOut(100, start))
		{
			Serial <<  a++ << ", " << b-- << ", Hello World y!\r\n";
			
			start = CTimer::GetTick();
		}
		
		if(CTimer::IsTimedOut(1000, LEDS))
		{
			uint8_t i = i2c_start_wait(0x40);
			
			if(i == 1)     // set device address and write mode
			{
				Serial << "i2c start\r\n";
				i2c_write(0x0c);                        // write address = 0x0c
				i2c_write(0x55);                        // write value 0b01010101
				i2c_stop();                             // set stop conditon = release bus
				
				LEDS = CTimer::GetTick();
			}
			else
			{
				Serial << "i2c Failed\r\n";
				
				LEDS = CTimer::GetTick();
				
				//i2c_start_wait(0x40+I2C_WRITE);     // set device address and write mode
				//i2c_write(0x0c);                        // write address = 0x0c
				//i2c_write(0x55);                        // write value 0b01010101
				//i2c_stop();
				//
				//LEDS = CTimer::GetTick();
			}
			
			tock = ~tock;
		}
    }
}