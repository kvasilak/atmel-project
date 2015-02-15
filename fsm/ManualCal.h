/* 
* FSMManualCal.h
*
* Created: 2/6/2015 9:34:03 PM
* Author: keith
*/


#ifndef __FSMMANUALCAL_H__
#define __FSMMANUALCAL_H__
#include "State.h"
#include "States.h"

class FSMManualCal :public CState
{
	public:
	FSMManualCal(CController& FsmManager);
	void OnEntry();
	void HandleEvent(EVENT evt);
	void OnExit();
	
	private:
	void Calibrate();
	
	enum CalStates
	{
		Idle,
		Fill,
		Filling,
		Dump,
		Dumping,
		Done
	};
		
	CalStates State;

}; //FSMManualCal

#endif //__FSMMANUALCAL_H__
