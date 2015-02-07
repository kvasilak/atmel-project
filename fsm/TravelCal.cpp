/* 
* FSMTravelCal.cpp
*
* Created: 2/6/2015 9:33:41 PM
* Author: keith
*/


#include "TravelCal.h"
#include "..\CController.h"


FSMTravelCal::FSMTravelCal(CController& SMManager) :
CState(SMManager, STATE_TRAVEL_CALIBRATE)
{
}

void FSMTravelCal::OnEntry()
{
	//cout << "\nCStateIdle::OnEntry()" << endl;
}

void FSMTravelCal::HandleEvent(EVENT evt)
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

void FSMTravelCal::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}
