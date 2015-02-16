/*
  Debouncer.h - Debounced Digital inputs
  Copyright (c) 2013 Keith Vasilakes.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* 
 This code will debounce the specified input.
 create an object, then call attach with the pin number in setup() for each object
 call update() on each object in loop() before using any IO
 */

#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include <inttypes.h>

enum IO_Ports{
	IO_PORTA,
	IO_PORTB,
	IO_PORTC,
	IO_PORTD
	};

class Debounced
{
public:
	Debounced();
	void Attach(IO_Ports port, uint8_t pin);           // set the pin to use
	void Update();					//update the pin state, call frequently
	int Level() {return PinLevel;}
	operator int() const {return PinLevel;}  //Allow using the class like an int

  
private:
	enum DebounceStates_e {CHECKING, BOUNCING};
	bool GetPin();
	uint8_t PinLevel;		//the current debounced pin level
	IO_Ports PinPort;		//the port the pin is on
	uint8_t PinNumber;	//the pin to use
	unsigned long DebounceStart;
	DebounceStates_e State;
	bool Attached;
};

#endif
