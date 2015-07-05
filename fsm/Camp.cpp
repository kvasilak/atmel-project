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
CState(SMManager, eStates::STATE_CAMP),
LevelComplete(false)
{
}

void FsmCamp::OnEntry()
{
	CSerial::is() << " FsmCamp::OnEntry()\r\n";
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

			//CMMA8451::is().writeRegister8(0x2A, 0x3d); //slow rate, low noise, Active
				
			//CMMA8451::is().writeRegister8(0x2B, 2); //high res

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

//Accel should be orentated such that X is at 0 G and Z is at zero G
//This would be with the corner dot in the upper left
// 
// 
// pitch   roll       left  right
// ok       ok         hold   hold
// ok       leftlow    fill   dump
// ok       rightlow   dump   fill
// ok       lefthigh   dump   fill
// ok       righthigh  fill   dump
// low      ok         fill   fill
// low      leftlow    fill   hold
// low      rightlow   hold   fill
// high     ok         dump   dump
// high     lefthigh   dump   hold
// high     righthigh  hold   dump

void FsmCamp::LevelIt()
{
	int16_t X;
	int16_t Y;
	int16_t Z;
	
	//Read Acccel
	CMMA8451::is().ReadXYZ(int16_t &X, int16_t &Y, int16_t &Z);
	
	//determine x and z errors
	int16_t roll = nvm::is().GetCampX();
	int16_t pitch = n vm::is().GetCampZ();

	//Pitch high
	if( pitch > Z + pitchtol )
	{
		// pitch   roll       left  right
		// high     lefthigh   dump   hold
		if(roll > X + rolltol)
		{
			Cio::is().Left(eValveStates::Dump);
			Cio::is().Right(eValveStates::Hold);
		}
		// pitch   roll       left  right
		// high     righthigh  hold   dump
		else if(roll < X - rolltol)
		{
			Cio::is().Left(eValveStates::Hold);
			Cio::is().Right(eValveStates::Dump);
		}
		// pitch   roll       left  right
		// high     ok         dump   dump
		else
		{
			Cio::is().Left(eValveStates::Dump);
			Cio::is().Right(eValveStates::Dump);
		}
	}	
	//Pitch Low
	else if( pitch < Z - pitchtol )
	{
		// pitch   roll       left  right
		// low      rightlow   hold   fill
		if(roll > X + rolltol)
		{
			Cio::is().Left(eValveStates::Hold);
			Cio::is().Right(eValveStates::Fill);
		}
		// pitch   roll       left  right
		// low      leftlow    fill   hold
		else if(roll < X - rolltol)
		{
			Cio::is().Left(eValveStates::Fill);
			Cio::is().Right(eValveStates::Hold);
		}
		// pitch   roll       left  right
		// low      ok         fill   fill
		else
		{
			Cio::is().Left(eValveStates::Fill);
			Cio::is().Right(eValveStates::Fill);
		}
	}
	
	else //pitch OK
	{
		// pitch   roll       left  right
		// ok       ok         hold   hold
		if(roll > X + rolltol)
		{
			Cio::is().Left(eValveStates::Hold);
			Cio::is().Right(eValveStates::Hold);
			
			//wait a few seconds to make sure we are actually level
			//leveling complete!!
		}
		// pitch   roll       left  right
		// ok       leftlow    fill   dump
		else if(roll < X - rolltol)
		{
			Cio::is().Left(eValveStates::Fill);
			Cio::is().Right(eValveStates::Dump);
		}
		// pitch   roll       left  right
		// ok       rightlow   dump   fill
		else
		{
			Cio::is().Left(eValveStates::Dump);
			Cio::is().Right(eValveStates::Fill);
		}
		// pitch   roll       left  right
		// ok       lefthigh   dump   fill
		else
		{
			Cio::is().Left(eValveStates::Dump);
			Cio::is().Right(eValveStates::Fill);
		}
		// pitch   roll       left  right
		// ok       righthigh  fill   dump
		else
		{
			Cio::is().Left(eValveStates::Fill);
			Cio::is().Right(eValveStates::Dump);
		}
	}
	
}

