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
#include "nvm.h"

static const int16_t pitchtol = 10;
static const int16_t rolltol = 10;

FsmCamp::FsmCamp(CController& SMManager) :
CState(SMManager, eStates::STATE_CAMP),
LevelComplete(false),
ReadyToSleep(false)
{
}

void FsmCamp::OnEntry()
{
	int16_t roll = nvm::is().GetCampX();
 	int16_t pitch = nvm::is().GetCampZ();

	CSerial::is() << " FsmCamp::OnEntry(), roll cal; " << roll << " pitch cal; " << pitch << "\r\n";
	start = CTimer::GetTick();
	
	CMMA8451::is().writeRegister8(0x2A, 0x3d); //slow rate, low noise, Active
	
	CMMA8451::is().writeRegister8(0x2B, 2); //high res
	
	LevelComplete = false;
	ReadyToSleep = false;
}

void FsmCamp::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run camp FSM 
			//CMMA8451::is().ReadXYZ();
				
			if(!LevelComplete)
			{
				LevelIt();
			}
			else if(ReadyToSleep)
			{
				Cio::is().Sleep();
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
		case eEvents::IgnitionOnEvent:
			Cio::is().Wakeup();
			
			//relevel on wakeup
			LevelComplete = false;
			ReadyToSleep = false;
			break;
		case eEvents::IgnitionOffEvent:
			
			if(LevelComplete)
			{
				Cio::is().Sleep();
			}
			break;
		default:
		break;
	}
}

void FsmCamp::OnExit()
{
	CSerial::is() << " FsmCamp::OnExit()\r\n";
}

//pitch + means nose high
//pitch - means nose low

//roll + means left down, right up
//roll - means left up, right down 

//Accel should be orientated such that X is at 0 G and Z is at zero G
//This would be with the corner dot in the upper left

void FsmCamp::LevelIt()
{
 	int16_t X;
 	int16_t Y;
 	int16_t Z;
 	//Read Acccel
 	CMMA8451::is().ReadXYZ(X, Y, Z);
 	//determine x and z errors
 	int16_t rollcal = nvm::is().GetCampX();
 	int16_t pitchcal = nvm::is().GetCampZ();
	 
	 CSerial::is() << "pitch err; " << Z -pitchcal << " roll err; " << X - rollcal << "\n";
  //pitch up if( Z > pitchcal + pitchtol )
 {
	 CSerial::is() << "pitch up\n";
	 Cio::is().Left(eValveStates::Dump);
	 Cio::is().Right(eValveStates::Dump);
 }
 //pitch down
 else if( Z < pitchcal - pitchtol )
 {
	 CSerial::is() << "pitch down\n";
	 Cio::is().Left(eValveStates::Fill);
	 Cio::is().Right(eValveStates::Fill);
 }
 //pitch within tolerance
 else
 {
	 //Roll up ( right up )
	 if(X > rollcal + rolltol)
	 {
		 CSerial::is() << "roll up\n";
		 Cio::is().Left(eValveStates::Fill);
		 Cio::is().Right(eValveStates::Dump);
	 }
	 //Roll down (Right Down)
	 else if(X < rollcal - rolltol)
	 {
		 CSerial::is() << "roll down\n";
		 Cio::is().Left(eValveStates::Dump);
		 Cio::is().Right(eValveStates::Fill);
	 }
	 //Roll within tolerance
	 else
	 {
		 CSerial::is() << "pitch and roll within tolerance\n";
		 Cio::is().Left(eValveStates::Hold);
		 Cio::is().Right(eValveStates::Hold);
		 
		 LevelComplete = true;
	 }
   }
}

