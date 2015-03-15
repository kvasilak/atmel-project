/* 
* FSMCampCal.cpp
*
* Created: 2/6/2015 9:33:22 PM
* Author: keith
*/


#include "CampCal.h"
#include "..\CController.h"


FSMCampCal::FSMCampCal(CController& SMManager) :
CState(SMManager, eStates::STATE_MANUAL)
{
}

void FSMCampCal::OnEntry()
{
	//cout << "\nCStateIdle::OnEntry()" << endl;
}

void FSMCampCal::HandleEvent(eEvents evt)
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

void FSMCampCal::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}
