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
#include "nvm.h"

FSMCampCal::FSMCampCal(CController& SMManager) :
CState(SMManager, eStates::STATE_CAMP_CALIBRATE)
{
}

void FSMCampCal::OnEntry()
{
	CSerial::is() << " FSMTravelCal::OnEntry()\r\n";
	//Blink = CTimer::GetTick();
	
	//Cio::is().AllOff();
}

void FSMCampCal::HandleEvent(eEvents evt)
{
	//static bool Active = false;
	switch(evt)
	{
		case eEvents::TimerEvent:
		
			CSerial::is() << "Tmr Evt\n";
			//if(CTimer::IsTimedOut(250, Blink))
			//{
				//if(Active)
				//{
					//CLeds::is().ActiveOn();
					//CLeds::is().CampOn();
					//Active = !Active;
				//}
				//else
				//{
					//CLeds::is().ActiveOff();
					//CLeds::is().CampOff();
					//Active = !Active;
				//}
				//
				//Blink = CTimer::GetTick();
			//}
			break;
		case eEvents::OutSideEvent:
			CSerial::is() << "Camp cal, outside event\n";
			//Cio::is().OutsideRemote();
			break;
		case eEvents::CalibrateEvent:
		CSerial::is() << "calibrate event\n";
			//user pressed the the cal button again, save 
			//int16_t X;
			//int16_t Y;
			//int16_t Z;
			//
			////Read Acccel
			//CMMA8451::is().ReadXYZ(X, Y, Z);
			//
			//nvm::is().SetCampX(X);
			//nvm::is().SetCampY(Y);
			//nvm::is().SetCampZ(Z);
			//
			//nvm::is().Save();
			
			//CSerial::is() << "Camp Cal Complete; X, " << X << ", Y, " << Y <<", Z, " << Z << "\n";
			
			//m_SMManager.ChangeState(eStates::STATE_CAMP);
			
			CSerial::is() << "Camp Cal Complete;\n";
			break;
			//Cancel calibration on any button press except cal or outside remote
		case eEvents::TravelEvent:
		case eEvents::RockerEvent:
		case eEvents::CampEvent:
			m_SMManager.ChangeState(eStates::STATE_CAMP);
		default:
		break;
	}
}

void FSMCampCal::OnExit()
{
	//CLeds::is().ActiveOn();
	CSerial::is() << " FSMTravelCal::OnExit()\r\n";

}
