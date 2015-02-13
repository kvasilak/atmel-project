/* 
* CController.cpp
*
* Created: 1/24/2015 8:13:35 PM
* Author: keith
*/


#include "CController.h"


// default constructor
CController::CController() :
m_stateManual(*this), //call the state constructor with a pointer to the manager class ( this )
m_stateTravel(*this), 
m_stateCamp(*this),
m_stateManualCal(*this),
m_stateTravelCal(*this),
m_stateCampCal(*this),
m_CurrState(STATE_MANUAL)
{
	//this order determines the state machine order
	m_StateList[0] = &m_stateManual;
	m_StateList[1] = &m_stateTravel;
	m_StateList[2] = &m_stateCamp;
	m_StateList[3] = &m_stateManualCal;
	m_StateList[4] = &m_stateTravelCal;
	m_StateList[5] = &m_stateCampCal;
	
	m_StateList[m_CurrState]->OnEntry();
} 


void CController::ScheduleEvent(EVENT evt)
{
	//cout << "Event received: " << events[evt] << endl;
	m_StateList[m_CurrState]->HandleEvent(evt);
}

//Exit the old state, enter the new state
void CController::ChangeState(STATE newState)
{
	// put up a warning if the function is called
	//from the entry or exit function.. not legal! 
	//(guard is not thread safe)
	static bool entered = false;
	if(entered)
	{
		;//DO NOT Call from OnEntry() or OnExit()!!
	}
	
	entered = true;
	
	if(m_CurrState != newState)
	{
		m_StateList[m_CurrState]->OnExit();
		
		m_CurrState = newState;
		
		m_StateList[m_CurrState]->OnEntry();
	}
	
	entered = false;
}


//switch to manual mode if user presses any manual button
//Manual buttons are; up, down, remote up, remote down, remote ru/lu/rd/ld
//
//switch to camp mode if user presses camp button and no manual button is pressed
//
//switch to travel mode if user presses travel button and no manual button is pressed
void CController::GetMode()
{
	
	//any change in rocker position shall result in a manual mode event
	//if we are already in manual mode nothing changes
	if(true)//Rocker.changed())
	{
		ScheduleEvent(ManualEvent);
		//HandleRockerswitch();
	}
	
}

//called on a 100ms timer
void CController::Run()
{
	ScheduleEvent(TimerEvent);
}


