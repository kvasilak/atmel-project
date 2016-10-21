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

static const uint32_t ONESECOND = 1000;
static const uint32_t ONEMINUTE = 60 * ONESECOND;

static const uint32_t BUTTONHOLDTIME = 300 * ONESECOND;
static const uint32_t BUTTONWAKETIME = 5 * ONEMINUTE; //how long to stay awake after a button wake event

FsmManual::FsmManual(CController& SMManager) :
CState(SMManager, eStates::STATE_MANUAL),
WakeTime(BUTTONHOLDTIME)
{
}

void FsmManual::OnEntry()
{
	CLeds::is().CampOff();
	CLeds::is().TravelOff();
	
	Cio::is().AllOff();
	//Cio::is().ResetButtons();
	CSerial::is() << " FsmManual::OnEntry()\r\n";
	
	ButtonWakeStart = CTimer::GetTick();
	WakeTime = BUTTONHOLDTIME;
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
			
			if(Cio::is().IsHolding()) 
			{
				CSerial::is() << "Rock is holding\n";
				WakeTime = BUTTONHOLDTIME;
			}
			
			ButtonWakeStart = CTimer::GetTick();

			break;
		case eEvents::OutSideEvent:
			Cio::is().OutsideRemote();
			
			if(Cio::is().IsHolding()) 
			{
				CSerial::is() << "OS is holding\n";
				WakeTime = BUTTONHOLDTIME;
			}
			
			ButtonWakeStart = CTimer::GetTick();
			break;
		case eEvents::SteeringEvent:
			Cio::is().SteeringRemote();
			//Steering remote does not wake controller
			break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL_CALIBRATE);
			break;
		case eEvents::TimerEvent:
			//we are awake due to a button press
			//timeout and sleep
			if(Cio::is().ButtonWake)
			{
				if(CTimer::IsTimedOut(WakeTime, ButtonWakeStart))
				{
					Cio::is().ButtonWake = false;
					
					m_SMManager.ScheduleEvent(eEvents::IgnitionOffEvent);
				}
			}
			
			break;
		case eEvents::IgnitionOnEvent:
			if(Cio::is().Awake == false)
			{
				Cio::is().ButtonWake = false;
				Cio::is().Awake = true;
				Cio::is().Wakeup();
				
				Cio::is().UpdateButtons();

				CSerial::is() << " FsmManual::Ignition On\r\n";
			}
			else
			{
				CSerial::is() << "Awake already\n";

				Cio::is().ButtonWake = false;
				
			}
			break;
		case eEvents::IgnitionOffEvent:
			Cio::is().Awake = false;

			CSerial::is() << " FsmManual::Ignition Off\r\n";
			
			Cio::is().Sleep();
			break;
		case eEvents::ButtonWakeEvent:
		
		Cio::is().ButtonWake = true;
		
		WakeTime = BUTTONWAKETIME;
		
		ButtonWakeStart = CTimer::GetTick();
		
		if(!Cio::is().Awake)
		{
			m_SMManager.ScheduleEvent(eEvents::IgnitionOnEvent);

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

