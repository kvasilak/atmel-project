/* 
* FSMTravelCal.h
*
* Created: 2/6/2015 9:33:42 PM
* Author: keith
*/


#ifndef __FSMTRAVELCAL_H__
#define __FSMTRAVELCAL_H__

#include <stdint.h>
#include "State.h"
#include "States.h"


class FSMTravelCal :public CState
{
	public:
	FSMTravelCal(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();
	
	private:
	uint32_t Blink;

}; //FSMTravelCal

#endif //__FSMTRAVELCAL_H__
