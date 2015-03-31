/* 
* Manual.cpp
*
* Created: 2/1/2015 10:14:32 PM
* Author: keith
*/


#include "Manual.h"
#include "..\CController.h"
#include "..\Cio.h"
#include "..\CSerial.h"
#include "..\CLeds.h"

FsmManual::FsmManual(CController& SMManager) :
CState(SMManager, eStates::STATE_MANUAL)
{
}

void FsmManual::OnEntry()
{
	CLeds::is().CampOff();
	CLeds::is().TravelOKOff();
	
	CSerial::is() << " FsmManual::OnEntry()\r\n";
}

void FsmManual::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TravelEvent:
			//change state if no manual buttons are active
			if(Cio::is().IsHolding())
			{
				m_SMManager.ChangeState(eStates::STATE_TRAVEL, evt);
			}
		break;
		case eEvents::CampEvent:
			//change state if no manual buttons are active
			if(Cio::is().IsHolding())
			{
				m_SMManager.ChangeState(eStates::STATE_CAMP, evt);
			}
		break;
		case eEvents::RockerEvent:
			Cio::is().RockerSwitch();
		break;
		case eEvents::OutSideEvent:
			Cio::is().OutsideRemote();
		break;
		case eEvents::SteeringEvent:
			Cio::is().SteeringRemote();
		break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL_CALIBRATE);
		break;
		case eEvents::TimerEvent:
			//run travel FSM?
		break;
		default:
		break;
	}
}

void FsmManual::OnExit()
{
	CSerial::is() << " FsmManual::OnExit()\r\n";
}

