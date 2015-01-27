/* 
* CController.cpp
*
* Created: 1/24/2015 8:13:35 PM
* Author: keith
*/


#include "CController.h"

// default constructor
CController::CController()
{
} //CController

//switch to manual mode if user presses any manual button
//Manual buttons are; up, down, remote up, remote down, remote ru/lu/rd/ld
//
//switch to camp mode if user presses camp button and no manual button is pressed
//
//switch to travel mode if user presses travel button and no manual button is pressed
void CController::GetMode()
{
	
}

//determine if the mode change requires a state machine change
void CController::CheckEvents()
{
	GetMode();
}


void CController::LeftUp()
{
	
}

void CController::LeftDown()
{
	
}

void CController::RightUp()
{
	
}

void CController::RightDown()
{
	
}

void CController::LeftHold()
{
	
}

void CController::RightHold()
{
	
}

void CController::Manual(FsmEvents const e)
{
	switch(e)
	{
		case EventEntry:
			break;
		case EventExit:
			break;
		case UpEvent:
			break;
		case DownEvent:
			break;
		case HoldEvent:
			break;
		case LeftUpEvent:
			break;
		case LeftDownEvent:
			break;
		case RightUpEvent:
			break;
		case RightDownEvent:
			break;
		//case TravelEvent:
		//	LeftHold();
		//	RightHold();
		//	//SetNextState(Travel);
		//	break;
		//case CampEvent:
		//	LeftHold();
		//	RightHold();
			//SetNextState(Camp);
		//	break;
	}
}

void CController::Travel(FsmEvents const e)
{
	switch(e)
	{
		case EventEntry:
			break;
		case EventExit:
			break;
		case UpEvent:
			LeftUp();
			RightUp();
			//SetNextState(Manual);
			break;
		case DownEvent:
			LeftDown();
			RightDown();
			//SetNextState(Manual);
			break;
		case LeftUpEvent:
			LeftUp();
			//SetNextState(Manual);
			break;
		case LeftDownEvent:
			LeftDown();
			//SetNextState(Manual);
			break;
		case RightUpEvent:
			RightUp();
			//SetNextState(Manual);
			break;
		case RightDownEvent:
			RightDown();
			//SetNextState(Manual);
			break;
		case HoldEvent:
			LeftHold();
			RightHold();
			//SetNextState(Manual);
			break;
		//case CampEvent:
			//SetNextState(Camp);
		//	break;
	}
}

void CController::Camp(FsmEvents const e)
{
	switch(e)
	{
		case EventEntry:
			break;
		case EventExit:
			break;
		case UpEvent:
			//SetNextState(Manual);
			break;
		case DownEvent:
			//SetNextState(Manual);
			break;
		case LeftUpEvent:
			//SetNextState(Manual);
			break;
		case LeftDownEvent:
			//SetNextState(Manual);
			break;
		case RightUpEvent:
			//SetNextState(Manual);
			break;
		case RightDownEvent:
			//SetNextState(Manual);
			break;
		case HoldEvent:
			//SetNextState(Manual);
			break;
		//case TravelEvent:
			//SetNextState(Travel);
		//	break;
	}
}

