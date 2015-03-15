/* 
* FSMCampCal.h
*
* Created: 2/6/2015 9:33:22 PM
* Author: keith
*/


#ifndef __FSMCAMPCAL_H__
#define __FSMCAMPCAL_H__
#include "State.h"
#include "States.h"

class FSMCampCal :public CState
{
	public:
	FSMCampCal(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();

}; //FSMCampCal

#endif //__FSMCAMPCAL_H__
