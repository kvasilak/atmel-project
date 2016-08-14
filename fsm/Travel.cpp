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
	CLeds::is().TravelOKOn();
	LeftSide.Init(LeftRear);
	RightSide.Init(RightRear);

	LeftSide.SetLongFilter(false);
	RightSide.SetLongFilter(false);
	
	Starting = true;
	
	CSerial::is() << " FsmTravel::OnEntry()\r\n";
	//CSerial::is() << "Travel Cal vals; Left, " << nvm::is().GetLeftTravel() << ", Right, " << nvm::is().GetRightTravel() << "\n";
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
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::SteeringEvent:
			Cio::is().SteeringRemote();
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::CampEvent:
			m_SMManager.ChangeState(eStates::STATE_CAMP);
		break;
		case eEvents::TravelEvent:
			if(Cio::is().TravelSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_MANUAL);
			}
		break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL_CALIBRATE);
			break;
		case eEvents::IgnitionOnEvent:
			Cio::is().Awake = true;
			Cio::is().Wakeup();
				
			CLeds::is().TravelOKOn();
			
			//reset so we get back to ride height quickly
			LeftSide.AtHeight(false);
			RightSide.AtHeight(false);
		
			LeftSide.SetLongFilter(false);
			RightSide.SetLongFilter(false);
			
			Starting = true;
			CSerial::is() << " FsmTravel::Ignition On\r\n";
			
			break;
		case eEvents::IgnitionOffEvent:
			Cio::is().Awake = false;
			CSerial::is() << " FsmTravel::Ignition Off\r\n";
		
			Cio::is().Sleep();
			break;
		case eEvents::ButtonWakeEvent:
			Cio::is().Wakeup();
		
			Cio::is().ResetButtons();
			CSerial::is() << " FsmCamp::Button wake\r\n";
			break;
		default:
		break;
	}
}

void FsmTravel::OnExit()
{
	CSerial::is() << " FsmTravel::OnExit()\r\n";
	CLeds::is().TravelOKOff();
}