/* 
* FSMManualCal.h
*
* Created: 2/6/2015 9:34:03 PM
* Author: keith
*/


#ifndef __FSMMANUALCAL_H__
#define __FSMMANUALCAL_H__

#include <stdint.h>
#include "State.h"
#include "States.h"

class FSMManualCal :public CState
{
	public:
	FSMManualCal(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();
	
	private:
	void Calibrate();
	void CalcSpeed();
	bool IsMoving();
	
	enum CalStates
	{
		Idle,
		Fill,
		StartFilling,
		Filling,
		Dump,
		StartDumping,
		Dumping,
		Done
	};
		
	CalStates State;
	
	uint32_t Blink;
	
	uint16_t leftmax;
	uint16_t rightmax;
	uint16_t leftmin;
	uint16_t rightmin;
	uint32_t SpeedTime;
	
	uint32_t LeftSpeed;
	uint32_t RightSpeed;

}; //FSMManualCal

#endif //__FSMMANUALCAL_H__
