/* 
* Manual.cpp
*
* Created: 2/1/2015 10:14:32 PM
* Author: keith
*/


#include "Manual.h"
#include "..\CController.h"


FsmManual::FsmManual(CController& SMManager) :
CState(SMManager, STATE_MANUAL)
{
}

void FsmManual::OnEntry()
{
	Handleswitches();
}

void FsmManual::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TravelEvent:
			//change state if no manual buttons are active
			if(true)
			{
				m_SMManager.ChangeState(STATE_TRAVEL);
			}
		break;
		case CampEvent:
			//change state if no manual buttons are active
			if(true)
			{
				m_SMManager.ChangeState(STATE_CAMP);
			}
		break;
		case ManualEvent:
			Handleswitches();
		break;
		case CalibrateEvent:
			m_SMManager.ChangeState(STATE_MANUAL_CALIBRATE);
		break;
		case TimerEvent:
			//run travel FSM?
		break;
		default:
		break;
	}
}

void FsmManual::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}

//check which button caused the event and decide if we need to act
void FsmManual::Handleswitches()
{
	
}