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
	LimitLow(100),            // set limits to 100 from high and low
	LimitHigh(924),
	LongFilter(false),
	IsAtHeight(false)
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

    CSerial::is() << ">Corner,";
    CSerial::is() << CornerName[corner];
    CSerial::is() << StateStrs[s] ;
    CSerial::is() << "<\r\n";

    State = s;
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
void CCorner::FilterForce(int32_t setpoint)
{
    int i;

    if(true == LongFilter)
    {
        filter_reg = (setpoint << FILTER_SHIFT);
    
        //initialize filters
        for(i=0; i<10; i++)
        {
            SmoothAvg[i] = setpoint;
        }
    
        for(i=0; i<100; i++)
        {
            HeightAvg[i] = setpoint;
        }
    
        int32_t slow = (filter_reg >> FILTER_SHIFT);
        
        slowheight = setpoint;
    
        CSerial::is().Dec();
        CSerial::is() << CornerName[corner];
        CSerial::is() << "Filter Force " << (int16_t)setpoint << ", slow " << (int16_t)slow <<"\n";
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


void CCorner::SetLongFilter(bool slow, int32_t setpoint)
{
    //reset filters if switching to long filter
    if((false == LongFilter) && (true == slow))
    {
        LongFilter = true;
        FilterForce(setpoint);
    }
    else
    {
        LongFilter = false;
    }

	if(slow)
	{
		CycleTime = 10000; //seconds between readings
        //CycleTime = 1000; //testing
	}
	else
	{
        IsAtHeight = false;
		CycleTime = 100; //back to fast updates
	}
	
}

void CCorner::AtHeight(bool at)
{
    IsAtHeight = at;
	
	//reset valve state machine
	SetState(ValveIniting);
}

void CCorner::NotAtHeight()
{
    if( !LongFilter)
    {
        IsAtHeight = false;
    }
}


bool CCorner::AtHeight()
{
    return IsAtHeight;
}

//Uses the low pass IIR filter described in "Simple Software Lowpass Filter.pdf"
//And two simple FIR filters
void CCorner::FilterHeight(int32_t setpoint)
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
            CSerial::is() << (int16_t)(height - setpoint);
            CSerial::is() << ") ";
                               
            CSerial::is() << (int16_t)slowheight;
            
            CSerial::is() << " [";
            CSerial::is() << (int16_t)(slowheight - setpoint);
            CSerial::is() << "]\n";
                               
           CSerial::is().Hex();

           UpdateTime = CTimer::GetTick();
        }
    }
}



void CCorner::Run(int32_t setpoint)
{		
    FilterHeight(setpoint); 
    
    int16_t height;
    
     if(LongFilter) 
     { 
         height = slowheight; 
     }
     else
     {
          height = GetAvgHeight();
     }

    switch (State)
    {
        case ValveIniting: 
            FillOff();
            DumpOff();
            SetState(ValveHolding);
            break;
        case ValveHolding:
            //below setpoint
                       
            if( height < (setpoint - WideDeadBand))
            {
                NotAtHeight();
                FillOn();
                Cio::is().BlinkTravel(true);
                SetLongFilter(false,setpoint);
                SetState(ValveFilling);
            }
            //if within WideDeadBand, only pulse the valve
            //so we don't over shoot the setpoint due to the long lag time
            else if(height < (setpoint - DeadBand)   )
            {
                NotAtHeight();
                FillOn();               

                //calc total pulse time as a multiple of deadbands from setpoint
                //we know height < setpoint or we wouldn't be here
                PulseTotal = /*((setpoint - height) / DeadBand) * */ PulseTime; 
                StartTime = CTimer::GetTick();

                CSerial::is().Dec();
                CSerial::is() << CornerName[corner];
                CSerial::is() << "Fill Pulse\n";
                CSerial::is().Hex();
                
                SetState(ValveFillPulse);
            }
            else if(height > (setpoint + WideDeadBand)) 
            {
                NotAtHeight();
                Cio::is().BlinkTravel(true);
                SetLongFilter(false, setpoint);
                SetState(ValveDumping);
                DumpOn();   
            }              
            //if within WideDeadBand, only pulse the valve
            //so we don't over shoot the setpoint due to the long lag time
            else if(height > (setpoint + DeadBand) )
            {                        
                NotAtHeight();
                DumpOn();
                
                //calc total pulse time as a multiple of deadbands from setpoint
                //we know height > setpoint or we wouldn't be here
                PulseTotal = /*((height - setpoint) / DeadBand) * */ PulseTime; 
                StartTime = CTimer::GetTick();
                
                CSerial::is().Dec();
                 CSerial::is() << CornerName[corner];
                 CSerial::is() << " Dump Pulse\n";
                 CSerial::is().Hex();

                SetState(ValveDumpPulse);
            }
            else //within +- 1 deadband
            {
                //CSerial::is() << CornerName[corner];
                 //CSerial::is() << "At Height\n";
                 
                //might not be perfectly at height, but should be pretty close
                IsAtHeight = true;
            }

            break;
        case ValveFillPulse:
            if(CTimer::IsTimedOut(PulseTotal, StartTime))
            {
                FillOff();
                FilterForce(setpoint);
                StartTime = CTimer::GetTick();
                SetState(ValveCycleDelay);
            }
            break;
        case ValveFilling:
            if(height >= (setpoint-WideDeadBand))
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
                FilterForce(setpoint);
                StartTime = CTimer::GetTick();
                SetState(ValveCycleDelay);
            }
            break;
        case ValveDumping:
            if(height <= (setpoint + WideDeadBand))
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




