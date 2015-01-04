/* 
* CTimer.cpp
*
* Created: 1/1/2015 9:28:03 PM
* Author: keith
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "CTimer.h"
#include "mutex.h"

volatile uint32_t	CTimer::TimerTic =0; //1.024 ms per tic


void CTimer::Init()
{
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 250.000 kHz
	// Mode: CTC top=OCR0A
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	// Timer Period: 1 ms
	TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (1<<WGM01) | (0<<WGM00);
	TCCR0B=(0<<WGM02) | (0<<CS02) | (1<<CS01) | (1<<CS00);
	TCNT0=0x00;
	OCR0A=0xF9;
	OCR0B=0x00;
		
	//enable interrupt on compare
	TIMSK0 = _BV(OCIE0A);
}


//returns TRUE when time expires
bool CTimer::IsTimedOut(uint32_t period, uint32_t startTime)
{
	//detects counter rollover due to the magic of fixed point math
	return (GetTick() - startTime) > period ? true : false;
}

//Get the system tic
uint32_t CTimer::GetTick()
{
	CMutex m;
	return CTimer::TimerTic;
}

//uint32_t millis()
//{
//	return CTimer::GetTick();
//}

// clock interrupt - clear flag immediately to resume count
ISR(TIMER0_COMPA_vect)
{
	CTimer::TimerTic++;     // 1.0 ms, Clock tic
}
