/* 
* Travel.cpp
*
* Created: 2/1/2015 10:14:47 PM
* Author: keith
*/



#include "CController.h"
#include "CLeds.h"
#include "CSerial.h"

#include "Travel.h"
#include "nvm.h"

FsmTravel::FsmTravel(CController& SMManager) :
CState(SMManager, eStates::STATE_TRAVEL),
LeftSide(LeftRear),
RightSide(RightRear),
Starting(true),
Start(0),
filterwait(0),
waiting(false)
{
}

void FsmTravel::OnEntry()
{
	CLeds::is().TravelOn();
    Cio::is().BlinkTravel(true);
	LeftSide.Init(LeftRear);
	RightSide.Init(RightRear);

	LeftSide.SetLongFilter(false, (int32_t)nvm::is().GetLeftTravel());
	RightSide.SetLongFilter(false, (int32_t)nvm::is().GetRightTravel());
    
    LeftSide.AtHeight(false);
    RightSide.AtHeight(false);
	
	Starting = true;
    waiting = false;

	Start = CTimer::GetTick();
    
    FilterState = FilterStart;
	
	CSerial::is() << " FsmTravel::OnEntry()\r\n";
	CSerial::is() << "Travel Cal vals; Left, " << nvm::is().GetLeftTravel() << ", Right, " << nvm::is().GetRightTravel() << "\n";
}

void FsmTravel::HandleEvent(eEvents evt)
{
	switch(evt)
	{
		case eEvents::TimerEvent:
			//run travel FSM
			LeftSide.Run(nvm::is().GetLeftTravel());
			RightSide.Run(nvm::is().GetRightTravel());
			
            
            switch(FilterState)
            {
                case FilterStart:
                    if(LeftSide.AtHeight() && RightSide.AtHeight() )
                    {
                        filterwait = CTimer::GetTick();
                        FilterState = FilterWait;
                    }
                    break;
                case FilterWait:
                     if(CTimer::IsTimedOut(filterwait, 1000))
                     {
                        CSerial::is() << " *******Setting long travel Filter ******\r\n";
                        LeftSide.SetLongFilter(true, (int32_t)nvm::is().GetLeftTravel());
                        RightSide.SetLongFilter(true, (int32_t)nvm::is().GetRightTravel());
                         
                        Cio::is().BlinkTravel(false);
                        CLeds::is().TravelOn();
                     
                         FilterState = FilterLong;
                     }     
                     else
                     {
                         if((LeftSide.AtHeight() == false) || (RightSide.AtHeight() == false) )
                         {
                             //restart timer
                             filterwait = CTimer::GetTick();
                         }
                     }                
                    break;
                case FilterLong:
                     if((LeftSide.AtHeight() == false) || (RightSide.AtHeight() == false) )
                     {
                        //long filter turned off in Corner class                    
                        FilterState = FilterStart;
                     }
                break;
            }
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
			m_SMManager.ChangeState(eStates::STATE_CAMP);
		break;
		case eEvents::TravelEvent:
			if(Cio::is().TravelSwitches())
			{
				m_SMManager.ChangeState(eStates::STATE_MANUAL);
			}
		break;
		case eEvents::CalibrateEvent:
			m_SMManager.ChangeState(eStates::STATE_TRAVEL_CALIBRATE);
			break;
        case eEvents::ButtonWakeEvent:
        case eEvents::IgnitionOnEvent:
            if( !Cio::is().Awake)
            {
                Cio::is().Awake = true;
                Cio::is().Wakeup();
            
                //Cio::is().ButtonWake = false;
                
                CLeds::is().TravelOn();
                        
                //reset so we get back to ride height quickly
                LeftSide.AtHeight(false);
                RightSide.AtHeight(false);
        
                LeftSide.SetLongFilter(false, (int32_t)nvm::is().GetLeftTravel());
                RightSide.SetLongFilter(false, (int32_t)nvm::is().GetRightTravel());
            
                Starting = true;
                CSerial::is() << " FsmTravel::Ignition On\r\n";
            }                        
            break;

			//let the manua heightstate handle it
		case eEvents::IgnitionOffEvent:
			CSerial::is() << " FsmTravel::Ignition Off\r\n";
            
            Cio::is().Sleep();

			break;
		default:
		break;
	}
}

void FsmTravel::OnExit()
{
    Cio::is().BlinkTravel(false);
	CSerial::is() << " FsmTravel::OnExit()\r\n";
	CLeds::is().TravelOff();
}