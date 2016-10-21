/* 
* FSMTravelCal.cpp
*
* Created: 2/6/2015 9:33:41 PM
* Author: keith
*/

#include <avr/eeprom.h>
#include "TravelCal.h"
#include "CController.h"
#include "CController.h"
#include "Cio.h"
#include "CSerial.h"
#include "CLeds.h"
#include "CTimer.h"
#include "CADC.h"
#include "nvm.h"

FSMTravelCal::FSMTravelCal(CController& SMManager) :
CState(SMManager, eStates::STATE_TRAVEL_CALIBRATE)
{
}

void FSMTravelCal::OnEntry()
{
	CSerial::is() << "FSMTravelCal::OnEntry()\r\n";
	Blink = CTimer::GetTick();
	
	Cio::is().AllOff();
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
					CLeds::is().TravelOn();
					Active = !Active;
				}
				else
				{
					CLeds::is().ActiveOff();
					CLeds::is().TravelOff();
					Active = !Active;
				}
				
				Blink = CTimer::GetTick();
			}
			break;
		case eEvents::OutSideEvent:
			//allow user to change height with outside remote
			CSerial::is() << "Travel cal, outside event\n";
			Cio::is().OutsideRemote();
			break;
		case eEvents::CalibrateEvent:
			//user pressed the the cal button again, save settings
			//
			//nvm::is().SetLeftTravel(CADC::is().GetLeftHeight());
			//nvm::is().SetRightTravel(CADC::is().GetRightHeight());
			//
			//nvm::is().Save();
			//
			//CSerial::is() << "Travel Cal Complete; Left, " << nvm::is().GetLeftTravel() << ", Right, " << nvm::is().GetRightTravel() << "\n";
			//
			//m_SMManager.ChangeState(eStates::STATE_TRAVEL);
			break;
			//cancel calibration
		case eEvents::TravelEvent:
		case eEvents::RockerEvent:
		case eEvents::CampEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL);
			break;
		default:
		break;
	}
}

void FSMTravelCal::OnExit()
{
	CLeds::is().ActiveOn();
	CSerial::is() << PSTR(" FSMTravelCal::OnExit()\r\n");
}
