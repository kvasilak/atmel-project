/* 
* Travel.cpp
*
* Created: 2/1/2015 10:14:47 PM
* Author: keith
*/


#include "Travel.h"
#include "..\CController.h"


FsmTravel::FsmTravel(CController& SMManager) :
CState(SMManager, STATE_TRAVEL)
{
}

void FsmTravel::OnEntry()
{
	//cout << "\nCStateIdle::OnEntry()" << endl;
}

void FsmTravel::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TimerEvent:
			//run travel FSM
		case ManualEvent:
			m_SMManager.ChangeState(STATE_MANUAL);
		break;
		case CampEvent:
			m_SMManager.ChangeState(STATE_CAMP);
		break;
		case CalibrateEvent:
			m_SMManager.ChangeState(STATE_TRAVEL_CALIBRATE);
		default:
		break;
	}
}

void FsmTravel::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}