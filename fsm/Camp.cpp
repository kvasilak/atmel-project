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
SlowPitch(0),
PowerLevel(true)
{
}

void FsmCamp::OnEntry()
{
	CLeds::is().CampOn();
    
    CMMA8451::is().writeRegister8(0x2A, 0x3d); //slow rate, low noise, Active
    	
    CMMA8451::is().writeRegister8(0x2B, 2); //high res
    
    GetPitchRollCal();

	CSerial::is() << " FsmCamp::OnEntry(), roll cal; " << RollCal << " pitch cal; " << PitchCal << " tol; " << PitchTol << "\r\n";
	Start = CTimer::GetTick();
	
	IsLevel  = false;
	ReadyToSleep = false;
    
    GetPitchRoll();
    
    for(int i=0; i<FilterSize; i++)
    {
        AvgRoll[i] = Roll;
        AvgPitch[i] = Pitch;   
    }

    RollFilterStep = 0;
    PitchFilterStep = 0;

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
		case eEvents::CampEvent:

			if(Cio::is().CampSwitches())
			{
				CSerial::is() << " FsmCamp::camp switches\r\n";
					
				m_SMManager.ChangeState(eStates::STATE_MANUAL);
			}
		break;
		case eEvents::TravelEvent:
			if(Cio::is().TravelSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_TRAVEL);
			}
		break;
		case eEvents::CalibrateEvent:
			if(Cio::is().CalibrateSwitch())
			{
				m_SMManager.ChangeState(eStates::STATE_CAMP_CALIBRATE);
			}
		break;
        case eEvents::ButtonWakeEvent:
        case eEvents::IgnitionOnEvent:
            if( Cio::is().Awake == false)
            {            
                Cio::is().Awake = true;

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


uint16_t FsmCamp::FilterIt(int32_t* filter, uint8_t *FilterStep, int16_t it)
{
    int32_t Avg=0;
    
    filter[*FilterStep] = it;
    
    if(++(*FilterStep) >= FilterSize) *FilterStep = 0;

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

    //Read Acccel
    CMMA8451::is().ReadXYZ(X, Y, Z);
    
    if(PowerLevel)
    {
        Roll = Y;
        
        Pitch = Z;
    }
    else
    {
        Roll = Z;
        
        Pitch = Y;
    }
    
    SlowRoll = FilterIt( AvgRoll, &RollFilterStep, Roll);
    
    SlowPitch =  FilterIt(AvgPitch, &PitchFilterStep, Pitch);
    
    CSerial::is() << "Roll;  " << Roll << ", Avg; " <<  (int16_t)SlowRoll << ", err; " << (int16_t)(SlowRoll - RollCal) << "\n";
    CSerial::is() << "Pitch; " << Pitch <<  ", Avg; " <<  (int16_t)SlowPitch << ", err; " << (int16_t)(SlowPitch - PitchCal) << "\n";
    
    SetPitchState(PitchState);
   
}


void FsmCamp::GetPitchRollCal(void)
{
    //int16_t xcal = nvm::is().GetCampX();
    int16_t ycal = nvm::is().GetCampY();
    int16_t zcal = nvm::is().GetCampZ();
    
    int16_t absZcal = zcal > 0? zcal: 0-zcal;
    
    //if microlevel is vertical +- 10 degrees we must be power level
    
    if(absZcal > QUARTER_G)
    {
       PowerLevel = false; 
       
       CSerial::is() << "** Electro Level Cal!!\n";
    }
    else
    {
        PowerLevel = true;
        
        CSerial::is() << "** Power Level Cal!!\n";
    }
    
    if(PowerLevel )
    {
        //powerlevel
      RollCal = ycal;;
      PitchCal = zcal;  
    }
    else
    {
        //Electrolevel
        RollCal = zcal;;
        PitchCal = ycal;
    }    
}

bool FsmCamp::IsPitchUp(bool slow)
{
    bool up = false;
    
    int16_t pitch = Pitch;
    int16_t tol = PitchTol;
    
    if(slow)
    {
        pitch = SlowPitch;
        tol = PitchTol * 2;
    }
    
    if(PowerLevel )
    {
       if( pitch > PitchCal + tol)
        {
            up = true;
        }
    }
    else
    {
        //Electro Level pitch is backwards
        //if( pitch < PitchCal + tol)
        if( pitch < PitchCal - tol)
        {
            up = true;
        }
    }
    
    return up;
}

bool FsmCamp::IsPitchDown(bool slow)
{
     bool down = false;
     
     int16_t pitch = Pitch;
     int16_t tol = PitchTol;
     
     if(slow)
     {
         pitch = SlowPitch;
         tol = PitchTol * 2;  
     }         
     
     if(PowerLevel )
     {
         if( pitch < PitchCal - tol)
         {
             down = true;
         }
     }
     else
     {
         //Electro Level pitch is backwards
         //if( pitch < PitchCal - tol)
         if( pitch > PitchCal + tol)
         {
             down = true;
         }
     }
     
     return down;
}

bool FsmCamp::IsPitchOK(void)
{
    bool ok = false;
    if(PowerLevel )
    {
        if( (Pitch > PitchCal - PitchTol) && (Pitch < PitchCal + PitchTol))//we're level
        {
            ok = true;
        }
    }  
    else
    {
        if( (Pitch > PitchCal - PitchTol) && (Pitch < PitchCal + PitchTol))//we're level
        {
            ok = true;
        }
    }  

return ok;
}

void FsmCamp::LevelMachine(void)
{

    static uint32_t Current = 0;
    static const uint32_t CheckTime = 500; //milliseconds
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
                if(IsPitchUp())
                {
                    SetPitchState(CampNoseHigh);
                }
                //Nose Low
                else  if( IsPitchDown()) 
                {
                    SetPitchState(CampNoseLow);
                }
                else //we're level pitch wise, check roll
                {
                    SetPitchState(CampLevel);
                }
                
		    break;
		    case CampNoseHigh:
			    //Nose down ( rear too Damn low)
                if( IsPitchDown() )
                {
                    SetPitchState(CampNoseLow);
                }
                //we're level, check roll
                else if( IsPitchOK()) 
                {
                    SetPitchState(CampLevel);
                }
                else //still nose high
               {
                    //decide which side to raise
                    if(Roll < RollCal - RollTol)
                    {
                        Cio::is().Right(eValveStates::Hold);
                        Cio::is().Left(eValveStates::Fill);
                    }
                    else if(Roll > RollCal + RollTol) 
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
		
		    case CampNoseLow:
                //Nose High
                if( IsPitchUp()) 
                {
                    SetPitchState(CampNoseHigh);
                }
                    
                //we're level, check roll
                else if( IsPitchOK()) 
                {
                    SetPitchState(CampLevel);
                }
			    else //still low
			    {
                    //decide which side to lower
                    if(Roll < RollCal - RollTol)
                    {
                        Cio::is().Left(eValveStates::Hold);
                        Cio::is().Right(eValveStates::Dump);
                    }
                    else if(Roll > RollCal + RollTol) 
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
                 //Nose High
                 if( IsPitchUp()) 
                 {
                     SetPitchState(CampNoseHigh);
                 }
                 //rear high
                 else  if( IsPitchDown()) 
                 {
                     SetPitchState(CampNoseLow);
                 }
                 else //Pitch and roll level
                 {
                    if(Roll < RollCal - RollTol)
                    {
                        Cio::is().Right(eValveStates::Dump);
                        Cio::is().Left(eValveStates::Fill);
                    }
                    else if(Roll > RollCal + RollTol) 
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
                    if(  IsPitchUp(true)) 
                    {
                        SetPitchState(CampNoseHigh);
                    }
                    //rear high
                    else  if( IsPitchDown(true)) 
                    {
                        SetPitchState(CampNoseLow);
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
