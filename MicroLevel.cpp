/*
 * Microlevel.cpp
 *
 * Created: 11/4/2014 9:24:15 PM
 *  Author: keith
 */ 

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>
#include "CSerial.h"
#include "CBlink.h"

uint16_t a = 0;
uint8_t b = 0;

int main(void)
{
	CSerial Serial;

    while(1)
    {
		Serial <<  a++ << ", " << b-- << ", Hello World!\r\n";

		_delay_ms(100);
    }
}