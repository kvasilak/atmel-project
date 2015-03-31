/* 
* FSMManualCal.cpp
*
* Created: 2/6/2015 9:34:03 PM
* Author: keith
*/


#include "ManualCal.h"
#include "..\CController.h"
#include "..\Cio.h"
#include "..\CSerial.h"
#include "..\CLeds.h"
#include "..\CTimer.h"


FSMManualCal::FSMManualCal(CController& SMManager) :
CState(SMManager, eStates::STATE_MANUAL_CALIBRATE), 
State(Idle)
{
}

void FSMManualCal::OnEntry()
{
	CSerial::is() << " FSMManualCal::OnEntry()\r\n";
	Blink = CTimer::GetTick();
	
	//Always start by filling the bags to find the upper limits
	State = Fill;
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
		case eEvents::CalibrateEvent:
			//Pressing the calibrate button a second time, 
			//at least 5 seconds after the first press will cancel the calibration
			if(CTimer::IsTimedOut(5000, Blink))
			{
				CSerial::is() << "Manual cal, Cal event\n";
				m_SMManager.ChangeState(eStates::STATE_MANUAL);
			}
			
			break;
		default:
			CSerial::is() << "Manual cal, Default\n";
			break;
	}
}

void FSMManualCal::OnExit()
{
	CSerial::is() << "FSMManualCal::OnExit()\r\n";
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
			
			State = Filling;
		break;
		case Filling:
			//Wait for max height to be reached
			if(true)
			{
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
			
			State = Dumping;
		break;
		case Dumping:
			//wait for min height
			if(true)
			{
				State = Done;
			}
		break;
		case Done:
			//Close Valves
			Cio::is().AllOff();
			
			//Save heights in EEPROM
			
			//back to manual mode
			//m_SMManager.ChangeState(eStates::STATE_MANUAL);
			
			State = Idle;
		break;
		default:
			// close valves
			
			State = Idle;
	}
}