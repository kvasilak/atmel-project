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
#include "CADC.h"

static const uint32_t ONESECOND = 1000;
static const uint32_t ONEMINUTE = 60 * ONESECOND;

static const uint32_t BUTTONHOLDTIME = 30 * ONESECOND; //how long to stay awake in HOLD
static const uint32_t REMOTEWAKETIME = 1 * ONEMINUTE; //how long to stay awake if outside remote pressed
static const uint32_t BUTTONWAKETIME = 1/*5*/ * ONEMINUTE; //how long to stay awake in RAISE or LOWER

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
	CSerial::is() << " FsmManual::OnEntry()\n";
	
	Blink = CTimer::GetTick();
	
	ButtonWakeStart = CTimer::GetTick();
	WakeTime = BUTTONHOLDTIME;
}

void FsmManual::HandleEvent(eEvents evt)
{
	//uint16_t Lheight, Rheight;
	
	switch(evt)
	{
		case eEvents::TravelEvent:
			if(Cio::is().TravelSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_TRAVEL);
			}
			break;
		case eEvents::CampEvent:
        //ignore button up events
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
			else
			{
				WakeTime = BUTTONWAKETIME;
			}
			
			ButtonWakeStart = CTimer::GetTick();

			break;
		case eEvents::OutSideEvent:
        
            //if filling or dumping switch to holding
            if(false == Cio::is().IsHolding()) 
            {
                Cio::is().AllOff();
            }
        
			Cio::is().OutsideRemote();
			
			WakeTime = REMOTEWAKETIME;
			
			CSerial::is() << "OS is holding\n";
			
			ButtonWakeStart = CTimer::GetTick();
			break;
            //Steering remote no longer supported
		//case eEvents::SteeringEvent:
			//Cio::is().SteeringRemote();
			////Steering remote does not wake controller
			//WakeTime = REMOTEWAKETIME;
			//break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL_CALIBRATE);
			break;
            //stay awake forever on buttonpress. timeout if no button presssed
		case eEvents::TimerEvent:
			//we are awake due to a button press
			//timeout and sleep
			if(Cio::is().ButtonWake)
			{
			    if(CTimer::IsTimedOut(WakeTime, ButtonWakeStart))
			    {
                    //CSerial::is() << " FsmManual::button wake timneout\r\n";
                    
                    //Cio::is().ButtonWake = false; //only check once
                         
                    if( Cio::is().IsHolding() ) //timeout only if nothing pressed
                    {
					    Cio::is().ButtonWake = false;
					
					    m_SMManager.ScheduleEvent(eEvents::IgnitionOffEvent);
                    }                        
			    }
			}

			break;
		case eEvents::IgnitionOnEvent:
			Cio::is().ButtonWake = false;
			//Cio::is().Awake = true;
			Cio::is().Wakeup();
				
			Cio::is().UpdateButtons();

			CSerial::is() << " FsmManual::Ignition On\r\n";

			break;
		case eEvents::IgnitionOffEvent:
			CSerial::is() << " FsmManual::Ignition Off\r\n";
			
			Cio::is().Sleep();
			break;
		case eEvents::ButtonWakeEvent:

		    if(!Cio::is().Awake)
		    {
			    Cio::is().ButtonWake = true;
			
			    ButtonWakeStart = CTimer::GetTick();
			
			    Cio::is().Wakeup();
				
			    Cio::is().UpdateButtons();

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

