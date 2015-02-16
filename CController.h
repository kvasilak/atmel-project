/* 
* CController.h
*
* Created: 1/24/2015 8:13:35 PM
* Author: keith
*/


#ifndef __CCONTROLLER_H__
#define __CCONTROLLER_H__
#include "fsm/Events.h"
#include "fsm/States.h"

//include state classes
#include "fsm/Camp.h"
#include "fsm/Manual.h"
#include "fsm/Travel.h"
#include "fsm/CampCal.h"
#include "fsm/ManualCal.h"
#include "fsm/TravelCal.h"

#include "Cio.h"

class CController 
{

public:
	CController();
	
	void ScheduleEvent(EVENT evt);
	
	void Run();

	private:
	
	//FSM specific //////////////////
	
	friend class FsmManual;
	friend class FsmTravel;
	friend class FsmCamp;
	friend class FSMManualCal;
	friend class FSMTravelCal;
	friend class FSMCampCal;
	

	
	FsmManual		m_stateManual;
	FsmTravel		m_stateTravel;
	FsmCamp			m_stateCamp;

	FSMManualCal	m_stateManualCal;
	FSMTravelCal	m_stateTravelCal;
	FSMCampCal		m_stateCampCal;

	//The list of states in the same order as in states.h
	CState *m_StateList[6];
	
	/////////////////////////////

	CController(const CController&);
	CController& operator=(const CController&);

	STATE m_CurrState;

	void ChangeState(STATE newState);

	void CheckEvent();

	void LeftUp();
	void LeftDown();
	void RightUp();
	void RightDown();
	void LeftHold();
	void RightHold();

	bool IsTimedOut();
	
}; //CController


#endif //__CCONTROLLER_H__
