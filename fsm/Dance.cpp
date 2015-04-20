/* 
* FSMManualCal.cpp
*
* Created: 2/6/2015 9:34:03 PM
* Author: keith
*/


#include "ManualCal.h"
#include "CController.h"
#include "Cio.h"
#include "CSerial.h"
#include "CLeds.h"
#include "CTimer.h"
#include "CADC.h"
#include "nvm.h"


FSMManualCal::FSMManualCal(CController& SMManager) :
CState(SMManager, eStates::STATE_MANUAL_CALIBRATE), 
State(Idle)
{
}

void FSMManualCal::OnEntry()
{
	CSerial::is() << " FSMManualCal::OnEntry()\r\n";
	Blink = CTimer::GetTick();
	
	Cio::is().AllOff();
	
	//Always start by filling the bags 
	State = Fill;

	SpeedTime = CTimer::GetTick();
}

//Manual calibration basically means finding the upper and lower 
//suspension travel limits. These limits are used to determine if we 
//should keep trying to adjust during travel or Camp mode
void FSMManualCal::HandleEvent(eEvents evt)
{
	static bool Active = false;
	switch(evt)
	{
		case eEvents::TimerEvent:
		
			CalcSpeed();
			Calibrate();
			
			//blink active LED
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
		default:
			CSerial::is() << "Manual cal, Default\n";
			break;
	}
}

void FSMManualCal::OnExit()
{
	CLeds::is().ActiveOn();
	CSerial::is() << "FSMManualCal::OnExit()\r\n";
}

void FSMManualCal::CalcSpeed()
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
		
		CSerial::is() << "speed,  " << RightSpeed << ", " << LeftSpeed << "\n";

		SpeedTime = CTimer::GetTick();
	}
}

bool FSMManualCal::IsMoving()
{
	bool moving = false;
	
	if( (LeftSpeed > 2) || (RightSpeed > 2))
	{
		moving = true;
	}
	
	return moving;
}

//calibration state machine
void FSMManualCal::Calibrate()
{
	switch(State)
	{
		case Fill:
			//open both fill valves
			Cio::is().RightFillOn();
			Cio::is().LeftFillOn();
			
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
			if(false == IsMoving())
			{
				CSerial::is() << "Max Height\n";
				
				leftmax = CADC::is().GetLeftHeight();
				rightmax = CADC::is().GetRightHeight();
				State = Dump;
			}
		break;
		case Dump:
			//Close fill
			Cio::is().RightFillOff();
			Cio::is().LeftFillOff();
			
			//open dump valves 
			Cio::is().RightDumpOn();
			Cio::is().LeftDumpOn();
			
			State = StartDumping;
			CSerial::is() << "start dumping\n";
		break;
		case StartDumping:
			if(true == IsMoving())
			{
				State = Dumping;
			}
		break;
		case Dumping:
			//wait for min height
			if(false == IsMoving())
			{
				CSerial::is() << "Min height\n";
				
				leftmin = CADC::is().GetLeftHeight();
				rightmin = CADC::is().GetRightHeight();
				State = Done;
			}
		break;
		case Done:
			//Close Valves
			Cio::is().AllOff();

			//back to manual mode
			m_SMManager.ChangeState(eStates::STATE_MANUAL);
			
			State = Idle;
		break;
		default:
			// close valves
			
			State = Idle;
	}
}