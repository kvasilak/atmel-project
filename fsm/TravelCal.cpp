/* 
* FSMTravelCal.cpp
*
* Created: 2/6/2015 9:33:41 PM
* Author: keith
*/


#include "TravelCal.h"
#include "..\CController.h"
#include "..\CController.h"
#include "..\Cio.h"
#include "..\CSerial.h"
#include "..\CLeds.h"
#include "..\CTimer.h"

FSMTravelCal::FSMTravelCal(CController& SMManager) :
CState(SMManager, eStates::STATE_TRAVEL_CALIBRATE)
{
}

void FSMTravelCal::OnEntry()
{
	CSerial::is() << " FSMTravelCal::OnEntry()\r\n";
	Start = CTimer::GetTick();
}

void FSMTravelCal::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run camp FSM
			break;
		case eEvents::OutSideEvent:
			CSerial::is() << "Travel cal, outside event\n";
			Cio::is().OutsideRemote();
			break;
		case eEvents::CalibrateEvent:
			if(CTimer::IsTimedOut(5000, Start))
			{
				CSerial::is() << "Manual cal, Cal event\n";

				m_SMManager.ChangeState(eStates::STATE_TRAVEL);
			}
		default:
		break;
	}
}

void FSMTravelCal::OnExit()
{
	CSerial::is() << " FSMTravelCal::OnExit()\r\n";
}
