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
CState(SMManager, STATE_MANUAL)
{
}

void FsmManual::OnEntry()
{
	CLeds::is().CampOff();
	CLeds::is().TravelOKOff();
	
	CSerial::is() << " FsmManual::OnEntry()\r\n";
	
	Cio::is().RockerSwitch();
	Cio::is().OutsideRemote();
	Cio::is().SteeringRemote();
}

void FsmManual::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TravelEvent:
			//change state if no manual buttons are active
			if(Cio::is().IsHolding())
			{
				m_SMManager.ChangeState(STATE_TRAVEL);
			}
		break;
		case CampEvent:
			//change state if no manual buttons are active
			if(Cio::is().IsHolding())
			{
				m_SMManager.ChangeState(STATE_CAMP);
			}
		break;
		case RockerEvent:
			Cio::is().RockerSwitch();
		break;
		case OutSideEvent:
			Cio::is().OutsideRemote();
		break;
		case SteeringEvent:
			Cio::is().SteeringRemote();
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
	CSerial::is() << " FsmManual::OnExit()\r\n";
}

