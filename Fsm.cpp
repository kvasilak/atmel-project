/*
 * Fsm.cpp
 *
 * Created: 1/25/2015 5:58:54 PM
 *  Author: keith
 */ 
#include "fsm.h"

CFsm::CFsm():
State(0),
TargetState(0)
{
}

//Start the state machine, there HAS to be an initial state
void CFsm::Init(FsmStateFunc_t initialstate)
{
	//C_ASSERT(initialstate != NULL);
	
	State = initialstate;
	(*this.*CFsm::State)(EventEntry);
	
	//C_ASSERT(fsm->TargetState == NULL); // verify that initial state entry did not try to change state
}

//set the next state
void CFsm::SetNextState(FsmStateFunc_t f)
{
	TargetState = f;
}

//Called to send an event to the current state
bool CFsm::Dispatch(FsmEvents e)
{
	//call the current state with the event
	bool const result = (*this.*CFsm::State)(e);

	// did the state function call cause a state change?
	if (TargetState != 0)
	{
		//FsmStateFunc_t const TargetStateSnapshot = TargetState;
		//(void)targetStateSnapshot; // unused when assertions disabled

		//C_ASSERT(result == c_fsmResult_Handled); // transitions should always return handled

		// 1) exit old state
		(*this.*CFsm::State)(EventExit);

		//C_ASSERT(targetStateSnapshot == fsm->TargetState); // transitions are illegal during the exit

		// 2) enter new state
		(*this.*CFsm::TargetState)(EventEntry);

		//C_ASSERT(targetStateSnapshot == fsm->TargetState); // transitions are illegal during the entry

		// 3) update state
		State = TargetState;
		
		TargetState = 0;
	}

	return result;
}

