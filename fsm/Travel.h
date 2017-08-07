/* 
* Travel.h
*
* Created: 2/1/2015 10:14:47 PM
* Author: keith
*/


#ifndef __TRAVEL_H__
#define __TRAVEL_H__

#include "State.h"
#include "corner.h"

 #define FILTER_STATES_LIST(macro) \
 macro(FilterStart),     \
 macro(FilterWait),     \
 macro(FilterLong)
 
 typedef enum
 {
     FILTER_STATES_LIST(ENUMIFY)
 }FilterStates_e;

class FsmTravel :public CState
{
	public:
	FsmTravel(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();

private:
	CCorner LeftSide;
	CCorner RightSide;
	bool Starting;
	uint32_t Start;
    uint32_t filterwait;
    bool waiting;
    FilterStates_e FilterState;
}; //Travel

#endif //__TRAVEL_H__
