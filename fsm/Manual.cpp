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
	//Cio::is().ResetButtons();
	CSerial::is() << " FsmManual::OnEntry()\r\n";
}

void FsmManual::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TravelEvent:
			if(Cio::is().TravelSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_TRAVEL);
			}
			break;
		case eEvents::CampEvent:
			if(Cio::is().CampSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_CAMP);
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
			//No dancing m_SMManager.ChangeState(eStates::STATE_MANUAL_CALIBRATE);
			break;
		case eEvents::TimerEvent:
			//nothing to do here
			break;
		case eEvents::IgnitionOnEvent:
			if(Cio::is().Awake == false)
			{
				Cio::is().Awake = true;
				Cio::is().Wakeup();
				
				CSerial::is() << " FsmManual::Ignition On\r\n";
			}
			else
			{
				CSerial::is() << "Awake already\n";
			}
			break;
		case eEvents::IgnitionOffEvent:
			Cio::is().Awake = false;

			CSerial::is() << " FsmManual::Ignition Off\r\n";
			
			Cio::is().Sleep();
			break;
		case eEvents::ButtonWakeEvent:
			if(Cio::is().Awake == false)
			{
				Cio::is().Awake = true;
				Cio::is().Wakeup();
		
				//Cio::is().ResetButtons();
				CSerial::is() << " FsmManual::BUtton Wake\r\n";
			}
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

