#ifndef STATE_H
#define STATE_H

#include "States.h"
#include "Events.h"

class CSMManager;

class CState
{
	protected:
	// States inherited from this class can access State Manager
	// through this member
	CSMManager& m_SMManager;
	STATE m_nID;
	
	public:
	CState(CSMManager& SMManager, STATE nID) : m_SMManager(SMManager), m_nID(nID){}
	const STATE GetID() const {return m_nID;}
	
	virtual void OnEntry() = 0;
	
	// this function is executed each time current state receives some event
	virtual void HandleEvent(EVENT evt) = 0;
	
	virtual void OnExit() = 0;
};

#endif // STATE_H
