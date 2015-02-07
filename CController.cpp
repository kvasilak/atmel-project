/* 
* CController.cpp
*
* Created: 1/24/2015 8:13:35 PM
* Author: keith
*/


#include "CController.h"

// default constructor
CController::CController() :
m_stateCamp(*this), //call the state constructor with a pointer to the manager class ( this )
m_stateManual(*this), 
m_stateTravel(*this), 
m_stateCampCal(*this),
m_stateTravelCal(*this),
m_stateManualCal(*this),
m_pCurrState(0)
{
	ChangeState(STATE_MANUAL);
} //CController

//switch to manual mode if user presses any manual button
//Manual buttons are; up, down, remote up, remote down, remote ru/lu/rd/ld
//
//switch to camp mode if user presses camp button and no manual button is pressed
//
//switch to travel mode if user presses travel button and no manual button is pressed
void CController::GetMode()
{
	
}

void CController::ScheduleEvent(EVENT evt)
{
	//cout << "Event received: " << events[evt] << endl;
	m_pCurrState->HandleEvent(evt);
}

void CController::ChangeState(STATE newState)
{
	//state order must match states.h
	static CState *m_stateList[] = {&m_stateManual, &m_stateTravel, &m_stateCamp, &m_stateManualCal, &m_stateTravelCal, &m_stateCampCal,};
		
	if(m_pCurrState)
	{
		m_pCurrState->OnExit();
		m_prevStateID = m_pCurrState->GetID();
	}
	
	m_pCurrState = m_stateList[newState];
	
	//switch(newState)
	//{
		//case STATE_MANUAL:
			//m_pCurrState = &m_stateManual;
		//break;
		//case STATE_TRAVEL:
			//m_pCurrState = &m_stateTravel;
		//break;
		//case STATE_CAMP:
			//m_pCurrState = &m_stateCamp;
		//break;
		//case STATE_CAMP_CALIBRATE:
			//m_pCurrState = &m_stateCampCal;
		//break;
		//case STATE_TRAVEL_CALIBRATE:
			//m_pCurrState = &m_stateTravelCal;
		//break;
		//case STATE_MANUAL_CALIBRATE:
			//m_pCurrState = &m_stateManualCal;
		//break;
		//default:
			//m_pCurrState = &m_stateManual;
	//}
	
	m_pCurrState->OnEntry();
	
}

