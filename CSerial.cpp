/***************************************************************************
 *   Copyright (C) 2008 by Damian Kmiecik                                  *
 *   Updates by Keith Vasilakes, Jan 1 2015                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *                             CSerial FILE                                   *
 *                                                                         *
 ***************************************************************************/
//Headers
#include "common.h"			//GLOBAL DEFINITIONS
#include <avr/io.h>			//I/O
#include <inttypes.h>		//
#include "cserial.h"
#include <avr/pgmspace.h>	//PGM space read
//Definitions


CSerial::CSerial() :
m_Base(10)
{
	//Set baudrate and enabling interfaces
	//115200 8n1, atmega164 @ 16mhz specific
	
	UBRR0H = 0;
	UBRR0L = 16; 
	UCSR0A |= _BV(U2X0);

	UCSR0C = (3 << UCSZ00); //8n1 
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   // Enable RX and TX
}

//Put text on UART
void CSerial::put(const char* text) 
{
	while(*text){
		this->send(*text);
		text++;
	}
}

#ifdef UART_PGM
//Put text from PGM
void CSerial::put_p(const char *text) 
{
	while(pgm_read_byte(text)){
		this->send(pgm_read_byte(text));
		*text++;
	}
}

#endif
//Put number on UART
void CSerial::put(int16_t number) 
{
	char buf[16] = {0};
	register int i = 14;
	bool neg =  false;
	
	if(number < 0) 
	{
		number = number * (-1);
		neg = true;
	}
	
	if(number != 0)
	{

		for(; number && i ; --i, number /= m_Base)
		{
			buf[i] = "0123456789abcdef"[number % m_Base];
		}
		
		if(neg) 
		{
			buf[i] = '-';
			this->put(&buf[i]);
		} 
		else 
		{
			this->put(&buf[i+1]);
		}

	}
	else
	{
		this->put("0");
	}
}

//Transmit one byte
void CSerial::send(uint8_t data) 
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = data;	
}
#ifdef UART_RECIVE
//Recive one byte
uint8_t CSerial::get() 
{
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}
#endif


#ifdef UART_STREAMS

CSerial &operator<<(CSerial &rs232, uint8_t val)
{
	rs232.put(val);
	return rs232;
}

CSerial &operator<<(CSerial &rs232, uint16_t val) {
	rs232.put(val);
	return rs232;
}

CSerial &operator<<(CSerial &rs232, int val)
{
	rs232.put(val);
	return rs232;
}

//
CSerial &operator<<(CSerial &rs232, const char* val) {
	rs232.put(val);
	return rs232;
}

#ifdef UART_PGM
//Compilator not recognize type const char*, i have to throwing this to void* before streaming
//plz use (void*)PSTR(text) if you want put text from PGM
CSerial &operator<<(CSerial &rs232, void* val) {
	rs232.put_p((const char*)val);
	return rs232;
}
#endif //PGM

#endif

