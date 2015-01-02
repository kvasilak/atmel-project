/* 
* CTimer.h
*
* Created: 1/1/2015 9:28:03 PM
* Author: keith
*/


#ifndef __CTIMER_H__
#define __CTIMER_H__

#include <inttypes.h>

class CTimer
{
	public:
	//detect counter rollover due to the magic of fixed point math ((TimerTic - startTime > period) ? true : false;)
	static bool IsTimedOut(uint16_t period, uint16_t startTime);
	
	//Get the current ms timer tick
	static uint16_t GetTick(void);
	
	//Start the timer
	static void Init(void);
	
	static void TimerISR(void)  __attribute__((interrupt));
	
	static volatile uint16_t	TimerTic; //1.024 ms per tic
	

	private:
	CTimer();
	~CTimer();
	//static volatile uint16_t	TimerTic; //1.024 ms per tic
}; //CTimer

#endif //__CTIMER_H__
