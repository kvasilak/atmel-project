/* 
* FSMDance.h
*
* Created: 2/6/2015 9:34:03 PM
* Author: keith
*/


#ifndef __FSMDANCE_H__
#define __FSMDANCE_H__

#include <stdint.h>
#include "State.h"
#include "States.h"

class FSMDance :public CState
{
	public:
	FSMDance(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();
	
	private:
	void ShakeIt();
	void CalcSpeed();
	bool IsMoving();
	
	enum CalStates
	{
		Idle,
		Fill,
		StartFilling,
		Filling,
		TiltLeft,
		TiltRight,
		FillLeft,
		FillRight,
		Dump1,
		Fill1,
		Dump2,
		Fill2,
		Dump3,
		Done
	};
		
	CalStates State;
	
	uint32_t Blink;
	
	uint32_t SpeedTime;
	uint32_t DanceTime;
	
	uint32_t LeftSpeed;
	uint32_t RightSpeed;
	uint8_t rolls;

}; //FSMDance

#endif //__FSMDANCE_H__
