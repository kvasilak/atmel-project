/* 
* FSMManualCal.cpp
*
* Created: 2/6/2015 9:34:03 PM
* Author: keith
*/


#include "ManualCal.h"
#include "..\CController.h"


FSMManualCal::FSMManualCal(CController& SMManager) :
CState(SMManager, STATE_MANUAL)
{
}

void FSMManualCal::OnEntry()
{
	//cout << "\nCStateIdle::OnEntry()" << endl;
}

void FSMManualCal::HandleEvent(EVENT evt)
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

void FSMManualCal::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}
