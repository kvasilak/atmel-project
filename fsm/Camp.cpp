/* 
* Camp.cpp
*
* Created: 2/1/2015 10:14:57 PM
* Author: keith
*/


#include "Camp.h"
#include "..\CController.h"


FsmCamp::FsmCamp(CController& SMManager) :
CState(SMManager, STATE_MANUAL)
{
}

void FsmCamp::OnEntry()
{
	//cout << "\nCStateIdle::OnEntry()" << endl;
}

void FsmCamp::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case UpEvent:
		case DownEvent:
			m_SMManager.ChangeState(STATE_MANUAL);
		break;
		case TravelEvent:
			m_SMManager.ChangeState(STATE_TRAVEL);
		break;
		default:
		break;
	}
}

void FsmCamp::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}