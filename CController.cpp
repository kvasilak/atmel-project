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


//check all inputs for a state change
//We will generate Manual, Travel and camp events
//Manual event anytime there is any change of the rocker or either remote
//Travel event for any change of the travel button
//Camp event for any change of the camp button
void CController::CheckEvent()
{
/*	static bool OldRockerUp				= false;
	static bool OldRockerDown			= false;
	static bool OldInsideUp				= false;
	static bool OldInsideDown			= false;
	
	static bool OldOutsideLeftUp		= false;
	static bool OldOutsideLeftDown		= false;
	static bool OldOutsideRightUp		= false;
	static bool OldOutsideRightDown		= false;
	
	static bool OldCamp					= false;
	static bool OldInsideCamp			= false;
	
	static bool OldTravel				= false;
	static bool OldInsideTravel			= false;
	*/
	
/*	
	//Manual inputs
	bool ManualChanged  = OldRockerUp		!= RockerUp;
	ManualChanged |= OldRockerDown			!= RockerDown;
	ManualChanged |= OldInsideUp			!= InsideUp;
	ManualChanged |= OldInsideDown			!= InsideDown;

	ManualChanged |= OldOutsideLeftUp		!= OutsideLeftUp;
	ManualChanged |= OldOutsideLeftDown		!= OutsideLeftDown;
	ManualChanged |= OldOutsideRightUp		!= OutsideRightUp;
	ManualChanged |= OldOutsideRightDown	!= OutsideRightDown;
	
	//Update old values
	OldRockerUp			= RockerUp;
	OldRockerDown		= RockerDown;
	OldInsideUp			= InsideUp;
	OldInsideDown		= InsideDown;
	
	OldOutsideLeftUp	= OutsideLeftUp;
	OldOutsideLeftDown	= OutsideLeftDown;
	OldOutsideRightUp	= OutsideRightUp;
	OldOutsideRightDown	= OutsideRightDown;

	if(ManualChanged)
	{
		ScheduleEvent(ManualEvent);
	}
	
	//Camp inputs
	bool CampChanged	= OldCamp			!= Camp;
	CampChanged			|= OldInsideCamp	|= InsideCamp;
	
	if(CampChanged)
	{
		ScheduleEvent(CampEvent);
	}
	
	//Travel inputs
	bool TravelChanged	= OldTravel				!= Travel;
	TravelChanged		|= OldInsideTravel		!= InsideTravel;
	
	if(TravelChanged)
	{
		ScheduleEvent(TravelEvent);
	}
	*/
}

//called on a 100ms timer
void CController::Run()
{
	ScheduleEvent(TimerEvent);
}

/*
//Switches				//Switches
if(OldRockerUp != RockerUp)

OldRockerDown				RockerDown



//inside remote				//inside remote
OldInsideUp					InsideUp
OldInsideDown				InsideDown



//Outside Remote				//Outside Remote
OldOutsideLeftUp				OutsideLeftUp
OldOutsideLeftDown				OutsideLeftDown
OldOutsideRightUp				OutsideRightUp
OldOutsideRightDown				OutsideRightDown
*/
