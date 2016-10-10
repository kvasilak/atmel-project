/* 
* MauualCal.cpp
*
* Created: 10/8/2016 5:27:24 PM
* Author: keith
*/

#include <avr/eeprom.h>
#include "ManualCal.h"
#include "CController.h"
#include "CController.h"
#include "Cio.h"
#include "CSerial.h"
#include "CLeds.h"
#include "CTimer.h"
#include "CADC.h"
#include "nvm.h"

ManualCal::ManualCal(CController& SMManager) :
CState(SMManager, eStates::STATE_MANUAL_CALIBRATE)
{
}

void ManualCal::OnEntry()
{
	CSerial::is() << " ManualCal::OnEntry()\r\n";

	
}

//Manual calibration basically means finding the lower 
//suspension travel limits. set the height and the controller won't go any lower 
//in manual, camp or travel
void ManualCal::HandleEvent(eEvents evt)
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
		case eEvents::OutSideEvent:
			CSerial::is() << "Manual cal, outside event\n";
			Cio::is().OutsideRemote();
			break;
		case eEvents::CalibrateEvent:
			//user pressed the the cal button again, save 
		
			nvm::is().SetLeftLowest(CADC::is().GetLeftHeight());
			nvm::is().SetRightLowest(CADC::is().GetRightHeight());
			
			nvm::is().Save();
			
			CSerial::is() << "Manual Cal Complete; Left, " << nvm::is().GetLeftLowest() << ", Right, " << nvm::is().GetRightLowest() << "\n";
			
			m_SMManager.ChangeState(eStates::STATE_MANUAL);
			break;
			//Cancel calibration  on any button press except cal or outside remote
		case eEvents::TravelEvent:
		case eEvents::RockerEvent:
		case eEvents::CampEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL);
			break;
		default:
		break;
	}
}

void ManualCal::OnExit()
{
	CLeds::is().ActiveOn();
	CSerial::is() << "ManualCal::OnExit()\r\n";
}