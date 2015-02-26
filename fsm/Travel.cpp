/* 
* Travel.cpp
*
* Created: 2/1/2015 10:14:47 PM
* Author: keith
*/



#include "..\CController.h"
#include "..\CLeds.h"
#include "..\CSerial.h"

#include "Travel.h"

FsmTravel::FsmTravel(CController& SMManager) :
CState(SMManager, STATE_TRAVEL),
LeftSide(LeftRear),
RightSide(RightRear)
{
}

void FsmTravel::OnEntry()
{
	static bool inited;
	
	if(!inited)
	{
		LeftSide.Init(LeftRear);
		RightSide.Init(RightRear);
		
		inited = true;
	}
	
	CSerial::is() << " FsmTravel::OnEntry()\r\n";
}

void FsmTravel::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TimerEvent:
			//run travel FSM
			//LeftSide.Run(512);
			RightSide.Run(512);
			break;
		case RockerEvent:
		case OutSideEvent:
		case SteeringEvent:
			m_SMManager.ChangeState(STATE_MANUAL, evt);
		break;
		case CampEvent:
			m_SMManager.ChangeState(STATE_CAMP, evt);
		break;
		case TravelEvent:
			Cio::is().TravelSwitches();
		break;
		case CalibrateEvent:
			m_SMManager.ChangeState(STATE_TRAVEL_CALIBRATE, evt);
		default:
		break;
	}
}

void FsmTravel::OnExit()
{
	CSerial::is() << " FsmTravel::OnExit()\r\n";
}