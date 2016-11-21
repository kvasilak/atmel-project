/* 
* Camp.h
*
* Created: 2/1/2015 10:14:57 PM
* Author: keith
*/


#ifndef __CAMP_H__
#define __CAMP_H__

#include <stdint.h>
#include "State.h"
#include "States.h"
#include "..\Cio.h"
#include "..\CSerial.h"
#include "..\CLeds.h"
#include "..\CTimer.h"
#include "..\MMA8451.h"
#include "common.h"

    #define PITCH_STATES_LIST(macro) \
    macro(CampIniting),     \
    macro(CampRearLow),     \
    macro(CampRearHigh),    \
	macro(CampLevel),       \
    macro(CampComplete)
    
    typedef enum
    {
	    PITCH_STATES_LIST(ENUMIFY)
    }PitchStates_e;
	
	#define ROLL_STATES_LIST(macro)\
	macro(RollHold),				\
	macro(RollStateLeft),			\
	macro(RollStateRight),			\
	macro(RollStateLevel),			\
	macro(RollStateLast)
	
	typedef enum
	{
		ROLL_STATES_LIST(ENUMIFY)
	}RollStates_e;
	
class FsmCamp :public virtual CState
{
	public:
	FsmCamp(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();
	
	private:
	void LevelIt();
    void LevelMachine();
	void SetPitchState(PitchStates_e s);
	void SetRollState(RollStates_e s);
    void LevelRoll(int16_t Y);
	
	uint32_t Start;
	uint32_t DebugDelay;
	uint32_t MinTime;
	bool IsLevel;
	bool ReadyToSleep;
	PitchStates_e PitchState;
	uint32_t DebugTime;
	RollStates_e RollState;
	bool PitchEntered;
	bool RollEntered;

}; //Camp

#endif //__CAMP_H__
