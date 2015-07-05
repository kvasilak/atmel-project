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
RightSide(RightRear),
Starting(true)
{
}

void FsmTravel::OnEntry()
{
	LeftSide.Init(LeftRear);
	RightSide.Init(RightRear);

	LeftSide.SetLongFilter(false);
	RightSide.SetLongFilter(false);
	
	Starting = true;
	
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
			
			if(Starting)
			{
				//switch to a slower filter as soon as we reach ride height
				if(LeftSide.AtHeight() && RightSide.AtHeight() )
				{
					CSerial::is() << " *******Setting long travel Filter ******\r\n";
					LeftSide.SetLongFilter(true);
					RightSide.SetLongFilter(true);
					
					Starting = false;
				}
			}
			
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
			break;
		case eEvents::IgnitionOnEvent:
			Cio::is().Wakeup();
			
			//reset so we get back to ride height quickly
			LeftSide.AtHeight(false);
			RightSide.AtHeight(false);
		
			LeftSide.SetLongFilter(false);
			RightSide.SetLongFilter(false);
			
			Starting = true;
			CSerial::is() << " FsmTravel::Ignition On\r\n";
			break;
		case eEvents::IgnitionOffEvent:
			CSerial::is() << " FsmTravel::Ignition Off\r\n";
		
			Cio::is().Sleep();
			break;
		default:
		break;
	}
}

void FsmTravel::OnExit()
{
	CSerial::is() << " FsmTravel::OnExit()\r\n";
}