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
#include <avr/pgmspace.h>	//PGM space read
#include <avr/interrupt.h>

#include "cserial.h"
#include "circularbuffer.h"

static CircularBuffer <uint8_t, 100> TxBuf;
static CircularBuffer <uint8_t, 100> RxBuf;

__extension__ typedef int __guard __attribute__((mode (__DI__)));

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);}
void __cxa_guard_release (__guard *g) {*(char *)g = 1;}

CSerial::CSerial():
m_Base(10),
IsOpen(false)
{
}
	

void CSerial::Init()
{
	//Set baudrate and enabling interfaces
	//115200 8n1, atmega164 @ 16mhz specific
	IsOpen = false;
	
	UBRR0H = 0;
	UBRR0L = 16; 
	UCSR0A |= _BV(U2X0);

	UCSR0C = (3 << UCSZ00); //8n1 
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   // Enable RX and TX
	
	IsOpen = true;
	sei();
}

//Put text on UART
void CSerial::put(const char* text) 
{
	while(*text)
	{
		//drop data if buffer overflows
		TxBuf.Put(*text);
		text++;
	}
	
	//make sure the interrupt is enabled
	UCSR0B |= _BV(UDRIE0);
}

#ifdef UART_PGM
//Put text from PGM
void CSerial::put_p(const char *text) 
{
	while(pgm_read_byte(text))
	{
		//drop data if buffer overflows
		TxBuf.Put(pgm_read_byte(text));
		*text++;
	}
	
	//make sure the interrupt is enabled
	UCSR0B |= _BV(TXCIE0);
}

#endif

void CSerial::put32(int32_t number)
{
	put16(number & 0x00FF);
	put16((number & 0xFF00) > 16);
}

//Put number on UART
void CSerial::put16(int16_t number) 
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
	rs232.put16(val);
	return rs232;
}

CSerial &operator<<(CSerial &rs232, uint16_t val) {
	rs232.put16((int16_t)val);
	return rs232;
}

CSerial &operator<<(CSerial &rs232, int32_t val){
	rs232.put32(val);
	return rs232;
}

CSerial &operator<<(CSerial &rs232, int val)
{
	rs232.put16(val);
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

ISR(USART0_UDRE_vect)
{
	uint8_t c;

	if(true == TxBuf.Get(c))
	{
		UDR0 = c;
	}
	else
	{
		//disable tx data empty interrupt
		UCSR0B &= ~_BV(TXCIE0);
	}
}

#endif

