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

static const int16_t pitchtol = 10;
static const int16_t rolltol = 5;//10;

FsmCamp::FsmCamp(CController& SMManager) :
CState(SMManager, eStates::STATE_CAMP),
Start(0),
MinTime(100),
IsLevel(false),
ReadyToSleep(false),
DebugTime(1000)
{
}

void FsmCamp::OnEntry()
{
	CLeds::is().CampOn();
	
	int16_t roll = nvm::is().GetCampY();
 	int16_t pitch = nvm::is().GetCampZ();

	CSerial::is() << " FsmCamp::OnEntry(), roll cal; " << roll << " pitch cal; " << pitch << "\r\n";
	Start = CTimer::GetTick();
	
	CMMA8451::is().writeRegister8(0x2A, 0x3d); //slow rate, low noise, Active
	
	CMMA8451::is().writeRegister8(0x2B, 2); //high res
	
	IsLevel  = false;
	ReadyToSleep = false;
    
    int16_t y,z;
    GetYZ(&y, &z);
    
    memset(AvgY, y, FilterSize);
    memset(AvgZ, z, FilterSize);
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

	CSerial::is() << ">>>>Pitch, ";
	CSerial::is() << StateStrs[s] ;
	CSerial::is() << "<\r\n";

	PitchState = s;
}


//Get Filtered Y and Z
void FsmCamp::GetYZ(int16_t *y, int16_t *z)
{
    int16_t X=0;
	int16_t Y=0;
	int16_t Z=0;

    int32_t YAvg;
    int32_t ZAvg;

	//determine Pitch and roll errors
	int16_t rollcal = nvm::is().GetCampY();
	int16_t pitchcal = nvm::is().GetCampZ();
    
    //Read Acccel
	CMMA8451::is().ReadXYZ(X, Y, Z);
    
    if(CTimer::IsTimedOut(DebugTime, DebugDelay))
    {
        CSerial::is() << "X; " << X << ", Y; " << Y << ", Z; " << Z ;
        
        CSerial::is()  << "** roll err; " << Y - rollcal  << ";   pitch err; " << Z -pitchcal << "\n";
        
        DebugDelay = CTimer::GetTick();
    }

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
    
    YAvg /= FilterSize;
    ZAvg /= FilterSize;

    *y = YAvg;
    *z = ZAvg;
}

void FsmCamp::LevelMachine()
{
	int16_t Y=0;
	int16_t Z=0;

    static uint32_t Current = 0;
    static const uint32_t CheckTime = 100; //milliseconds
    static uint32_t CompleteStart =  0;
    static const uint32_t ReCheckTime = 5; // seconds before we recheck levelness once level

    //Only check level every 100ms
    if(CTimer::IsTimedOut(CheckTime, Current))
    {
        Current = CTimer::GetTick();

        GetYZ(&Y, &Z);

	    //determine Pitch and roll errors
	    int16_t rollcal = nvm::is().GetCampY();

	    int16_t pitchcal = nvm::is().GetCampZ();

	    switch(PitchState)
	    {
		    case CampIniting:
			    Cio::is().Left(eValveStates::Hold);
			    Cio::is().Right(eValveStates::Hold);
			
                //first get coach level front to rear
                //rear low
                if( Z	> pitchcal + pitchtol )
                {
                    SetPitchState(CampRearLow);
                }
                //rear high
                else  if( Z < pitchcal - pitchtol )
                {
                    SetPitchState(CampRearHigh);
                }
                else //we're levelpitchwise, check roll
                {
                    SetPitchState(CampLevel);
                }
                
		    break;
		    case CampRearLow:
			    //pitch up ( rear too Damn low)
                //rear high
                if( Z < pitchcal - pitchtol )
                {
                    SetPitchState(CampRearHigh);
                }
                //we're level, check roll
                else if( (Z > pitchcal - pitchtol) && (Z < pitchcal + pitchtol))
                {
                    SetPitchState(CampLevel);
                }
                else //still low
                {
                    //decide which side to raise
                    if(Y < rollcal - rolltol)//Roll up ( Left down )
                    {
                        Cio::is().Right(eValveStates::Hold);
                        Cio::is().Left(eValveStates::Fill);
                    }
                    else if(Y > rollcal + rolltol) //Roll down (Right Down)
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
                if( Z > pitchcal + pitchtol )
                {
                    SetPitchState(CampRearLow);
                }
                    
                //we're level, check roll
                else if( (Z > pitchcal - pitchtol) && (Z < pitchcal + pitchtol))//we're level
                {
                    SetPitchState(CampLevel);
                }
			    else //still high
			    {
                    //decide which side to lower
                    if(Y < rollcal - rolltol)//Roll up ( Left down )
                    {
                        Cio::is().Left(eValveStates::Hold);
                        Cio::is().Right(eValveStates::Dump);
                    }
                    else if(Y > rollcal + rolltol) //Roll down (Right Down)
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
                 if( Z	> pitchcal + pitchtol )
                 {
                     SetPitchState(CampRearLow);
                 }
                 //rear high
                 else  if( Z < pitchcal - pitchtol )
                 {
                     SetPitchState(CampRearHigh);
                 }
                 else //pitch and roll level
                 {
                    if(Y > rollcal + rolltol)//Roll up ( Left down )
                    {
                        Cio::is().Right(eValveStates::Dump);
                        Cio::is().Left(eValveStates::Fill);
                    }
                    else if(Y < rollcal - rolltol) //Roll down (Right Down)
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
                SetPitchState(CampCompleteEnter);
                break;
            case CampComplete:
                //rear low, don't adjust till we change significantly
                //and then SLOWLY
                if(CTimer::IsTimedOut(ReCheckTime, CompleteStart))
                {
                    if( Z	> pitchcal + (pitchtol *2))
                    {
                        SetPitchState(CampRearLow);
                    }
                    //rear high
                    else  if( Z < pitchcal - (pitchtol *2) )
                    {
                        SetPitchState(CampRearHigh);
                    }
                    else //pitch is level check roll
                    {
                        if( (Y < rollcal - (rolltol *2)) || (Y > rollcal + (rolltol*2)))
                        {
                            //Check roll
                            SetPitchState(CampLevel);

                            //if(Y > rollcal + rolltol)//Roll up ( Left down )
                            //{
                            //    Cio::is().Left(eValveStates::Hold);
                            //    Cio::is().Right(eValveStates::Dump);
                            //}
                            //else if(Y < rollcal - rolltol) //Roll down (Right Down)
                            //{
                            //    Cio::is().Right(eValveStates::Hold);
                            //    Cio::is().Left(eValveStates::Dump);
                            //}
                            //else //Roll level
                            //{
                            //    Cio::is().Left(eValveStates::Hold);
                            //    Cio::is().Right(eValveStates::Hold);
                           // }
                        }  
                        else //all level, restart timer for next check
                        {
                            CompleteStart =  CTimer::GetTick();
                        }          
                    }
                }
            //}
            //else
            //{
            //     Start = CTimer::GetTick();
            //    PitchEntered = true;
                break;
	    }
    }        
}
