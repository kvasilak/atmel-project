/***************************************************************************
 *   Copyright (C) 2008 by Damian Kmiecik                                  *
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
 *                           UART HEADER FILE                              *
 *                                                                         *
 ***************************************************************************/
#ifndef UART_H
#define UART_H

//Headers
#include <avr/io.h>			//I/O
#include <inttypes.h>			//
#include <avr/pgmspace.h>		//PGM space read

//UART class
class CSerial 
{
	public:
		static CSerial &Instance()
		{
			static CSerial serial;
			
			return serial;
		}
		void Init();						//INIT
		void put(const char *text);		//PUT TEXT ON LCD
		#ifdef UART_PGM
		void put_p(const char *text);	//PUT TEXT FROM PGM SPACE
		#endif
		
		void put(int16_t number);		//PUT NUMBER ON LCD
		#ifdef UART_RECIVE
		uint8_t get();				//Get single byte
		#endif
		
		void Hex() {m_Base = 16; };		//show numbers in Base 16 ( hex )
		void Dec() {m_Base = 10; };		//Show numbers in Base 10 ( decimal)
			
	private:
		CSerial();
		uint8_t m_Base;					//Decimal numbers by default
		bool IsOpen;
};
//
#ifdef UART_STREAMS
CSerial &operator<<(CSerial &rs232, uint8_t val);
CSerial &operator<<(CSerial &rs232, uint16_t val);
CSerial &operator<<(CSerial &rs232, int val);
CSerial &operator<<(CSerial &rs232, const char* val);
#ifdef UART_PGM
CSerial &operator<<(CSerial &rs232, void* val);
#endif
#endif

#endif

