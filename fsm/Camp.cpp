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
Start(0),
MinTime(250)
{
}

void FsmCamp::OnEntry()
{
	int16_t roll = nvm::is().GetCampX();
 	int16_t pitch = nvm::is().GetCampZ();

	CSerial::is() << " FsmCamp::OnEntry(), roll cal; " << roll << " pitch cal; " << pitch << "\r\n";
	Start = CTimer::GetTick();
	
	CMMA8451::is().writeRegister8(0x2A, 0x3d); //slow rate, low noise, Active
	
	CMMA8451::is().writeRegister8(0x2B, 2); //high res
	
}

void FsmCamp::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run camp FSM 
			LevelIt();

		break;
		case eEvents::RockerEvent:
		case eEvents::OutSideEvent:
		case eEvents::SteeringEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::CampEvent:
			if(Cio::is().CampSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_MANUAL);
			}
		break;
		case eEvents::TravelEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL, evt);
		break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_CAMP_CALIBRATE);
		break;
		case eEvents::IgnitionOnEvent:
			Cio::is().Wakeup();
			CLeds::is().CampOn();
			//relevel on wakeup
			
			CSerial::is() << " FsmCamp::Ignition On\r\n";

			break;
		case eEvents::IgnitionOffEvent:

			Cio::is().Sleep();
			
			CSerial::is() << " FsmCamp::Ignition Off\r\n";

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
	 
	 
 //pitch up ( rear too Damn low) if( Z	> pitchcal + pitchtol )
 {
	 //Raise the lower corner
	 if(X > rollcal + rolltol)//Roll up ( Left down )
	 {
		 //Raise Left
		 if(CTimer::IsTimedOut(MinTime, Start))
		 {
			 CSerial::is() << "left up\n";
			 Cio::is().Left(eValveStates::Fill);
			 Cio::is().Right(eValveStates::Hold);
			 
			 Start = CTimer::GetTick();
		 }
		 
	 }
	 else if(X < rollcal - rolltol) //Roll down (Right Down)
	 {
		 //Raise Right
		if(CTimer::IsTimedOut(MinTime, Start))
		{
			CSerial::is() << "Right up\n";
			Cio::is().Left(eValveStates::Hold);
			Cio::is().Right(eValveStates::Fill);
			 		 
			Start = CTimer::GetTick();
		}
	 }
	 else
	 {
		//Raise left and right
		if(CTimer::IsTimedOut(MinTime, Start))
		{
			CSerial::is() << "left up\n";
			Cio::is().Left(eValveStates::Fill);
			Cio::is().Right(eValveStates::Fill);
			 		 
			Start = CTimer::GetTick();
		}
	 }
 }
 //pitch down ( rear too Damn high )
 else if( Z < pitchcal - pitchtol )
 {
	 //lower the higher corner
	 if(X > rollcal + rolltol)//Roll up ( Left down )
	 {
		 //Lower Left
		 if(CTimer::IsTimedOut(MinTime, Start))
		 {
			 CSerial::is() << "left Down\n";
			 Cio::is().Left(eValveStates::Dump);
			 Cio::is().Right(eValveStates::Hold);
			 
			 Start = CTimer::GetTick();
		 }
		 
	 }
	 else if(X < rollcal - rolltol) //Roll down (Right Down)
	 {
		 //Lower Right
		 if(CTimer::IsTimedOut(MinTime, Start))
		 {
			 CSerial::is() << "Right down\n";
			 Cio::is().Left(eValveStates::Hold);
			 Cio::is().Right(eValveStates::Dump);
			 
			 Start = CTimer::GetTick();
		 }
	 }
	 else
	 {
		 //Lower left and right
		 if(CTimer::IsTimedOut(MinTime, Start))
		 {
			 CSerial::is() << "left right down\n";
			 Cio::is().Left(eValveStates::Dump);
			 Cio::is().Right(eValveStates::Dump);
			 
			 Start = CTimer::GetTick();
		 }
	 }
 }
 //Level pitch
 else
 {
	//rotate
	if(X > rollcal + rolltol)//Roll up ( Left down )
	{
		//Raise Left, lower right
		 if(CTimer::IsTimedOut(MinTime, Start))
		 {
			 CSerial::is() << "left up right down\n";
			 Cio::is().Left(eValveStates::Fill);
			 Cio::is().Right(eValveStates::Dump);
			 
			 Start = CTimer::GetTick();
		 }
	
	}
	else if(X < rollcal - rolltol) //Roll down (Right Down)
	{
		//Raise Right, lower left
		 if(CTimer::IsTimedOut(MinTime, Start))
		 {
			 CSerial::is() << "left down right up\n";
			 Cio::is().Left(eValveStates::Dump);
			 Cio::is().Right(eValveStates::Fill);
			 
			 Start = CTimer::GetTick();
		 }
	}
	else
	{
		//perfect
		 if(CTimer::IsTimedOut(MinTime, Start))
		 {
			 CSerial::is() << "left right Purrfect\n";
			 Cio::is().Left(eValveStates::Hold);
			 Cio::is().Right(eValveStates::Hold);
			 
			 Start = CTimer::GetTick();
		 }
	}
 }
}

