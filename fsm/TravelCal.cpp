/* 
* FSMTravelCal.cpp
*
* Created: 2/6/2015 9:33:41 PM
* Author: keith
*/


#include "TravelCal.h"
#include "..\CController.h"


FSMTravelCal::FSMTravelCal(CController& SMManager) :
CState(SMManager, eStates::STATE_TRAVEL_CALIBRATE)
{
}

void FSMTravelCal::OnEntry()
{
	//cout << "\nCStateIdle::OnEntry()" << endl;
}

void FSMTravelCal::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
		//run camp FSM
		case eEvents::RockerEvent:
		case eEvents::OutSideEvent:
		case eEvents::SteeringEvent:
		m_SMManager.ChangeState(eStates::STATE_MANUAL);
		break;
		case eEvents::TravelEvent:
		m_SMManager.ChangeState(eStates::STATE_TRAVEL);
		break;
		case eEvents::CalibrateEvent:
		m_SMManager.ChangeState(eStates::STATE_CAMP_CALIBRATE);
		default:
		break;
	}
}

void FSMTravelCal::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}
