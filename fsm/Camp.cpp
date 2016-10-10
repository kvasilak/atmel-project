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

static const int16_t pitchtol = 50;//10;
static const int16_t rolltol = 50;//10;

FsmCamp::FsmCamp(CController& SMManager) :
CState(SMManager, eStates::STATE_CAMP),
Start(0),
MinTime(250),
IsLevel(false),
ReadyToSleep(false)
{
}

void FsmCamp::OnEntry()
{
	CLeds::is().CampOn();
	
	int16_t roll = nvm::is().GetCampY();
 	int16_t pitch = nvm::is().GetCampZ();

	CSerial::is() << " FsmCamp::OnEntry(), roll cal; " << roll << " pitch cal; " << pitch << "\r\n";
	Start = CTimer::GetTick();
	
	CMMA8451::is().writeRegister8(0x2A, 0x3d); //slow rate, low noise, Active
	
	CMMA8451::is().writeRegister8(0x2B, 2); //high res
	
	IsLevel  = false;
	ReadyToSleep = false;
	
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
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::SteeringEvent:
			Cio::is().SteeringRemote();
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::CampEvent:
			CSerial::is() << " FsmCamp::Camp event\r\n";
		
			if(!m_SMManager.ButtonWakeFirst)
			{
				CSerial::is() << " FsmCamp::button wake first\r\n";
				
				CLeds::is().CampOn();
				m_SMManager.ButtonWakeFirst = true;
			}
			else
			{
				if(Cio::is().CampSwitches())
				{
					CSerial::is() << " FsmCamp::camp switches\r\n";
					
					m_SMManager.ChangeState(eStates::STATE_MANUAL);
					
					m_SMManager.ButtonWakeFirst = false;
				}
			}
		break;
		case eEvents::TravelEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL);
		break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_CAMP_CALIBRATE);
		break;
		case eEvents::IgnitionOnEvent:

		    Cio::is().Awake = true;

			ReadyToSleep = false;
			IsLevel = false;

			Cio::is().Wakeup();
			CLeds::is().CampOn();
			//relevel on wakeup
			
			Start = CTimer::GetTick();
			
			CSerial::is() << " FsmCamp::Ignition On\r\n";

			break;
		case eEvents::IgnitionOffEvent:
			if(IsLevel)
			{
				CSerial::is() << " FsmCamp::Sleep\r\n";
				Cio::is().Awake = false;
				Cio::is().Sleep();
			}

			ReadyToSleep = true;
			CSerial::is() << " FsmCamp::Ignition Off\r\n";

			break;
		case eEvents::ButtonWakeEvent:
			if(!Cio::is().Awake)
			{
				Cio::is().Wakeup();
			
				Cio::is().ButtonWake = true;

				CSerial::is() << " FsmCamp::Button wake\r\n";
				Start = CTimer::GetTick();
				
				m_SMManager.ButtonWakeFirst = false;
				Cio::is().Awake = true;
				
			}
			ReadyToSleep = true; //sleep after we achieve level
			
			break;
		default:
		break;
	}
}

void FsmCamp::OnExit()
{
	CSerial::is() << " FsmCamp::OnExit()\r\n";
	CLeds::is().CampOff();
}


//pitch + means nose high
//pitch - means nose low

//roll + means left down, right up
//roll - means left up, right down 

//Accel should be orientated such that X is at 0 G and Z is at zero G
//This would be with the corner dot in the upper left

void FsmCamp::LevelIt()
{
 	int16_t X=0;
 	int16_t Y=0;
 	int16_t Z=0;
 	//Read Acccel
 	CMMA8451::is().ReadXYZ(X, Y, Z);
 	//determine Pitch and roll errors 	int16_t rollcal = nvm::is().GetCampY();
 	int16_t pitchcal = nvm::is().GetCampZ();
	 
	 CSerial::is() << "Y; " << Y << ", Z; " << Z ;
	 
	 CSerial::is()  << " Y err; " << Y - rollcal  << ";   Z err; " << Z -pitchcal << "\n";


 //pitch up ( rear too Damn low) if( Z	> pitchcal + pitchtol )
 {
	 //Raise the lower corner
	 if(Y > rollcal + rolltol)//Roll up ( Left down )
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
	 else if(Y < rollcal - rolltol) //Roll down (Right Down)
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
	 if(Y > rollcal + rolltol)//Roll up ( Left down )
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
	 else if(Y < rollcal - rolltol) //Roll down (Right Down)
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
	if(Y > rollcal + rolltol)//Roll up ( Left down )
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
	else if(Y < rollcal - rolltol) //Roll down (Right Down)
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
		Cio::is().Left(eValveStates::Hold);
		Cio::is().Right(eValveStates::Hold);
			 
		//perfect
		 if(CTimer::IsTimedOut(10000, Start))
		 {
			 IsLevel = true;
			 
			 CSerial::is() << "left right Purrfect\n";
			 
			 //if level for 5 seconds and ignition is off, go to sleep
			 if(ReadyToSleep)
			 {
				Cio::is().Awake = false;
				Cio::is().Sleep();
			 }
			 else
			 {
				 CSerial::is() << "Not ready to sleep\n";
			 }
		 }
	}
 }

}

