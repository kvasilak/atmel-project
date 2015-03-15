/* 
* Camp.h
*
* Created: 2/1/2015 10:14:57 PM
* Author: keith
*/


#ifndef __CAMP_H__
#define __CAMP_H__

#include "State.h"
#include "States.h"

class FsmCamp :public virtual CState
{
	public:
	FsmCamp(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();

}; //Camp

#endif //__CAMP_H__
