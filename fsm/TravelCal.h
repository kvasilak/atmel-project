/* 
* FSMTravelCal.h
*
* Created: 2/6/2015 9:33:42 PM
* Author: keith
*/


#ifndef __FSMTRAVELCAL_H__
#define __FSMTRAVELCAL_H__
#include "State.h"
#include "States.h"

class FSMTravelCal :public CState
{
	public:
	FSMTravelCal(CController& FsmManager);
	void OnEntry();
	void HandleEvent(EVENT evt);
	void OnExit();

}; //FSMTravelCal

#endif //__FSMTRAVELCAL_H__
