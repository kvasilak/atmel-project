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

static const int16_t pitchtol = 10;
static const int16_t rolltol = 5;//10;

FsmCamp::FsmCamp(CController& SMManager) :
CState(SMManager, eStates::STATE_CAMP),
Start(0),
MinTime(100),
IsLevel(false),
ReadyToSleep(false),
DebugTime(1000),
PitchEntered(false),
RollEntered(false)
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
    
    SetPitchState(CampIniting);
	
	DebugDelay = CTimer::GetTick();
}

void FsmCamp::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run camp FSM 
			//LevelIt();
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
	PitchEntered = false;
}

void FsmCamp::SetRollState(RollStates_e s)
{
	static const char *StateStrs[] = {ROLL_STATES_LIST(STRINGIFY)};

	CSerial::is() << ">>>>Roll, ";
	CSerial::is() << StateStrs[s] ;
	CSerial::is() << "<\r\n";

	RollState = s;
	RollEntered = false;
}

bool FsmCamp::LevelRoll(int16_t Y)
{
    int16_t rollcal = nvm::is().GetCampY();
    bool level = false;
    
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
        
        level = true;
    }   
    return level;
}

void FsmCamp::LevelMachine()
{
	int16_t X=0;
	int16_t Y=0;
	int16_t Z=0;
	//Read Acccel
	CMMA8451::is().ReadXYZ(X, Y, Z);
	//determine Pitch and roll errors	int16_t rollcal = nvm::is().GetCampY();
	int16_t pitchcal = nvm::is().GetCampZ();
    
    //TODO do we need a low pass filter?
	 	
	if(CTimer::IsTimedOut(DebugTime, DebugDelay))
	{
		CSerial::is() << "X; " << X << ", Y; " << Y << ", Z; " << Z ;
		 	
		CSerial::is()  << "** roll err; " << Y - rollcal  << ";   pitch err; " << Z -pitchcal << "\n";
		 	
		DebugDelay = CTimer::GetTick();
	}
		 
	switch(PitchState)
	{
		case CampIniting:
			Cio::is().Left(eValveStates::Hold);
			Cio::is().Right(eValveStates::Hold);
			
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
            else //we're levelpitnhwise
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
            //we're level
            else if( (Z > pitchcal - pitchtol) && (Z < pitchcal + pitchtol))
            {
                SetPitchState(CampLevel);
            }
            else
            {
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
                PitchEntered = true;
            }
		    break;
		
		case CampRearHigh:
            //rear low
            if( Z > pitchcal + pitchtol )
            {
                SetPitchState(CampRearLow);
            }
                
            //we're level
            else if( (Z > pitchcal - pitchtol) && (Z < pitchcal + pitchtol))//we're level
            {
                SetPitchState(CampLevel);
            }
			else
			{
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
                else //Roll level
                {
                    Cio::is().Left(eValveStates::Dump);
                    Cio::is().Right(eValveStates::Dump);
                }
                PitchEntered = true;
			}
		    break;
		case CampLevel:
			if(PitchEntered)
			{
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
                      if(LevelRoll(Y))
                      {
                          SetPitchState(CampComplete);
                      }
                     
                 }                     
			}
			else
			{
                LevelRoll(Y);
                
                PitchEntered = true;
			}               
		    break;
        case CampComplete:
            if(PitchEntered)
            {
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
                else //pitch is level
                {
                    if( (Z > pitchcal - pitchtol) && (Z < pitchcal + pitchtol))
                    {
                        //Check roll
                        if(Y > rollcal + rolltol)//Roll up ( Left down )
                        {
                            Cio::is().Left(eValveStates::Hold);
                            Cio::is().Right(eValveStates::Dump);
                        }
                        else if(Y < rollcal - rolltol) //Roll down (Right Down)
                        {
                            Cio::is().Right(eValveStates::Hold);
                            Cio::is().Left(eValveStates::Dump);
                        }
                        else //Roll level
                        {
                            Cio::is().Left(eValveStates::Hold);
                            Cio::is().Right(eValveStates::Hold);
                        }
                    }            
                }            
            }
            else
            {
                 Start = CTimer::GetTick();
                PitchEntered = true;
            }
            break;
	}
}