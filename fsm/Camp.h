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
    macro(CampCompleteEnter), \
    macro(CampComplete)
    
    typedef enum
    {
	    PITCH_STATES_LIST(ENUMIFY)
    }PitchStates_e;
	
#define FilterSize 10

class FsmCamp :public virtual CState
{
	public:
	FsmCamp(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();
	
	private:
    void GetPitchRoll(void);
    void GetPitchRollCal(void);
    
    void LevelMachine(void);
	void SetPitchState(PitchStates_e s);
    void GetYZ(int16_t &slowy, int16_t &slowz, int16_t &fasty, int16_t &fastz);
    uint32_t FilterIt(int32_t* filter, int16_t roll);
	
	uint32_t Start;
	uint32_t DebugDelay;
	uint32_t MinTime;
	bool IsLevel;
	bool ReadyToSleep;
	PitchStates_e PitchState;
	uint32_t DebugTime;

    int32_t AvgY[FilterSize];
    int32_t AvgZ[FilterSize];
    int32_t AvgRoll[FilterSize];
    int32_t AvgPitch[FilterSize];
    uint16_t FilterStep;
    
    int16_t Roll;
    int16_t SlowRoll;
    int16_t Pitch;
    int16_t SlowPitch;
    int16_t RollCal;
    int16_t PitchCal;
    bool PowerLevel;

}; //Camp

#endif //__CAMP_H__
