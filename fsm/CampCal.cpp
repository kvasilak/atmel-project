/* 
* FSMCampCal.cpp
*
* Created: 2/6/2015 9:33:22 PM
* Author: keith
*/


#include "CampCal.h"
#include "..\CController.h"


FSMCampCal::FSMCampCal(CController& SMManager) :
CState(SMManager, STATE_MANUAL)
{
}

void FSMCampCal::OnEntry()
{
	//cout << "\nCStateIdle::OnEntry()" << endl;
}

void FSMCampCal::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TimerEvent:
		//run camp FSM
		case ManualEvent:
		m_SMManager.ChangeState(STATE_MANUAL);
		break;
		case TravelEvent:
		m_SMManager.ChangeState(STATE_TRAVEL);
		break;
		case CalibrateEvent:
		m_SMManager.ChangeState(STATE_CAMP_CALIBRATE);
		default:
		break;
	}
}

void FSMCampCal::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}
