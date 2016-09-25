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
	void HandleEvent(eEvents evt);
	void OnExit();
	
	private:
	uint32_t ButtonWakeStart;

}; //Manual

#endif //__MANUAL_H__
