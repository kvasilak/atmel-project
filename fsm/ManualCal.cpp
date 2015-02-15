/* 
* FSMManualCal.cpp
*
* Created: 2/6/2015 9:34:03 PM
* Author: keith
*/


#include "ManualCal.h"
#include "..\CController.h"


FSMManualCal::FSMManualCal(CController& SMManager) :
CState(SMManager, STATE_MANUAL_CALIBRATE), 
State(Idle)
{
}

void FSMManualCal::OnEntry()
{
	//Always start by filling the bags to find the upper limits
	State = Fill;
}

//Manual calibration basically means finding the upper and lower 
//suspension travel limits. These limits are used to determine if we 
//should keep trying to adjust during travel or Camp mode
void FSMManualCal::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TimerEvent:
			Calibrate();
		break;
		default:
		break;
	}
}

void FSMManualCal::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}

//calibration state machine
void FSMManualCal::Calibrate()
{
	switch(State)
	{
		case Fill:
			//open both fill valves
			State = Filling;
		break;
		case Filling:
			//Wait for max height to be reached
			if(true)
			{
				State = Dump;
			}
		break;
		case Dump:
			//open dump valves 
			State = Dumping;
		break;
		case Dumping:
			//wait for min height
			if(true)
			{
				State = Done;
			}
		break;
		case Done:
			//Close Valves
			//Save heights in EEPROM
			
			//back to manual mode
			m_SMManager.ChangeState(STATE_MANUAL);
			
			State = Idle;
		break;
		default:
			// close valves
			
			State = Idle;
	}
}