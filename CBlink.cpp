/* 
* CBlink.cpp
*
* Created: 11/4/2014 9:25:23 PM
* Author: keith
*/

#include <avr/io.h>
#include "CBlink.h"

CBlink::CBlink(volatile uint8_t* port, uint8_t pin, bool initiallyLit/* =false */)
{
	//Set the port addresses for the DDR and PORT values
	m_port = port;
	//NOTE: If you expand the Dependencies leaf in the Solution Explorer
	//  and open the corresponding io???.h file (for the ATMega1280
	//  this file is iomxx0_1.h) and look for the PORT you are using you
	//  will see that the values are PINn followed by DDRn and then PORTn
	//  so given the PORTn as a parameter the corresponding DDRn is PORTn - 1.
	m_ddr_port = port - 1;
	
	//Set the DDRn.pin as output
	*m_ddr_port = _BV(pin);
	
	m_pin = pin;
	m_isLedLit = initiallyLit;
	
	//Set initial state of the LED
	SetLitState(m_isLedLit);
}

void CBlink::SetLitState(bool state)
{
	m_isLedLit = state;
	
	if (m_isLedLit)
	*m_port |= _BV(m_pin);
	else
	*m_port &= ~_BV(m_pin);
}

void CBlink::ToggleLitState()
{
	SetLitState(!m_isLedLit);
}
