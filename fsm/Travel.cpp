/* 
* Travel.cpp
*
* Created: 2/1/2015 10:14:47 PM
* Author: keith
*/


#include "Travel.h"
#include "..\CController.h"
#include "..\CLeds.h"
#include "..\CSerial.h"

FsmTravel::FsmTravel(CController& SMManager) :
CState(SMManager, STATE_TRAVEL)
{
}

void FsmTravel::OnEntry()
{
	CSerial::is() << " FsmTravel::OnEntry()\r\n";
	Cio::is().TravelSwitches();
}

void FsmTravel::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TimerEvent:
			//run travel FSM
			break;
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
	CSerial::is() << " FsmTravel::OnExit()\r\n";
	Cio::is().TravelSwitches();
}