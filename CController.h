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

class CController 
{

public:
	CController();
	
	void ScheduleEvent(EVENT evt);

	private:
	
	//FSM specific //////////////////
	friend class FsmCamp;
	friend class FsmManual;
	friend class FsmTravel;


	FsmCamp m_stateCamp;
	FsmManual m_stateManual;
	FsmTravel m_stateTravel;
	/////////////////////////////

	CController(const CController&);
	CController& operator=(const CController&);

	CState* m_pCurrState;
	STATE m_prevStateID;

	void ChangeState(STATE newState);

	void GetMode();
	void CheckEvents();

	void LeftUp();
	void LeftDown();
	void RightUp();
	void RightDown();
	void LeftHold();
	void RightHold();

	
}; //CController

#endif //__CCONTROLLER_H__
