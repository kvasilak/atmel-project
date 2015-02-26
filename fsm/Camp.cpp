/* 
* Camp.cpp
*
* Created: 2/1/2015 10:14:57 PM
* Author: keith
*/


#include "Camp.h"
#include "..\CController.h"
#include "..\CLeds.h"
#include "..\CSerial.h"

FsmCamp::FsmCamp(CController& SMManager) :
CState(SMManager, STATE_CAMP)
{
}

void FsmCamp::OnEntry()
{
	CSerial::is() << " FsmCamp::OnEntry()\r\n";
}

void FsmCamp::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TimerEvent:
			//run camp FSM 
		break;
		case RockerEvent:
		case OutSideEvent:
		case SteeringEvent:
			m_SMManager.ChangeState(STATE_MANUAL, evt);
		break;
		case CampEvent:
			Cio::is().CampSwitches();
		break;
		case TravelEvent:
			m_SMManager.ChangeState(STATE_TRAVEL, evt);
		break;
		case CalibrateEvent:
			m_SMManager.ChangeState(STATE_CAMP_CALIBRATE, evt);
		default:
		break;
	}
}

void FsmCamp::OnExit()
{
	CSerial::is() << " FsmCamp::OnExit()\r\n";
}