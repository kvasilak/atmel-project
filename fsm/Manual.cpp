/* 
* Manual.cpp
*
* Created: 2/1/2015 10:14:32 PM
* Author: keith
*/

#include <avr/sleep.h>
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
	
	Cio::is().AllOff();
	Cio::is().FillReset();
	CSerial::is() << " FsmManual::OnEntry()\r\n";
}

void FsmManual::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TravelEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL, evt);
			break;
		case eEvents::CampEvent:
			m_SMManager.ChangeState(eStates::STATE_CAMP, evt);
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
		case eEvents::IgnitionOnEvent:
			Cio::is().AllOff();
			Cio::is().FillReset();
			CSerial::is() << " FsmManual::Ignition On\r\n";
			break;
		case eEvents::IgnitionOffEvent:
			Cio::is().AllOff();
			Cio::is().FillReset();
			CSerial::is() << " FsmManual::Ignition Off\r\n";
			
			//Cio::is().LowPower();
			Cio::is().Sleep();
			//set_sleep_mode(<mode>);
			//sleep_mode();
			break;
		default:
		break;
	}
}

void FsmManual::OnExit()
{
	CSerial::is() << " FsmManual::OnExit()\r\n";
	
	Cio::is().AllOff();
}

