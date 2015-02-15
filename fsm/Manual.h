/* 
* Manual.h
*
* Created: 2/1/2015 10:14:32 PM
* Author: keith
*/


#ifndef __MANUAL_H__
#define __MANUAL_H__

#include "State.h"
#include "States.h"

class FsmManual :public CState
{
	public:
	FsmManual(CController& FsmManager);
	void OnEntry();
	void HandleEvent(EVENT evt);
	void OnExit();
	
	private:
	void Handleswitches();
	
	enum FillStates
	{
		Holding,
		Filling,
		Dumping
	};
	
	FillStates LeftState;
	FillStates RightState;

}; //Manual

#endif //__MANUAL_H__
