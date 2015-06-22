/* 
* Camp.cpp
*
* Created: 2/1/2015 10:14:57 PM
* Author: keith
*/


#include "Camp.h"
#include "..\CController.h"
#include "..\CLeds.h"
#include "..\CSerial.h"

FsmCamp::FsmCamp(CController& SMManager) :
CState(SMManager, eStates::STATE_CAMP)
{
}

void FsmCamp::OnEntry()
{
	CSerial::is() << " FsmCamp::OnEntry()\r\n";
	start = CTimer::GetTick();
}

void FsmCamp::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run camp FSM 
			if(CTimer::IsTimedOut(1000, start))
			{
				CMMA8451::is().ReadXYZ();

				CMMA8451::is().writeRegister8(0x2A, 0x3d); //slow rate, low noise, Active
				
				CMMA8451::is().writeRegister8(0x2B, 2); //high res
				
				start = CTimer::GetTick();
			}
		break;
		case eEvents::RockerEvent:
		case eEvents::OutSideEvent:
		case eEvents::SteeringEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::CampEvent:
			Cio::is().CampSwitches();
		break;
		case eEvents::TravelEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL, evt);
		break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_CAMP_CALIBRATE);
		break;
		case IgnitionOnEvent:
			break;
		case IgnitionOffEvent:
			break;
		default:
		break;
	}
}

void FsmCamp::OnExit()
{
	CSerial::is() << " FsmCamp::OnExit()\r\n";
}