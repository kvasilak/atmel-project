/*
 * fsm.h
 *
 * Created: 1/24/2015 10:49:58 PM
 *  Author: keith
 */ 


#ifndef FSM_H_
#define FSM_H_
#include <inttypes.h>

//Event base class
//derive your even class from this
//your events will have positive values
//class CFsmEvents
//{
//	public:
	typedef enum
	{
		EventEntry,
		EventExit,
		//TimerEvent ,
		UpEvent,
		DownEvent,
		HoldEvent,
		LeftUpEvent,
		LeftDownEvent,
		RightUpEvent,
		RightDownEvent//,
		//TravelEvent,
		//CampEvent//,
		//CalibrateEvent
	}FsmEvents;
//};

class CFsm;

typedef bool (CFsm::*FsmStateFunc_t)(FsmEvents const e);

/* Event base class */
class CFsm
{
public:
	

	CFsm();
	void Init(FsmStateFunc_t initialstate);
	bool Dispatch(FsmEvents e);
	void SetNextState(FsmStateFunc_t f);

private:

    FsmStateFunc_t State;       /**< the current state              */
    FsmStateFunc_t TargetState; /**< target state during transition */
};

#endif /* FSM_H_ */