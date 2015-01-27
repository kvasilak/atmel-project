/* 
* CController.h
*
* Created: 1/24/2015 8:13:35 PM
* Author: keith
*/


#ifndef __CCONTROLLER_H__
#define __CCONTROLLER_H__
#include "Fsm.h"

//class ControllerEvents //: public FsmEvents
//{
	//public:
	//enum
	//{
		//TimerEvent = 100,
		//UpEvent,
		//DownEvent,
		//HoldEvent,
		//LeftUpEvent,
		//LeftDownEvent,
		//RightUpEvent,
		//RightDownEvent,
		//TravelEvent,
		//CampEvent,
		//CalibrateEvent
	//}Events;
//};

class CController : public CFsm
{

public:
	CController();
	~CController();

private:
	CController( const CController &c );
	CController& operator=( const CController &c );
	
	void GetMode();
	void CheckEvents();
	
	void Run();
	
	//State functions
	void Manual(FsmEvents const e); // state handlers
	void Travel(FsmEvents const e);
	void Camp(FsmEvents const e);
	
	private:
	
	void LeftUp();
	void LeftDown();
	void RightUp();
	void RightDown();
	void LeftHold();
	void RightHold();

	
}; //CController

#endif //__CCONTROLLER_H__
