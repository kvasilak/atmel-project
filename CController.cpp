/* 
* CController.cpp
*
* Created: 1/24/2015 8:13:35 PM
* Author: keith
*/


#include "CController.h"
#include "CSerial.h"
#include <avr/io.h>
#include "common.h"
#include "CTimer.h"
#include "MMA8451.h"
#include "CADC.h"

// default constructor
CController::CController() :
m_stateManual(*this), //call the state constructor with a pointer to the manager class ( this )
m_stateTravel(*this), 
m_stateCamp(*this),
m_stateManualCal(*this),
m_stateTravelCal(*this),
m_stateCampCal(*this),
m_CurrState(eStates::STATE_MANUAL),
IgnitionChangeStart(0),
ButtonWakeStart(0),
ButtonWakeSeconds(0),
IgnitionEventPending(false),
LastIgnitionOn(false),
ButtonWakeup(false)
{
	//this order determines the state machine order
	m_StateList[0] = &m_stateManual;
	m_StateList[1] = &m_stateTravel;
	m_StateList[2] = &m_stateCamp;
	m_StateList[3] = &m_stateManualCal;
	m_StateList[4] = &m_stateTravelCal;
	m_StateList[5] = &m_stateCampCal;
} 

void CController::Init()
{
	m_StateList[(int)m_CurrState]->OnEntry();
	Time = CTimer::GetTick();
	
	//update the rocker switch state
	ScheduleEvent(eEvents::RockerEvent);
	
	bool on = Cio::is().IsIgnitionOn();
	CSerial::is() <<"ignition " << on << "\n";
	
	
	//set the sleep flag so as will go to sleep after int 
	IgnitionEventPending = true;

	CLeds::is().ActiveOn();
	Cio::is().CompressorOn();
 		
  	Cio::is().Wakeup();
	Cio::is().Awake = true;
	
}

void CController::ScheduleEvent(eEvents evt)
{
	static const char *events[]  = {
		"No Event",
		"Timer Event",
		"Rocker Event",
		"Outside Event",
		"Steering Event",
		"Travel Event",
		"Camp Event",
		"Calibrate Event",
		"Ignition On Event",
		"Ignition Off Event",
		"Button Wake Event"
	};

	//Don't show timer events, too much noise
	if(evt > eEvents::TimerEvent)
	{
		CSerial::is() << "***State Change: " << events[(int)evt] << "\n";
	}
	
	m_StateList[(int)m_CurrState]->HandleEvent(evt);
}

//Exit the old state, enter the new state
void CController::ChangeState(eStates newState, eEvents evt)
{
	// put up a warning if the function is called
	//from the entry or exit function.. not legal! 
	//(guard is not thread safe)
	static bool entered = false;
	if(entered)
	{
		CSerial::is() << "DO NOT Call from OnEntry() or OnExit()!!\n";
	}
	
	entered = true;
	
	if(m_CurrState != newState)
	{
		m_StateList[(int)m_CurrState]->OnExit();
		
		m_CurrState = newState;
		
		m_StateList[(int)m_CurrState]->OnEntry();
		
		if(evt != eEvents::NoEvent)
		{
			m_StateList[(int)m_CurrState]->HandleEvent(evt);
		}
	}
	
	entered = false;
}


//check all inputs for a state change
//We will generate Manual, Travel and camp events
//Manual event anytime there is any change of the rocker or either remote
//Travel event for any change of the travel button
//Camp event for any change of the camp button
void CController::CheckEvent()
{
	if(Cio::IgnitionChanged)
	{
		CSerial::is() << "Ignition Changed\n";
		
		IgnitionChangeStart = CTimer::GetTick();
		
		LastIgnitionOn = Cio::is().IsIgnitionOn();
		
		Cio::IgnitionChanged = false;
		
		IgnitionEventPending = true;
	}
	
	
	if(IgnitionEventPending)
	{
		//Don't cycle power too fast
		if(CTimer::IsTimedOut(1000, IgnitionChangeStart))
		{
			CSerial::is() << "Ignition pending timeout\n";
			
			//Was this a true event? Or just noise
			if(LastIgnitionOn == Cio::is().IsIgnitionOn())
			{
				//LastIgnitionOn = Cio::is().IsIgnitionOn();
				
				IgnitionEventPending = false;
				
				if(LastIgnitionOn)
				{
					CSerial::is() << "IgnitionOnEvent\n";

					ScheduleEvent(eEvents::IgnitionOnEvent);
					
					ButtonWakeup = false;
				}
				else
				{
					CSerial::is() << "IgnitionOffEvent\n";										
					
					ScheduleEvent(eEvents::IgnitionOffEvent);
				}
			}
		}
	}

	if( Cio::ButtonChanged)
	{ 
		Cio::ButtonChanged = false;
		CSerial::is() << "Button wake\n";		
		
		if(Cio::is().Awake)
		{
			CSerial::is() << "Button restart\n";		
			ButtonWakeStart = CTimer::GetTick();
			ButtonWakeSeconds = 0;
		}
		else
		{
			CSerial::is() << "Button start\n";		
			
		
			ButtonWakeStart = CTimer::GetTick();
		
			ButtonWakeup = true;
			ButtonWakeSeconds = 0;
		
			ScheduleEvent(eEvents::ButtonWakeEvent);
		}
	}
	
	if(ButtonWakeup)
	{
		if(CTimer::IsTimedOut(1000, ButtonWakeStart))
		{
			ButtonWakeStart = CTimer::GetTick();		
			
			ButtonWakeSeconds++;
			
			//wait 5 minutes after last button press then resume sleep
			//if(ButtonWakeSeconds > 300)
			if(ButtonWakeSeconds > 10)
			{
				CSerial::is() << "Button wake timeout\n";
				ScheduleEvent(eEvents::IgnitionOffEvent);
			}
		}
	}
	
	if(Cio::is().RockerChanged())
	{
		ScheduleEvent(eEvents::RockerEvent);
	}
	
	if(Cio::is().OutSideRemoteChanged())
	{
		ScheduleEvent(eEvents::OutSideEvent);
	}
	
	//if(Cio::is().SteeringRemoteChanged())
	if(Cio::is().UpDownChanged())
	{
		ScheduleEvent(eEvents::SteeringEvent);
	}

 	if(Cio::is().CampChanged())
 	{
 		ScheduleEvent(eEvents::CampEvent);
 	}

	if(Cio::is().TravelChanged())
	{
		CSerial::is() << "***TravelChanged\n";	
		
		ScheduleEvent(eEvents::TravelEvent);
	}
	
	if(Cio::is().CalibrateChanged())
	{
		ScheduleEvent(eEvents::CalibrateEvent);
	}
	
}

//called on a 100ms timer
void CController::Run()
{
	Cio::is().Run();
	
	CheckEvent();
	
	if(CTimer::IsTimedOut(50, Time))
	{
		if( Cio::is().Awake)
		{
			ScheduleEvent(eEvents::TimerEvent);

			//LED brightness controlled by voltage on dimmer input
			//Todo Scale to resistor divider
			//Todo what to do about dash light dimmer
			//CLeds::is().Dim(CADC::is().GetDimmer());
		}
		Time = CTimer::GetTick();
	}
}


