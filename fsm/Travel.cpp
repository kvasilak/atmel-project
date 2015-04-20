/* 
* Travel.cpp
*
* Created: 2/1/2015 10:14:47 PM
* Author: keith
*/



#include "CController.h"
#include "CLeds.h"
#include "CSerial.h"

#include "Travel.h"
#include "nvm.h"

FsmTravel::FsmTravel(CController& SMManager) :
CState(SMManager, eStates::STATE_TRAVEL),
LeftSide(LeftRear),
RightSide(RightRear)
{
}

void FsmTravel::OnEntry()
{
	LeftSide.Init(LeftRear);
	RightSide.Init(RightRear);

	
	CSerial::is() << " FsmTravel::OnEntry()\r\n";
	CSerial::is() << "Travel Cal vals; Left, " << nvm::is().GetLeftTravel() << ", Right, " << nvm::is().GetRightTravel() << "\n";
}

void FsmTravel::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run travel FSM
			LeftSide.Run(nvm::is().GetLeftTravel());
			RightSide.Run(nvm::is().GetRightTravel());
			break;
		case eEvents::RockerEvent:
		case eEvents::OutSideEvent:
		case eEvents::SteeringEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::CampEvent:
			m_SMManager.ChangeState(eStates::STATE_CAMP, evt);
		break;
		case eEvents::TravelEvent:
			Cio::is().TravelSwitches();
		break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL_CALIBRATE);
		default:
		break;
	}
}

void FsmTravel::OnExit()
{
	CSerial::is() << " FsmTravel::OnExit()\r\n";
}