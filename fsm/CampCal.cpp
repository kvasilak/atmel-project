/* 
* FSMCampCal.cpp
*
* Created: 2/6/2015 9:33:22 PM
* Author: keith
*/


#include "CampCal.h"
#include "..\CController.h"
#include "..\Cio.h"
#include "..\CSerial.h"
#include "..\CLeds.h"
#include "..\CTimer.h"

FSMCampCal::FSMCampCal(CController& SMManager) :
CState(SMManager, eStates::STATE_MANUAL)
{
}

void FSMCampCal::OnEntry()
{
	CSerial::is() << " FSMTravelCal::OnEntry()\r\n";
	Start = CTimer::GetTick();
}

void FSMCampCal::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run camp FSM
			break;
		case eEvents::CalibrateEvent:
			if(CTimer::IsTimedOut(5000, Start))
			{
				CSerial::is() << "Manual cal, Cal event\n";
				m_SMManager.ChangeState(eStates::STATE_CAMP);
			}
		default:
		break;
	}
}

void FSMCampCal::OnExit()
{
	CSerial::is() << " FSMTravelCal::OnExit()\r\n";

}
