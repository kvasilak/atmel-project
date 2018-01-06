/* 
* Camp.cpp
*
* Created: 2/1/2015 10:14:57 PM
* Author: keith
*/


#include "Camp.h"
#include "..\CController.h"
#include "..\CLeds.h"
#include "..\CSerial.h"
#include "nvm.h"
#include "CTimer.h"
#include "string.h"

static const int16_t PitchTol = 20;
static const int16_t RollTol = 20;//10;

#define ONE_G 4096    // 2g mode so 4096 counts per G
#define HALF_G 2048   // 1/2 G = 2048
#define Third_G 1356  // 1/3 G = 1365
#define QUARTER_G 1024 // 1/4 G = 1024

FsmCamp::FsmCamp(CController& SMManager) :
CState(SMManager, eStates::STATE_CAMP),
Start(0),
MinTime(100),
IsLevel(false),
ReadyToSleep(false),
DebugTime(1000),
Roll(0),
SlowRoll(0),
Pitch(0),
SlowPitch(0)
PowerLevel(true)
{
}

void FsmCamp::OnEntry()
{
	CLeds::is().CampOn();
    
    CMMA8451::is().writeRegister8(0x2A, 0x3d); //slow rate, low noise, Active
    	
    CMMA8451::is().writeRegister8(0x2B, 2); //high res
    
    GetPitchRollCal();

	CSerial::is() << " FsmCamp::OnEntry(), roll cal; " << RollCal << " pitch cal; " << PitchCal << " tol; " << pitchtol << "\r\n";
	Start = CTimer::GetTick();
	
	IsLevel  = false;
	ReadyToSleep = false;
    
    int16_t x,y,z;
    CMMA8451::is().ReadXYZ(x,y,z);
    
    for(int i=0; i<FilterSize; i++)
    {
        AvgX[i] = x;
        AvgY[i] = y;
        AvgZ[i] = z;        
    }

    FilterStep = 0;

    SetPitchState(CampIniting);
	
	DebugDelay = CTimer::GetTick();
}

void FsmCamp::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run camp FSM 
            LevelMachine();

		break;
		case eEvents::RockerEvent:
		case eEvents::OutSideEvent:
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::SteeringEvent:
			Cio::is().SteeringRemote();
			m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
		break;
		case eEvents::CampEvent:
			CSerial::is() << " FsmCamp::Camp event\r\n";
            
			if(Cio::is().CampSwitches())
			{
				CSerial::is() << " FsmCamp::camp switches\r\n";
					
				m_SMManager.ChangeState(eStates::STATE_MANUAL);
			}
		break;
		case eEvents::TravelEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL);
		break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_CAMP_CALIBRATE);
		break;
        case eEvents::ButtonWakeEvent:
        case eEvents::IgnitionOnEvent:
            if( Cio::is().Awake == false)
            {            
                Cio::is().Awake = true;
            
                //Cio::is().ButtonWake = false;

               // ReadyToSleep = false;
                IsLevel = false;

                Cio::is().Wakeup();
                CLeds::is().CampOn();
                //relevel on wakeup
                SetPitchState(CampIniting);
            
                Start = CTimer::GetTick();
            
                CSerial::is() << " FsmCamp::Ignition On\r\n";
            }
            break;

        //let the manual state handle it
		case eEvents::IgnitionOffEvent:
			CSerial::is() << " FsmCamp::Ignition Off\r\n";
            m_SMManager.ChangeState(eStates::STATE_MANUAL, evt);
			break;
		default:
		break;
	}
}

void FsmCamp::OnExit()
{
	CSerial::is() << " FsmCamp::OnExit()\r\n";
	CLeds::is().CampOff();
}

void FsmCamp::SetPitchState(PitchStates_e s)
{
	static const char *StateStrs[] = {PITCH_STATES_LIST(STRINGIFY)};

	CSerial::is() << "Pitch State, ";
	CSerial::is() << StateStrs[s] ;
	CSerial::is() << "\n";

	PitchState = s;
}


//Get Filtered Y and Z
void FsmCamp::GetYZ(int16_t &slowy, int16_t &slowz,int16_t &fasty, int16_t &fastz)
{
    int16_t X=0;
	int16_t Y=0;
	int16_t Z=0;

    int32_t YAvg;
    int32_t ZAvg;

	//Read Acccel
	CMMA8451::is().ReadXYZ(X, Y, Z);
    fasty = Y;
    fastz = Z;

    AvgY[FilterStep] = Y;
    AvgZ[FilterStep] = Z;
    
    if(++FilterStep >= FilterSize) FilterStep = 0;

    YAvg = AvgY[0];
    ZAvg = AvgZ[0];

    for(int i=1;i<FilterSize; i++)
    {
        YAvg += AvgY[i];
        ZAvg += AvgZ[i];
    }
    
    YAvg = YAvg / FilterSize;
    ZAvg /= FilterSize;

    slowy = YAvg;
    slowz = ZAvg;
    
     if(CTimer::IsTimedOut(DebugTime, DebugDelay))
     {
         //determine Pitch and roll errors
         //int32_t rollcal = nvm::is().GetCampY();
         //int32_t pitchcal = nvm::is().GetCampZ();
         
         
         CMMA8451::is().ReadXYZ();
         //CSerial::is() << "Roll;  " << Y << ", Avg; " <<  (int16_t)YAvg << ", err; " << (int16_t)(YAvg - rollcal) << "\n";
         //CSerial::is() << "Pitch; " << Z <<  ", Avg; " <<  (int16_t)ZAvg << ", err; " << (int16_t)(ZAvg -pitchcal) << "\n";
         
         DebugDelay = CTimer::GetTick();
     }
}

uint16_t FsmCamp::FilterIt(int32_t* filter, int16_t roll)
{
    int32_t Avg=0;
    
    filter[FilterStep] = roll;
    
    if(++FilterStep >= FilterSize) FilterStep = 0;

    for(int i=0;i<FilterSize; i++)
    {
        Avg += filter[i];
    }
    
    Avg /= FilterSize;

    return Avg;
}
    

void FsmCamp::GetPitchRoll(void)
{
    int16_t X;
    int16_t Y;
    int16_t Z;

    //int32_t YAvg;
    //int32_t ZAvg;

    //Read Acccel
    CMMA8451::is().ReadXYZ(X, Y, Z);
    
     //double pitch, roll, Xg, Yg, Zg;
    //acc.read(&Xg, &Yg, &Zg);
//
    //
    ////Roll & Pitch Equations
    //roll  = (atan2(-fYg, fZg)*180.0)/M_PI;
    //pitch = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg))*180.0)/M_PI;

    
    if(PowerLevel)
    {
        Roll = Y;
        
        Pitch = Z;
    }
    else
    {
        Roll = Z;// X;
        
        Pitch = Y;// Z;
    }
    
    SlowRoll = FilterIt( AvgRoll, Roll);
    
    SlowPitch =  FilterIt(AvgPitch, Pitch);
    
}


void FsmCamp::GetPitchRollCal(void)
{
    int16_t xcal = nvm::is().GetCampX();
    int16_t ycal = nvm::is().GetCampY();
    int16_t zcal = nvm::is().GetCampZ();
    
    int16_t absZcal = zcal > 0? zcal: 0-zcal;
    
    //if microlevel is vertical +- 10 degrees we must be power level
    
    if(absZcal > QUARTER_G)
    {
       PowerLevel = false; 
    }
    else
    {
        PowerLevel = true;
    }
    
    if(PowerLevel )
    {
        //powerlevel
      RollCal = xcal;;
      PitchCal = zcal;  
    }
    else
    {
        //Electrolevel
        RollCal = zcal;;
        PitchCal = ycal;
    }    
}


void FsmCamp::LevelMachine(void)
{

    static uint32_t Current = 0;
    static const uint32_t CheckTime = 100; //milliseconds
    static uint32_t CompleteStart =  0;
    static const uint32_t ReCheckTime = 5; // seconds before we recheck levelness once level

    //Only check level every 100ms
    if(CTimer::IsTimedOut(CheckTime, Current))
    {
        Current = CTimer::GetTick();

        GetPitchRoll(); 

	    switch(PitchState)
	    {
		    case CampIniting:
			    Cio::is().Left(eValveStates::Hold);
			    Cio::is().Right(eValveStates::Hold);
			
                //first get coach level front to rear
                //rear low
                if( Pitch > PitchCal + PitchTol )
                {
                    SetPitchState(CampRearLow);
                }
                //rear high
                else  if( Pitch < PitchCal - PitchTol )
                {
                    SetPitchState(CampRearHigh);
                }
                else //we're level pitch wise, check roll
                {
                    SetPitchState(CampLevel);
                }
                
		    break;
		    case CampRearLow:
			    //pitch up ( rear too Damn low)
                //rear high
                if( Pitch < (PitchCal - PitchTol) )
                {
                    SetPitchState(CampRearHigh);
                }
                //we're level, check roll
                else if( (Pitch > PitchCal - PitchTol) && (Pitch < PitchCal + PitchTol))
                {
                    SetPitchState(CampLevel);
                }
                else //still low
                {
                    //decide which side to raise
                    if(Roll < RollCal - RollTol)//Roll up ( Left down )
                    {
                        Cio::is().Right(eValveStates::Hold);
                        Cio::is().Left(eValveStates::Fill);
                    }
                    else if(Roll > RollCal + RollTol) //Roll down (Right Down)
                    {
                        Cio::is().Left(eValveStates::Hold);
                        Cio::is().Right(eValveStates::Fill);
                    }
                    else //roll OK
                    {
                        Cio::is().Left(eValveStates::Fill);
                        Cio::is().Right(eValveStates::Fill);
                    }
                }
		        break;
		
		    case CampRearHigh:
                //rear low
                if( Pitch > PitchCal + PitchTol )
                {
                    SetPitchState(CampRearLow);
                }
                    
                //we're level, check roll
                else if( (Pitch > PitchCal - PitchTol) && (Pitch < PitchCal + PitchTol))//we're level
                {
                    SetPitchState(CampLevel);
                }
			    else //still high
			    {
                    //decide which side to lower
                    if(Roll < RollCal - RollTol)//Roll up ( Left down )
                    {
                        Cio::is().Left(eValveStates::Hold);
                        Cio::is().Right(eValveStates::Dump);
                    }
                    else if(Roll > RollCal + RollTol) //Roll down (Right Down)
                    {
                        Cio::is().Right(eValveStates::Hold);
                        Cio::is().Left(eValveStates::Dump);
                    }
                    else //Roll level, both down
                    {
                        Cio::is().Left(eValveStates::Dump);
                        Cio::is().Right(eValveStates::Dump);
                    }
			    }
		        break;
		    case CampLevel:
                 //rear low
                 if( Pitch > PitchCal + PitchTol )
                 {
                     SetPitchState(CampRearLow);
                 }
                 //rear high
                 else  if( Pitch < PitchCal - PitchTol )
                 {
                     SetPitchState(CampRearHigh);
                 }
                 else //Pitch and roll level
                 {
                    if(Roll < RollCal - RollTol)//Roll up ( Left down )
                    {
                        Cio::is().Right(eValveStates::Dump);
                        Cio::is().Left(eValveStates::Fill);
                    }
                    else if(Roll > RollCal + RollTol) //Roll down (Right Down)
                    {
                        Cio::is().Left(eValveStates::Dump);
                        Cio::is().Right(eValveStates::Fill);
                    }
                    else //roll level
                    {
                        Cio::is().Left(eValveStates::Hold);
                        Cio::is().Right(eValveStates::Hold);
                        
                        SetPitchState(CampCompleteEnter);
                    }                      
                 }                                  
		        break;
            case CampCompleteEnter:
                CompleteStart = CTimer::GetTick();
                SetPitchState(CampComplete);
                break;
            case CampComplete:
                //rear low, don't adjust till we change significantly
                //and then SLOWLY
                if(CTimer::IsTimedOut(ReCheckTime, CompleteStart))
                {
                    if( SlowPitch > PitchCal + (PitchTol *2))
                    {
                        SetPitchState(CampRearLow);
                    }
                    //rear high
                    else  if( SlowPitch < PitchCal - (PitchTol *2) )
                    {
                        SetPitchState(CampRearHigh);
                    }
                    else //pitch is level check roll
                    {
                        if( (SlowRoll < RollCal - (RollTol *2)) || (SlowRoll > RollCal + (RollTol*2)))
                        {
                            //Check roll
                            SetPitchState(CampLevel);
                        }  
                        else //all level, restart timer for next check
                        {
                            CompleteStart =  CTimer::GetTick();
                        }          
                    }
                }
                break;
	    }
    }        
}
