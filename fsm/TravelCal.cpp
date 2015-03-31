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
	Blink = CTimer::GetTick();
}

void FSMTravelCal::HandleEvent(eEvents evt)
{
	static bool Active = false;
	
	switch(evt)
	{
		case eEvents::TimerEvent:
			if(CTimer::IsTimedOut(250, Blink))
			{
				if(Active)
				{
					CLeds::is().ActiveOn();
					Active = !Active;
				}
				else
				{
					CLeds::is().ActiveOff();
					Active = !Active;
				}
				
				Blink = CTimer::GetTick();
			}
			break;
			break;
		case eEvents::OutSideEvent:
			CSerial::is() << "Travel cal, outside event\n";
			Cio::is().OutsideRemote();
			break;
		case eEvents::CalibrateEvent:
			//user pressed the the cal button again, save settings
			CSerial::is() << "Travel cal, Cal event\n";
			
			m_SMManager.ChangeState(eStates::STATE_TRAVEL);
		default:
		break;
	}
}

void FSMTravelCal::OnExit()
{
	CSerial::is() << " FSMTravelCal::OnExit()\r\n";
}
