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
	static bool IsTimedOut(uint32_t period, uint32_t startTime);
	
	//Get the current ms timer tick
	static uint32_t GetTick(void);
	
	//Start the timer
	static void Init(void);
	
	static void TimerISR(void)  __attribute__((interrupt));
	
	static volatile uint32_t TimerTic; //1.024 ms per tic
	

	private:
	CTimer();
	~CTimer();

	
}; //CTimer

#endif //__CTIMER_H__
