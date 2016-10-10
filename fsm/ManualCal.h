/* 
* MauualCal.h
*
* Created: 10/8/2016 5:27:24 PM
* Author: keith
*/


#ifndef __MAUUALCAL_H__
#define __MAUUALCAL_H__

#include <stdint.h>
#include "State.h"
#include "States.h"

class ManualCal :public CState
{
//variables
public:
	ManualCal(CController& FsmManager);
	void OnEntry();
	void HandleEvent(eEvents evt);
	void OnExit();
	
protected:
private:
	ManualCal();

//functions
public:
	
protected:
private:
	ManualCal( const ManualCal &c );
	ManualCal& operator=( const ManualCal &c );
	uint32_t Blink;

}; //MauualCal

#endif //__MAUUALCAL_H__
