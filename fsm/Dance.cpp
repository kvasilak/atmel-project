/* 
* FSMDance.cpp
*
* Created: 2/6/2015 9:34:03 PM
* Author: keith
*/


#include "dance.h"
#include "CController.h"
#include "Cio.h"
#include "CSerial.h"
#include "CLeds.h"
#include "CTimer.h"
#include "CADC.h"
#include "nvm.h"


FSMDance::FSMDance(CController& SMManager) :
CState(SMManager, eStates::STATE_MANUAL_CALIBRATE), 
State(Idle)
{
}

void FSMDance::OnEntry()
{
	CSerial::is() << " FSMDance::OnEntry()\r\n";
	Blink = CTimer::GetTick();
	
	Cio::is().AllOff();
	
	//Always start by filling the bags 
	State = Filling;

	SpeedTime = CTimer::GetTick();
	
	rolls = 0;
	
}

//Manual calibration basically means finding the upper and lower 
//suspension travel limits. These limits are used to determine if we 
//should keep trying to adjust during travel or Camp mode
void FSMDance::HandleEvent(eEvents evt)
{
	static bool Active = false;
	switch(evt)
	{
		case eEvents::TimerEvent:
		
			CalcSpeed();
			ShakeIt();
			
			//blink active LED
			if(CTimer::IsTimedOut(150, Blink))
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
		default:
			CSerial::is() << "Manual cal, Default\n";
			break;
	}
}

void FSMDance::OnExit()
{
	CLeds::is().ActiveOn();
	CSerial::is() << "FSMDance::OnExit()\r\n";
}

void FSMDance::CalcSpeed()
{
static uint16_t oldleft= CADC::is().GetLeftHeight();
static uint16_t oldright= CADC::is().GetRightHeight();

	//calculate speed, counts/second
	if(CTimer::IsTimedOut(1000, SpeedTime))
	{
		uint16_t left = CADC::is().GetLeftHeight();
		uint16_t right = CADC::is().GetRightHeight();
	
		if(left > oldleft)
		{
			LeftSpeed = left - oldleft;
		}
		else
		{
			LeftSpeed = oldleft - left;
		}
	
		if(right > oldright)
		{
			RightSpeed = right - oldright;
		}
		else
		{
			RightSpeed = oldright - right;
		}
		
		oldright = right;
		oldleft = left;
		
		//CSerial::is() << "speed,  " << RightSpeed << ", " << LeftSpeed << "\n";

		SpeedTime = CTimer::GetTick();
	}
}

bool FSMDance::IsMoving()
{
	bool moving = false;
	
	if( (LeftSpeed > 2) || (RightSpeed > 2))
	{
		moving = true;
	}
	
	return moving;
}

//calibration state machine
void FSMDance::ShakeIt()
{
	switch(State)
	{
		case Idle:
		break;
		case Fill:
			//open both fill valves
			Cio::is().Right(eValveStates::Fill);
			Cio::is().Left(eValveStates::Fill);
			
			State = StartFilling;
			
			CSerial::is() << "start filling\n";
		break;
		case StartFilling:
			if(true == IsMoving())
			{
				CSerial::is() << "filling\n";
				State = Filling;
			}
		break;
		case Filling:
			//Wait for max height to be reached
			//if(false == IsMoving())
			{
				CSerial::is() << "Max Height\n";
				
				State = TiltLeft;
				Cio::is().Right(eValveStates::Fill);
				Cio::is().Left(eValveStates::Dump);
			}
		break;
		case TiltLeft:
			if(CTimer::IsTimedOut(12000, DanceTime))
			{
				DanceTime = CTimer::GetTick();
				
				State = TiltRight;
				Cio::is().Right(eValveStates::Dump);
				Cio::is().Left(eValveStates::Fill);
			}
		break;
		case TiltRight: //complete
			if(CTimer::IsTimedOut(12000, DanceTime))
			{
				DanceTime = CTimer::GetTick();
				
				//Hip Roll 3 times
				
				CSerial::is() << "Roll #; " <<rolls << "\n";
				if(rolls < 3)
				{
					State = Filling;
					Cio::is().Right(eValveStates::Hold);
					Cio::is().Left(eValveStates::Dump);
				}
				else
				{
					State = Dump1;
					Cio::is().Right(eValveStates::Dump);
					Cio::is().Left(eValveStates::Dump);
				}
				
				rolls++;
			}
		break;
		case Dump1:
			if(CTimer::IsTimedOut(1000, DanceTime))
			{
				DanceTime = CTimer::GetTick();
				
				State = Fill1;
				Cio::is().Right(eValveStates::Fill);
				Cio::is().Left(eValveStates::Fill);
			}
		break;
		case Fill1:
			if(CTimer::IsTimedOut(15000, DanceTime))
			{
				DanceTime = CTimer::GetTick();
				
				State = Dump2;
				Cio::is().Right(eValveStates::Dump);
				Cio::is().Left(eValveStates::Dump);
			}
		break;
		case Dump2:
			if(CTimer::IsTimedOut(5000, DanceTime))
			{
				DanceTime = CTimer::GetTick();
				
				State = Fill2;
				Cio::is().Right(eValveStates::Fill);
				Cio::is().Left(eValveStates::Fill);
			}
		break;
		case Fill2:
			if(CTimer::IsTimedOut(15000, DanceTime))
			{
				DanceTime = CTimer::GetTick();
				
				State = Dump3;
				Cio::is().Right(eValveStates::Dump);
				Cio::is().Left(eValveStates::Dump);
			}
		break;
		case Dump3:
			if(CTimer::IsTimedOut(15000, DanceTime))
			{
				DanceTime = CTimer::GetTick();
				
				State = Done;
				Cio::is().AllOff();
			}
		break;
		case Done:
			//back to manual mode
			CLeds::is().ActiveOn();
			m_SMManager.ChangeState(eStates::STATE_MANUAL);
			
			State = Idle;
		break;
		default:
			// close valves
			
			State = Idle;
	}
}