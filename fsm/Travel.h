/* 
* Travel.h
*
* Created: 2/1/2015 10:14:47 PM
* Author: keith
*/


#ifndef __TRAVEL_H__
#define __TRAVEL_H__

#include "State.h"
#include "corner.h"

#define HEIGHTSIZE 100

 #define FILTER_STATES_LIST(macro) \
 macro(FilterStart),     \
 macro(FilterWait),     \
 macro(FilterLong)
 
 typedef enum
 {
     FILTER_STATES_LIST(ENUMIFY)
 }FilterStates_e;

class FsmTravel :public CState
{
	public:
	FsmTravel(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();
    void LeastSquares(int size, int16_t *x, int16_t *m, int16_t *b);
    
private:
    void UpdateHeights(void);
    
    
	CCorner LeftSide;
	CCorner RightSide;
	bool Starting;
	uint32_t Start;
    uint32_t filterwait;
    bool waiting;
    FilterStates_e FilterState;
    int16_t LeftHeights[HEIGHTSIZE];
    int16_t RightHeights[HEIGHTSIZE];
    int16_t LeftHeight;
    int16_t RightHeight;
    int HeightIndex;
}; //Travel

#endif //__TRAVEL_H__
