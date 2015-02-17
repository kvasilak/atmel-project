/* 
* Manual.cpp
*
* Created: 2/1/2015 10:14:32 PM
* Author: keith
*/


#include "Manual.h"
#include "..\CController.h"
#include "..\Cio.h"
#include "..\CSerial.h"


FsmManual::FsmManual(CController& SMManager) :
CState(SMManager, STATE_MANUAL)
{
}

void FsmManual::OnEntry()
{
	CSerial::is() << " FsmManual::OnEntry()\r\n";
	//Handleswitches();
}

void FsmManual::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TravelEvent:
			//change state if no manual buttons are active
			if(Cio::is().LeftState == SolenoidHolding && Cio::is().RightState == SolenoidHolding)
			{
				m_SMManager.ChangeState(STATE_TRAVEL);
			}
		break;
		case CampEvent:
			//change state if no manual buttons are active
			if(Cio::is().LeftState == SolenoidHolding && Cio::is().RightState == SolenoidHolding)
			{
				m_SMManager.ChangeState(STATE_CAMP);
			}
		break;
		case ManualEvent:
			//nothing to do here
		break;
		case CalibrateEvent:
			m_SMManager.ChangeState(STATE_MANUAL_CALIBRATE);
		break;
		case TimerEvent:
			//run travel FSM?
		break;
		default:
		break;
	}
}

void FsmManual::OnExit()
{
	CSerial::is() << " FsmManual::OnExit()\r\n";
}

