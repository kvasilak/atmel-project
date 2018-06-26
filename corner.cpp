/*************************************************************************
**   Microlevel 
**
** Copyright 2015 Keith Vasilakes
**
** This file is part of Microlevel, an Air ride controller.
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
**    corner.cpp
**************************************************************************/

//#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>
#include "corner.h"
#include "common.h"
#include "Cio.h"
#include "CSerial.h"
#include "CTimer.h"
#include "CADC.h"
#include "CLeds.h"

static const char *CornerName[] = {"Left,", "Right,"};
    
//This class is responsible for managing the height of a corner.
//There is one instance of this class for each corner of the vehicle usually 4
//This is where the decisions are made as to add or remove air from the suspension
CCorner::CCorner(Position p):
	DeadBand(DEAD_BAND),
    WideDeadBand(DEAD_BAND*5),
	CycleTime(100), //ms between updates
	State(ValveIniting),
	PulseTime(100),
    Setpoint(512),
	LimitLow(100),            // set limits to 100 from high and low
	LimitHigh(924),
    LongFilter(false)
   
{

	LastTime 		= CTimer::GetTick();
	filter_reg 		= GetAvgHeight() << FILTER_SHIFT;
   
    UpdateTime = CTimer::GetTick();
}

void CCorner::Init(Position p)
{
    count = 0;
    corner = p;
    SlowAt = 0;
    SmoothAt = 0;
    
    HeightSpeed = 0;
    
    FilterReset();  
    
    SetState(ValveIniting);
}
void CCorner::Limits(int16_t Low, int16_t high)
{
    LimitLow = Low +2;
    LimitHigh = high -2;
}
   
int32_t CCorner::GetAvgHeight()
{
    int32_t h = 0;
    
    switch(corner)
    {
        case LeftRear:
        h = CADC::is().GetLeftAvgHeight();
    
        break;
    case RightRear:
        h = CADC::is().GetRightAvgHeight();
        break;
    }
    return h;
}    

//open or close the fill solenoid for this corner
void CCorner::FillOn()
{
    switch(corner)
   {
        case LeftRear:
            Cio::is().LeftFillOn();
            break;
        case RightRear:
            Cio::is().RightFillOn();
            break;
   }   
}
void CCorner::FillOff()
{
	switch(corner)
	{
		case LeftRear:
			Cio::is().LeftFillOff();
			break;
		case RightRear:
			Cio::is().RightFillOff();
			break;
	}
}

//open or close the dump solenoid for this corner
void CCorner::DumpOn()
{
    switch(corner)
   {
        case LeftRear:
           Cio::is().LeftDumpOn();
            break;
        case RightRear:
            Cio::is().RightDumpOn();
            break;
   }   
}

void CCorner::DumpOff()
{
	switch(corner)
	{
		case LeftRear:
			Cio::is().LeftDumpOff();
			break;
		case RightRear:
			Cio::is().RightDumpOff();
			break;
	}
}

void CCorner::SetState(ValveOp s)
{
    static const char *StateStrs[] = {VALVE_STATES_LIST(STRINGIFY)};

    CSerial::is() << CornerName[corner];
    CSerial::is() << StateStrs[s] ;
    CSerial::is() << "\r\n";

    State = s;
}


void CCorner::SetSetPoint(int32_t height)
{
    Setpoint = height;
}

void CCorner::FilterReset()
{
    int i;

    int32_t height 			= GetAvgHeight();
    
    filter_reg = (height << FILTER_SHIFT);
    
    //initialize filters
    for(i=0; i<10; i++)
    {
        SmoothAvg[i] = height;
    }
    
    for(i=0; i<100; i++)
    {
        HeightAvg[i] = height;
    }
    
    int32_t slow = (filter_reg >> FILTER_SHIFT);
    
    CSerial::is() << CornerName[corner];
    CSerial::is() << "Filter reset " << height << ", reg " << slow <<"\n";
}

//Force the filters to the calibrated value
void CCorner::FilterForce(void)
{
    int i;

    if(true == LongFilter)
    {
        filter_reg = (Setpoint << FILTER_SHIFT);
    
        //initialize filters
        for(i=0; i<10; i++)
        {
            SmoothAvg[i] = Setpoint;
        }
    
        for(i=0; i<100; i++)
        {
            HeightAvg[i] = Setpoint;
        }
    
        int32_t slow = (filter_reg >> FILTER_SHIFT);
        
        slowheight = Setpoint;
    
        CSerial::is().Dec();
        CSerial::is() << CornerName[corner];
        CSerial::is() << "Filter Force " << (int16_t)Setpoint << ", slow " << (int16_t)slow <<"\n";
        CSerial::is().Hex();
    }    
}

uint16_t CCorner::Average(uint16_t value)
{
    uint32_t average = 0;
    
    //rollover if we hit the end
    if(SlowAt > 99) SlowAt = 0;
    
    //add new value to array
    HeightAvg[SlowAt] = value;
    
    SlowAt++;
    
    //Calculate new average
    for(int i=0; i<100; i++)
    {
        average += HeightAvg[i];
    }
    
    return (uint16_t)(average/100);
}

uint16_t CCorner::Smooth(uint16_t value)
{
    uint32_t average = 0;
    
    //rollover if we hit the end
    if(SmoothAt > 9) SmoothAt = 0;
    
    //add new value to array
    SmoothAvg[SmoothAt] = value;
    
    SmoothAt++;
    
    //Calculate new average
    for(int i=0; i<10; i++)
    {
        average += SmoothAvg[i];
    }
    
    return (uint16_t)(average/10);
}


void CCorner::SetLongFilter(bool slow) 
{
    //reset filters if switching to long filter
    if((false == LongFilter) && (true == slow))
    {
        LongFilter = true;
        FilterForce();
    }
    else
    {
        LongFilter = false;
    }

	if(slow)
	{
		CycleTime = 1000; //milliseconds between readings
	}
	else
	{
		CycleTime = 100; // fast updates
	}
	
}

void CCorner::NotAtHeight()
{
    //reset valve state machine
    SetState(ValveIniting);
}


bool CCorner::IsAtHeight(int32_t height)
{
     if( (height > (Setpoint - WideDeadBand)) && (height < (Setpoint + WideDeadBand)) )
     {
         return true;
     }

    return false;
}

//Uses the low pass IIR filter described in "Simple Software Lowpass Filter.pdf"
//And two simple FIR filters
void CCorner::FilterHeight(void)
{
    int32_t height 			= GetAvgHeight();
    
	//sample Slowly
	if(CTimer::IsTimedOut(CycleTime, LastTime) )
	{        
        //fast smoothing filter to take out sampling noise
        SmoothHeight = Smooth(height);
            
        //Longer filter to take out bumps and roll
        AverageHeight = Average( SmoothHeight);

        //IIR filter to take out even more bump and roll
        // Update IIR filter with current sample.
        filter_reg = filter_reg - (filter_reg >> FILTER_SHIFT) + AverageHeight;

        // Scale output for unity gain.
        slowheight = (filter_reg >> FILTER_SHIFT);


		LastTime = CTimer::GetTick();       
        
        if(CTimer::IsTimedOut(250, UpdateTime))
        {
           switch(corner)
           {
                case LeftRear:
                    CSerial::is() << "L Height: ";
                    break;
                case RightRear:
                    CSerial::is() << "R Height: ";
                    break;
           } 
           
           CSerial::is().Dec();
            CSerial::is() << (int16_t)height;
            
            CSerial::is() << " (";
            CSerial::is() << (int16_t)(height - Setpoint);
            CSerial::is() << ") ";
                               
            CSerial::is() << (int16_t)slowheight;
            
            CSerial::is() << " [";
            CSerial::is() << (int16_t)(slowheight - Setpoint);
            CSerial::is() << "]\n";
                               
           CSerial::is().Hex();

           UpdateTime = CTimer::GetTick();
        }
    }
}



void CCorner::Run(int32_t height)
{		
    switch (State)
    {
        case ValveIniting: 
            FillOff();
            DumpOff();
            
            if (height > 0) 
            {
                SetState(ValveHolding);
            }
            
            break;
        case ValveHolding:
            //below setpoint
                       
            if( height < (Setpoint - WideDeadBand))
            {
                FillOn();
                Cio::is().BlinkTravel(true);
                SetState(ValveFilling);
            }
            //if within WideDeadBand, only pulse the valve
            //so we don't over shoot the setpoint due to the long lag time
            else if(height < (Setpoint - DeadBand)   )
            {
                //might not be perfectly at height, but should be pretty close
                //IsAtHeight = true;
                FillOn();               

                //calc total pulse time as a multiple of deadbands from setpoint
                //we know height < setpoint or we wouldn't be here
                PulseTotal = /*((setpoint - height) / DeadBand) * */ PulseTime; 
                StartTime = CTimer::GetTick();
                
                SetState(ValveFillPulse);
            }
            else if(height > (Setpoint + WideDeadBand)) 
            {
                Cio::is().BlinkTravel(true);
                SetState(ValveDumping);
                DumpOn();   
            }              
            //if within WideDeadBand, only pulse the valve
            //so we don't over shoot the setpoint due to the long lag time
            else if(height > (Setpoint + DeadBand) )
            {                        
                //might not be perfectly at height, but should be pretty close
                //IsAtHeight = true;
                DumpOn();
                
                //calc total pulse time as a multiple of deadbands from setpoint
                //we know height > setpoint or we wouldn't be here
                PulseTotal = /*((height - setpoint) / DeadBand) * */ PulseTime; 
                StartTime = CTimer::GetTick();

                SetState(ValveDumpPulse);
            }
            else //within +- 1 deadband
            {
                //CSerial::is() << CornerName[corner];
                 //CSerial::is() << "At Height\n";
            }

            break;
        case ValveTweek:
            if(height < (Setpoint - DeadBand)   )
            {
                //NotAtHeight();
                FillOn();

                //calc total pulse time as a multiple of deadbands from setpoint
                //we know height < setpoint or we wouldn't be here
                PulseTotal = /*((setpoint - height) / DeadBand) * */ PulseTime;
                StartTime = CTimer::GetTick();
            
                SetState(ValveFillPulse);
            }
            else if(height > (Setpoint + DeadBand) )
            {
                //NotAtHeight();
                DumpOn();
            
                //calc total pulse time as a multiple of deadbands from setpoint
                //we know height > setpoint or we wouldn't be here
                PulseTotal = /*((height - setpoint) / DeadBand) * */ PulseTime;
                StartTime = CTimer::GetTick();

                SetState(ValveDumpPulse);
            }
            break;
        case ValveFillPulse:
            if(CTimer::IsTimedOut(PulseTotal, StartTime))
            {
                FillOff();
                //FilterForce();
                StartTime = CTimer::GetTick();
                SetState(ValveTweekDelay);
            }
            break;
        case ValveFilling:
            if(height >= (Setpoint-WideDeadBand))
            {    
                FillOff();
                StartTime = CTimer::GetTick();
                SetState(ValveCycleDelay);
            }
            break;
        case ValveDumpPulse:
            if(CTimer::IsTimedOut(PulseTotal, StartTime))
            {
                DumpOff();
                //FilterForce();
                StartTime = CTimer::GetTick();
                SetState(ValveTweekDelay);
            }
            break;
        case ValveDumping:
            if(height <= (Setpoint + WideDeadBand))
            {    
                DumpOff();
                StartTime = CTimer::GetTick();
                SetState(ValveCycleDelay);
            }
            break;
        case ValveCycleDelay:
            if(CTimer::IsTimedOut(1000, StartTime))
            {
                SetState(ValveHolding);
            }
            break;
        case ValveTweekDelay:
            if(CTimer::IsTimedOut(1000, StartTime))
            {
                SetState(ValveTweek);
            }
            break;
        default:
            CSerial::is() << ">Corner,Default,State!!<";
            FillOff();
            DumpOff();
            SetState(ValveHolding);
    }
}

//used to calibrate height limits.
//the coach may not have travel from 0 to 1024
//wait for both heights to be all the way down
//determined by the height changing less that 2 counts in 1 second
//Waits 10 seconds for the cases where it is slow to start or stop moving
bool CCorner::IsMoving()
{
    bool ret = true;

    //Serial.print(">AirRide,msg,speed; ");
    //Serial.print(HeightSpeed);
    //Serial.println("<");
    
    if(HeightSpeed > 1)
    {
        ret =  true;
    }
    else
    {
        ret =  false;
    }
    return ret;
}




