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
	uint8_t RU = 0;
	uint8_t RD = 0xff;
	
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
		}
		
		if(CTimer::IsTimedOut(1000, LEDS))
		{
			if(i2c_start_wait(0x40))     // set device address and write mode
			{
				Serial << "i2c go\r\n";
				  if(i2c_write(0x80)==0) //autoincrement for all register 0x80
				   if(i2c_write(0x81)==0)       //MODE1
					if(i2c_write(0x03)==0)       //MODE2
					 if(i2c_write(RU)==0)       //PWM0
					  if(i2c_write(RD)==0)       //PWM1
					   if(i2c_write(0x0F)==0)       //PWM2
						if(i2c_write(0x77)==0)       //PWM3
						 if(i2c_write(0x07)==0)       //PWM4
						  if(i2c_write(0x70)==0)       //PWM5
						   if(i2c_write(0xf7)==0)       //PWM6
							if(i2c_write(0x7f)==0)       //PWM7
							 if(i2c_write(0xFF)==0)       //GRPPWM
							  if(i2c_write(0x00)==0)       //GRPFREQ
							   if(i2c_write(0xAA)==0)       //LEDOUT0
								if(i2c_write(0xAA)==0)  
								;
				i2c_stop();
				
				LEDS = CTimer::GetTick();
				
				RU += 16;
				RD -= 16;
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