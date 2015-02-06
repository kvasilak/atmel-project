/*
* state machine base class
* 
* from; http://bojan-komazec.blogspot.com/2011/12/finite-state-machine-in-c.html
*/

#ifndef STATE_H
#define STATE_H

#include "States.h"
#include "Events.h"

class CController;

class CState
{
	protected:
	// States inherited from this class can access State Manager
	// through this member
	CController& m_SMManager;
	STATE m_nID;
	
	public:
	CState(CController& CController, STATE nID) : m_SMManager(CController), m_nID(nID){}
	const STATE GetID() const {return m_nID;}
	
	virtual void OnEntry() = 0;
	
	// this function is executed each time current state receives some event
	virtual void HandleEvent(EVENT evt) = 0;
	
	virtual void OnExit() = 0;
};

#endif // STATE_H
