/*************************************************************************
**   Microlevel
**
** Copyright 2015 Keith Vasilakes
**
** This file is part of Microlevel an Air ride controller.
**
** Microlevel is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation, either version 3 of the License, or (at your option) 
** any later version.
**
** Microlevel is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
** implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public 
** License for more details.
**
** You should have received a copy of the GNU General Public License along with Microlevel. If not, see
** http://www.gnu.org/licenses/.
**
**    Corner.h

todo
there is a lagtime from opening/closing the valve to a height change.
need to predict movement or use shorter bursts to sneak up on height

**************************************************************************/

#ifndef INCLUDE_CORNER_H
#define INCLUDE_CORNER_H

#include <stdint.h>
#include "common.h"

enum Position { 
        LeftRear, 
        RightRear
        };


//enum ValveOp { Filling, Dumping, Holding, LastState };
	
    #define VALVE_STATES_LIST(macro)\
        macro(ValveIniting),     \
        macro(ValveFilling),      \
        macro(ValveFillPulse),    \
        macro(ValveDumping),      \
        macro(ValveDumpPulse),    \
        macro(ValveHoldEntry),    \
        macro(ValveHolding),      \
        macro(ValveCycleDelay),     \
        macro(ValveLastState)
    
typedef enum 
{
  VALVE_STATES_LIST(ENUMIFY)
}ValveOp;


// Parameter K for the filter
#define FILTER_SHIFT    8//8
#define DEAD_BAND       7//10

class CCorner 
{
	public:
	CCorner(Position c);
	void Run(int32_t height);
    void FillOn();
	void FillOff();
    void DumpOn();
	void DumpOff();
    void Init(Position p);
    void Limits(int16_t Low, int16_t high);
    void SetLongFilter(bool slow);
    void AtHeight(bool at);
    void NotAtHeight();
    bool AtHeight();
    int16_t GetHeight();
    void FilterHeight( void);
    bool IsMoving();
    void SetSetPoint(int32_t height);
  
	private:
	int16_t DeadBand;			//how far the corner has to move before we try to adjust the position
    int16_t WideDeadBand;
	Position corner;
	uint32_t CycleTime;		    //The minimum time between valve state changes. Prevents valve chatter
	ValveOp State;
	uint32_t LastTime;		    //the last time valve changed state
	int32_t filter_reg; 
    uint32_t StartTime;
    uint32_t UpdateTime;
    uint32_t PulseTotal;
    uint32_t PulseTime;
    int32_t Setpoint;
    
    int16_t LimitLow;
    int16_t LimitHigh;
    
   
    void SetState(ValveOp s);
    
    int16_t HeightAvg[100];
    int32_t AverageHeight;
    uint16_t Average(uint16_t value);
    
    int16_t SmoothAvg[10];
    int32_t SmoothHeight;
    uint16_t Smooth(uint16_t value);
    void FilterReset(void);
    void FilterForce(void);
    int32_t GetAvgHeight();
    
    bool LongFilter;
    int SlowAt;
    int SmoothAt;
    int32_t slowheight; 
    int count;
    bool IsAtHeight;
    
    int32_t HeightSpeed;
    int32_t OldHeight;
    uint32_t SpeedTime;
    int32_t HeightAverages[10];
    int HeightAverageCount;
    
};
#endif //INCLUDE_CORNER_H
