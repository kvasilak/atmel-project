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


// default constructor
CController::CController() :
m_stateManual(*this), //call the state constructor with a pointer to the manager class ( this )
m_stateTravel(*this), 
m_stateCamp(*this),
m_stateManualCal(*this),
m_stateTravelCal(*this),
m_stateCampCal(*this),
m_CurrState(eStates::STATE_MANUAL)
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
	t2 = CTimer::GetTick();
	
	//update the rocker switch state
	ScheduleEvent(eEvents::RockerEvent);
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
		"Calibrate Event"
	};

	//Don't show timer events, too much noise
	if(evt > eEvents::TimerEvent)
	{
		CSerial::is() << events[(int)evt] << "\n";
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
	if(Cio::is().RockerChanged())
	{
		ScheduleEvent(eEvents::RockerEvent);
	}
	
	if(Cio::is().OutSideRemoteChanged())
	{
		ScheduleEvent(eEvents::OutSideEvent);
	}
	
	if(Cio::is().SteeringRemoteChanged())
	{
		ScheduleEvent(eEvents::SteeringEvent);
	}

 	if(Cio::is().CampChanged())
 	{
 		ScheduleEvent(eEvents::CampEvent);
 	}

	if(Cio::is().TravelChanged())
	{
		ScheduleEvent(eEvents::TravelEvent);
	}
	
}

//called on a 100ms timer
void CController::Run()
{
	//uint8_t me;
	Cio::is().Run();
	
	CheckEvent();
	
	if(CTimer::IsTimedOut(100, Time))
	{
		ScheduleEvent(eEvents::TimerEvent);
		Time = CTimer::GetTick();
	}
	
	if(CTimer::IsTimedOut(1000, t2))
	{
		
		CMMA8451::is().ReadXYZ();

		
		CMMA8451::is().writeRegister8(0x2A, 0x3d); //slow rate, low noise, Active
		
		CMMA8451::is().writeRegister8(0x2B, 2); //high res
		
		t2 = CTimer::GetTick();
	}
}


