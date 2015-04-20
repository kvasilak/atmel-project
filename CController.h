/* 
* CController.h
*
* Created: 1/24/2015 8:13:35 PM
* Author: keith
*/


#ifndef __CCONTROLLER_H__
#define __CCONTROLLER_H__
#include <stddef.h>
#include "fsm/Events.h"
#include "fsm/States.h"

//include state classes
#include "Camp.h"
#include "Manual.h"
#include "Travel.h"
#include "CampCal.h"
#include "dance.h"
#include "TravelCal.h"

#include "Cio.h"

class CController 
{

public:
	CController();
	
	void ScheduleEvent(eEvents evt);
	
	void Run();
	void Init();
	
	private:
	
	//FSM specific //////////////////
	
	friend class FsmManual;
	friend class FsmTravel;
	friend class FsmCamp;
	friend class FSMDance;
	friend class FSMTravelCal;
	friend class FSMCampCal;
	

	
	FsmManual		m_stateManual;
	FsmTravel		m_stateTravel;
	FsmCamp			m_stateCamp;

	FSMDance	m_stateManualCal;
	FSMTravelCal	m_stateTravelCal;
	FSMCampCal		m_stateCampCal;

	//The list of states in the same order as in states.h
	CState *m_StateList[6];
	
	/////////////////////////////

	CController(const CController&);
	CController& operator=(const CController&);

	eStates m_CurrState;

	void ChangeState(eStates newState, eEvents evt = eEvents::NoEvent);

	void CheckEvent();

	void LeftUp();
	void LeftDown();
	void RightUp();
	void RightDown();
	void LeftHold();
	void RightHold();

	bool IsTimedOut();
	
	uint32_t Time;
	
}; //CController


#endif //__CCONTROLLER_H__
